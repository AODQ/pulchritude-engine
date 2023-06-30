#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gfx/shader-module.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleAssetShaderModule;

PULE_exportFn PuleAssetShaderModule
puleAssetShaderModuleCreateFromPaths(
  PuleStringView const shaderModuleLabel,
  PuleStringView const vertexPath,
  PuleStringView const fragmentPath
);

// does not destroy underlying shader module
PULE_exportFn void puleAssetShaderModuleDestroy(
  PuleAssetShaderModule const assetShaderModule
);

PULE_exportFn PuleStringView puleAssetShaderModuleLabel(
  PuleAssetShaderModule const
);
PULE_exportFn PuleGfxShaderModule puleAssetShaderModuleGfxHandle(
  PuleAssetShaderModule const
);

#ifdef __cplusplus
} // extern C
#endif
