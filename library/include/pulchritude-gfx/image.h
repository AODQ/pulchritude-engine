#pragma once

#include <pulchritude-gfx/shader-module.h>
#include <pulchritude-gfx/gfx.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleGfxImageMagnification_nearest,
} PuleGfxImageMagnification;

typedef enum {
  PuleGfxImageWrap_clampToEdge,
} PuleGfxImageWrap;

typedef struct {
  uint64_t id;
} PuleGfxSampler;

typedef struct {
  PuleGfxImageMagnification minify;
  PuleGfxImageMagnification magnify;
  PuleGfxImageWrap wrapU;
  PuleGfxImageWrap wrapV;
  //PuleGfxImageWrap wrapW; // only valid on 3D targets
} PuleGfxSamplerCreateInfo;

PULE_exportFn PuleGfxSampler puleGfxSamplerCreate(
  PuleGfxSamplerCreateInfo const createInfo
);
PULE_exportFn void puleGfxSamplerDestroy(
  PuleGfxSampler const sampler
);

typedef struct {
  uint64_t id;
} PuleGfxGpuImage;

typedef enum {
  PuleGfxImageByteFormat_rgba8U,
  PuleGfxImageByteFormat_rgb8U,
} PuleGfxImageByteFormat;

typedef enum {
  PuleGfxImageTarget_i2D,
} PuleGfxImageTarget;

typedef struct {
  uint32_t width;
  uint32_t height;
  PuleGfxImageTarget target;
  PuleGfxImageByteFormat byteFormat;
  PuleGfxSampler sampler; // TODO this should technically be changable
  void const * nullableInitialData;
} PuleGfxImageCreateInfo;

PULE_exportFn PuleGfxGpuImage puleGfxGpuImageCreate(
  PuleGfxImageCreateInfo const imageCreateInfo
);

PULE_exportFn void puleGfxGpuImageDestroy(
  PuleGfxGpuImage const image
);

#ifdef __cplusplus
} // C
#endif
