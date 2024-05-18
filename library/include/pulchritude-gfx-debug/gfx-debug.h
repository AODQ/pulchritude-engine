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
  PuleF32m44 const transform,
  PuleF32m44 const view,
  PuleF32m44 const projection
);

// Writes out the debug data to the render-pass
typedef struct {
  PuleGfxDebugRecorder recorder;
  PuleGpuCommandListRecorder commandListRecorder;

  // need this to call the begin/end render-pass;
  //   it's a safety measure to ensure a render-pass is active when drawing
  PuleGpuActionRenderPassBegin renderPassBegin;
} PuleGfxDebugSubmitInfo;
PULE_exportFn void puleGfxDebugSubmit(
  PuleGfxDebugSubmitInfo const submitInfo
);

typedef enum {
  PuleGfxDebugRenderType_line,
  PuleGfxDebugRenderType_quad,
  PuleGfxDebugRenderType_cube,
  PuleGfxDebugRenderType_sphere,
  PuleGfxDebugRenderType_plane,
} PuleGfxDebugRenderType;

typedef struct PuleGfxDebugRenderLine {
  PuleGfxDebugRenderType type PULE_param(PuleGfxDebugRenderType_line);
  PuleF32v2 a;
  PuleF32v2 b;
  PuleF32v3 color;
} PuleGfxDebugRenderLine;

typedef struct PuleGfxDebugRenderQuad {
  PuleGfxDebugRenderType type PULE_param(PuleGfxDebugRenderType_quad);
  PuleF32v2 originCenter;
  float angle;
  PuleF32v2 dimensionsHalf;
  PuleF32v3 color;
} PuleGfxDebugRenderQuad;

typedef struct PuleGfxDebugRenderCube {
  PuleGfxDebugRenderType type PULE_param(PuleGfxDebugRenderType_cube);
  PuleF32v3 originCenter;
  PuleF32m33 rotationMatrix;
  PuleF32v3 dimensionsHalf;
  PuleF32v3 color;
} PuleGfxDebugRenderCube;

typedef struct PuleGfxDebugRenderSphere {
  PuleGfxDebugRenderType type PULE_param(PuleGfxDebugRenderType_sphere);
  PuleF32v3 originCenter;
  float radius;
  PuleF32v3 color;
} PuleGfxDebugRenderSphere;

typedef struct PuleGfxDebugRenderPlane {
  PuleGfxDebugRenderType type PULE_param(PuleGfxDebugRenderType_plane);
  PuleF32v3 originCenter;
  PuleF32m33 rotationMatrix;
  PuleF32v3 color;
} PuleGfxDebugRenderPlane;

typedef union PuleGfxDebugRenderParam {
  PuleGfxDebugRenderType type;
  PuleGfxDebugRenderLine line;
  PuleGfxDebugRenderQuad quad;
  PuleGfxDebugRenderCube cube;
  PuleGfxDebugRenderSphere sphere;
  PuleGfxDebugRenderPlane plane;
} PuleGfxDebugRenderParam;

PULE_exportFn void puleGfxDebugRender(
  PuleGfxDebugRecorder const debugRecorder,
  PuleGfxDebugRenderParam const param
);

#ifdef __cplusplus
}
#endif
