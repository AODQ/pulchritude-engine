#include <pulchritude-compiler/compiler.h>

// this is a simple LLVM IR wrapper, which is meant to simplify the API
//   to play with LLVM, and not to be used in any actual code.

#include <pulchritude-error/error.h>
#include <pulchritude-time/time.h>

#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/LLJIT.h>
#include <llvm-c/Orc.h>

#include <atomic>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace pint {

struct Type {
  LLVMTypeRef type;
  PuleIRDataTypeInfo typeInfo;
};

struct Value {
  PuleIRType type;
  PuleIRValueKind kind;
  LLVMValueRef value;
};

struct ModuleInfo {
  LLVMContextRef context;
  LLVMModuleRef module;
  std::string moduleName; // seems LLVM C-API doesn't interface module::getName
  pule::ResourceContainer<LLVMBuilderRef> builders;
  pule::ResourceContainer<Type> types;
  pule::ResourceContainer<Value> values;
  std::unordered_map<PuleIRDataType, PuleIRType> genericTypes;
};


pule::ResourceContainer<pint::ModuleInfo, PuleIRModule> modules;
};

extern "C" {

// -- modules ------------------------------------------------------------------

PuleIRModule puleIRModuleCreate(PuleIRModuleCreateInfo const ci) {
  pint::ModuleInfo moduleInfo;

  moduleInfo.context = LLVMContextCreate();
  moduleInfo.module = (
    LLVMModuleCreateWithNameInContext(ci.label.contents, moduleInfo.context)
  );
  moduleInfo.moduleName = std::string(ci.label.contents);


  PuleIRModule puModule = pint::modules.create(moduleInfo);
  auto & module = *pint::modules.at(puModule);

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

  puleLog("creating ir module");

  // build all the generic data types
  for (
    auto & it : {
      std::pair<
        LLVMTypeRef (*)(LLVMContextRef),
        std::pair<PuleIRDataType, char const *>
      >
    { &LLVMInt8TypeInContext,     { PuleIRDataType_u8, "PuleIRDataType_u8", }, },
    { &LLVMInt16TypeInContext,    { PuleIRDataType_u16, "PuleIRDataType_u16 ", }, },
    { &LLVMInt32TypeInContext,    { PuleIRDataType_u32, "PuleIRDataType_u32 ", }, },
    { &LLVMInt64TypeInContext,    { PuleIRDataType_u64, "PuleIRDataType_u64 ", }, },
    { &LLVMFloatTypeInContext,    { PuleIRDataType_f32, "PuleIRDataType_f32 ", }, },
    { &LLVMDoubleTypeInContext,   { PuleIRDataType_f64, "PuleIRDataType_f64 ", }, },
    { &LLVMInt1TypeInContext,     { PuleIRDataType_bool, "PuleIRDataType_bool ", }, },
    { &LLVMVoidTypeInContext,     { PuleIRDataType_void, "PuleIRDataType_void ", }, },
  }) {
    genericType[it.second.first] = {
      type.create(
        pint::Type {
          .type = it.first(ctx),
          .typeInfo = PuleIRDataTypeInfo {
            .module = puModule, .type = it.second.first, .genericInfo = {},
          },
        }
      )
    };
  }

  genericType[PuleIRDataType_ptr] = {
    type.create(
      pint::Type {
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
  auto moduleInfo = pint::modules.at(module);
  for (auto & builder : moduleInfo->builders) {
    LLVMDisposeBuilder(builder.second);
  }
  LLVMDisposeModule(moduleInfo->module);
  LLVMContextDispose(moduleInfo->context);
}

void puleIRModuleDump(PuleIRModule const module) {
  auto moduleInfo = pint::modules.at(module);
  auto str = LLVMPrintModuleToString(moduleInfo->module);
  puleLog("%s", str);
  LLVMDisposeMessage(str);
}

// -- jit engine ---------------------------------------------------------------

namespace pint {

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


pule::ResourceContainer<pint::JITEngine, PuleIRJitEngine> jitEngines;
bool jitInitialized = false;

} // namespace


PuleIRJitEngine puleIRJitEngineCreate(PuleIRJitEngineCreateInfo const ci) {
  if (!pint::jitInitialized) {
    pint::jitInitialized = true;
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

  pint::JITEngine jitEngine = {
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
            .GenericFlags = LLVMJITSymbolGenericFlagsCallable, \
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

  return pint::jitEngines.create(jitEngine);
}
void puleIRJitEngineDestroy(PuleIRJitEngine const jitEngine) {
  // TODO
  (void)jitEngine;
}

void puleIRJitEngineAddModule(
  PuleIRJitEngine const jitEngine,
  PuleIRModule const module
) {
  auto jit = pint::jitEngines.at(jitEngine);
  auto moduleInfo = pint::modules.at(module);

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

typedef struct {
  std::atomic<bool> found;
  uint64_t address;
} pulintIRJITLookupContext;

static void pulintIRJitLookupCallback(
  LLVMErrorRef error,
  LLVMOrcCSymbolMapPairs result, size_t numPairs,
  void * ctx
) {
  PULE_assert(numPairs != 0 && "couldn't find the symbol");
  PULE_assert(numPairs == 1 && "only one lookup at a time");
  auto & context = *reinterpret_cast<pulintIRJITLookupContext *>(ctx);
  context.address = result[0].Sym.Address;
  context.found = true;
  (void)error;
}


void * puleIRJitEngineFunctionAddress(
  PuleIRJitEngine const jitEngine,
  PuleStringView const functionName
) {
  auto const & jit = pint::jitEngines.at(jitEngine);
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

  auto lookupCtx = pulintIRJITLookupContext {};
  LLVMOrcExecutionSessionLookup(
    jit->executionSession,
    LLVMOrcLookupKindDLSym,
    dylibs.data(), dylibs.size(),
    &symbol, 1,
    pulintIRJitLookupCallback, &lookupCtx
  );

  LLVMOrcReleaseSymbolStringPoolEntry(symbolPoolRef);

  while (true) {
    if (lookupCtx.found) { break; }
    puleSleepMicrosecond({.valueMicro = 1'000});
  }

  return (void *)lookupCtx.address;
}

// -- types --------------------------------------------------------------------

PuleIRType puleIRTypeCreate(PuleIRDataTypeInfo const createInfo) {
  auto moduleInfo = pint::modules.at(createInfo.module);
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
      std::vector<LLVMTypeRef> paramTypes;
      for (size_t i = 0; i < createInfo.fnInfo.parameterCount; ++ i) {
        paramTypes.emplace_back(
          moduleInfo->types.at(createInfo.fnInfo.parameterTypes[i].id)->type
        );
      }
      return {
        moduleInfo->types.create({
          LLVMFunctionType(
            moduleInfo->types.at(createInfo.fnInfo.returnType.id)->type,
            paramTypes.data(),
            paramTypes.size(),
            createInfo.fnInfo.variadic
          ),
          createInfo,
        })
      };
    }
    case PuleIRDataType_struct: {
      LLVMTypeRef structType = (
        LLVMStructCreateNamed(
          moduleInfo->context,
          createInfo.structInfo.label.contents
        )
      );
      std::vector<LLVMTypeRef> memberTypes;
      for (size_t i = 0; i < createInfo.structInfo.memberCount; ++ i) {
        memberTypes.emplace_back(
          moduleInfo->types.at(createInfo.structInfo.memberTypes[i].id)->type
        );
      }
      LLVMStructSetBody(
        structType,
        memberTypes.data(),
        memberTypes.size(),
        createInfo.structInfo.packed
      );
      return { moduleInfo->types.create({structType, createInfo,}) };
    }
    case PuleIRDataType_array: {
      return {
        moduleInfo->types.create({
          LLVMArrayType(
            moduleInfo->types.at(createInfo.arrayInfo.elementType.id)->type,
            createInfo.arrayInfo.elementCount
          ),
          createInfo,
        })
      };
    }
  }
}

PuleIRDataTypeInfo puleIRTypeInfo(
  PuleIRModule const puModule,
  PuleIRType const type
) {
  return pint::modules.at(puModule)->types.at(type.id)->typeInfo;
}

// -- values -------------------------------------------------------------------

PuleIRType puleIRValueType(
  PuleIRModule const puModule,
  PuleIRValue const puValue
) {
  pint::Value const & value = (
    *pint::modules.at(puModule)->values.at(puValue.id)
  );
  return value.type;
}

PuleIRValueKind puleIRValueKind(
  PuleIRModule const puModule,
  PuleIRValue const puValue
) {
  pint::Value const & value = (
    *pint::modules.at(puModule)->values.at(puValue.id)
  );
  return value.kind;
}

PuleIRValue puleIRValueVoid() { return { .id = 0 }; }

// -- code blocks --------------------------------------------------------------

PuleIRCodeBlock puleIRCodeBlockCreate(
  PuleIRCodeBlockCreateInfo const createInfo
) {
  auto moduleInfo = pint::modules.at(createInfo.module);
  LLVMBuilderRef const builder = (
    LLVMCreateBuilderInContext(moduleInfo->context)
  );
  LLVMBasicBlockRef const block = (
    LLVMAppendBasicBlockInContext(
      moduleInfo->context,
      moduleInfo->values.at(createInfo.context.id)->value,
      createInfo.label.contents
    )
  );
  LLVMPositionBuilderAtEnd(builder, block);
  return {
    .id = reinterpret_cast<uint64_t>(moduleInfo->builders.create(builder)), };
}

void puleIRCodeBlockTerminate(PuleIRCodeBlockTerminateCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module);
  LLVMBuilderRef builder = module.builders.at(ci.codeBlock.id);
  switch (ci.type) {
    case PuleIRCodeBlockTerminateType_ret:
      if (ci.ret.value.id == 0) {
        LLVMBuildRetVoid(builder);
      } else {
        LLVMBuildRet(builder, module.values.at(ci.ret.value.id)->value);
      }
    break;
  }
}

// -- value creation -----------------------------------------------------------

PuleIRValue puleIRValueConstCreate(
  PuleIRValueConstCreateInfo const createInfo
) {
  auto & module = *pint::modules.at(createInfo.module);
  auto & type = *module.types.at(createInfo.type.id);
  PuleIRDataType puDataType = (
    puleIRTypeInfo(createInfo.module, createInfo.type).type
  );
  if (createInfo.kind == PuleIRValueKind_function) {
    PULE_assert(puDataType == PuleIRDataType_function);
    return {
      module.values.create({
        .type = createInfo.type,
        .kind = PuleIRValueKind_function,
        .value = (
          LLVMAddFunction(
            module.module,
            createInfo.label.contents,
            type.type
          )
        ),
      })
    };
  }
  switch (createInfo.kind) {
    default: PULE_assert(false && "TODO");
    case PuleIRValueKind_constString: {
      PULE_assert(puDataType == PuleIRDataType_ptr);
      LLVMTypeRef stringType = (
        LLVMArrayType(
          module.types.at(module.genericTypes.at(PuleIRDataType_u8).id)->type,
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
    }
    case PuleIRValueKind_constInteger: {
      switch (puDataType) {
        default:
          PULE_assert(false && "can not create const value of this type");
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
                  createInfo.constInteger.u64,
                  createInfo.constInteger.isSigned
                )
              )
            })
          };
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
        }
      } // switch (createInfo.type)
    }
  } // switch (createInfo.kind)

  PULE_assert(false);
  return { .id = 0 };
}

} // extern "C"

// -- code block build ---------------------------------------------------------

namespace pint {

PuleIRValue puleIRBuildReturn(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module);
  if (ci.ret.value.id == 0) {
    LLVMBuildRetVoid(module.builders.at(ci.codeBlock.id));
    return puleIRValueVoid();
  }
  return {
    module.values.create(pint::Value {
      .type = puleIRValueType(ci.module, ci.ret.value),
      .kind = puleIRValueKind(ci.module, ci.ret.value),
      .value = (
        LLVMBuildRet(
          module.builders.at(ci.codeBlock.id),
          module.values.at(ci.ret.value.id)->value
        )
      ),
    })
  };
}
PuleIRValue puleIRBuildCall(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module);
  // resulting type is the return type of the function
  PuleIRType const returnType = (
    puleIRTypeInfo(
      ci.module, ci.call.functionType
    ).fnInfo.returnType
  );
  std::vector<LLVMValueRef> arguments;
  for (size_t i = 0; i < ci.call.argumentCount; ++ i) {
    arguments.emplace_back(module.values.at(ci.call.arguments[i].id)->value);
  }
  return {
    module.values.create(pint::Value {
      .type = returnType,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildCall2(
          module.builders.at(ci.codeBlock.id),
          module.types.at(ci.call.functionType.id)->type,
          module.values.at(ci.call.function.id)->value,
          arguments.data(),
          arguments.size(),
          ci.call.label.contents
        )
      ),
    })
  };
}

PuleIRValue puleIRBuildAllocate(
  PuleIRBuildCreateInfo const ci
) {
  auto & module = *pint::modules.at(ci.module);
  auto & valueSizeInfo = *module.values.at(ci.allocate.valueSize.id);
  if (
    valueSizeInfo.kind != PuleIRValueKind_constInteger
    && valueSizeInfo.kind != PuleIRValueKind_dynamic
  ) {
    PULE_assert(false && "can not allocate with count of this type");
  }
  return {
    module.values.create(pint::Value {
      .type = puleIRTypeCreate({ci.module, PuleIRDataType_ptr, {}}),
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildAlloca(
          module.builders.at(ci.codeBlock.id),
          module.types.at(ci.allocate.type.id)->type,
          ci.allocate.label.contents
        )
      ),
    })
  };
}

void puleIRBuildFree(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module);
  LLVMBuildFree(
    module.builders.at(ci.codeBlock.id),
    module.values.at(ci.free.value.id)->value
  );
}

PuleIRValue puleIRBuildLoad(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module);
  return {
    module.values.create(pint::Value {
      .type = ci.load.type,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildLoad2(
          module.builders.at(ci.codeBlock.id),
          module.types.at(ci.load.type.id)->type,
          module.values.at(ci.load.value.id)->value,
          ci.load.label.contents
        )
      ),
    })
  };
}

void puleIRBuildStore(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module);
  LLVMBuildStore(
    module.builders.at(ci.codeBlock.id),
    module.values.at(ci.store.value.id)->value,
    module.values.at(ci.store.dst.id)->value
  );
}

PuleIRValue puleIRBuildOperation(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module);

  bool const isBinary = (
       ci.op.type != PuleIROperator_neg
    && ci.op.type != PuleIROperator_not
  );

  pint::Value const & lhs = *module.values.at(ci.op.unary.lhs.id);
  pint::Type const & lhsType = *module.types.at(lhs.type.id);

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

  PULE_assert(isLhsFloatingPoint && ci.op.floatingPoint);

  pint::Value const & rhs = *module.values.at(ci.op.binary.lhs.id);

  static const std::unordered_map<
    PuleIROperator,
    std::vector<
      LLVMValueRef (*)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, char const *)
    >
  > operatorToBinaryIntFn = {
    { PuleIROperator_add, { LLVMBuildAdd,  LLVMBuildAdd},  },
    { PuleIROperator_sub, { LLVMBuildSub,  LLVMBuildSub},  },
    { PuleIROperator_mul, { LLVMBuildMul,  LLVMBuildMul},  },
    { PuleIROperator_div, { LLVMBuildUDiv, LLVMBuildSDiv}, },
    { PuleIROperator_mod, { LLVMBuildSRem, LLVMBuildSRem}, },
    { PuleIROperator_and, { LLVMBuildAnd,  LLVMBuildAnd},  },
    { PuleIROperator_or,  { LLVMBuildOr,   LLVMBuildOr},   },
    { PuleIROperator_xor, { LLVMBuildXor,  LLVMBuildXor},  },
    { PuleIROperator_shl, { LLVMBuildShl,  LLVMBuildShl},  },
    { PuleIROperator_shr, { LLVMBuildLShr, LLVMBuildLShr}, },
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

  bool const isMathematic = (
       ci.op.type == PuleIROperator_add
    || ci.op.type == PuleIROperator_sub
    || ci.op.type == PuleIROperator_mul
    || ci.op.type == PuleIROperator_div
    || ci.op.type == PuleIROperator_mod
    || ci.op.type == PuleIROperator_and
    || ci.op.type == PuleIROperator_or
    || ci.op.type == PuleIROperator_xor
    || ci.op.type == PuleIROperator_shl
    || ci.op.type == PuleIROperator_shr
  );

  if (isMathematic && !ci.op.floatingPoint) {
    return {
      module.values.create(pint::Value {
        .type = lhs.type, // TODO this is wrong, needs to be max
        .kind = PuleIRValueKind_dynamic,
        .value = (
          operatorToBinaryIntFn.at(ci.op.type)[(int32_t)isLhsSigned](
            module.builders.at(ci.codeBlock.id),
            lhs.value,
            rhs.value,
            ci.op.label.contents
          )
        ),
      })
    };
  } else if (isMathematic && ci.op.floatingPoint) {
    PULE_assert(operatorToBinaryFloatFn.contains(ci.op.type));
    return {
      module.values.create(pint::Value {
        .type = lhs.type, // TODO this is wrong, needs to be max
        .kind = PuleIRValueKind_dynamic,
        .value = (
          operatorToBinaryFloatFn.at(ci.op.type)(
            module.builders.at(ci.codeBlock.id),
            lhs.value,
            rhs.value,
            ci.op.label.contents
          )
        ),
      })
    };
  }

  // logical
  LLVMIntPredicate predicateInt;
  LLVMRealPredicate predicateReal;
  switch (ci.op.type) {
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
      module.values.create(pint::Value {
        .type = returnTypeBool,
        .kind = PuleIRValueKind_dynamic,
        .value = (
          LLVMBuildFCmp(
            module.builders.at(ci.codeBlock.id),
            predicateReal,
            lhs.value,
            rhs.value,
            ci.op.label.contents
          )
        ),
      })
    };
  }
  return {
    module.values.create(pint::Value {
      .type = returnTypeBool,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildICmp(
          module.builders.at(ci.codeBlock.id),
          predicateInt,
          lhs.value,
          rhs.value,
          ci.op.label.contents
        )
      ),
    })
  };
}

} // namespace pint

extern "C" {

PuleIRValue puleIRBuild(PuleIRBuildCreateInfo const createInfo) {
  switch (createInfo.instrType) {
    case PuleIRInstrType_return:
      return pint::puleIRBuildReturn(createInfo);
    case PuleIRInstrType_call:
      return pint::puleIRBuildCall(createInfo);
    case PuleIRInstrType_allocate:
      return pint::puleIRBuildAllocate(createInfo);
    case PuleIRInstrType_free:
      pint::puleIRBuildFree(createInfo);
      return { .id = 0 };
    case PuleIRInstrType_load:
      return pint::puleIRBuildLoad(createInfo);
    case PuleIRInstrType_store:
      pint::puleIRBuildStore(createInfo);
      return { .id = 0 };
    case PuleIRInstrType_operation:
      return pint::puleIRBuildOperation(createInfo);
  }
}

} // extern "C"
