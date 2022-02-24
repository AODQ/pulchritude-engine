#pragma once

#include <pulchritude-core/core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  PuleWindow window;
  void * implementation;
} PuleGfxContext;

typedef struct {
  uint64_t id;
} PuleGfxGpuImage;

typedef enum {
} PuleGfxImageByteFormat;

typedef enum {
} PuleGfxImageChannel;

typedef struct {
  uint64_t id;
} PuleGfxGpuFramebuffer;

typedef struct {
  uint64_t id;
} PuleGfxGpuBuffer;

typedef struct {
  uint64_t id;
} PuleGfxShader;

typedef struct {
  uint64_t id;
} PuleGfxPass;


PULE_exportFn void puleGfxCreate();
PULE_exportFn void puleGfxDestroy();

PULE_exportFn void puleGfxFrameStart();
PULE_exportFn void puleGfxFrameEnd();

PULE_exportFn void puleGfxGpuImageCreate(
  uint32_t width,
  uint32_t height,
  PuleGfxImageByteFormat,
  PuleGfxImageChannel,
  void const * data
);

#ifdef __cplusplus
} // extern C
#endif
