# asset-model

## structs
### PuleAssetModelLoadInfo
```c
struct {
  allocator : PuleAllocator;
  modelPath : PuleStringView;
  /* doesn't destroy */
  modelSource : PuleDsValue;
  loadBuffers : bool;
  loadImages : bool;
  error : PuleError ptr;
};
```
### PuleAssetModel
```c
struct {
  allocator : PuleAllocator;
  sceneLen : uint32_t;
  scenes : PuleAssetModelScene ptr;
  nodeLen : uint32_t;
  nodes : PuleAssetModelNode ptr;
  cameraLen : uint32_t;
  cameras : PuleAssetModelCamera ptr;
  meshLen : uint32_t;
  meshes : PuleAssetModelMesh ptr;
  materialLen : uint32_t;
  materials : PuleAssetModelMaterial ptr;
  skinLen : uint32_t;
  skins : PuleAssetModelSkin ptr;
  accessorLen : uint32_t;
  accessors : PuleAssetModelAccessor ptr;
  bufferViewLen : uint32_t;
  bufferViews : PuleAssetModelBufferView ptr;
  bufferLen : uint32_t;
  buffers : PuleAssetModelBuffer ptr;
  imageLen : uint32_t;
  images : PuleAssetModelImage ptr;
  textureLen : uint32_t;
  textures : PuleAssetModelTexture ptr;
};
```
### PuleAssetModelScene
```c
struct {
  nodeLen : uint32_t;
  nodes : PuleAssetModelNode ptr ptr;
  name : PuleString;
};
```
### PuleAssetModelNode
```c
struct {
  camera : PuleAssetModelCamera ptr;
  skin : PuleAssetModelSkin ptr;
  mesh : PuleAssetModelMesh ptr;
  childrenLen : uint32_t;
  children : PuleAssetModelNode ptr ptr;
  transform : PuleF32m44;
  morphWeightLen : uint32_t;
  morphWeights : float ptr;
  name : PuleString;
};
```
### PuleAssetModelMesh
```c
struct {
  primitiveLen : uint32_t;
  /*  allocates data  */
  primitives : PuleAssetModelMeshPrimitive ptr;
  weightLen : uint32_t;
  weights : float ptr;
  name : PuleString;
};
```
### PuleAssetModelMeshPrimitive
```c
struct {
  attributeAccessors : PuleAssetModelAccessor ptr [PuleAssetModelAttributeSize];
  material : PuleAssetModelMaterial ptr;
  topology : PuleGpuDrawPrimitive;
  morphTargetLen : uint32_t;
  /*  TODO  */
  morphTargets : void ptr;
};
```
### PuleAssetModelCamera
```c
struct {
  isPerspective : bool;
  transform : PuleF32m44;
  name : PuleString;
};
```
### PuleAssetModelAccessor
 TODO sparse 
```c
struct {
  bufferView : PuleAssetModelBufferView ptr;
  byteOffset : uint32_t;
  dataType : PuleGpuAttributeDataType;
  convertFixedToNormalized : bool;
  elementCount : uint32_t;
  elementType : PuleAssetModelElementType;
  name : PuleString;
};
```
### PuleAssetModelSkin
```c
struct {
  inverseBindMatrixAccessor : PuleAssetModelAccessor ptr;
  rootSkeletonNode : PuleAssetModelNode ptr;
  jointNodeIdxLen : uint32_t;
  jointNodeIdxes : PuleAssetModelNode ptr;
  name : PuleString;
};
```
### PuleAssetModelBufferView
```c
struct {
  buffer : PuleAssetModelBuffer ptr;
  byteOffset : uint32_t;
  byteLen : uint32_t;
  byteStride : uint32_t;
  isIndexTarget : bool;
  name : PuleString;
};
```
### PuleAssetModelBuffer
```c
struct {
  resourceUri : PuleString;
  dataView : PuleBufferView;
  name : PuleString;
};
```
### PuleAssetModelMaterialPbrMetallicRoughness
```c
struct {
  baseColorFactor : float [4];
  metallicFactor : float = 1.000000;
  roughnessFactor : float = 1.000000;
};
```
### PuleAssetModelMaterial
```c
struct {
  name : PuleString;
  pbrMetallicRoughness : PuleAssetModelMaterialPbrMetallicRoughness;
};
```
### PuleAssetModelTexture
```c
struct {
  sampler : PuleAssetModelSampler ptr;
  srcImg : PuleAssetModelImage ptr;
  name : PuleString;
};
```
### PuleAssetModelImage
```c
struct {
  resourceUri : PuleString;
  name : PuleString;
  /* .id=0 if image not requested to load */
  image : PuleAssetImage;
};
```
### PuleAssetModelSampler
```c
struct {
  filterMag : PuleAssetModelFilterMag;
  filterMin : PuleAssetModelFilterMin;
  wrapU : PuleAssetModelWrap;
  wrapV : PuleAssetModelWrap;
  name : PuleString;
};
```

## enums
### PuleErrorAssetModel
```c
enum {
  none,
  decode,
}
```
### PuleAssetModelAttribute
```c
enum {
  index,
  origin,
  normal,
  tangent,
  uvcoord_0,
  uvcoord_1,
  uvcoord_2,
  uvcoord_3,
  color_0,
  color_1,
  color_2,
  color_3,
  joints_0,
  joints_1,
  joints_2,
  joints_3,
  weights_0,
  weights_1,
  weights_2,
  weights_3,
}
```
### PuleAssetModelElementType
```c
enum {
  scalar,
  vec2,
  vec3,
  vec4,
  mat2,
  mat3,
  mat4,
}
```
### PuleAssetModelFilterMag
```c
enum {
  nearest,
  linear,
}
```
### PuleAssetModelFilterMin
```c
enum {
  nearest,
  nearestMipmapNearest,
  nearestMipmapLinear,
  linear,
  linearMipmapNearest,
  linearMipmapLinear,
}
```
### PuleAssetModelWrap
```c
enum {
  clampToEdge,
  repeat,
  repeatMirrored,
}
```

## functions
### puleAssetModelLoad
```c
puleAssetModelLoad(
  info : PuleAssetModelLoadInfo
) PuleAssetModel;
```
