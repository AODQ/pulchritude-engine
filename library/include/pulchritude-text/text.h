#pragma once

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

#ifdef __cplusplus
} // extern C
#endif
