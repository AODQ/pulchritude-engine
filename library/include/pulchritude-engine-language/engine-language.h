#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-error/error.h>
#include <pulchritude-stream/stream.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorEL_none,
  PuleErrorEL_compileError,
} PuleErrorEL;

// -- module -------------------------------------------------------------------

typedef struct { uint64_t id; } PuleELModule;

PULE_exportFn PuleELModule puleELModuleCreate(
  PuleStreamRead const stream,
  PuleStringView const name,
  PuleError * const error
);
PULE_exportFn void puleELModuleDestroy(PuleELModule const module);

// -- jit engine ---------------------------------------------------------------

typedef struct { uint64_t id; } PuleELJitEngine;

typedef struct PuleELJitEngineCreateInfo {
  bool optimize PULE_param(true);
  bool insertEngineSymbols PULE_param(true);
} PuleELJitEngineCreateInfo;
PULE_exportFn PuleELJitEngine puleELJitEngineCreate(
  PuleELJitEngineCreateInfo const ci
);
PULE_exportFn void puleELJitEngineDestroy(PuleELJitEngine const jitEngine);

PULE_exportFn void puleELJitEngineAddModule(
  PuleELJitEngine const jitEngine,
  PuleELModule const module
);

PULE_exportFn void * puleELJitEngineFunctionAddress(
  PuleELJitEngine const jitEngine,
  PuleStringView const functionName
);

#ifdef __cplusplus
} // extern "C"
#endif
