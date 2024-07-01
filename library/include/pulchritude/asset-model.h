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
  PuleAssetModelAttribute_origin = 0,
  PuleAssetModelAttribute_normal = 1,
  PuleAssetModelAttribute_tangent = 2,
  PuleAssetModelAttribute_uvcoord_0 = 3,
  PuleAssetModelAttribute_uvcoord_1 = 4,
  PuleAssetModelAttribute_uvcoord_2 = 5,
  PuleAssetModelAttribute_uvcoord_3 = 6,
  PuleAssetModelAttribute_color_0 = 7,
  PuleAssetModelAttribute_color_1 = 8,
  PuleAssetModelAttribute_color_2 = 9,
  PuleAssetModelAttribute_color_3 = 10,
  PuleAssetModelAttribute_joints_0 = 11,
  PuleAssetModelAttribute_joints_1 = 12,
  PuleAssetModelAttribute_joints_2 = 13,
  PuleAssetModelAttribute_joints_3 = 14,
  PuleAssetModelAttribute_weights_0 = 15,
  PuleAssetModelAttribute_weights_1 = 16,
  PuleAssetModelAttribute_weights_2 = 17,
  PuleAssetModelAttribute_weights_3 = 18,
} PuleAssetModelAttribute;
const uint32_t PuleAssetModelAttributeSize = 19;
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
  PuleAssetModelBufferViewUsage_none = 0,
  PuleAssetModelBufferViewUsage_attribute = 1,
  PuleAssetModelBufferViewUsage_elementIdx = 2,
} PuleAssetModelBufferViewUsage;
const uint32_t PuleAssetModelBufferViewUsageSize = 3;
typedef enum {
  PuleAssetModelAnimationInterpolation_linear = 0,
  PuleAssetModelAnimationInterpolation_step = 1,
  PuleAssetModelAnimationInterpolation_cubicspline = 2,
} PuleAssetModelAnimationInterpolation;
const uint32_t PuleAssetModelAnimationInterpolationSize = 3;
typedef enum {
  PuleAssetModelAnimationTarget_translation = 0,
  PuleAssetModelAnimationTarget_rotation = 1,
  PuleAssetModelAnimationTarget_scale = 2,
  PuleAssetModelAnimationTarget_weights = 3,
} PuleAssetModelAnimationTarget;
const uint32_t PuleAssetModelAnimationTargetSize = 4;
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
struct PuleAssetModelMorphTarget;
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
struct PuleAssetModelAnimationChannel;
struct PuleAssetModelAnimationSampler;
struct PuleAssetModelAnimation;
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
  uint32_t animationLen;
  PuleAssetModelAnimation * animations;
  uint32_t loadWarningLen;
  PuleString * loadWarnings;
} PuleAssetModel;
typedef struct PuleAssetModelScene {
  uint32_t nodeLen;
  PuleAssetModelNode * * nodes;
  PuleString name;
  uint32_t index;
} PuleAssetModelScene;
typedef struct PuleAssetModelNode {
  PuleAssetModelCamera * camera;
  PuleAssetModelSkin * skin;
  PuleAssetModelMesh * mesh;
  uint32_t childrenLen;
  struct PuleAssetModelNode * * children;
  PuleF32m44 transform;
  bool hasTranslate;
  PuleF32v3 translate;
  bool hasRotate;
  PuleF32q rotate;
  bool hasScale;
  PuleF32v3 scale;
  uint32_t morphWeightLen;
  float * morphWeights;
  PuleString name;
  uint32_t index;
} PuleAssetModelNode;
typedef struct PuleAssetModelMesh {
  uint32_t primitiveLen;
  /*  allocates data  */
  PuleAssetModelMeshPrimitive * primitives;
  uint32_t weightLen;
  float * weights;
  PuleString name;
  uint32_t index;
} PuleAssetModelMesh;
typedef struct PuleAssetModelMorphTarget {
  /* 
    origin, normal, tangent
   */
  PuleAssetModelAccessor * attributeAccessor[3] ;
} PuleAssetModelMorphTarget;
typedef struct PuleAssetModelMeshPrimitive {
  PuleAssetModelAccessor * elementIdxAccessor;
  PuleAssetModelAccessor * attributeAccessors[PuleAssetModelAttributeSize] ;
  PuleAssetModelMaterial * material;
  PuleGpuDrawPrimitive topology;
  uint32_t morphTargetLen;
  PuleAssetModelMorphTarget * morphTargets;
  uint32_t drawElementCount;
} PuleAssetModelMeshPrimitive;
typedef struct PuleAssetModelCamera {
  bool isPerspective;
  PuleF32m44 transform;
  PuleString name;
  uint32_t index;
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
  /*  only kept for v3 and v4  */
  PuleF32v3 rangeMax;
  /*  only kept for v3 and v4  */
  PuleF32v3 rangeMin;
  uint32_t index;
} PuleAssetModelAccessor;
typedef struct PuleAssetModelSkin {
  PuleAssetModelAccessor * inverseBindMatrixAccessor;
  PuleAssetModelNode * rootSkeletonNode;
  uint32_t jointNodeIdxLen;
  PuleAssetModelNode * jointNodeIdxes;
  PuleString name;
  uint32_t index;
} PuleAssetModelSkin;
typedef struct PuleAssetModelBufferView {
  PuleAssetModelBuffer * buffer;
  uint32_t byteOffset;
  uint32_t byteLen;
  uint32_t byteStride;
  PuleAssetModelBufferViewUsage usage;
  PuleString name;
  uint32_t index;
} PuleAssetModelBufferView;
typedef struct PuleAssetModelBuffer {
  PuleString resourceUri;
  PuleBuffer data;
  PuleString name;
  uint32_t index;
} PuleAssetModelBuffer;
typedef struct PuleAssetModelMaterialPbrMetallicRoughness {
  PuleAssetModelTexture * baseColorTexture;
  PuleF32v4 baseColorFactor;
  float metallicFactor PULE_defaultField(1.000000);
  float roughnessFactor PULE_defaultField(1.000000);
} PuleAssetModelMaterialPbrMetallicRoughness;
typedef struct PuleAssetModelMaterial {
  PuleAssetModelMaterialPbrMetallicRoughness pbrMetallicRoughness;
  PuleString name;
  uint32_t index;
} PuleAssetModelMaterial;
typedef struct PuleAssetModelTexture {
  PuleAssetModelSampler * sampler;
  PuleAssetModelImage * srcImg;
  PuleString name;
  uint32_t index;
} PuleAssetModelTexture;
typedef struct PuleAssetModelImage {
  PuleString resourceUri;
  /* .id=0 if image not requested to load */
  PuleAssetImage image;
  PuleString name;
  uint32_t index;
} PuleAssetModelImage;
typedef struct PuleAssetModelAnimationChannel {
  PuleAssetModelAnimationSampler * sampler;
  PuleAssetModelNode * node;
  PuleAssetModelAnimationTarget target;
} PuleAssetModelAnimationChannel;
typedef struct PuleAssetModelAnimationSampler {
  PuleAssetModelAccessor * timeline;
  PuleAssetModelAnimationInterpolation interpolation;
  PuleAssetModelAccessor * output;
} PuleAssetModelAnimationSampler;
typedef struct PuleAssetModelAnimation {
  uint32_t channelLen;
  PuleAssetModelAnimationChannel channels[16] ;
  uint32_t samplerLen;
  PuleAssetModelAnimationSampler samplers[16] ;
  PuleString name;
  uint32_t index;
} PuleAssetModelAnimation;
typedef struct PuleAssetModelSampler {
  PuleAssetModelFilterMag filterMag;
  PuleAssetModelFilterMin filterMin;
  PuleAssetModelWrap wrapU;
  PuleAssetModelWrap wrapV;
  PuleString name;
  uint32_t index;
} PuleAssetModelSampler;

// functions
PULE_exportFn PuleAssetModel puleAssetModelLoad(PuleAssetModelLoadInfo info);

#ifdef __cplusplus
} // extern C
#endif
