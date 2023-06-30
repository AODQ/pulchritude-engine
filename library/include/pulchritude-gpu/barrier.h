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
} PuleGpuFence;

typedef enum {
  PuleGpuFenceConditionFlag_all,
} PuleGpuFenceConditionFlag;

PULE_exportFn PuleGpuFence puleGpuFenceCreate(
  PuleGpuFenceConditionFlag const condition
);
PULE_exportFn void puleGpuFenceDestroy(PuleGpuFence const fence);
PULE_exportFn bool puleGpuFenceCheckSignal(
  PuleGpuFence const fence,
  PuleNanosecond const timeout
);

typedef enum {
  PuleGpuMemoryBarrierFlag_attribute = 0x1,
  PuleGpuMemoryBarrierFlag_element = 0x2,
  PuleGpuMemoryBarrierFlag_bufferUpdate = 0x4,
  PuleGpuMemoryBarrierFlag_all = 0x7,
  PuleGpuMemoryBarrierFlagEnd = 0x5
} PuleGpuMemoryBarrierFlag;

PULE_exportFn void puleGpuMemoryBarrier(PuleGpuMemoryBarrierFlag const barrier);

#ifdef __cplusplus
} // extern C
#endif
