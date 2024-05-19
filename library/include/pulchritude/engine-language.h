/* auto generated file engine-language */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/stream.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  bool optimize;
  bool insertEngineSymbols;
} PuleELJitEngineCreateInfo;

// enum
typedef enum {
  PuleErrorEL_none,
  PuleErrorEL_compileError,
} PuleErrorEL;

// entities
typedef struct { uint64_t id; } PuleELModule;
typedef struct { uint64_t id; } PuleELJitEngine;

// functions
PULE_exportFn PuleELModule puleELModuleCreate(PuleStreamRead stream, PuleStringView name, PuleError * error);
PULE_exportFn void puleELModuleDestroy(PuleELModule module);
PULE_exportFn PuleELJitEngine puleELJitEngineCreate(PuleELJitEngineCreateInfo ci);
PULE_exportFn void puleELJitEngineDestroy(PuleELJitEngine jitEngine);
PULE_exportFn void puleELJitEngineAddModule(PuleELJitEngine jitEngine, PuleELModule module);
PULE_exportFn void * puleELJitEngineFunctionAddress(PuleELJitEngine jitEngine, PuleStringView functionName);

#ifdef __cplusplus
} // extern C
#endif
