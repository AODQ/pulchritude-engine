# asset-sprite

## structs
### PuleAssetSpriteTilemapCreateInfo
```c
struct {
  label : PuleStringView;
  spritesheet : PuleAssetSpritesheet;
  tileTexelDimensions : PuleI32v2;
  tileCount : PuleI32v2;
};
```
### PuleAssetSpritesheetCreateInfo
```c
struct {
  label : PuleStringView;
  image : PuleAssetImage;
  /* leave 0 to create this lazily, TODO {0} */
  imageGpu : PuleGpuImage;
};
```
### PuleAssetSpriteCreateInfo
```c
struct {
  label : PuleStringView;
  spritesheet : PuleAssetSpritesheet;
  texcoordUpperLeft : PuleI32v2;
  texelDims : PuleI32v2;
};
```
### PuleAssetSpriteAnimationKeyframe
```c
struct {
  sprite : PuleAssetSprite;
  timestamp : PuleMillisecond;
  originOffset : PuleI32v2;
};
```
### PuleAssetSpriteAnimation
```c
struct {
  label : PuleStringView;
  keyframes : PuleAssetSpriteAnimationKeyframe ptr;
  keyframeCount : size_t;
  loop : bool;
};
```
### PuleAssetSpriteLibraryAnimationLibrary
```c
struct {
  animations : PuleAssetSpriteAnimation ptr;
  animationCount : size_t;
};
```
### PuleAssetSpriteLibrarySkeletonBone
```c
struct {
  name : PuleStringView;
  parentIndex : size_t;
  originOffset : PuleI32v2;
  animationLibrary : PuleAssetSpriteLibraryAnimationLibrary;
};
```
### PuleAssetSpriteLibrarySkeleton
```c
struct {
  bones : PuleAssetSpriteLibrarySkeletonBone ptr;
  boneToSpriteLibrary : PuleAssetSpriteLibraryAnimationLibrary ptr;
  boneCount : size_t;
};
```
### PuleAssetSpriteLibraryData
```c
union {
  singleSprite : PuleAssetSprite;
  animationLibrary : PuleAssetSpriteLibraryAnimationLibrary;
  skeleton : PuleAssetSpriteLibrarySkeleton;
};
```
### PuleAssetSpriteLibraryCreateInfo
```c
struct {
  spritesheet : PuleAssetSpritesheet;
  data : PuleAssetSpriteLibraryData;
  dataType : PuleAssetSpriteLibraryDataType;
};
```
### PuleAssetSpriteInstanceCreateInfo
```c
struct {
  defaultAnimation : PuleStringView;
  spriteLibrary : PuleAssetSpriteLibrary;
};
```

## enums
### PuleAssetSpriteLibraryDataType
```c
enum {
  singleSprite,
  animationLibrary,
  skeleton,
}
```

## entities
### PuleAssetSpritesheet
### PuleAssetSprite
### PuleAssetSpriteLibrary
### PuleAssetSpriteTilemap
### PuleAssetSpriteInstance
### PuleAssetSpriteInstanceDrawRecorder

## functions
### puleAssetSpriteTilemapCreate
```c
puleAssetSpriteTilemapCreate(
  ci : PuleAssetSpriteTilemapCreateInfo
) PuleAssetSpriteTilemap;
```
### puleAssetSpriteTilemapDestroy
```c
puleAssetSpriteTilemapDestroy(
  tm : PuleAssetSpriteTilemap
) void;
```
### puleAssetSpriteTilemapSetTile
 tileIndex can be -1 to indicate no sprite 
```c
puleAssetSpriteTilemapSetTile(
  tilemap : PuleAssetSpriteTilemap,
  origin : PuleI32v2,
  tileIndex : int64_t,
  flipX : bool,
  flipY : bool
) void;
```
### puleAssetSpritesheetCreate
```c
puleAssetSpritesheetCreate(
  info : PuleAssetSpritesheetCreateInfo
) PuleAssetSpritesheet;
```
### puleAssetSpritesheetDestroy
```c
puleAssetSpritesheetDestroy(
  spritesheet : PuleAssetSpritesheet
) void;
```
### puleAssetSpriteCreate
```c
puleAssetSpriteCreate(
  info : PuleAssetSpriteCreateInfo
) PuleAssetSprite;
```
### puleAssetSpriteDestroy
```c
puleAssetSpriteDestroy(
  sprite : PuleAssetSprite
) void;
```
### puleAssetSpriteLibraryCreate
```c
puleAssetSpriteLibraryCreate(
  info : PuleAssetSpriteLibraryCreateInfo
) PuleAssetSpriteLibrary;
```
### puleAssetSpriteLibraryDestroy
```c
puleAssetSpriteLibraryDestroy(
  spriteLibrary : PuleAssetSpriteLibrary
) void;
```
### puleAssetSpritesheetFetchSprite
```c
puleAssetSpritesheetFetchSprite(
  spritesheet : PuleAssetSpritesheet,
  label : PuleStringView
) PuleAssetSprite;
```
### puleAssetSpriteLibraryFetchSpritesheet
```c
puleAssetSpriteLibraryFetchSpritesheet(
  library : PuleAssetSpriteLibrary,
  label : PuleStringView
) PuleAssetSpritesheet;
```
### puleAssetSpriteInstanceCreate
```c
puleAssetSpriteInstanceCreate(
  info : PuleAssetSpriteInstanceCreateInfo
) PuleAssetSpriteInstance;
```
### puleAssetSpriteInstanceDestroy
```c
puleAssetSpriteInstanceDestroy(
  spriteInstance : PuleAssetSpriteInstance
) void;
```
### puleAssetSpriteInstanceAnimationSet
```c
puleAssetSpriteInstanceAnimationSet(
  spriteInstance : PuleAssetSpriteInstance,
  animationLabel : PuleStringView
) void;
```
### puleAssetSpriteInstanceAnimationUpdate
```c
puleAssetSpriteInstanceAnimationUpdate(
  spriteInstance : PuleAssetSpriteInstance,
  deltaTime : PuleMillisecond
) void;
```
### puleAssetSpriteInstanceSkeletonBoneAnimationSet
```c
puleAssetSpriteInstanceSkeletonBoneAnimationSet(
  spriteInstance : PuleAssetSpriteInstance,
  boneIndex : size_t,
  animationLabel : PuleStringView
) void;
```
### puleAssetSpriteInstanceSkeletonBoneAnimationUpdate
```c
puleAssetSpriteInstanceSkeletonBoneAnimationUpdate(
  spriteInstance : PuleAssetSpriteInstance,
  boneIndex : size_t,
  deltaTime : PuleMillisecond
) void;
```
### puleAssetSpriteInstanceDrawRecorder
```c
puleAssetSpriteInstanceDrawRecorder(
  commandListRecorder : PuleGpuCommandListRecorder,
  viewportMin : PuleI32v2,
  viewportMax : PuleI32v2
) PuleAssetSpriteInstanceDrawRecorder;
```
### puleAssetSpriteInstanceDrawRecorderFinish

  for now can can only be called a second time after command list is finished,
  otherwise the GPU memory from the first call will be overwritten while
  the command list is in-flight

```c
puleAssetSpriteInstanceDrawRecorderFinish(
  drawRecorder : PuleAssetSpriteInstanceDrawRecorder
) void;
```
### puleAssetSpriteInstanceDraw
```c
puleAssetSpriteInstanceDraw(
  drawRecorder : PuleAssetSpriteInstanceDrawRecorder,
  spriteInstance : PuleAssetSpriteInstance,
  position : PuleF32v2
) void;
```
### puleAssetSpriteTilemapRender
```c
puleAssetSpriteTilemapRender(
  drawRecorder : PuleAssetSpriteInstanceDrawRecorder,
  tilemap : PuleAssetSpriteTilemap,
  cameraOffset : PuleF32v2,
  colorMultiply : PuleF32v4
) void;
```
### puleAssetSpriteLibraryDeserialize
```c
puleAssetSpriteLibraryDeserialize(
  assetPath : PuleDsValue
) PuleAssetSpriteLibrary;
```
