/* auto generated file asset-image */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/stream.h>
#include <pulchritude/allocator.h>
#include <pulchritude/error.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs

// enum
typedef enum {
  PuleErrorAssetImage_none,
  PuleErrorAssetImage_decode,
} PuleErrorAssetImage;
typedef enum {
  PuleAssetImageSupportFlag_none,
  PuleAssetImageSupportFlag_read,
  PuleAssetImageSupportFlag_write,
  PuleAssetImageSupportFlag_readWrite,
} PuleAssetImageSupportFlag;
typedef enum {
  PuleAssetImageFormat_rgbaU8,
  PuleAssetImageFormat_rgbaU16,
  PuleAssetImageFormat_rgbU8,
} PuleAssetImageFormat;

// entities
typedef struct { uint64_t id; } PuleAssetImage;

// functions
PULE_exportFn PuleAssetImageSupportFlag puleAssetImageExtensionSupported(char const * extension);
PULE_exportFn PuleAssetImage puleAssetImageLoadFromStream(PuleAllocator allocator, PuleStreamRead imageSource, PuleAssetImageFormat requestedFormat, PuleError * error);
PULE_exportFn void puleAssetImageDestroy(PuleAssetImage image);
PULE_exportFn void * puleAssetImageDecodedData(PuleAssetImage image);
PULE_exportFn size_t puleAssetImageDecodedDataLength(PuleAssetImage image);
PULE_exportFn uint32_t puleAssetImageWidth(PuleAssetImage image);
PULE_exportFn uint32_t puleAssetImageHeight(PuleAssetImage image);

#ifdef __cplusplus
} // extern C
#endif
