/* auto generated file asset-sprite */
#pragma once
#include "core.hpp"

#include "asset-sprite.h"
#include "array.hpp"
#include "asset-image.hpp"
#include "core.hpp"
#include "data-serializer.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "time.hpp"

namespace pule {
struct AssetSpritesheet {
  PuleAssetSpritesheet _handle;
  inline operator PuleAssetSpritesheet() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetSpritesheetDestroy(this->_handle);
  }
  inline PuleAssetSprite fetchSprite(PuleStringView label) {
    return puleAssetSpritesheetFetchSprite(this->_handle, label);
  }
  static inline AssetSpritesheet create(PuleAssetSpritesheetCreateInfo info) {
    return { ._handle = puleAssetSpritesheetCreate(info),};
  }
};
}
  inline void destroy(pule::AssetSpritesheet self) {
    return puleAssetSpritesheetDestroy(self._handle);
  }
  inline PuleAssetSprite fetchSprite(pule::AssetSpritesheet self, PuleStringView label) {
    return puleAssetSpritesheetFetchSprite(self._handle, label);
  }
namespace pule {
struct AssetSprite {
  PuleAssetSprite _handle;
  inline operator PuleAssetSprite() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetSpriteDestroy(this->_handle);
  }
  static inline AssetSprite create(PuleAssetSpriteCreateInfo info) {
    return { ._handle = puleAssetSpriteCreate(info),};
  }
  static inline AssetSprite sheetFetchSprite(PuleAssetSpritesheet spritesheet, PuleStringView label) {
    return { ._handle = puleAssetSpritesheetFetchSprite(spritesheet, label),};
  }
};
}
  inline void destroy(pule::AssetSprite self) {
    return puleAssetSpriteDestroy(self._handle);
  }
namespace pule {
struct AssetSpriteLibrary {
  PuleAssetSpriteLibrary _handle;
  inline operator PuleAssetSpriteLibrary() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetSpriteLibraryDestroy(this->_handle);
  }
  inline PuleAssetSpritesheet fetchSpritesheet(PuleStringView label) {
    return puleAssetSpriteLibraryFetchSpritesheet(this->_handle, label);
  }
  static inline AssetSpriteLibrary create(PuleAssetSpriteLibraryCreateInfo info) {
    return { ._handle = puleAssetSpriteLibraryCreate(info),};
  }
  static inline AssetSpriteLibrary deserialize(PuleDsValue assetPath) {
    return { ._handle = puleAssetSpriteLibraryDeserialize(assetPath),};
  }
};
}
  inline void destroy(pule::AssetSpriteLibrary self) {
    return puleAssetSpriteLibraryDestroy(self._handle);
  }
  inline PuleAssetSpritesheet fetchSpritesheet(pule::AssetSpriteLibrary self, PuleStringView label) {
    return puleAssetSpriteLibraryFetchSpritesheet(self._handle, label);
  }
namespace pule {
struct AssetSpriteTilemap {
  PuleAssetSpriteTilemap _handle;
  inline operator PuleAssetSpriteTilemap() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetSpriteTilemapDestroy(this->_handle);
  }
  inline void setTile(PuleI32v2 origin, int64_t tileIndex, bool flipX, bool flipY) {
    return puleAssetSpriteTilemapSetTile(this->_handle, origin, tileIndex, flipX, flipY);
  }
  static inline AssetSpriteTilemap create(PuleAssetSpriteTilemapCreateInfo ci) {
    return { ._handle = puleAssetSpriteTilemapCreate(ci),};
  }
};
}
  inline void destroy(pule::AssetSpriteTilemap self) {
    return puleAssetSpriteTilemapDestroy(self._handle);
  }
  inline void setTile(pule::AssetSpriteTilemap self, PuleI32v2 origin, int64_t tileIndex, bool flipX, bool flipY) {
    return puleAssetSpriteTilemapSetTile(self._handle, origin, tileIndex, flipX, flipY);
  }
namespace pule {
struct AssetSpriteInstance {
  PuleAssetSpriteInstance _handle;
  inline operator PuleAssetSpriteInstance() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetSpriteInstanceDestroy(this->_handle);
  }
  inline void animationSet(PuleStringView animationLabel) {
    return puleAssetSpriteInstanceAnimationSet(this->_handle, animationLabel);
  }
  inline void animationUpdate(PuleMillisecond deltaTime) {
    return puleAssetSpriteInstanceAnimationUpdate(this->_handle, deltaTime);
  }
  inline void skeletonBoneAnimationSet(size_t boneIndex, PuleStringView animationLabel) {
    return puleAssetSpriteInstanceSkeletonBoneAnimationSet(this->_handle, boneIndex, animationLabel);
  }
  inline void skeletonBoneAnimationUpdate(size_t boneIndex, PuleMillisecond deltaTime) {
    return puleAssetSpriteInstanceSkeletonBoneAnimationUpdate(this->_handle, boneIndex, deltaTime);
  }
  static inline AssetSpriteInstance create(PuleAssetSpriteInstanceCreateInfo info) {
    return { ._handle = puleAssetSpriteInstanceCreate(info),};
  }
};
}
  inline void destroy(pule::AssetSpriteInstance self) {
    return puleAssetSpriteInstanceDestroy(self._handle);
  }
  inline void animationSet(pule::AssetSpriteInstance self, PuleStringView animationLabel) {
    return puleAssetSpriteInstanceAnimationSet(self._handle, animationLabel);
  }
  inline void animationUpdate(pule::AssetSpriteInstance self, PuleMillisecond deltaTime) {
    return puleAssetSpriteInstanceAnimationUpdate(self._handle, deltaTime);
  }
  inline void skeletonBoneAnimationSet(pule::AssetSpriteInstance self, size_t boneIndex, PuleStringView animationLabel) {
    return puleAssetSpriteInstanceSkeletonBoneAnimationSet(self._handle, boneIndex, animationLabel);
  }
  inline void skeletonBoneAnimationUpdate(pule::AssetSpriteInstance self, size_t boneIndex, PuleMillisecond deltaTime) {
    return puleAssetSpriteInstanceSkeletonBoneAnimationUpdate(self._handle, boneIndex, deltaTime);
  }
namespace pule {
struct AssetSpriteInstanceDrawRecorder {
  PuleAssetSpriteInstanceDrawRecorder _handle;
  inline operator PuleAssetSpriteInstanceDrawRecorder() const {
    return _handle;
  }
  inline void finish() {
    return puleAssetSpriteInstanceDrawRecorderFinish(this->_handle);
  }
};
}
  inline void finish(pule::AssetSpriteInstanceDrawRecorder self) {
    return puleAssetSpriteInstanceDrawRecorderFinish(self._handle);
  }
namespace pule {
using AssetSpriteTilemapCreateInfo = PuleAssetSpriteTilemapCreateInfo;
}
namespace pule {
using AssetSpritesheetCreateInfo = PuleAssetSpritesheetCreateInfo;
}
namespace pule {
using AssetSpriteCreateInfo = PuleAssetSpriteCreateInfo;
}
namespace pule {
using AssetSpriteAnimationKeyframe = PuleAssetSpriteAnimationKeyframe;
}
namespace pule {
using AssetSpriteAnimation = PuleAssetSpriteAnimation;
}
namespace pule {
using AssetSpriteLibraryAnimationLibrary = PuleAssetSpriteLibraryAnimationLibrary;
}
namespace pule {
using AssetSpriteLibrarySkeletonBone = PuleAssetSpriteLibrarySkeletonBone;
}
namespace pule {
using AssetSpriteLibrarySkeleton = PuleAssetSpriteLibrarySkeleton;
}
namespace pule {
using AssetSpriteLibraryData = PuleAssetSpriteLibraryData;
}
namespace pule {
using AssetSpriteLibraryCreateInfo = PuleAssetSpriteLibraryCreateInfo;
}
namespace pule {
using AssetSpriteInstanceCreateInfo = PuleAssetSpriteInstanceCreateInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleAssetSpriteLibraryDataType const e) {
  switch (e) {
    case PuleAssetSpriteLibraryDataType_singleSprite: return puleString("singleSprite");
    case PuleAssetSpriteLibraryDataType_animationLibrary: return puleString("animationLibrary");
    case PuleAssetSpriteLibraryDataType_skeleton: return puleString("skeleton");
    default: return puleString("N/A");
  }
}
}
