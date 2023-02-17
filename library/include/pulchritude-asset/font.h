#pragma once

#include <pulchritude-array/array.h>
#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-math/math.h>
#include <pulchritude-string/string.h>

// simple font library rendering text into textures

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleAssetFont;

typedef enum {
  PuleErrorAssetFont_none,
  PuleErrorAssetFont_decode,
} PuleErrorAssetFont;

PULE_exportFn PuleAssetFont puleAssetFontLoad(
  PuleBufferView const fontSource,
  PuleError * const error
);
PULE_exportFn void puleAssetFontDestroy(PuleAssetFont const font);

typedef struct {
  PuleAssetFont font;
  PuleF32v2 fontScale;
  PuleF32v2 fontOffset;
  bool renderFlippedY;

  uint32_t glyphCodepoint;

  PuleBufferView destinationBuffer;
  PuleU32v2 destinationBufferDim;
} PuleAssetFontRenderInfo;

// the buffer must be packed as {
//   color[0].a, color[1].a, .., color[width].a, color[1+width].a, ...
// }
PULE_exportFn void puleAssetFontRenderToU8Buffer(
  PuleAssetFontRenderInfo const info,
  PuleError * const error
);

#ifdef __cplusplus
} // extern C
#endif
