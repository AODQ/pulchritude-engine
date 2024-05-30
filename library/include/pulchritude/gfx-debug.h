/* auto generated file gfx-debug */
#pragma once
#include "core.h"

#include "error.h"
#include "gpu.h"
#include "math.h"
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleGfxDebugRenderType_line = 0,
  PuleGfxDebugRenderType_quad = 1,
  PuleGfxDebugRenderType_cube = 2,
  PuleGfxDebugRenderType_sphere = 3,
  PuleGfxDebugRenderType_plane = 4,
} PuleGfxDebugRenderType;
const uint32_t PuleGfxDebugRenderTypeSize = 5;

// entities

// structs
struct PuleGfxDebugRecorder;
struct PuleGfxDebugSubmitInfo;
struct PuleGfxDebugRenderLine;
struct PuleGfxDebugRenderQuad;
struct PuleGfxDebugRenderCube;
struct PuleGfxDebugRenderSphere;
struct PuleGfxDebugRenderPlane;
union PuleGfxDebugRenderParam;

typedef struct PuleGfxDebugRecorder {
  uint64_t id;
} PuleGfxDebugRecorder;
/*  Writes out the debug data to the render-pass  */
typedef struct PuleGfxDebugSubmitInfo {
  PuleGfxDebugRecorder recorder;
  PuleGpuCommandListRecorder commandListRecorder;
  /* 
    need this to call the begin/end render-pass; it's a safety measure to
    ensure a render-pass is active when drawing
   */
  PuleGpuActionRenderPassBegin renderPassBegin;
} PuleGfxDebugSubmitInfo;
typedef struct PuleGfxDebugRenderLine {
  PuleGfxDebugRenderType type;
  PuleF32v2 a;
  PuleF32v2 b;
  PuleF32v3 color;
} PuleGfxDebugRenderLine;
typedef struct PuleGfxDebugRenderQuad {
  PuleGfxDebugRenderType type;
  PuleF32v2 originCenter;
  float angle;
  PuleF32v2 dimensionsHalf;
  PuleF32v3 color;
} PuleGfxDebugRenderQuad;
typedef struct PuleGfxDebugRenderCube {
  PuleGfxDebugRenderType type;
  PuleF32v3 originCenter;
  PuleF32m33 rotationMatrix;
  PuleF32v3 dimensionsHalf;
  PuleF32v3 color;
} PuleGfxDebugRenderCube;
typedef struct PuleGfxDebugRenderSphere {
  PuleGfxDebugRenderType type;
  PuleF32v3 originCenter;
  float radius;
  PuleF32v3 color;
} PuleGfxDebugRenderSphere;
typedef struct PuleGfxDebugRenderPlane {
  PuleGfxDebugRenderType type;
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

// functions
PULE_exportFn void puleGfxDebugInitialize(PulePlatform platform);
PULE_exportFn void puleGfxDebugShutdown();
PULE_exportFn PuleGfxDebugRecorder puleGfxDebugStart(PuleF32m44 transform, PuleF32m44 view, PuleF32m44 projection);
PULE_exportFn void puleGfxDebugSubmit(PuleGfxDebugSubmitInfo submitInfo);
PULE_exportFn void puleGfxDebugRender(PuleGfxDebugRecorder debugRecorder, PuleGfxDebugRenderParam param);

#ifdef __cplusplus
} // extern C
#endif
