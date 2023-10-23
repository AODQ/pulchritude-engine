#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-math/math.h>
#include <pulchritude-platform/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn void puleGfxDebugInitialize(PulePlatform const platform);
PULE_exportFn void puleGfxDebugShutdown();

typedef struct {
  uint64_t const id;
} PuleGfxDebugRecorder;

PULE_exportFn PuleGfxDebugRecorder puleGfxDebugStart(
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleGpuActionRenderPassBegin const renderPassBegin,
  PuleF32m44 const transform
);
PULE_exportFn void puleGfxDebugEnd(
  PuleGfxDebugRecorder const recorder
);

typedef enum {
  PuleGfxDebugRenderType_line,
  PuleGfxDebugRenderType_quad,
} PuleGfxDebugRenderType;

typedef struct PuleGfxDebugRenderLine {
  PuleGfxDebugRenderType type PULE_param(PuleGfxDebugRenderType_line);
  PuleF32v2 const a;
  PuleF32v2 const b;
  PuleF32v3 const color;
} PuleGfxDebugRenderLine;

typedef struct PuleGfxDebugRenderQuad {
  PuleGfxDebugRenderType type PULE_param(PuleGfxDebugRenderType_quad);
  PuleF32v2 const originCenter;
  PuleF32v2 const dimensionsHalf;
  PuleF32v3 const color;
} PuleGfxDebugRenderQuad;

typedef union PuleGfxDebugRenderParam {
  PuleGfxDebugRenderType type;
  PuleGfxDebugRenderLine line;
  PuleGfxDebugRenderQuad quad;
} PuleGfxDebugRenderParam;

PULE_exportFn void puleGfxDebugRender(
  PuleGfxDebugRecorder const debugRecorder,
  PuleGfxDebugRenderParam const param
);

#ifdef __cplusplus
}
#endif
