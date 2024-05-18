#pragma once

#include <pulchritude-gpu/gpu.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-time/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t id;
} PuleGpuSemaphore;

PULE_exportFn PuleGpuSemaphore puleGpuSemaphoreCreate(
  PuleStringView const label
);
PULE_exportFn void puleGpuSemaphoreDestroy(PuleGpuSemaphore const semaphore);

typedef struct {
  uint64_t id;
} PuleGpuFence;

typedef enum {
  PuleGpuFenceConditionFlag_all,
} PuleGpuFenceConditionFlag;

typedef enum {
  PuleGpuSignalTime_forever = 2000000000,
} PuleGpuSignalTime;

PULE_exportFn PuleGpuFence puleGpuFenceCreate(PuleStringView const label);
PULE_exportFn void puleGpuFenceDestroy(PuleGpuFence const fence);
PULE_exportFn bool puleGpuFenceWaitSignal(
  PuleGpuFence const fence,
  PuleNanosecond const timeout
);
PULE_exportFn void puleGpuFenceReset(PuleGpuFence const fence);

PULE_exportFn PuleGpuSemaphore puleGpuFrameStart();
// same value returned from puleGpuFrameStart
PULE_exportFn PuleGpuSemaphore puleGpuSwapchainAvailableSemaphore();
// call before pulePlatformSwapFramebuffer
PULE_exportFn void puleGpuFrameEnd(
  size_t const waitSemaphoreCount,
  PuleGpuSemaphore const * const waitSemaphores
);


#ifdef __cplusplus
} // extern C
#endif
