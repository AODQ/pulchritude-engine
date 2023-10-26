#pragma once

#if defined(PULCHRITUDE_FEATURE_TEXT)

#include <pulchritude-core/core.h>

#include <pulchritude-asset/font.h>
#include <pulchritude-gpu/commands.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t id;
} PuleTextRenderer;

typedef enum {
  PuleTextType_bitmap,
  PuleTextType_sdf,
  PuleTextTypeSize
} PuleTextType;

// TODO - should the font be copied? right now it's not
PULE_exportFn PuleTextRenderer puleTextRendererCreate(
  PuleAssetFont const font,
  PuleTextType const type
);
PULE_exportFn void puleTextRendererDestroy(PuleTextRenderer const renderer);

typedef struct PuleTextRenderInfo {
  size_t fontScale;
  PuleF32m44 transform;
  PuleF32v4 color;
  PuleStringView text;
} PuleTextRenderInfo;

PULE_exportFn void puleTextRender(
  PuleTextRenderer const textRenderer,
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleTextRenderInfo const * const textInfo,
  size_t const textInfoCount
);

typedef struct PuleTextRender2DInfo {
  size_t fontScale;
  PuleI32v2 position;
  PuleF32v4 color;
  PuleStringView text;
} PuleTextRender2DInfo;

PULE_exportFn void puleTextRender2D(
  PuleTextRenderer const textRenderer,
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleTextRender2DInfo const * const textInfo,
  size_t const textInfoCount
);

#ifdef __cplusplus
} // extern C
#endif

#endif // PULCHRITUDE_FEATURE_TEXT
