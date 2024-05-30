/* auto generated file asset-font */
#pragma once
#include "core.h"

#include "error.h"
#include "math.h"
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorAssetFont_none = 0,
  PuleErrorAssetFont_decode = 1,
} PuleErrorAssetFont;
const uint32_t PuleErrorAssetFontSize = 2;

// entities
typedef struct PuleAssetFont { uint64_t id; } PuleAssetFont;

// structs
struct PuleAssetFontRenderInfo;

typedef struct PuleAssetFontRenderInfo {
  PuleAssetFont font;
  PuleF32v2 fontScale;
  PuleF32v2 fontOffset;
  bool renderFlippedY;
  uint32_t glyphCodepoint;
  PuleBufferViewMutable destinationBuffer;
  PuleU32v2 destinationBufferDim;
} PuleAssetFontRenderInfo;

// functions
PULE_exportFn PuleAssetFont puleAssetFontLoad(PuleBufferView fontSource, struct PuleError * error);
PULE_exportFn PuleAssetFont puleAssetFontLoadFromPath(PuleStringView path, struct PuleError * error);
PULE_exportFn void puleAssetFontDestroy(PuleAssetFont font);
/* 
    the buffer must be packed as {
      color[0].a, color[1].a, .., color[width].a, color[1+width].a, ...
    }
 */
PULE_exportFn void puleAssetFontRenderToU8Buffer(PuleAssetFontRenderInfo info, struct PuleError * error);

#ifdef __cplusplus
} // extern C
#endif
