/* auto generated file text */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/asset-font.h>
#include <pulchritude/gpu.h>
#include <pulchritude/math.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  uint64_t id;
} PuleTextRenderer;
typedef struct {
  size_t fontScale;
  PuleF32m44 transform;
  PuleF32v4 color;
  PuleStringView text;
} PuleTextRenderInfo;
typedef struct {
  size_t fontScale;
  PuleI32v2 position;
  PuleF32v4 color;
  PuleStringView text;
} PuleTextRender2DInfo;

// enum
typedef enum {
  PuleTextType_bitmap,
  PuleTextType_sdf,
} PuleTextType;

// entities

// functions
PULE_exportFn PuleTextRenderer puleTextRendererCreate(PuleAssetFont font, PuleTextType type);
PULE_exportFn void puleTextRendererDestroy(PuleTextRenderer renderer);
PULE_exportFn void puleTextRender(PuleTextRenderer textRenderer, PuleGpuCommandListRecorder commandListRecorder, PuleTextRenderInfo const * textInfo, size_t textInfoCount);
PULE_exportFn void puleTextRender2D(PuleTextRenderer textRenderer, PuleGpuCommandListRecorder commandListRecorder, PuleTextRender2DInfo const * textInfo, size_t textInfoCount);

#ifdef __cplusplus
} // extern C
#endif
