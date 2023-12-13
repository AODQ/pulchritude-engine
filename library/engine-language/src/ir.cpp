#include "ir.hpp"

#include <pulchritude-engine-language/engine-language.h>

// this is a simple LLVM IR wrapper

#include <pulchritude-error/error.h>
#include <pulchritude-time/time.h>

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/LLJIT.h>
#include <llvm-c/Orc.h>
#include <llvm-c/Target.h>

#include <llvm-c/Transforms/Scalar.h>

#include <atomic>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pint::ir {

struct Type {
  LLVMTypeRef type;
  PuleIRDataTypeInfo typeInfo;
  // allocate types for typeInfo.fn/struct member/param ptr
  std::vector<PuleIRType> memberOrParameterTypes;
};

struct Value {
  PuleIRType type;
  PuleIRValueKind kind;
  LLVMValueRef value;
};

struct ModuleInfo {
  LLVMContextRef context;
  LLVMModuleRef module;
  LLVMTargetDataRef dataLayout;

  std::string moduleName; // seems LLVM C-API doesn't interface module::getName
  pule::ResourceContainer<LLVMBuilderRef> builders;
  std::unordered_map<uint64_t, LLVMBasicBlockRef> basicBlocks;
  pule::ResourceContainer<Type, PuleIRType> types;
  pule::ResourceContainer<Value, PuleIRValue> values;
  std::unordered_map<LLVMValueRef, PuleIRValue> valueLookup;
  std::unordered_map<PuleIRDataType, PuleIRType> genericTypes;
  std::vector<LLVMValueRef> functionValues;
};


pule::ResourceContainer<pint::ir::ModuleInfo, PuleIRModule> modules;
};

// -- modules ------------------------------------------------------------------

PuleIRModule puleIRModuleCreate(PuleIRModuleCreateInfo const ci) {
  pint::ir::ModuleInfo moduleInfo;

  moduleInfo.context = LLVMContextCreate();
  moduleInfo.module = (
    LLVMModuleCreateWithNameInContext(ci.label.contents, moduleInfo.context)
  );
  moduleInfo.dataLayout = LLVMGetModuleDataLayout(moduleInfo.module);
  moduleInfo.moduleName = std::string(ci.label.contents);

  PuleIRModule puModule = pint::ir::modules.create(moduleInfo);
  auto & module = *pint::ir::modules.at(puModule);

  if (ci.ir.contents != nullptr) {
    char * error = nullptr;
    if (
      LLVMParseIRInContext(
        module.context,
        LLVMCreateMemoryBufferWithMemoryRange(
          ci.ir.contents,
          ci.ir.len,
          "",
          false
        ),
        &module.module,
        &error
      )
    ) {
      puleLogError("failed to parse IR: %s", error);
      LLVMDisposeMessage(error);
      return { .id = 0 };
    }
    return puModule;
  }

  auto & type = module.types;
  auto & genericType = module.genericTypes;
  auto & ctx = module.context;

  // build all the generic data types
  for (auto & it : {
      std::pair<
        LLVMTypeRef (*)(LLVMContextRef),
        std::pair<PuleIRDataType, char const *>
      >
      {&LLVMInt8TypeInContext,  {PuleIRDataType_u8,   "PuleIRDataType_u8",  },},
      {&LLVMInt16TypeInContext, {PuleIRDataType_u16,  "PuleIRDataType_u16", },},
      {&LLVMInt32TypeInContext, {PuleIRDataType_u32,  "PuleIRDataType_u32", },},
      {&LLVMInt64TypeInContext, {PuleIRDataType_u64,  "PuleIRDataType_u64", },},
      {&LLVMFloatTypeInContext, {PuleIRDataType_f32,  "PuleIRDataType_f32", },},
      {&LLVMDoubleTypeInContext,{PuleIRDataType_f64,  "PuleIRDataType_f64", },},
      {&LLVMInt1TypeInContext,  {PuleIRDataType_bool, "PuleIRDataType_bool",},},
      {&LLVMVoidTypeInContext,  {PuleIRDataType_void, "PuleIRDataType_void",},},
  }) {
    LLVMTypeRef typeref = it.first(ctx);
    genericType[it.second.first] = {
      type.create(
        pint::ir::Type {
          .type = typeref,
          .typeInfo = PuleIRDataTypeInfo {
            .module = puModule, .type = it.second.first,
            .byteCount = (
              it.second.first == PuleIRDataType_void
              ? 0
              : LLVMStoreSizeOfType(module.dataLayout, typeref)
            ),
            .genericInfo = {},
          },
        }
      )
    };
  }

  genericType[PuleIRDataType_ptr] = {
    type.create(
      pint::ir::Type {
        .type = LLVMPointerTypeInContext(ctx, 0),
        .typeInfo = {
          .module = puModule, .type = PuleIRDataType_ptr, .genericInfo = {},
        },
      }
    )
  };
  genericType[PuleIRDataType_i8]  = genericType[PuleIRDataType_u8];
  genericType[PuleIRDataType_i16] = genericType[PuleIRDataType_u16];
  genericType[PuleIRDataType_i32] = genericType[PuleIRDataType_u32];
  genericType[PuleIRDataType_i64] = genericType[PuleIRDataType_u64];

  return puModule;
}

void puleIRModuleDestroy(PuleIRModule const module) {
  auto moduleInfo = pint::ir::modules.at(module);
  for (auto & builder : moduleInfo->builders) {
    LLVMDisposeBuilder(builder.second);
  }
  LLVMDisposeModule(moduleInfo->module);
  LLVMContextDispose(moduleInfo->context);
}

void puleELModuleVerify(PuleELModule const module) {
  auto moduleInfo = pint::ir::modules.at({module.id});
  LLVMPassManagerRef passManager = (
    LLVMCreateFunctionPassManagerForModule(moduleInfo->module)
  );
  LLVMAddVerifierPass(passManager);

  LLVMInitializeFunctionPassManager(passManager);
  for (auto & fn : moduleInfo->functionValues) {
    LLVMRunFunctionPassManager(passManager, fn);
  }
  LLVMFinalizeFunctionPassManager(passManager);
  LLVMDisposePassManager(passManager);
}

void puleELModuleOptimize(PuleELModule const module) {
  auto moduleInfo = pint::ir::modules.at({module.id});
  LLVMPassManagerRef passManager = (
    LLVMCreateFunctionPassManagerForModule(moduleInfo->module)
  );
  LLVMAddInstructionCombiningPass(passManager);
  LLVMAddCFGSimplificationPass(passManager);
  LLVMAddDeadStoreEliminationPass(passManager);
  LLVMAddScalarizerPass(passManager);
  LLVMAddReassociatePass(passManager);
  LLVMAddTailCallEliminationPass(passManager);
  LLVMAddDemoteMemoryToRegisterPass(passManager);
  LLVMAddGVNPass(passManager);
  LLVMAddAggressiveDCEPass(passManager);

  LLVMInitializeFunctionPassManager(passManager);
  for (auto & fn : moduleInfo->functionValues) {
    LLVMRunFunctionPassManager(passManager, fn);
  }
  LLVMFinalizeFunctionPassManager(passManager);
  LLVMDisposePassManager(passManager);
}

void puleELModuleDump(PuleELModule const module) {
  auto moduleInfo = pint::ir::modules.at({module.id});
  auto str = LLVMPrintModuleToString(moduleInfo->module);
  puleLog("%s", str);
  LLVMDisposeMessage(str);
}

void puleELValueDump(PuleELModule const module, PuleIRValue const value) {
  auto moduleInfo = pint::ir::modules.at({module.id});
  auto str = LLVMPrintValueToString(moduleInfo->values.at(value)->value);
  puleLog("%s", str);
  LLVMDisposeMessage(str);
}

// -- jit engine ---------------------------------------------------------------

namespace pint::ir {

struct JITEngine {
  std::unordered_map<std::string, LLVMOrcJITDylibRef> dylib;
  std::vector<LLVMOrcMaterializationUnitRef> engineMaterializationUnits;
  LLVMOrcThreadSafeContextRef context;
  LLVMOrcLLJITBuilderRef builder;
  LLVMOrcLLJITRef jit;
  LLVMOrcExecutionSessionRef executionSession;
  LLVMOrcObjectLinkingLayerRef objectLayer;
  LLVMTargetMachineRef targetMachine;
};


pule::ResourceContainer<pint::ir::JITEngine, PuleELJitEngine> jitEngines;
bool jitInitialized = false;

} // namespace


extern "C" {
PuleELJitEngine puleELJitEngineCreate(PuleELJitEngineCreateInfo const ci) {
  if (!pint::ir::jitInitialized) {
    pint::ir::jitInitialized = true;
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMLinkInMCJIT();
  }

  char * defaultTriple = LLVMGetDefaultTargetTriple();
  char * error = nullptr;
  LLVMTargetRef target = nullptr;
  if (LLVMGetTargetFromTriple(defaultTriple, &target, &error)) {
    puleLogError("failed to get target from triple: %s", error);
    LLVMDisposeMessage(error);
    return { .id = 0 };
  }

  if (!LLVMTargetHasJIT(target)) {
    puleLogError("target does not support JIT");
    return { .id = 0 };
  }

  LLVMOrcLLJITBuilderRef builder = LLVMOrcCreateLLJITBuilder();
  LLVMOrcLLJITRef jit;
  LLVMOrcCreateLLJIT(&jit, builder);

  pint::ir::JITEngine jitEngine = {
    .dylib = {},
    .context = LLVMOrcCreateNewThreadSafeContext(),
    .builder = builder,
    .jit = jit,
    .executionSession = LLVMOrcLLJITGetExecutionSession(jit),
    .objectLayer = {},
    .targetMachine = (
      LLVMCreateTargetMachine(
        target, defaultTriple, "", "",
        LLVMCodeGenLevelDefault,
        LLVMRelocDefault, // probably need to use LLVMRelocPIC
        LLVMCodeModelJITDefault
      )
    )
  };

  if (ci.insertEngineSymbols) {
    #define orcSym(name) \
      { \
        LLVMOrcExecutionSessionIntern(jitEngine.executionSession, #name), \
        LLVMJITEvaluatedSymbol { \
          .Address = (uint64_t)(void *)name, \
          .Flags = { \
            .GenericFlags = ( \
                LLVMJITSymbolGenericFlagsCallable \
              | LLVMJITSymbolGenericFlagsExported \
            ), \
            .TargetFlags = 0, \
          }, \
        }, \
      }
    std::vector<LLVMOrcCSymbolMapPair> symbolMap = {
      orcSym(puleLog),
      orcSym(puleCStr),
    };
    #undef orcSym
    jitEngine.engineMaterializationUnits.emplace_back(
      LLVMOrcAbsoluteSymbols(symbolMap.data(), symbolMap.size())
    );
  }

  return pint::ir::jitEngines.create(jitEngine);
}
void puleELJitEngineDestroy(PuleELJitEngine const jitEngine) {
  // TODO
  (void)jitEngine;
}
void puleELJitEngineAddModule(
  PuleELJitEngine const jitEngine,
  PuleELModule const module
) {
  auto jit = pint::ir::jitEngines.at(jitEngine);
  auto moduleInfo = pint::ir::modules.at({module.id});

  // TODO check if the name exists, and if so remove old one from the
  //      execution session

  LLVMOrcJITDylibRef dylib = (
    LLVMOrcExecutionSessionCreateBareJITDylib(
      jit->executionSession,
      moduleInfo->moduleName.c_str()
    )
  );
  jit->dylib.emplace(moduleInfo->moduleName, dylib);

  // move the LLVM module to the ORC context
  LLVMOrcThreadSafeModuleRef orcModule = (
    LLVMOrcCreateNewThreadSafeModule(moduleInfo->module, jit->context)
  );

  // add the dylib to the jit session
  LLVMOrcLLJITAddLLVMIRModule(jit->jit, dylib, orcModule);

  // add materialization units
  for (auto & mu : jit->engineMaterializationUnits) {
    LLVMOrcJITDylibDefine(dylib, mu);
  }
}
} // extern "C"

typedef struct {
  std::atomic<bool> found;
  uint64_t address;
} pulintELJITLookupContext;

static void pulintELJitLookupCallback(
  LLVMErrorRef error,
  LLVMOrcCSymbolMapPairs result, size_t numPairs,
  void * ctx
) {
  auto & context = *reinterpret_cast<pulintELJITLookupContext *>(ctx);
  if (numPairs == 0) {
    context.found = true;
    context.address = 0;
    return;
  }
  PULE_assert(numPairs == 1 && "only one lookup at a time");
  context.address = result[0].Sym.Address;
  context.found = true;
  (void)error;
}

void * puleELJitEngineFunctionAddress(
  PuleELJitEngine const jitEngine,
  PuleStringView const functionName
) {
  auto const & jit = pint::ir::jitEngines.at(jitEngine);
  std::vector<LLVMOrcCJITDylibSearchOrderElement> dylibs;
  for (auto & it : jit->dylib) {
    dylibs.emplace_back(LLVMOrcCJITDylibSearchOrderElement{
      .JD = it.second,
      .JDLookupFlags = LLVMOrcJITDylibLookupFlagsMatchExportedSymbolsOnly,
    });
  }
  auto symbolPoolRef = (
    LLVMOrcExecutionSessionIntern(jit->executionSession, functionName.contents)
  );
  LLVMOrcCLookupSetElement symbol = {
    .Name = symbolPoolRef,
    .LookupFlags = LLVMOrcSymbolLookupFlagsRequiredSymbol,
  };

  auto lookupCtx = pulintELJITLookupContext {};
  LLVMOrcExecutionSessionLookup(
    jit->executionSession,
    LLVMOrcLookupKindDLSym,
    dylibs.data(), dylibs.size(),
    &symbol, 1,
    pulintELJitLookupCallback, &lookupCtx
  );

  LLVMOrcReleaseSymbolStringPoolEntry(symbolPoolRef);

  while (true) {
    if (lookupCtx.found) { break; }
    puleSleepMicrosecond({.valueMicro = 1'000});
  }

  return (void *)lookupCtx.address;
}

// -- data types ---------------------------------------------------------------

bool puleIRDataTypeIsInt(PuleIRDataType const type) {
  switch (type) {
    case PuleIRDataType_bool:
    case PuleIRDataType_u8:
    case PuleIRDataType_u16:
    case PuleIRDataType_u32:
    case PuleIRDataType_u64:
    case PuleIRDataType_i8:
    case PuleIRDataType_i16:
    case PuleIRDataType_i32:
    case PuleIRDataType_i64:
      return true;
    default:
      return false;
  }
}

bool puleIRDataTypeIsFloat(PuleIRDataType const type) {
  switch (type) {
    case PuleIRDataType_f16:
    case PuleIRDataType_f32:
    case PuleIRDataType_f64:
      return true;
    default:
      return false;
  }
}

bool puleIRDataTypeIsSigned(PuleIRDataType const type) {
  switch (type) {
    case PuleIRDataType_u8:
    case PuleIRDataType_u16:
    case PuleIRDataType_u32:
    case PuleIRDataType_u64:
      return false;
    case PuleIRDataType_i8:
    case PuleIRDataType_i16:
    case PuleIRDataType_i32:
    case PuleIRDataType_i64:
      return true;
    case PuleIRDataType_f16:
    case PuleIRDataType_f32:
    case PuleIRDataType_f64:
      return true; // i guess technically true
    default:
      return false;
  }
}

uint32_t puleIRDataTypeByteCount(PuleIRDataType const type) {
  switch (type) {
    case PuleIRDataType_void:
    case PuleIRDataType_struct:
    case PuleIRDataType_array:
      PULE_assert(false && "can not get byte count of this type");
    case PuleIRDataType_bool:     return 1;
    case PuleIRDataType_function: return 8;
    case PuleIRDataType_ptr:      return 8;
    case PuleIRDataType_u8:       return 1;
    case PuleIRDataType_u16:      return 2;
    case PuleIRDataType_u32:      return 4;
    case PuleIRDataType_u64:      return 8;
    case PuleIRDataType_i8:       return 1;
    case PuleIRDataType_i16:      return 2;
    case PuleIRDataType_i32:      return 4;
    case PuleIRDataType_i64:      return 8;
    case PuleIRDataType_f16:      return 2;
    case PuleIRDataType_f32:      return 4;
    case PuleIRDataType_f64:      return 8;
  }
}

// -- types --------------------------------------------------------------------

PuleIRType puleIRTypeCreate(PuleIRDataTypeInfo const createInfo) {
  auto moduleInfo = pint::ir::modules.at(createInfo.module);
  switch (createInfo.type) {
    default: PULE_assert(false && "TODO");
    case PuleIRDataType_u8:
    case PuleIRDataType_u16:
    case PuleIRDataType_u32:
    case PuleIRDataType_u64:
    case PuleIRDataType_i8:
    case PuleIRDataType_i16:
    case PuleIRDataType_i32:
    case PuleIRDataType_i64:
    case PuleIRDataType_f32:
    case PuleIRDataType_f64:
    case PuleIRDataType_bool:
    case PuleIRDataType_void:
    case PuleIRDataType_ptr:
      return moduleInfo->genericTypes[createInfo.type];
    case PuleIRDataType_function: {
      std::vector<PuleIRType> paramTypes;
      std::vector<LLVMTypeRef> paramLlvmTypes;
      for (size_t i = 0; i < createInfo.fnInfo.parameterCount; ++ i) {
        paramTypes.emplace_back(createInfo.fnInfo.parameterTypes[i]);
        paramLlvmTypes.emplace_back(
          moduleInfo->types.at(paramTypes.back())->type
        );
      }
      auto ti = pint::ir::Type {
          .type = LLVMFunctionType(
            moduleInfo->types.at(createInfo.fnInfo.returnType)->type,
            paramLlvmTypes.data(),
            paramLlvmTypes.size(),
            createInfo.fnInfo.variadic
          ),
          .typeInfo = createInfo,
          .memberOrParameterTypes = paramTypes,
        };
      auto t = (
        moduleInfo->types.create(ti)
      );
      auto const tt = puleIRTypeInfo(createInfo.module, t);
      puleLog(
        "prev created function :: return type %llu :: parameters %p size %zu ",
        tt.fnInfo.returnType,
        tt.fnInfo.parameterTypes,
        tt.fnInfo.parameterCount
      );
      puleLog(
        "created function :: return type %llu :: parameters %p size %zu ",
        tt.fnInfo.returnType,
        tt.fnInfo.parameterTypes,
        tt.fnInfo.parameterCount
      );
      return t;
    }
    case PuleIRDataType_struct: {
      LLVMTypeRef structType = (
        LLVMStructCreateNamed(
          moduleInfo->context,
          createInfo.structInfo.label.contents
        )
      );
      std::vector<PuleIRType> memberTypes;
      std::vector<LLVMTypeRef> memberLlvmTypes;
      for (size_t i = 0; i < createInfo.structInfo.memberCount; ++ i) {
        memberTypes.emplace_back(createInfo.structInfo.memberTypes[i]);
        memberLlvmTypes.emplace_back(
          moduleInfo->types.at(memberTypes.back())->type
        );
      }
      LLVMStructSetBody(
        structType,
        memberLlvmTypes.data(),
        memberLlvmTypes.size(),
        createInfo.structInfo.packed
      );
      return (
        moduleInfo->types.create({
          .type = structType,
          .typeInfo = createInfo,
          .memberOrParameterTypes = memberTypes,
        })
      );
    }
    case PuleIRDataType_array: {
      return {
        moduleInfo->types.create({
          .type = LLVMArrayType(
            moduleInfo->types.at(createInfo.arrayInfo.elementType)->type,
            createInfo.arrayInfo.elementCount
          ),
          .typeInfo = createInfo,
          .memberOrParameterTypes = {},
        })
      };
    }
  }
}

PuleIRDataTypeInfo puleIRTypeInfo(
  PuleIRModule const puModule,
  PuleIRType const puType
) {
  auto & moduleInfo = *pint::ir::modules.at(puModule);
  auto & type = *moduleInfo.types.at(puType);
  // fix up typeinfo if it has pointers
  if (type.typeInfo.type == PuleIRDataType_function) {
    type.typeInfo.fnInfo.parameterCount = type.memberOrParameterTypes.size();
    type.typeInfo.fnInfo.parameterTypes = type.memberOrParameterTypes.data();
  }
  if (type.typeInfo.type == PuleIRDataType_struct) {
    type.typeInfo.structInfo.memberCount = type.memberOrParameterTypes.size();
    type.typeInfo.structInfo.memberTypes = type.memberOrParameterTypes.data();
  }
  return type.typeInfo;
}

// -- values -------------------------------------------------------------------

PuleIRType puleIRValueType(
  PuleIRModule const puModule,
  PuleIRValue const puValue
) {
  pint::ir::Value const & value = (
    *pint::ir::modules.at(puModule)->values.at(puValue)
  );
  return value.type;
}

PuleIRValueKind puleIRValueKind(
  PuleIRModule const puModule,
  PuleIRValue const puValue
) {
  pint::ir::Value const & value = (
    *pint::ir::modules.at(puModule)->values.at(puValue)
  );
  return value.kind;
}

PuleIRValue puleIRValueVoid() { return { .id = 0 }; }

// -- code blocks --------------------------------------------------------------

PuleIRCodeBlock puleIRCodeBlockCreate(
  PuleIRCodeBlockCreateInfo const createInfo
) {
  auto moduleInfo = pint::ir::modules.at(createInfo.module);
  LLVMBuilderRef const builder = (
    LLVMCreateBuilderInContext(moduleInfo->context)
  );
  LLVMBasicBlockRef const block = (
    LLVMAppendBasicBlockInContext(
      moduleInfo->context,
      moduleInfo->values.at(createInfo.context)->value,
      createInfo.label.contents
    )
  );
  uint64_t codeBlockID = (
    reinterpret_cast<uint64_t>(moduleInfo->builders.create(builder))
  );
  moduleInfo->basicBlocks.emplace(codeBlockID, block);
  if (createInfo.positionBuilderAtEnd) {
    LLVMPositionBuilderAtEnd(builder, block);
  }
  return { .id = codeBlockID };
}

void puleIRCodeBlockTerminate(PuleIRCodeBlockTerminateCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  LLVMBuilderRef builder = module.builders.at(ci.codeBlock.id);
  switch (ci.type) {
    case PuleIRCodeBlockTerminateType_ret:
      if (ci.ret.value.id == 0) {
        LLVMBuildRetVoid(builder);
      } else {
        LLVMBuildRet(builder, module.values.at(ci.ret.value)->value);
      }
    break;
  }
}

PuleIRCodeBlock puleIRCodeBlockLast(
  PuleIRModule const module, PuleIRValue const fnValue
) {
  LLVMValueRef fn = pint::ir::modules.at(module)->values.at(fnValue)->value;
  LLVMBasicBlockRef block = LLVMGetLastBasicBlock(fn);
  // todo cache this so dont have to iterte etc
  for (auto & it : pint::ir::modules.at(module)->basicBlocks) {
    if (it.second == block) {
      return { .id = it.first };
    }
  }
  return { .id = 0, };
}

void puleIRCodeBlockMoveAfter(
  PuleIRModule const module,
  PuleIRCodeBlock const codeBlock,
  PuleIRCodeBlock const moveTarget
) {
  auto & moduleInfo = *pint::ir::modules.at(module);
  LLVMMoveBasicBlockAfter(
    moduleInfo.basicBlocks.at(codeBlock.id),
    moduleInfo.basicBlocks.at(moveTarget.id)
  );
}

void puleIRCodeBlockPositionBuilderAtEnd(
  PuleIRModule const module,
  PuleIRCodeBlock const codeBlock
) {
//auto & moduleInfo = *pint::ir::modules.at(module);
//  LLVMPositionBuilderAtEnd(
//    moduleInfo.builders.at(codeBlock.id),
//    codeBlock.id
//  );
}

// -- value creation -----------------------------------------------------------

PuleIRValue puleIRValueConstCreate(
  PuleIRValueConstCreateInfo const createInfo
) {
  auto & module = *pint::ir::modules.at(createInfo.module);
  auto & type = *module.types.at(createInfo.type);
  PuleIRDataType puDataType = (
    puleIRTypeInfo(createInfo.module, createInfo.type).type
  );
  if (createInfo.kind == PuleIRValueKind_function) {
    PULE_assert(puDataType == PuleIRDataType_function);
    LLVMValueRef function = (
      LLVMAddFunction(
        module.module,
        createInfo.label.contents,
        type.type
      )
    );
    module.functionValues.emplace_back(function);
    PuleIRValue const fnValue = {
      module.values.create({
        .type = createInfo.type,
        .kind = PuleIRValueKind_function,
        .value = function,
      })
    };
    auto const fnInfo = puleIRTypeInfo(createInfo.module, createInfo.type);
    // TODO module.valueLookup.emplace (function)
    // insert function param values to lookup
    for (size_t i = 0; i < fnInfo.fnInfo.parameterCount; ++ i) {
      PuleIRValue const paramValue = {
        module.values.create({
          .type = fnInfo.fnInfo.parameterTypes[i],
          .kind = PuleIRValueKind_dynamic,
          .value = LLVMGetParam(function, i),
        })
      };
      module.valueLookup.emplace(
        LLVMGetParam(function, i),
        paramValue
      );
    }
    return fnValue;
  }
  switch (createInfo.kind) {
    default: PULE_assert(false && "TODO");
    case PuleIRValueKind_constString: {
      PULE_assert(puDataType == PuleIRDataType_ptr);
      LLVMTypeRef stringType = (
        LLVMArrayType(
          module.types.at(module.genericTypes.at(PuleIRDataType_u8))->type,
          createInfo.constString.string.len+1
        )
      );
      LLVMValueRef globalRef = (
        LLVMAddGlobal(module.module, stringType, createInfo.label.contents)
      );
      LLVMValueRef str = (
        LLVMConstStringInContext(
          module.context,
          createInfo.constString.string.contents,
          createInfo.constString.string.len,
          false
        )
      );
      LLVMSetGlobalConstant(globalRef, true);
      LLVMSetLinkage(globalRef, LLVMPrivateLinkage);
      LLVMSetInitializer(globalRef, str);
      return {
        module.values.create({
          .type = createInfo.type,
          .kind = PuleIRValueKind_constString,
          .value = globalRef,
        })
      };
      // module.valueLookup.emplace(globalRef, value); TODO
    }
    case PuleIRValueKind_constInt: {
      switch (puDataType) {
        default:
          PULE_assert(false && "can not create const value of this type");
        case PuleIRDataType_bool:
        case PuleIRDataType_u8:
        case PuleIRDataType_u16:
        case PuleIRDataType_u32:
        case PuleIRDataType_u64:
        case PuleIRDataType_i8:
        case PuleIRDataType_i16:
        case PuleIRDataType_i32:
        case PuleIRDataType_i64: {
          return {
            module.values.create({
              .type = createInfo.type,
              .kind = PuleIRValueKind_dynamic,
              .value = (
                LLVMConstInt(
                  type.type,
                  createInfo.constInt.u64,
                  createInfo.constInt.isSigned
                )
              )
            })
          };
          // TODO module.valueLookup.emplace
        }
      } // switch (createInfo.type)
    }
    case PuleIRValueKind_constFloat: {
      switch (puDataType) {
        default:
          PULE_assert(false && "can not create const value of this type");
        case PuleIRDataType_f16:
        case PuleIRDataType_f32:
        case PuleIRDataType_f64: {
          return {
            module.values.create({
              .type = createInfo.type,
              .kind = PuleIRValueKind_dynamic,
              .value = LLVMConstReal( type.type, createInfo.constFloat.f64)
            })
          };
          // TODO module.valueLookup.emplace
        }
      } // switch (createInfo.type)
    }
  } // switch (createInfo.kind)

  PULE_assert(false);
  return { .id = 0 };
}

size_t puleIRValueFunctionParameterCount(
  PuleIRModule const m,
  PuleIRValue const fnValue
) {
  return LLVMCountParams(pint::ir::modules.at(m)->values.at(fnValue)->value);
}

PuleIRValue puleIRValueFunctionParameter(
  PuleIRModule const m,
  PuleIRValue const fnValue,
  size_t const index
) {
  auto & module = *pint::ir::modules.at(m);
  LLVMValueRef paramValue = (
    LLVMGetParam(module.values.at(fnValue)->value, index)
  );
  return module.valueLookup.at(paramValue);
}

PuleIRValue puleIRValueConstI64(PuleIRModule const m, int64_t const i64) {
  return (
    puleIRValueConstCreate({
      .module = m,
      .type = puleIRTypeCreate({
        .module = m,
        .type = PuleIRDataType_i64,
        .genericInfo = {},
      }),
      .kind = PuleIRValueKind_constInt,
      .constInt = { .u64 = (uint64_t)i64, .isSigned = true, },
    })
  );
}

// -- code block build ---------------------------------------------------------

namespace pint::ir {

PuleIRValue puleIRBuildReturn(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & ret = ci.instr.ret;
  if (ret.value.id == 0) {
    LLVMBuildRetVoid(module.builders.at(ci.codeBlock.id));
    return puleIRValueVoid();
  }
  return {
    module.values.create(pint::ir::Value {
      .type = puleIRValueType(ci.module, ret.value),
      .kind = puleIRValueKind(ci.module, ret.value),
      .value = (
        LLVMBuildRet(
          module.builders.at(ci.codeBlock.id),
          module.values.at(ret.value)->value
        )
      ),
    })
  };
}
PuleIRValue puleIRBuildCall(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & call = ci.instr.call;
  // resulting type is the return type of the function
  PuleIRType const returnType = (
    puleIRTypeInfo(
      ci.module, call.functionType
    ).fnInfo.returnType
  );
  std::vector<LLVMValueRef> arguments;
  for (size_t i = 0; i < call.argumentCount; ++ i) {
    arguments.emplace_back(module.values.at(call.arguments[i])->value);
  }
  return {
    module.values.create(pint::ir::Value {
      .type = returnType,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildCall2(
          module.builders.at(ci.codeBlock.id),
          module.types.at(call.functionType)->type,
          module.values.at(call.function)->value,
          arguments.data(),
          arguments.size(),
          call.label.contents
        )
      ),
    })
  };
}

PuleIRValue puleIRBuildAllocate(
  PuleIRBuildCreateInfo const ci
) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & stackAlloc = ci.instr.stackAlloc;
  auto & valueSizeInfo = *module.values.at(stackAlloc.valueSize);
  if (
    valueSizeInfo.kind != PuleIRValueKind_constInt
    && valueSizeInfo.kind != PuleIRValueKind_dynamic
  ) {
    puleLog("Kind: %d", valueSizeInfo.kind);
    PULE_assert(false && "can not allocate with count of this type");
  }
  std::string const label = (
      std::to_string(ci.codeBlock.id) + "_"
    + std::string(stackAlloc.label.contents)
  );
  return {
    module.values.create(pint::ir::Value {
      .type = puleIRTypeCreate({ci.module, PuleIRDataType_ptr, {}}),
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildAlloca(
          module.builders.at(ci.codeBlock.id),
          module.types.at(stackAlloc.type)->type,
          label.c_str()
        )
      ),
    })
  };
}

void puleIRBuildFree(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  LLVMBuildFree(
    module.builders.at(ci.codeBlock.id),
    module.values.at(ci.instr.free.value)->value
  );
}

PuleIRValue puleIRBuildLoad(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & load = ci.instr.load;
  std::string const label = (
      std::to_string(ci.codeBlock.id) + "_"
    + std::string(load.label.contents)
  );
  return {
    module.values.create(pint::ir::Value {
      .type = load.type,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildLoad2(
          module.builders.at(ci.codeBlock.id),
          module.types.at(load.type)->type,
          module.values.at(load.value)->value,
          label.c_str()
        )
      ),
    })
  };
}

void puleIRBuildStore(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  LLVMBuildStore(
    module.builders.at(ci.codeBlock.id),
    module.values.at(ci.instr.store.value)->value,
    module.values.at(ci.instr.store.dst)->value
  );
}

PuleIRValue puleIRBuildTrunc(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & trunc = ci.instr.trunc;
  std::string const label = (
      std::to_string(ci.codeBlock.id) + "_"
    + std::string(trunc.label.contents)
  );
  return {
    module.values.create(pint::ir::Value {
      .type = trunc.type,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildTrunc(
          module.builders.at(ci.codeBlock.id),
          module.values.at(trunc.value)->value,
          module.types.at(trunc.type)->type,
          label.c_str()
        )
      ),
    })
  };
}

PuleIRValue puleIRBuildCast(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & cast = ci.instr.cast;
  static const std::unordered_map<PuleIRCast, LLVMOpcode> castToOpcode {
    { PuleIRCast_intTrunc,    LLVMTrunc, },
    { PuleIRCast_uintExtend,  LLVMZExt, },
    { PuleIRCast_intExtend,   LLVMSExt, },
    { PuleIRCast_floatToUint, LLVMFPToUI, },
    { PuleIRCast_floatToInt,  LLVMFPToSI, },
    { PuleIRCast_uintToFloat, LLVMUIToFP, },
    { PuleIRCast_intToFloat,  LLVMSIToFP, },
    { PuleIRCast_floatTrunc,  LLVMFPTrunc, },
    { PuleIRCast_floatExtend, LLVMFPExt, },
  };
  std::string const label = (
      std::to_string(ci.codeBlock.id) + "_"
    + std::string(cast.label.contents)
  );
  return {
    module.values.create(pint::ir::Value {
      .type = cast.type,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildCast(
          module.builders.at(ci.codeBlock.id),
          castToOpcode.at(cast.castType),
          module.values.at(cast.value)->value,
          module.types.at(cast.type)->type,
          label.c_str()
        )
      ),
    })
  };
}

void puleIRBuildSwitch(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & switch_ = ci.instr.switch_;
  LLVMValueRef const value = module.values.at(switch_.value)->value;
  LLVMBasicBlockRef const blockDefault = (
    module.basicBlocks.at(switch_.codeBlockDefault.id)
  );
  LLVMValueRef const switchInstr = (
    LLVMBuildSwitch(
      module.builders.at(ci.codeBlock.id),
      value,
      blockDefault,
      switch_.caseCount
    )
  );
  for (size_t it = 0; it < switch_.caseCount; ++ it) {
    LLVMValueRef const caseValue = (
      module.values.at(switch_.cases[it].value)->value
    );
    LLVMBasicBlockRef const caseBlock = (
      module.basicBlocks.at(switch_.cases[it].codeBlock.id)
    );
    LLVMAddCase(switchInstr, caseValue, caseBlock);
  }
}

PuleIRValue puleIRBuildOperation(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto op = ci.instr.op;
  bool const isBinary = (
       op.type != PuleIROperator_neg
    && op.type != PuleIROperator_not
  );

  pint::ir::Value const & lhs = *module.values.at(op.unary.lhs);
  pint::ir::Type const & lhsType = *module.types.at(lhs.type);

  if (!isBinary) {
    // handle
  }

  bool const isLhsFloatingPoint = (
       lhsType.typeInfo.type == PuleIRDataType_f16
    || lhsType.typeInfo.type == PuleIRDataType_f32
    || lhsType.typeInfo.type == PuleIRDataType_f64
  );
  bool const isLhsSigned = (
       lhsType.typeInfo.type == PuleIRDataType_i8
    || lhsType.typeInfo.type == PuleIRDataType_i16
    || lhsType.typeInfo.type == PuleIRDataType_i32
    || lhsType.typeInfo.type == PuleIRDataType_i64
  );

  PULE_assert(!(isLhsFloatingPoint ^ op.floatingPoint));

  pint::ir::Value const & rhs = *module.values.at(op.binary.rhs);

  static const std::unordered_map<
    PuleIROperator,
    std::vector<
      LLVMValueRef (*)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, char const *)
    >
  > operatorToBinaryIntFn = {
    { PuleIROperator_add,     { LLVMBuildAdd,  LLVMBuildAdd},  },
    { PuleIROperator_sub,     { LLVMBuildSub,  LLVMBuildSub},  },
    { PuleIROperator_mul,     { LLVMBuildMul,  LLVMBuildMul},  },
    { PuleIROperator_div,     { LLVMBuildUDiv, LLVMBuildSDiv}, },
    { PuleIROperator_mod,     { LLVMBuildSRem, LLVMBuildSRem}, },
    { PuleIROperator_and,     { LLVMBuildAnd,  LLVMBuildAnd},  },
    { PuleIROperator_or,      { LLVMBuildOr,   LLVMBuildOr},   },
    { PuleIROperator_xor,     { LLVMBuildXor,  LLVMBuildXor},  },
    { PuleIROperator_bit_and, { LLVMBuildAnd,  LLVMBuildAnd},  },
    { PuleIROperator_bit_or,  { LLVMBuildOr,   LLVMBuildOr},   },
    { PuleIROperator_bit_xor, { LLVMBuildXor,  LLVMBuildXor},  },
    { PuleIROperator_shl,     { LLVMBuildShl,  LLVMBuildShl},  },
    { PuleIROperator_shr,     { LLVMBuildLShr, LLVMBuildLShr}, },
  };

  static const std::unordered_map<
    PuleIROperator,
    LLVMValueRef (*)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, char const *)
  > operatorToBinaryFloatFn = {
    { PuleIROperator_add, LLVMBuildFAdd, },
    { PuleIROperator_sub, LLVMBuildFSub, },
    { PuleIROperator_mul, LLVMBuildFMul, },
    { PuleIROperator_div, LLVMBuildFDiv, },
    { PuleIROperator_mod, LLVMBuildFRem, },
  };

  bool const isLogicOp = (
       op.type == PuleIROperator_and
    || op.type == PuleIROperator_or
    || op.type == PuleIROperator_xor
  );

  if (isLogicOp) {
    // need both sides to be bool
    auto const boolType = (
      puleIRTypeCreate({
        .module = ci.module,
        .type = PuleIRDataType_bool,
      })
    );
    PuleIRValue lhsValue = (
      puleIRBuildCast({
        .module = ci.module,
        .codeBlock = ci.codeBlock,
        .instr = {
          .cast = {
            .castType = (
              puleIRCastTypeFromDataTypes(
                puleIRTypeInfo(
                  ci.module,
                  puleIRValueType(ci.module, op.binary.lhs)
                ).type,
                PuleIRDataType_bool
              )
            ),
            .value = op.binary.lhs,
            .type = boolType,
            .label = puleCStr("lhs-logic-op-cast"),
          }
        },
      })
    );
    PuleIRValue rhsValue = (
      puleIRBuildCast({
        .module = ci.module,
        .codeBlock = ci.codeBlock,
        .instr = {
          .cast = {
            .castType = (
              puleIRCastTypeFromDataTypes(
                puleIRTypeInfo(
                  ci.module,
                  puleIRValueType(ci.module, op.binary.rhs)
                ).type,
                PuleIRDataType_bool
              )
            ),
            .value = op.binary.rhs,
            .type = boolType,
            .label = puleCStr("rhs-logic-op-cast"),
          }
        },
      })
    );
    op.binary.lhs = lhsValue;
    op.binary.rhs = rhsValue;
  }

  bool const isMathematic = (
       op.type == PuleIROperator_add || op.type == PuleIROperator_sub
    || op.type == PuleIROperator_mul || op.type == PuleIROperator_div
    || op.type == PuleIROperator_mod || op.type == PuleIROperator_and
    || op.type == PuleIROperator_or  || op.type == PuleIROperator_xor
    || op.type == PuleIROperator_shl || op.type == PuleIROperator_shr
    || op.type == PuleIROperator_bit_and
    || op.type == PuleIROperator_bit_or
    || op.type == PuleIROperator_bit_xor
  );

  std::string const label = (
      std::to_string(ci.codeBlock.id) + "_"
    + std::string(op.label.contents)
  );
  if (isMathematic && !op.floatingPoint) {
    return {
      module.values.create(pint::ir::Value {
        .type = lhs.type, // TODO this is wrong, needs to be max
        .kind = PuleIRValueKind_dynamic,
        .value = (
          operatorToBinaryIntFn.at(op.type)[(int32_t)isLhsSigned](
            module.builders.at(ci.codeBlock.id),
            lhs.value,
            rhs.value,
            label.c_str()
          )
        ),
      })
    };
  } else if (isMathematic && op.floatingPoint) {
    PULE_assert(operatorToBinaryFloatFn.contains(op.type));
    return {
      module.values.create(pint::ir::Value {
        .type = lhs.type, // TODO this is wrong, needs to be max
        .kind = PuleIRValueKind_dynamic,
        .value = (
          operatorToBinaryFloatFn.at(op.type)(
            module.builders.at(ci.codeBlock.id),
            lhs.value,
            rhs.value,
            label.c_str()
          )
        ),
      })
    };
  }

  // logical
  LLVMIntPredicate predicateInt;
  LLVMRealPredicate predicateReal;
  switch (op.type) {
    default: PULE_assert(false);
    case PuleIROperator_eq:
      predicateInt = LLVMIntEQ;
      predicateReal = LLVMRealOEQ;
    break;
    case PuleIROperator_ne:
      predicateInt = LLVMIntNE;
      predicateReal = LLVMRealONE;
    break;
    case PuleIROperator_lt:
      predicateInt = LLVMIntSLT;
      predicateReal = LLVMRealOLT;
    break;
    case PuleIROperator_le:
      predicateInt = LLVMIntSLE;
      predicateReal = LLVMRealOLE;
    break;
    case PuleIROperator_gt:
      predicateInt = LLVMIntSGT;
      predicateReal = LLVMRealOGT;
    break;
    case PuleIROperator_ge:
      predicateInt = LLVMIntSGE;
      predicateReal = LLVMRealOGE;
    break;
  }

  auto const returnTypeBool = (
    puleIRTypeCreate({.module = ci.module, .type = PuleIRDataType_bool,})
  );

  if (isLhsFloatingPoint) {
    return {
      module.values.create(pint::ir::Value {
        .type = returnTypeBool,
        .kind = PuleIRValueKind_dynamic,
        .value = (
          LLVMBuildFCmp(
            module.builders.at(ci.codeBlock.id),
            predicateReal,
            lhs.value,
            rhs.value,
            label.c_str()
          )
        ),
      })
    };
  }
  return {
    module.values.create(pint::ir::Value {
      .type = returnTypeBool,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildICmp(
          module.builders.at(ci.codeBlock.id),
          predicateInt,
          lhs.value,
          rhs.value,
          label.c_str()
        )
      ),
    })
  };
}

void puleIRBuildBranch(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::ir::modules.at(ci.module);
  auto & branch = ci.instr.branch;
  if (branch.isConditional) {
    LLVMBuildCondBr(
      module.builders.at(ci.codeBlock.id),
      module.values.at(branch.conditional.condition)->value,
      // TODO
      module.basicBlocks.at(branch.conditional.codeBlockTrue.id),
      module.basicBlocks.at(branch.conditional.codeBlockFalse.id)
    );
  } else {
    LLVMBuildBr(
      module.builders.at(ci.codeBlock.id),
      module.basicBlocks.at(branch.unconditional.codeBlock.id)
    );
  }
}

} // namespace pint::ir

PuleIRCast puleIRCastTypeFromDataTypes(
  PuleIRDataType const from,
  PuleIRDataType const to
) {
  bool const fromFp = puleIRDataTypeIsFloat(from);
  bool const toFp = puleIRDataTypeIsFloat(to);
  bool const fromInt = puleIRDataTypeIsInt(from);
  bool const toInt = puleIRDataTypeIsInt(to);
  bool const fromSigned = puleIRDataTypeIsSigned(from);
  bool const toSigned = puleIRDataTypeIsSigned(to);
  uint32_t const fromByteCount = puleIRDataTypeByteCount(from);
  uint32_t const toByteCount = puleIRDataTypeByteCount(to);

  PULE_assert(fromFp || fromInt);
  PULE_assert(toFp || toInt);

  // int2float/float2int
  if (fromFp && toInt) {
    return toSigned ? PuleIRCast_floatToInt : PuleIRCast_floatToUint;
  }
  if (fromInt && toFp) {
    return fromSigned ? PuleIRCast_intToFloat : PuleIRCast_uintToFloat;
  }

  // int2int extend
  if (fromInt && toInt) {
    if (fromByteCount < toByteCount) {
      return toSigned ? PuleIRCast_intExtend : PuleIRCast_uintExtend;
    }
    return PuleIRCast_intTrunc;
  }

  // float2float extend
  if (fromByteCount < toByteCount) {
    return PuleIRCast_floatExtend;
  }
  return PuleIRCast_floatTrunc;
}

PuleIRValue puleIRBuild(PuleIRBuildCreateInfo const createInfo) {
  switch (createInfo.instr.instrType) {
    default: PULE_assert(false && "TODO");
    case PuleIRInstrType_return:
      return pint::ir::puleIRBuildReturn(createInfo);
    case PuleIRInstrType_call:
      return pint::ir::puleIRBuildCall(createInfo);
    case PuleIRInstrType_stackAlloc:
      return pint::ir::puleIRBuildAllocate(createInfo);
    case PuleIRInstrType_free:
      pint::ir::puleIRBuildFree(createInfo);
      return { .id = 0 };
    case PuleIRInstrType_load:
      return pint::ir::puleIRBuildLoad(createInfo);
    case PuleIRInstrType_store:
      pint::ir::puleIRBuildStore(createInfo);
      return { .id = 0 };
    case PuleIRInstrType_operation:
      return pint::ir::puleIRBuildOperation(createInfo);
    case PuleIRInstrType_branch:
      pint::ir::puleIRBuildBranch(createInfo);
      return { .id = 0 };
    case PuleIRInstrType_trunc:
      return pint::ir::puleIRBuildTrunc(createInfo);
    case PuleIRInstrType_cast:
      return pint::ir::puleIRBuildCast(createInfo);
    case PuleIRInstrType_switch:
      pint::ir::puleIRBuildSwitch(createInfo);
      return { .id = 0 };
  }
}
