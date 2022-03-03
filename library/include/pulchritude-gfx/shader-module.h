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
  char const * const vertexShaderSource,
  char const * const fragmentShaderSource,
  PuleError * const error
);
PULE_exportFn void puleGfxShaderModuleDestroy(
  PuleGfxShaderModule const shaderModule
);

#ifdef __cplusplus
} // C
#endif
