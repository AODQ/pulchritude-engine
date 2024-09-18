/* auto generated file engine-language */
#pragma once
#include "core.h"

#include "error.h"
#include "string.h"
#include "time.h"

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
typedef struct PuleELEngine { uint64_t id; } PuleELEngine;
typedef struct PuleELFence { uint64_t id; } PuleELFence;
typedef struct PuleELQueue { uint64_t id; } PuleELQueue;

// structs
struct PuleELModuleCreateInfo;
struct PuleELEngineCreateInfo;

typedef struct PuleELModuleCreateInfo {
  PuleStringView moduleIR;
  PuleStringView name;
  bool optimize PULE_defaultField(false);
  bool debug PULE_defaultField(false);
  PuleError * error;
} PuleELModuleCreateInfo;
typedef struct PuleELEngineCreateInfo {
  bool insertEngineSymbols PULE_defaultField(false);
} PuleELEngineCreateInfo;

// functions
PULE_exportFn PuleELModule puleELModuleCreate(PuleELModuleCreateInfo ci);
PULE_exportFn void puleELModuleDestroy(PuleELModule module);
PULE_exportFn PuleELEngine puleELEngineCreate(PuleELEngineCreateInfo ci);
PULE_exportFn void puleELEngineDestroy(PuleELEngine engine);
PULE_exportFn void puleELEngineAddModule(PuleELEngine engine, PuleELModule module);
PULE_exportFn bool puleELFenceWait(PuleELFence fence, PuleMicrosecond timeout);
PULE_exportFn void puleELFenceDestroy(PuleELFence fence);
PULE_exportFn PuleELQueue puleELQueueCreate(PuleELEngine engine);
PULE_exportFn void puleELQueueDestroy(PuleELQueue queue);
PULE_exportFn size_t puleELQueueStackLength(PuleELQueue queue);
PULE_exportFn void puleELQueueStackPush(PuleELQueue queue, uint64_t value);
PULE_exportFn uint64_t puleELQueueStackPop(PuleELQueue queue);
PULE_exportFn void puleELQueueSubmit(PuleELQueue queue, PuleStringView functionName);

#ifdef __cplusplus
} // extern C
#endif
