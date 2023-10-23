#pragma once

#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/shader-module.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleGpuImageMagnification_nearest,
} PuleGpuImageMagnification;

typedef enum {
  PuleGpuImageWrap_clampToEdge,
} PuleGpuImageWrap;

typedef struct {
  uint64_t id;
} PuleGpuSampler;

typedef struct {
  PuleGpuImageMagnification minify;
  PuleGpuImageMagnification magnify;
  PuleGpuImageWrap wrapU;
  PuleGpuImageWrap wrapV;
  //PuleGpuImageWrap wrapW; // only valid on 3D targets
} PuleGpuSamplerCreateInfo;

PULE_exportFn PuleGpuSampler puleGpuSamplerCreate(
  PuleGpuSamplerCreateInfo const createInfo
);
PULE_exportFn void puleGpuSamplerDestroy(
  PuleGpuSampler const sampler
);

typedef struct {
  uint64_t id;
} PuleGpuImage;

typedef enum {
  PuleGpuImageByteFormat_bgra8U,
  PuleGpuImageByteFormat_rgba8U,
  PuleGpuImageByteFormat_rgb8U,
  PuleGpuImageByteFormat_r8U,
  PuleGpuImageByteFormat_depth16,
} PuleGpuImageByteFormat;

typedef enum {
  PuleGpuImageTarget_i2D,
} PuleGpuImageTarget;

typedef struct {
  uint32_t width;
  uint32_t height;
  PuleGpuImageTarget target;
  PuleGpuImageByteFormat byteFormat;
  PuleGpuSampler sampler; // TODO this should technically be changable
  PuleStringView label;
  void const * optionalInitialData;
} PuleGpuImageCreateInfo;

PULE_exportFn PuleStringView puleGpuImageLabel(PuleGpuImage const image);

PULE_exportFn void puleGpuImageDestroy(PuleGpuImage const image);

PULE_exportFn PuleGpuImage puleGpuImageCreate(
  PuleGpuImageCreateInfo const imageCreateInfo
);

typedef struct {
  uint64_t id;
} PuleGpuFramebuffer;

typedef enum {
  PuleGpuFramebufferAttachment_color0,
  PuleGpuFramebufferAttachment_color1,
  PuleGpuFramebufferAttachment_color3,
  PuleGpuFramebufferAttachment_color4,
  PuleGpuFramebufferAttachment_depth,
  PuleGpuFramebufferAttachment_stencil,
  PuleGpuFramebufferAttachment_depthStencil,
  PuleGpuFramebufferAttachment_End,
} PuleGpuFramebufferAttachment;

typedef enum {
  PuleGpuFramebufferType_renderStorage,
  PuleGpuFramebufferType_imageStorage,
} PuleGpuFramebufferType;

typedef struct {
  uint64_t id;
} PuleGpuRenderStorage;

typedef struct {
  PuleGpuImage image;
  uint32_t mipmapLevel; // default 0
} PuleGpuFramebufferImageAttachment;

typedef union {
  PuleGpuFramebufferImageAttachment images[PuleGpuFramebufferAttachment_End];
  PuleGpuRenderStorage renderStorages[PuleGpuFramebufferAttachment_End];
} PuleGpuFramebufferAttachments;

typedef struct {
  PuleGpuFramebufferAttachments attachment;
  PuleGpuFramebufferType attachmentType;
} PuleGpuFramebufferCreateInfo;

PULE_exportFn PuleGpuFramebufferCreateInfo puleGpuFramebufferCreateInfo();

PULE_exportFn PuleGpuFramebuffer puleGpuFramebufferCreate(
  PuleGpuFramebufferCreateInfo const framebufferCreateInfo,
  PuleError * const error
);
PULE_exportFn void puleGpuFramebufferDestroy(
  PuleGpuFramebuffer const framebuffer
);
PuleGpuFramebufferAttachments puleGpuFramebufferAttachments(
  PuleGpuFramebuffer const framebuffer
);

typedef enum {
  PuleGpuImageAttachmentOpLoad_load,
  PuleGpuImageAttachmentOpLoad_clear,
  PuleGpuImageAttachmentOpLoad_dontCare,
} PuleGpuImageAttachmentOpLoad;

typedef enum {
  PuleGpuImageAttachmentOpStore_store,
  PuleGpuImageAttachmentOpStore_dontCare,
} PuleGpuImageAttachmentOpStore;

typedef enum {
  PuleGpuImageLayout_uninitialized,
  PuleGpuImageLayout_storage,
  PuleGpuImageLayout_attachmentColor,
  PuleGpuImageLayout_attachmentDepth,
  PuleGpuImageLayout_transferSrc,
  PuleGpuImageLayout_transferDst,
  PuleGpuImageLayout_presentSrc,
} PuleGpuImageLayout;

typedef struct {
  PuleGpuImage image;
  size_t mipmapLevelStart;
  size_t mipmapLevelCount;
  size_t arrayLayerStart;
  size_t arrayLayerCount;
  PuleGpuImageByteFormat byteFormat;
} PuleGpuImageView;

typedef struct {
  PuleGpuImageAttachmentOpLoad opLoad;
  PuleGpuImageAttachmentOpStore opStore;
  PuleGpuImageLayout layout;
  PuleF32v4 clearColor;
  PuleGpuImageView imageView;
} PuleGpuImageAttachment;

PULE_exportFn PuleStringView puleGpuImageLayoutLabel(
  PuleGpuImageLayout const layout
);

#ifdef __cplusplus
} // C
#endif
