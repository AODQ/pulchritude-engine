#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t id;
} PuleGfxShaderModule;

PULE_exportFn PuleGfxShaderModule puleGfxShaderModuleCreate(
  PuleStringView const vertexShaderSource,
  PuleStringView const fragmentShaderSource,
  PuleError * const error
);
PULE_exportFn void puleGfxShaderModuleDestroy(
  PuleGfxShaderModule const shaderModule
);

#ifdef __cplusplus
} // C
#endif
