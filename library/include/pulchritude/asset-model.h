/* auto generated file asset-model */
#pragma once
#include "core.h"

#include "allocator.h"
#include "asset-image.h"
#include "data-serializer.h"
#include "error.h"
#include "gpu.h"
#include "math.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorAssetModel_none = 0,
  PuleErrorAssetModel_decode = 1,
} PuleErrorAssetModel;
const uint32_t PuleErrorAssetModelSize = 2;
typedef enum {
  PuleAssetModelAttribute_index = 0,
  PuleAssetModelAttribute_origin = 1,
  PuleAssetModelAttribute_normal = 2,
  PuleAssetModelAttribute_tangent = 3,
  PuleAssetModelAttribute_uvcoord_0 = 4,
  PuleAssetModelAttribute_uvcoord_1 = 5,
  PuleAssetModelAttribute_uvcoord_2 = 6,
  PuleAssetModelAttribute_uvcoord_3 = 7,
  PuleAssetModelAttribute_color_0 = 8,
  PuleAssetModelAttribute_color_1 = 9,
  PuleAssetModelAttribute_color_2 = 10,
  PuleAssetModelAttribute_color_3 = 11,
  PuleAssetModelAttribute_joints_0 = 12,
  PuleAssetModelAttribute_joints_1 = 13,
  PuleAssetModelAttribute_joints_2 = 14,
  PuleAssetModelAttribute_joints_3 = 15,
  PuleAssetModelAttribute_weights_0 = 16,
  PuleAssetModelAttribute_weights_1 = 17,
  PuleAssetModelAttribute_weights_2 = 18,
  PuleAssetModelAttribute_weights_3 = 19,
} PuleAssetModelAttribute;
const uint32_t PuleAssetModelAttributeSize = 20;
typedef enum {
  PuleAssetModelElementType_scalar = 0,
  PuleAssetModelElementType_vec2 = 1,
  PuleAssetModelElementType_vec3 = 2,
  PuleAssetModelElementType_vec4 = 3,
  PuleAssetModelElementType_mat2 = 4,
  PuleAssetModelElementType_mat3 = 5,
  PuleAssetModelElementType_mat4 = 6,
} PuleAssetModelElementType;
const uint32_t PuleAssetModelElementTypeSize = 7;
typedef enum {
  PuleAssetModelFilterMag_nearest = 0,
  PuleAssetModelFilterMag_linear = 1,
} PuleAssetModelFilterMag;
const uint32_t PuleAssetModelFilterMagSize = 2;
typedef enum {
  PuleAssetModelFilterMin_nearest = 0,
  PuleAssetModelFilterMin_nearestMipmapNearest = 1,
  PuleAssetModelFilterMin_nearestMipmapLinear = 2,
  PuleAssetModelFilterMin_linear = 3,
  PuleAssetModelFilterMin_linearMipmapNearest = 4,
  PuleAssetModelFilterMin_linearMipmapLinear = 5,
} PuleAssetModelFilterMin;
const uint32_t PuleAssetModelFilterMinSize = 6;
typedef enum {
  PuleAssetModelWrap_clampToEdge = 0,
  PuleAssetModelWrap_repeat = 1,
  PuleAssetModelWrap_repeatMirrored = 2,
} PuleAssetModelWrap;
const uint32_t PuleAssetModelWrapSize = 3;

// entities

// structs
struct PuleAssetModelLoadInfo;
struct PuleAssetModel;
struct PuleAssetModelScene;
struct PuleAssetModelNode;
struct PuleAssetModelMesh;
struct PuleAssetModelMeshPrimitive;
struct PuleAssetModelCamera;
struct PuleAssetModelAccessor;
struct PuleAssetModelSkin;
struct PuleAssetModelBufferView;
struct PuleAssetModelBuffer;
struct PuleAssetModelMaterialPbrMetallicRoughness;
struct PuleAssetModelMaterial;
struct PuleAssetModelTexture;
struct PuleAssetModelImage;
struct PuleAssetModelSampler;

typedef struct PuleAssetModelLoadInfo {
  PuleAllocator allocator;
  PuleStringView modelPath;
  /* doesn't destroy */
  PuleDsValue modelSource;
  bool loadBuffers;
  bool loadImages;
  PuleError * error;
} PuleAssetModelLoadInfo;
typedef struct PuleAssetModel {
  PuleAllocator allocator;
  uint32_t sceneLen;
  PuleAssetModelScene * scenes;
  uint32_t nodeLen;
  PuleAssetModelNode * nodes;
  uint32_t cameraLen;
  PuleAssetModelCamera * cameras;
  uint32_t meshLen;
  PuleAssetModelMesh * meshes;
  uint32_t materialLen;
  PuleAssetModelMaterial * materials;
  uint32_t skinLen;
  PuleAssetModelSkin * skins;
  uint32_t accessorLen;
  PuleAssetModelAccessor * accessors;
  uint32_t bufferViewLen;
  PuleAssetModelBufferView * bufferViews;
  uint32_t bufferLen;
  PuleAssetModelBuffer * buffers;
  uint32_t imageLen;
  PuleAssetModelImage * images;
  uint32_t textureLen;
  PuleAssetModelTexture * textures;
} PuleAssetModel;
typedef struct PuleAssetModelScene {
  uint32_t nodeLen;
  PuleAssetModelNode * * nodes;
  PuleString name;
} PuleAssetModelScene;
typedef struct PuleAssetModelNode {
  PuleAssetModelCamera * camera;
  PuleAssetModelSkin * skin;
  PuleAssetModelMesh * mesh;
  uint32_t childrenLen;
  struct PuleAssetModelNode * * children;
  PuleF32m44 transform;
  uint32_t morphWeightLen;
  float * morphWeights;
  PuleString name;
} PuleAssetModelNode;
typedef struct PuleAssetModelMesh {
  uint32_t primitiveLen;
  /*  allocates data  */
  PuleAssetModelMeshPrimitive * primitives;
  uint32_t weightLen;
  float * weights;
  PuleString name;
} PuleAssetModelMesh;
typedef struct PuleAssetModelMeshPrimitive {
  PuleAssetModelAccessor * attributeAccessors[PuleAssetModelAttributeSize] ;
  PuleAssetModelMaterial * material;
  PuleGpuDrawPrimitive topology;
  uint32_t morphTargetLen;
  /*  TODO  */
  void * morphTargets;
} PuleAssetModelMeshPrimitive;
typedef struct PuleAssetModelCamera {
  bool isPerspective;
  PuleF32m44 transform;
  PuleString name;
} PuleAssetModelCamera;
/*  TODO sparse  */
typedef struct PuleAssetModelAccessor {
  PuleAssetModelBufferView * bufferView;
  uint32_t byteOffset;
  PuleGpuAttributeDataType dataType;
  bool convertFixedToNormalized;
  uint32_t elementCount;
  PuleAssetModelElementType elementType;
  PuleString name;
} PuleAssetModelAccessor;
typedef struct PuleAssetModelSkin {
  PuleAssetModelAccessor * inverseBindMatrixAccessor;
  PuleAssetModelNode * rootSkeletonNode;
  uint32_t jointNodeIdxLen;
  PuleAssetModelNode * jointNodeIdxes;
  PuleString name;
} PuleAssetModelSkin;
typedef struct PuleAssetModelBufferView {
  PuleAssetModelBuffer * buffer;
  uint32_t byteOffset;
  uint32_t byteLen;
  uint32_t byteStride;
  bool isIndexTarget;
  PuleString name;
} PuleAssetModelBufferView;
typedef struct PuleAssetModelBuffer {
  PuleString resourceUri;
  PuleBufferView dataView;
  PuleString name;
} PuleAssetModelBuffer;
typedef struct PuleAssetModelMaterialPbrMetallicRoughness {
  float baseColorFactor[4] ;
  float metallicFactor;
  float roughnessFactor;
} PuleAssetModelMaterialPbrMetallicRoughness;
typedef struct PuleAssetModelMaterial {
  PuleString name;
  PuleAssetModelMaterialPbrMetallicRoughness pbrMetallicRoughness;
} PuleAssetModelMaterial;
typedef struct PuleAssetModelTexture {
  PuleAssetModelSampler * sampler;
  PuleAssetModelImage * srcImg;
  PuleString name;
} PuleAssetModelTexture;
typedef struct PuleAssetModelImage {
  PuleString resourceUri;
  PuleString name;
  /* .id=0 if image not requested to load */
  PuleAssetImage image;
} PuleAssetModelImage;
typedef struct PuleAssetModelSampler {
  PuleAssetModelFilterMag filterMag;
  PuleAssetModelFilterMin filterMin;
  PuleAssetModelWrap wrapU;
  PuleAssetModelWrap wrapV;
  PuleString name;
} PuleAssetModelSampler;

// functions
PULE_exportFn PuleAssetModel puleAssetModelLoad(PuleAssetModelLoadInfo info);

#ifdef __cplusplus
} // extern C
#endif
