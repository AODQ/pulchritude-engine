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

typedef enum {
  PuleAssetMeshAttributeComponentDataType_u8,
  PuleAssetMeshAttributeComponentDataType_u16,
  PuleAssetMeshAttributeComponentDataType_u32,
  PuleAssetMeshAttributeComponentDataType_i8,
  PuleAssetMeshAttributeComponentDataType_i16,
  PuleAssetMeshAttributeComponentDataType_i32,
  PuleAssetMeshAttributeComponentDataType_f16, // unsupported right now
  PuleAssetMeshAttributeComponentDataType_f32,
} PuleAssetMeshAttributeComponentDataType;

typedef struct {
  size_t bufferIndex;
  size_t bufferByteOffset;
  size_t bufferByteStride;
  PuleAssetMeshAttributeComponentDataType componentDataType;
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
  PuleDsValue modelData;
  // callbacks that should probably allocate memory for model
  void (* loadBuffer)(
    PuleBufferView const bufferView,
    size_t const bufferIndex,
    void * const userData
  );
  // all buffers affected by the provided mesh will have been loaded
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

PULE_exportFn size_t puleAssetMeshAttributeComponentDataTypeByteSize(
  PuleAssetMeshAttributeComponentDataType const attribute
);

typedef struct {
  PuleAssetMeshAttribute attribute;
  PuleBufferView attributeBufferView; // buffer mapped from attribute
  PuleAssetMeshAttributeComponentDataType dstComponentDataType;
  size_t dstComponentCount;
  size_t attributeElementCount;
  PuleBufferViewMutable dstBufferView;
} PuleAssetModelConvertAttributeInfo;
PULE_exportFn void puleAssetModelConvertAttributeBuffer(
  PuleAssetModelConvertAttributeInfo const info
);

#ifdef __cplusplus
} // C
#endif
