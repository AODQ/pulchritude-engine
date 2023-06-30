#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-math/math.h>
#include <pulchritude-platform/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn void puleGfxDebugInitialize(PulePlatform const platform);
PULE_exportFn void puleGfxDebugShutdown();

PULE_exportFn void puleGfxDebugRenderLine(
  PuleF32v3 const originStart,
  PuleF32v3 const originEnd,
  PuleF32v3 const color
);

PULE_exportFn void puleGfxDebugRenderRectOutline(
  PuleF32v3 const originCenter,
  PuleF32v2 const dimensions,
  PuleF32v3 const color
);

PULE_exportFn void puleGfxDebugRenderCircle(
  PuleF32v3 const origin,
  float const radius,
  PuleF32v3 const colorSurface,
  PuleF32v3 const colorInner
);

PULE_exportFn void puleGfxDebugRenderPoint(
  PuleF32v3 const origin,
  float const pointWidth,
  PuleF32v3 const color
);

PULE_exportFn void puleGfxDebugFrameStart();

PULE_exportFn void puleGfxDebugRender(
  PuleGpuFramebuffer const framebuffer,
  PuleF32m44 const transform
);

#ifdef __cplusplus
}
#endif
