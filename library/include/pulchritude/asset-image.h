/* auto generated file asset-image */
#pragma once
#include "core.h"

#include "stream.h"
#include "allocator.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorAssetImage_none = 0,
  PuleErrorAssetImage_decode = 1,
} PuleErrorAssetImage;
const uint32_t PuleErrorAssetImageSize = 2;
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
PULE_exportFn PuleAssetImage puleAssetImageLoadFromStream(PuleAllocator allocator, PuleStreamRead imageSource, PuleAssetImageFormat requestedFormat, struct PuleError * error);
PULE_exportFn void puleAssetImageDestroy(PuleAssetImage image);
PULE_exportFn void * puleAssetImageDecodedData(PuleAssetImage image);
PULE_exportFn size_t puleAssetImageDecodedDataLength(PuleAssetImage image);
PULE_exportFn uint32_t puleAssetImageWidth(PuleAssetImage image);
PULE_exportFn uint32_t puleAssetImageHeight(PuleAssetImage image);

#ifdef __cplusplus
} // extern C
#endif
