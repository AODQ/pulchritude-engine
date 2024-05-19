/* auto generated file gfx-debug */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/gpu.h>
#include <pulchritude/math.h>
#include <pulchritude/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  uint64_t id;
} PuleGfxDebugRecorder;
/*  Writes out the debug data to the render-pass  */
typedef struct {
  PuleGfxDebugRecorder recorder;
  PuleGpuCommandListRecorder commandListRecorder;
  /* 
    need this to call the begin/end render-pass; it's a safety measure to
    ensure a render-pass is active when drawing
   */
  PuleGpuActionRenderPassBegin renderPassBegin;
} PuleGfxDebugSubmitInfo;
typedef struct {
  PuleGfxDebugRenderType type;
  PuleF32v2 a;
  PuleF32v2 b;
  PuleF32v3 color;
} PuleGfxDebugRenderLine;
typedef struct {
  PuleGfxDebugRenderType type;
  PuleF32v2 originCenter;
  float angle;
  PuleF32v2 dimensionsHalf;
  PuleF32v3 color;
} PuleGfxDebugRenderQuad;
typedef struct {
  PuleGfxDebugRenderType type;
  PuleF32v3 originCenter;
  PuleF32m33 rotationMatrix;
  PuleF32v3 dimensionsHalf;
  PuleF32v3 color;
} PuleGfxDebugRenderCube;
typedef struct {
  PuleGfxDebugRenderType type;
  PuleF32v3 originCenter;
  float radius;
  PuleF32v3 color;
} PuleGfxDebugRenderSphere;
typedef struct {
  PuleGfxDebugRenderType type;
  PuleF32v3 originCenter;
  PuleF32m33 rotationMatrix;
  PuleF32v3 color;
} PuleGfxDebugRenderPlane;
typedef union {
  PuleGfxDebugRenderType type;
  PuleGfxDebugRenderLine line;
  PuleGfxDebugRenderQuad quad;
  PuleGfxDebugRenderCube cube;
  PuleGfxDebugRenderSphere sphere;
  PuleGfxDebugRenderPlane plane;
} PuleGfxDebugRenderParam;

// enum
typedef enum {
  PuleGfxDebugRenderType_line,
  PuleGfxDebugRenderType_quad,
  PuleGfxDebugRenderType_cube,
  PuleGfxDebugRenderType_sphere,
  PuleGfxDebugRenderType_plane,
} PuleGfxDebugRenderType;

// entities

// functions
PULE_exportFn void puleGfxDebugInitialize(PulePlatform platform);
PULE_exportFn void puleGfxDebugShutdown();
PULE_exportFn PuleGfxDebugRecorder puleGfxDebugStart(PuleF32m44 transform, PuleF32m44 view, PuleF32m44 projection);
PULE_exportFn void puleGfxDebugSubmit(PuleGfxDebugSubmitInfo submitInfo);
PULE_exportFn void puleGfxDebugRender(PuleGfxDebugRecorder debugRecorder, PuleGfxDebugRenderParam param);

#ifdef __cplusplus
} // extern C
#endif
