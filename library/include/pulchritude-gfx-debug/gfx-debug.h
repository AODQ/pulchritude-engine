#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn void puleGfxDebugInitialize();
PULE_exportFn void puleGfxShutdown();

PULE_exportFn void puleGfxDebugRenderLine(
  PuleF32v3 const originStart,
  PuleF32v3 const originEnd,
  PuleF32v3 const color
);

PULE_exportFn void puleGfxDebugRenderRect(
  PuleF32v3 const originUl,
  PuleF32v3 const originLr,
  PuleF32v3 const color,
  bool const outline
);

PULE_exportFn void puleGfxDebugRenderCircle(
  PuleF32v3 const origin,
  float const radius,
  PuleF32v3 const color
);

PULE_exportFn void puleGfxDebugRenderPoint(
  PuleF32v3 const origin,
  float const pointWidth,
  PuleF32v3 const color
);

PULE_exportFn void puleGfxDebugFrameStart();

PULE_exportFn void puleGfxDebugRender(
  PuleGfxFramebuffer const framebuffer,
  PuleF32m44 const transform
);

#ifdef __cplusplus
}
#endif
