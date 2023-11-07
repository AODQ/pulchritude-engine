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
    -> [gpu upload] || probably where data gets sorted to fit GPU buffer
                       correctly
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
  PuleAssetMeshAttributeType_size,
} PuleAssetMeshAttributeType;

// .dontCare used only for requests
typedef enum {
  PuleAssetMeshComponentDataType_u8,
  PuleAssetMeshComponentDataType_u16,
  PuleAssetMeshComponentDataType_u32,
  PuleAssetMeshComponentDataType_i8,
  PuleAssetMeshComponentDataType_i16,
  PuleAssetMeshComponentDataType_i32,
  PuleAssetMeshComponentDataType_f16, // unsupported right now
  PuleAssetMeshComponentDataType_f32,
  PuleAssetMeshComponentDataType_dontCare,
} PuleAssetMeshComponentDataType;

typedef struct {
  size_t bufferIndex;
  size_t bufferByteOffset;
  size_t bufferByteStride;
  PuleAssetMeshComponentDataType componentDataType;
  uint32_t componentsPerVertex; // Must be 1, 2, 3, or 4
} PuleAssetMeshAttribute;

typedef struct {
  size_t bufferIndex;
  size_t bufferByteOffset;
  // always will be in u32
} PuleAssetMeshElement;

typedef struct {
  // all attributes *must* have same element count
  PuleAssetMeshAttribute attributes[PuleAssetMeshAttributeType_size];
  PuleAssetMeshElement element;
  size_t verticesToDispatch; // aka 'draw count'
  size_t attributeElementCount;
} PuleAssetMesh;

typedef struct {
  PuleAssetMeshComponentDataType componentDataType;
  uint32_t componentsPerVertex;
} PuleAssetMeshRequestedFormatAttribute;

typedef struct {
  PuleDsValue modelData;
  // callbacks that should probably allocate memory for model
  void (* loadBuffer)(
    PuleBufferView const bufferView,
    size_t const bufferIndex,
    void * const userData
  );
  void (* loadMesh)(
    PuleAssetMesh const mesh,
    size_t const meshIndex,
    void * const userData
  );
  void * userData;
} PuleAssetModelLoadInfo;

PULE_exportFn void puleAssetModelLoad(
  PuleAssetModelLoadInfo const loadInfo,
  PuleError * const error
);

#ifdef __cplusplus
} // C
#endif
