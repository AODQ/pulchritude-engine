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
#include <vector>

namespace {
LLVMContextRef contextLlvm = nullptr;
}

// -- module -------------------------------------------------------------------

PuleELModule puleELModuleCreate(
  PuleStreamRead const stream,
  PuleStringView const name,
  PuleError * const error
) {
  if (!contextLlvm) {
    contextLlvm = LLVMContextCreate();
  }
  LLVMModuleRef module = nullptr;
  char * errorStr = nullptr;
  PuleBuffer buffer = puleStreamDumpToBuffer(stream);
  puleScopeExit { puleBufferDestroy(buffer); };
  // copy the buffer excluding null byte
  LLVMMemoryBufferRef bufferLlvm = (
    LLVMCreateMemoryBufferWithMemoryRange(
      (char const *)buffer.data, buffer.byteLength-1, "", true
    )
  );
  //puleScopeExit { LLVMDisposeMemoryBuffer(bufferLlvm); };
  if (LLVMParseIRInContext(contextLlvm, bufferLlvm, &module, &errorStr)) {
    PULE_error(PuleErrorEL_compileError, "failed to parse IR: %s", errorStr);
    LLVMDisposeMessage(errorStr);
    return { .id = 0 };
  }
  LLVMSetModuleIdentifier(module, name.contents, name.len);
  return { .id = reinterpret_cast<uint64_t>(module), };
}

void puleELModuleDestroy(PuleELModule const module) {
  LLVMDisposeModule(reinterpret_cast<LLVMModuleRef>(module.id));
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
  auto & jit = *pint::ir::jitEngines.at(jitEngine);
  //for (auto & unit : jit.engineMaterializationUnits) {
  //  LLVMOrcDisposeMaterializationUnit(unit);
  //}
  LLVMOrcDisposeLLJIT(jit.jit);
  LLVMOrcDisposeThreadSafeContext(jit.context);
  pint::ir::jitEngines.destroy(jitEngine);
}

void puleELJitEngineAddModule(
  PuleELJitEngine const jitEngine,
  PuleELModule const module
) {
  auto jit = pint::ir::jitEngines.at(jitEngine);
  auto moduleRef = (LLVMModuleRef)module.id;

  // TODO check if the name exists, and if so remove old one from the
  //      execution session

  size_t moduleNameLen;
  char const * const moduleName = (
    LLVMGetModuleIdentifier(moduleRef, &moduleNameLen)
  );
  LLVMOrcJITDylibRef dylib = (
    LLVMOrcExecutionSessionCreateBareJITDylib(
      jit->executionSession,
      moduleName
    )
  );
  jit->dylib.emplace(moduleName, dylib);

  // move the LLVM module to the ORC context
  LLVMOrcThreadSafeModuleRef orcModule = (
    LLVMOrcCreateNewThreadSafeModule(moduleRef, jit->context)
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
