#pragma once

#include <pulchritude-gfx/gfx.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-time/time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t id;
} PuleGfxFence;

typedef enum {
  PuleGfxFenceConditionFlag_all,
} PuleGfxFenceConditionFlag;

PULE_exportFn PuleGfxFence puleGfxFenceCreate(
  PuleGfxFenceConditionFlag const condition
);
PULE_exportFn void puleGfxFenceDestroy(PuleGfxFence const fence);
PULE_exportFn bool puleGfxFenceCheckSignal(
  PuleGfxFence const fence,
  PuleNanosecond const timeout
);

typedef enum {
  PuleGfxMemoryBarrierFlag_attribute = 0x1,
  PuleGfxMemoryBarrierFlag_element = 0x2,
  PuleGfxMemoryBarrierFlag_bufferUpdate = 0x4,
  PuleGfxMemoryBarrierFlag_all = 0x7,
  PuleGfxMemoryBarrierFlagEnd = 0x5
} PuleGfxMemoryBarrierFlag;

PULE_exportFn void puleGfxMemoryBarrier(PuleGfxMemoryBarrierFlag const barrier);

#ifdef __cplusplus
} // extern C
#endif
