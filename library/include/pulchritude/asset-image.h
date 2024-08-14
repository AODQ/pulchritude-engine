/* auto generated file asset-image */
#pragma once
#include "core.h"

#include "allocator.h"
#include "array.h"
#include "error.h"
#include "math.h"
#include "stream.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorAssetImage_none = 0,
  PuleErrorAssetImage_decode = 1,
  PuleErrorAssetImage_encode = 2,
} PuleErrorAssetImage;
const uint32_t PuleErrorAssetImageSize = 3;
typedef enum {
  PuleAssetImageSupportFlag_none = 0,
  PuleAssetImageSupportFlag_read = 1,
  PuleAssetImageSupportFlag_write = 2,
  PuleAssetImageSupportFlag_readWrite = 3,
} PuleAssetImageSupportFlag;
const uint32_t PuleAssetImageSupportFlagSize = 4;
typedef enum {
  PuleAssetImageFormat_rgbaU8 = 0,
  PuleAssetImageFormat_rgbaU16 = 1,
  PuleAssetImageFormat_rgbU8 = 2,
} PuleAssetImageFormat;
const uint32_t PuleAssetImageFormatSize = 3;

// entities
typedef struct PuleAssetImage { uint64_t id; } PuleAssetImage;

// structs


// functions
PULE_exportFn PuleAssetImageSupportFlag puleAssetImageExtensionSupported(char const * extension);
PULE_exportFn PuleAssetImage puleAssetImageLoadFromStream(PuleAllocator allocator, PuleStreamRead imageSource, PuleStringView imageExtension, PuleAssetImageFormat requestedFormat, PuleError * error);
PULE_exportFn PuleAssetImage puleAssetImage(uint32_t width, uint32_t height, PuleAssetImageFormat format);
PULE_exportFn void puleAssetImageWriteToStream(PuleAssetImage image, PuleStreamWrite imageDst, PuleStringView imageExtension, PuleError * error);
PULE_exportFn void puleAssetImageDestroy(PuleAssetImage image);
PULE_exportFn PuleBufferView puleAssetImageData(PuleAssetImage image);
PULE_exportFn void * puleAssetImageDecodedData(PuleAssetImage image);
PULE_exportFn size_t puleAssetImageDecodedDataLength(PuleAssetImage image);
PULE_exportFn uint32_t puleAssetImageWidth(PuleAssetImage image);
PULE_exportFn uint32_t puleAssetImageHeight(PuleAssetImage image);
PULE_exportFn PuleF32v4 puleAssetImageTexel(PuleAssetImage image, uint32_t x, uint32_t y);
PULE_exportFn void puleAssetImageTexelSet(PuleAssetImage image, uint32_t x, uint32_t y, PuleF32v4 rgba);

#ifdef __cplusplus
} // extern C
#endif
