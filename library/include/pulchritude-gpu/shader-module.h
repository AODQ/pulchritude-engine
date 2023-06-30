#pragma once

#include <pulchritude-array/array.h>
#include <pulchritude-core/core.h>

#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t id;
} PuleGpuShaderModule;

PULE_exportFn PuleGpuShaderModule puleGpuShaderModuleCreate(
  PuleBufferView const vertexShaderBytecode,
  PuleBufferView const fragmentShaderBytecode,
  PuleError * const error
);
PULE_exportFn void puleGpuShaderModuleDestroy(
  PuleGpuShaderModule const shaderModule
);

#ifdef __cplusplus
} // C
#endif
