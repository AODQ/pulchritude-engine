#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/image.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleGpuImageChain;

PULE_exportFn PuleGpuImageChain puleGpuImageChain_create(
  PuleAllocator const allocator,
  PuleStringView const label,
  PuleGpuImageCreateInfo const createInfo
);
PULE_exportFn void puleGpuImageChain_destroy(
  PuleGpuImageChain const imageChain
);

PULE_exportFn PuleGpuImage puleGpuImageChain_current(
  PuleGpuImageChain const imageChain
);

//typedef struct {
//  union {
//    PuleGpuImageChain imageChain;
//    PuleGpuImage image;
//  };
//  bool isChain;
//} PuleGpuImageReference;

// allows you to reference images, even if they're behind a chain, and track
// their lifetimes (e.g. if they get reconstructed)
typedef struct { uint64_t id; } PuleGpuImageReference;

PULE_exportFn PuleGpuImageReference puleGpuImageReference_createImageChain(
   PuleGpuImageChain const imageChain
);
PULE_exportFn PuleGpuImageReference puleGpuImageReference_createImage(
  PuleGpuImage const image
);

PULE_exportFn void puleGpuImageReference_updateImageChain(
  PuleGpuImageChain const baseImageChain,
  PuleGpuImageChain const newImageChain
);

PULE_exportFn void puleGpuImageReference_updateImage(
  PuleGpuImage const baseImage,
  PuleGpuImage const newImage
);

PULE_exportFn void puleGpuImageReference_destroy(
  PuleGpuImageReference const reference
);

PULE_exportFn PuleGpuImage puleGpuImageReference_image(
  PuleGpuImageReference const reference
);

PULE_exportFn PuleGpuImage puleGpuWindowSwapchainImage();
PULE_exportFn PuleGpuImageReference puleGpuWindowSwapchainImageReference();

#ifdef __cplusplus
} // extern C
#endif
