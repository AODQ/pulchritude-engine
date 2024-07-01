/* auto generated file engine-language */
#pragma once
#include "core.h"

#include "error.h"
#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorEL_none = 0,
  PuleErrorEL_compileError = 1,
} PuleErrorEL;
const uint32_t PuleErrorELSize = 2;

// entities
typedef struct PuleELModule { uint64_t id; } PuleELModule;
typedef struct PuleELJitEngine { uint64_t id; } PuleELJitEngine;

// structs
struct PuleELJitEngineCreateInfo;

typedef struct PuleELJitEngineCreateInfo {
  bool optimize PULE_defaultField(false);
  bool insertEngineSymbols PULE_defaultField(false);
} PuleELJitEngineCreateInfo;

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
