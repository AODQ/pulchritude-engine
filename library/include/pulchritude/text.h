/* auto generated file text */
#pragma once
#include "core.h"

#include "asset-font.h"
#include "gpu.h"
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleTextType_bitmap = 0,
  PuleTextType_sdf = 1,
} PuleTextType;
const uint32_t PuleTextTypeSize = 2;

// entities

// structs
struct PuleTextRenderer;
struct PuleTextRenderInfo;
struct PuleTextRender2DInfo;

typedef struct PuleTextRenderer {
  uint64_t id;
} PuleTextRenderer;
typedef struct PuleTextRenderInfo {
  size_t fontScale;
  PuleF32m44 transform;
  PuleF32v4 color;
  PuleStringView text;
} PuleTextRenderInfo;
typedef struct PuleTextRender2DInfo {
  size_t fontScale;
  PuleI32v2 position;
  PuleF32v4 color;
  PuleStringView text;
} PuleTextRender2DInfo;

// functions
PULE_exportFn PuleTextRenderer puleTextRendererCreate(PuleAssetFont font, PuleTextType type);
PULE_exportFn void puleTextRendererDestroy(PuleTextRenderer renderer);
PULE_exportFn void puleTextRender(PuleTextRenderer textRenderer, PuleGpuCommandListRecorder commandListRecorder, PuleTextRenderInfo const * textInfo, size_t textInfoCount);
PULE_exportFn void puleTextRender2D(PuleTextRenderer textRenderer, PuleGpuCommandListRecorder commandListRecorder, PuleTextRender2DInfo const * textInfo, size_t textInfoCount);

#ifdef __cplusplus
} // extern C
#endif
