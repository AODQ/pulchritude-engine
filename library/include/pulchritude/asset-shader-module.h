/* auto generated file asset-shader-module */
#pragma once
#include "core.h"

#include "gpu.h"
#include "string.h"
#include "error.h"
#include "core.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities
typedef struct PuleAssetShaderModule { uint64_t id; } PuleAssetShaderModule;

// structs


// functions
PULE_exportFn PuleAssetShaderModule puleAssetShaderModuleCreateFromPaths(PuleStringView shaderModuleLabel, PuleStringView vertexPath, PuleStringView fragmentPath);
/* does not destroy underlying shader module */
PULE_exportFn void puleAssetShaderModuleDestroy(PuleAssetShaderModule assetShaderModule);
PULE_exportFn PuleStringView puleAssetShaderModuleLabel(PuleAssetShaderModule assetShaderModule);
PULE_exportFn PuleGpuShaderModule puleAssetShaderModuleHandle(PuleAssetShaderModule assetShaderModule);

#ifdef __cplusplus
} // extern C
#endif
