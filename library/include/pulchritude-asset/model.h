#pragma once

// capable of loading 3D PDS models, or providing the foundation to asset
//   modules that load non-PDS 3D models

// PDS model is very similar to glTF

#include <pulchritude-pds/pds.h>
#include <pulchrtiude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorAssetModel_none,
  PuleErrorAssetModel_decode,
} PuleErrorAssetModel;

typedef struct { uint64_t id; } PuleAssetMaterial;
typedef struct { uint64_t id; } PuleAssetMesh;
typedef struct { uint64_t id; } PuleAssetModel;

//typedef struct { uint64_t id; } PuleAssetModelInstance;

PULE_exportFn PuleAssetModel puleAssetModelCreateModelFromMeshMaterial(
  PuleAssetMesh const mesh,
  PuleAssetMaterial const material
);

//PULE_exportFn PuleAssetModelInstance puleAssetModelInstanceCreate(
//  PuleAssetModel const model
//);

#ifdef __cplusplus
} // C
#endif
