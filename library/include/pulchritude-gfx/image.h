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
  PuleGfxImageByteFormat_bgra8U,
  PuleGfxImageByteFormat_rgba8U,
  PuleGfxImageByteFormat_rgb8U,
  PuleGfxImageByteFormat_r8U,
  PuleGfxImageByteFormat_depth16,
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
  void const * optionalInitialData;
} PuleGfxImageCreateInfo;

PULE_exportFn PuleGfxGpuImage puleGfxGpuImageCreate(
  PuleGfxImageCreateInfo const imageCreateInfo
);

PULE_exportFn void puleGfxGpuImageDestroy(
  PuleGfxGpuImage const image
);

typedef struct {
  uint64_t id;
} PuleGfxFramebuffer;

typedef enum {
  PuleGfxFramebufferAttachment_color0,
  PuleGfxFramebufferAttachment_color1,
  PuleGfxFramebufferAttachment_color3,
  PuleGfxFramebufferAttachment_color4,
  PuleGfxFramebufferAttachment_depth,
  PuleGfxFramebufferAttachment_stencil,
  PuleGfxFramebufferAttachment_depthStencil,
  PuleGfxFramebufferAttachment_End,
} PuleGfxFramebufferAttachment;

typedef enum {
  PuleGfxFramebufferType_renderStorage,
  PuleGfxFramebufferType_imageStorage,
} PuleGfxFramebufferType;

typedef struct {
  uint64_t id;
} PuleGfxRenderStorage;

typedef struct {
  PuleGfxGpuImage image;
  uint32_t mipmapLevel; // default 0
} PuleGfxFramebufferImageAttachment;

typedef union {
  PuleGfxFramebufferImageAttachment images[PuleGfxFramebufferAttachment_End];
  PuleGfxRenderStorage renderStorages[PuleGfxFramebufferAttachment_End];
} PuleGfxFramebufferAttachments;

typedef struct {
  PuleGfxFramebufferAttachments attachment;
  PuleGfxFramebufferType attachmentType;
} PuleGfxFramebufferCreateInfo;

PULE_exportFn PuleGfxFramebufferCreateInfo puleGfxFramebufferCreateInfo();

PULE_exportFn PuleGfxFramebuffer puleGfxFramebufferCreate(
  PuleGfxFramebufferCreateInfo const framebufferCreateInfo,
  PuleError * const error
);
PULE_exportFn void puleGfxFramebufferDestroy(
  PuleGfxFramebuffer const framebuffer
);
PULE_exportFn PuleGfxGpuImage puleGfxWindowImage();
PuleGfxFramebufferAttachments puleGfxFramebufferAttachments(
  PuleGfxFramebuffer const framebuffer
);

typedef enum {
  PuleGfxImageAttachmentOpLoad_load,
  PuleGfxImageAttachmentOpLoad_clear,
  PuleGfxImageAttachmentOpLoad_dontCare,
} PuleGfxImageAttachmentOpLoad;

typedef enum {
  PuleGfxImageAttachmentOpStore_store,
  PuleGfxImageAttachmentOpStore_dontCare,
} PuleGfxImageAttachmentOpStore;

typedef enum {
  PuleGfxImageLayout_uninitialized,
  PuleGfxImageLayout_storage,
  PuleGfxImageLayout_attachmentColor,
  PuleGfxImageLayout_attachmentDepth,
  PuleGfxImageLayout_transferSrc,
  PuleGfxImageLayout_transferDst,
} PuleGfxImageLayout;

typedef struct {
  PuleGfxGpuImage image;
  size_t mipmapLevelStart;
  size_t mipmapLevelCount;
  size_t arrayLayerStart;
  size_t arrayLayerCount;
  PuleGfxImageByteFormat byteFormat;
} PuleGfxGpuImageView;

typedef struct {
  PuleGfxImageAttachmentOpLoad opLoad;
  PuleGfxImageAttachmentOpStore opStore;
  PuleGfxImageLayout layout;
  PuleF32v4 clearColor;
  PuleGfxGpuImageView imageView;
} PuleGfxImageAttachment;

#ifdef __cplusplus
} // C
#endif
