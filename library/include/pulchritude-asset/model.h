#pragma once

// capable of loading 3D PDS models, or providing the foundation to asset
//   modules that load non-PDS 3D models
//
// the general workflow when using this module should be:
//   - load file to stream
//   - load model with requested format
//   - upload data to GPU buffers, prepare GPU pipelines / command buffers


/*
  [file]/[stream] || contains filedata
    -> [pds value] || contains many buffers
    -> [asset model] || contains single buffer, ordered like it was in pds
    -> [gpu upload] || probably where data gets sorted to fit GPU buffer correctly

  the 'retain dynamic buffers' part is a bit interesting because all that needs to really
    be retained is the meta-data required for the renderer, as assumingly each time a new
    instance of the model/mesh is created then a new dynamic buffer needs to be
    allocated/uploaded
*/

// PDS model is very similar to glTF

#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorAssetModel_none,
  PuleErrorAssetModel_decode,
} PuleErrorAssetModel;

typedef struct { uint64_t id; } PuleAssetMaterial;

// TODO at some point we will need custom attributes
//  this will be PuleAssetMeshAttributeType_custom and accept an array of
//  attributes technically I could do _custom0, ..., _custom16; I can't think
//  of a reason why a model will need to store more than 16 custom attributes,
//  but obv this is hard-code limiting.
typedef enum {
  PuleAssetMeshAttributeType_origin,
  PuleAssetMeshAttributeType_uvCoord0,
  PuleAssetMeshAttributeType_normal,
  PuleAssetMeshAttributeType_Size,
} PuleAssetMeshAttributeType;

typedef enum {
  PuleAssetMeshComponentDataType_u8, // 0
  PuleAssetMeshComponentDataType_u16, // 1
  PuleAssetMeshComponentDataType_u32, // 2
  PuleAssetMeshComponentDataType_u8Normalized, // 3
  PuleAssetMeshComponentDataType_u16Normalized, // 4
  PuleAssetMeshComponentDataType_u32Normalized, // 5
  PuleAssetMeshComponentDataType_f16, // 6
  PuleAssetMeshComponentDataType_f32, // 7
  PuleAssetMeshComponentDataType_dontCare, // 8 used only for requests
} PuleAssetMeshComponentDataType;

typedef struct {
  PuleArrayView view;
  PuleAssetMeshComponentDataType componentDataType;
  uint32_t componentsPerVertex; // Must be 1, 2, 3, or 4
} PuleAssetMeshAttribute;

typedef struct {
  PuleArrayView view;
  PuleAssetMeshComponentDataType componentDataType;
} PuleAssetMeshElement;

typedef struct {
  // all attributes *must* have same element count that *must* match to
  // verticesToDispatch
  PuleAssetMeshAttribute attributes[PuleAssetMeshAttributeType_Size];
  PuleAssetMeshElement element;
  size_t verticesToDispatch; // aka 'draw count'
} PuleAssetMesh;

typedef struct {
  uint64_t id;
  PuleAssetMaterial * materials;
  size_t materialCount;
  PuleAssetMesh * meshes;
  size_t meshCount;
} PuleAssetModel;

//-- model ---------------------------------------------------------------------

PULE_exportFn void puleAssetModelDestroy(PuleAssetModel const model);

//-- mesh/attribute ------------------------------------------------------------

PULE_exportFn size_t puleAssetMeshComponentDataTypeByteLength(
  PuleAssetMeshComponentDataType const dataType
);

//-- loading/requests ----------------------------------------------------------

// you can request conversion to be done to a specified data-type, which is
//   convenient when rendering multiple meshes in a single draw call
// you can use 'dontCare', and value '0' to specify to not perform any
//   conversion

typedef struct {
  PuleAssetMeshComponentDataType componentDataType;
  uint32_t componentsPerVertex;
} PuleAssetMeshRequestedFormatAttribute;

typedef struct {
  PuleAssetMeshRequestedFormatAttribute
    attributes[PuleAssetMeshAttributeType_Size];
  PuleAssetMeshComponentDataType elementComponentDataType;
} PuleAssetMeshRequestedFormat;

typedef struct {
  PuleAllocator allocator;
  PuleStreamRead modelSource;
  PuleAssetMeshRequestedFormat requestedMeshFormat;
} PuleAssetModelCreateInfo;

PULE_exportFn PuleAssetModel puleAssetModelLoadFromStream(
  PuleAssetModelCreateInfo const createInfo,
  PuleError * const error
);

//PULE_exportFn PuleAssetModelInstance puleAssetModelInstanceCreate(
//  PuleAssetModel const model
//);

#ifdef __cplusplus
} // C
#endif
