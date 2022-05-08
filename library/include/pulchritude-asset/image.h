#pragma once

// facilitates image assets

#include <pulchritude-stream/stream.h>
#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorAssetImage_none,
  PuleErrorAssetImage_decode,
} PuleErrorAssetImage;

typedef struct {
  uint64_t id;
} PuleAssetImage;

typedef enum {
  PuleAssetImageSupportFlag_none = 0x0,
  PuleAssetImageSupportFlag_read = 0x1,
  PuleAssetImageSupportFlag_write = 0x2,
  PuleAssetImageSupportFlag_readWrite = 0x3,
} PuleAssetImageSupportFlag;

// the engine's asset image supports only PNG
PULE_exportFn PuleAssetImageSupportFlag puleAssetImageExtensionSupported(
  char const * const extension
);

typedef enum {
  PuleAssetImageFormat_rgbaU8,
  PuleAssetImageFormat_rgbaU16,
  PuleAssetImageFormat_rgbU8,
} PuleAssetImageFormat;

PULE_exportFn PuleAssetImage puleAssetImageLoadFromStream(
  PuleAllocator const allocator,
  PuleStreamRead const imageSource,
  PuleAssetImageFormat const requestedFormat,
  PuleError * const error
);

PULE_exportFn void puleAssetImageDestroy(PuleAssetImage const image);

PULE_exportFn void * puleAssetImageDecodedData(PuleAssetImage const image);
PULE_exportFn size_t puleAssetImageDecodedDataLength(
  PuleAssetImage const image
);
PULE_exportFn uint32_t puleAssetImageWidth(PuleAssetImage const image);
PULE_exportFn uint32_t puleAssetImageHeight(PuleAssetImage const image);


#ifdef __cplusplus
}
#endif
