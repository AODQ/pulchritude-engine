/* auto generated file asset-sprite */
#pragma once
#include "core.h"

#include "array.h"
#include "asset-image.h"
#include "core.h"
#include "data-serializer.h"
#include "gpu.h"
#include "math.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleAssetSpriteLibraryDataType_singleSprite = 0,
  PuleAssetSpriteLibraryDataType_animationLibrary = 1,
  PuleAssetSpriteLibraryDataType_skeleton = 2,
} PuleAssetSpriteLibraryDataType;
const uint32_t PuleAssetSpriteLibraryDataTypeSize = 3;

// entities
typedef struct PuleAssetSpritesheet { uint64_t id; } PuleAssetSpritesheet;
typedef struct PuleAssetSprite { uint64_t id; } PuleAssetSprite;
typedef struct PuleAssetSpriteLibrary { uint64_t id; } PuleAssetSpriteLibrary;
typedef struct PuleAssetSpriteTilemap { uint64_t id; } PuleAssetSpriteTilemap;
typedef struct PuleAssetSpriteInstance { uint64_t id; } PuleAssetSpriteInstance;
typedef struct PuleAssetSpriteInstanceDrawRecorder { uint64_t id; } PuleAssetSpriteInstanceDrawRecorder;

// structs
struct PuleAssetSpriteTilemapCreateInfo;
struct PuleAssetSpritesheetCreateInfo;
struct PuleAssetSpriteCreateInfo;
struct PuleAssetSpriteAnimationKeyframe;
struct PuleAssetSpriteAnimation;
struct PuleAssetSpriteLibraryAnimationLibrary;
struct PuleAssetSpriteLibrarySkeletonBone;
struct PuleAssetSpriteLibrarySkeleton;
union PuleAssetSpriteLibraryData;
struct PuleAssetSpriteLibraryCreateInfo;
struct PuleAssetSpriteInstanceCreateInfo;

typedef struct PuleAssetSpriteTilemapCreateInfo {
  PuleStringView label;
  PuleAssetSpritesheet spritesheet;
  PuleI32v2 tileTexelDimensions;
  PuleI32v2 tileCount;
} PuleAssetSpriteTilemapCreateInfo;
typedef struct PuleAssetSpritesheetCreateInfo {
  PuleStringView label;
  PuleAssetImage image;
  /* leave 0 to create this lazily, TODO {0} */
  PuleGpuImage imageGpu;
} PuleAssetSpritesheetCreateInfo;
typedef struct PuleAssetSpriteCreateInfo {
  PuleStringView label;
  PuleAssetSpritesheet spritesheet;
  PuleI32v2 texcoordUpperLeft;
  PuleI32v2 texelDims;
} PuleAssetSpriteCreateInfo;
typedef struct PuleAssetSpriteAnimationKeyframe {
  PuleAssetSprite sprite;
  PuleMillisecond timestamp;
  PuleI32v2 originOffset;
} PuleAssetSpriteAnimationKeyframe;
typedef struct PuleAssetSpriteAnimation {
  PuleStringView label;
  PuleAssetSpriteAnimationKeyframe * keyframes;
  size_t keyframeCount;
  bool loop;
} PuleAssetSpriteAnimation;
typedef struct PuleAssetSpriteLibraryAnimationLibrary {
  PuleAssetSpriteAnimation * animations;
  size_t animationCount;
} PuleAssetSpriteLibraryAnimationLibrary;
typedef struct PuleAssetSpriteLibrarySkeletonBone {
  PuleStringView name;
  size_t parentIndex;
  PuleI32v2 originOffset;
  PuleAssetSpriteLibraryAnimationLibrary animationLibrary;
} PuleAssetSpriteLibrarySkeletonBone;
typedef struct PuleAssetSpriteLibrarySkeleton {
  PuleAssetSpriteLibrarySkeletonBone * bones;
  PuleAssetSpriteLibraryAnimationLibrary * boneToSpriteLibrary;
  size_t boneCount;
} PuleAssetSpriteLibrarySkeleton;
typedef union PuleAssetSpriteLibraryData {
  PuleAssetSprite singleSprite;
  PuleAssetSpriteLibraryAnimationLibrary animationLibrary;
  PuleAssetSpriteLibrarySkeleton skeleton;
} PuleAssetSpriteLibraryData;
typedef struct PuleAssetSpriteLibraryCreateInfo {
  PuleAssetSpritesheet spritesheet;
  PuleAssetSpriteLibraryData data;
  PuleAssetSpriteLibraryDataType dataType;
} PuleAssetSpriteLibraryCreateInfo;
typedef struct PuleAssetSpriteInstanceCreateInfo {
  PuleStringView defaultAnimation;
  PuleAssetSpriteLibrary spriteLibrary;
} PuleAssetSpriteInstanceCreateInfo;

// functions
PULE_exportFn PuleAssetSpriteTilemap puleAssetSpriteTilemapCreate(PuleAssetSpriteTilemapCreateInfo ci);
PULE_exportFn void puleAssetSpriteTilemapDestroy(PuleAssetSpriteTilemap tm);
/*  tileIndex can be -1 to indicate no sprite  */
PULE_exportFn void puleAssetSpriteTilemapSetTile(PuleAssetSpriteTilemap tilemap, PuleI32v2 origin, int64_t tileIndex, bool flipX, bool flipY);
PULE_exportFn PuleAssetSpritesheet puleAssetSpritesheetCreate(PuleAssetSpritesheetCreateInfo info);
PULE_exportFn void puleAssetSpritesheetDestroy(PuleAssetSpritesheet spritesheet);
PULE_exportFn PuleAssetSprite puleAssetSpriteCreate(PuleAssetSpriteCreateInfo info);
PULE_exportFn void puleAssetSpriteDestroy(PuleAssetSprite sprite);
PULE_exportFn PuleAssetSpriteLibrary puleAssetSpriteLibraryCreate(PuleAssetSpriteLibraryCreateInfo info);
PULE_exportFn void puleAssetSpriteLibraryDestroy(PuleAssetSpriteLibrary spriteLibrary);
PULE_exportFn PuleAssetSprite puleAssetSpritesheetFetchSprite(PuleAssetSpritesheet spritesheet, PuleStringView label);
PULE_exportFn PuleAssetSpritesheet puleAssetSpriteLibraryFetchSpritesheet(PuleAssetSpriteLibrary library, PuleStringView label);
PULE_exportFn PuleAssetSpriteInstance puleAssetSpriteInstanceCreate(PuleAssetSpriteInstanceCreateInfo info);
PULE_exportFn void puleAssetSpriteInstanceDestroy(PuleAssetSpriteInstance spriteInstance);
PULE_exportFn void puleAssetSpriteInstanceAnimationSet(PuleAssetSpriteInstance spriteInstance, PuleStringView animationLabel);
PULE_exportFn void puleAssetSpriteInstanceAnimationUpdate(PuleAssetSpriteInstance spriteInstance, PuleMillisecond deltaTime);
PULE_exportFn void puleAssetSpriteInstanceSkeletonBoneAnimationSet(PuleAssetSpriteInstance spriteInstance, size_t boneIndex, PuleStringView animationLabel);
PULE_exportFn void puleAssetSpriteInstanceSkeletonBoneAnimationUpdate(PuleAssetSpriteInstance spriteInstance, size_t boneIndex, PuleMillisecond deltaTime);
PULE_exportFn PuleAssetSpriteInstanceDrawRecorder puleAssetSpriteInstanceDrawRecorder(PuleGpuCommandListRecorder commandListRecorder, PuleI32v2 viewportMin, PuleI32v2 viewportMax);
/* 
  for now can can only be called a second time after command list is finished,
  otherwise the GPU memory from the first call will be overwritten while
  the command list is in-flight
 */
PULE_exportFn void puleAssetSpriteInstanceDrawRecorderFinish(PuleAssetSpriteInstanceDrawRecorder drawRecorder);
PULE_exportFn void puleAssetSpriteInstanceDraw(PuleAssetSpriteInstanceDrawRecorder drawRecorder, PuleAssetSpriteInstance spriteInstance, PuleF32v2 position);
PULE_exportFn void puleAssetSpriteTilemapRender(PuleAssetSpriteInstanceDrawRecorder drawRecorder, PuleAssetSpriteTilemap tilemap, PuleF32v2 cameraOffset, PuleF32v4 colorMultiply);
PULE_exportFn PuleAssetSpriteLibrary puleAssetSpriteLibraryDeserialize(PuleDsValue assetPath);

#ifdef __cplusplus
} // extern C
#endif
