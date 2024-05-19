/* auto generated file asset-font */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/math.h>
#include <pulchritude/array.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleAssetFont font;
  PuleF32v2 fontScale;
  PuleF32v2 fontOffset;
  bool renderFlippedY;
  uint32_t glyphCodepoint;
  PuleBufferViewMutable destinationBuffer;
  PuleU32v2 destinationBufferDim;
} PuleAssetFontRenderInfo;

// enum
typedef enum {
  PuleErrorAssetFont_none,
  PuleErrorAssetFont_decode,
} PuleErrorAssetFont;

// entities
typedef struct { uint64_t id; } PuleAssetFont;

// functions
PULE_exportFn PuleAssetFont puleAssetFontLoad(PuleBufferView fontSource, PuleError * error);
PULE_exportFn PuleAssetFont puleAssetFontLoadFromPath(PuleStringView path, PuleError * error);
PULE_exportFn void puleAssetFontDestroy(PuleAssetFont font);
/* 
    the buffer must be packed as {
      color[0].a, color[1].a, .., color[width].a, color[1+width].a, ...
    }
 */
PULE_exportFn void puleAssetFontRenderToU8Buffer(PuleAssetFontRenderInfo info, PuleError * error);

#ifdef __cplusplus
} // extern C
#endif
