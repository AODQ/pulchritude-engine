/* auto generated file asset-shader-module */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/gpu.h>
#include <pulchritude/string.h>
#include <pulchritude/error.h>
#include <pulchritude/core.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs

// enum

// entities
typedef struct { uint64_t id; } PuleAssetShaderModule;

// functions
PULE_exportFn PuleAssetShaderModule puleAssetShaderModuleCreateFromPaths(PuleStringView shaderModuleLabel, PuleStringView vertexPath, PuleStringView fragmentPath);
/* does not destroy underlying shader module */
PULE_exportFn void puleAssetShaderModuleDestroy(PuleAssetShaderModule assetShaderModule);
PULE_exportFn PuleStringView puleAssetShaderModuleLabel(PuleAssetShaderModule assetShaderModule);
PULE_exportFn PuleGpuShaderModule puleAssetShaderModuleHandle(PuleAssetShaderModule assetShaderModule);

#ifdef __cplusplus
} // extern C
#endif
