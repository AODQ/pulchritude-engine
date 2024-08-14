/* auto generated file mt */
#pragma once
#include "core.h"

#include "error.h"
#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities
typedef struct PuleMtThread { uint64_t id; } PuleMtThread;
typedef struct PuleMtMutex { uint64_t id; } PuleMtMutex;

// structs
struct PuleMtThreadCreateInfo;

typedef struct PuleMtThreadCreateInfo {
  bool detached;
  void *(* fn)(void *);
  void * userData;
} PuleMtThreadCreateInfo;

// functions
PULE_exportFn PuleMtThread puleMtThreadCreate(PuleMtThreadCreateInfo ci);
PULE_exportFn void * puleMtThreadJoin(PuleMtThread t);
PULE_exportFn void puleMtThreadDetach(PuleMtThread t);
PULE_exportFn void puleMtThreadDestroy(PuleMtThread t);
PULE_exportFn void puleMtThreadExit(void * ret);
PULE_exportFn void puleMtThreadYield();
PULE_exportFn PuleMtMutex puleMtMutexCreate();
PULE_exportFn void puleMtMutexLock(PuleMtMutex m);
PULE_exportFn void puleMtMutexUnlock(PuleMtMutex m);
PULE_exportFn bool puleMtMutexTryLock(PuleMtMutex m);

#ifdef __cplusplus
} // extern C
#endif
