/* auto generated file asset-model */
#pragma once
#include "core.hpp"

#include "asset-model.h"
#include "allocator.hpp"
#include "asset-image.hpp"
#include "data-serializer.hpp"
#include "error.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "string.hpp"

namespace pule {
using AssetModelLoadInfo = PuleAssetModelLoadInfo;
}
namespace pule {
struct AssetModel {
  PuleAssetModel _handle;
  inline operator PuleAssetModel() const {
    return _handle;
  }
  static inline AssetModel load(PuleAssetModelLoadInfo info) {
    return { ._handle = puleAssetModelLoad(info),};
  }
};
}
namespace pule {
using AssetModelScene = PuleAssetModelScene;
}
namespace pule {
using AssetModelNode = PuleAssetModelNode;
}
namespace pule {
using AssetModelMesh = PuleAssetModelMesh;
}
namespace pule {
using AssetModelMorphTarget = PuleAssetModelMorphTarget;
}
namespace pule {
using AssetModelMeshPrimitive = PuleAssetModelMeshPrimitive;
}
namespace pule {
using AssetModelCamera = PuleAssetModelCamera;
}
namespace pule {
using AssetModelAccessor = PuleAssetModelAccessor;
}
namespace pule {
using AssetModelSkin = PuleAssetModelSkin;
}
namespace pule {
using AssetModelBufferView = PuleAssetModelBufferView;
}
namespace pule {
using AssetModelBuffer = PuleAssetModelBuffer;
}
namespace pule {
using AssetModelMaterialPbrMetallicRoughness = PuleAssetModelMaterialPbrMetallicRoughness;
}
namespace pule {
using AssetModelMaterial = PuleAssetModelMaterial;
}
namespace pule {
using AssetModelTexture = PuleAssetModelTexture;
}
namespace pule {
using AssetModelImage = PuleAssetModelImage;
}
namespace pule {
using AssetModelAnimationChannel = PuleAssetModelAnimationChannel;
}
namespace pule {
using AssetModelAnimationSampler = PuleAssetModelAnimationSampler;
}
namespace pule {
using AssetModelAnimation = PuleAssetModelAnimation;
}
namespace pule {
using AssetModelSampler = PuleAssetModelSampler;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorAssetModel const e) {
  switch (e) {
    case PuleErrorAssetModel_none: return puleString("none");
    case PuleErrorAssetModel_decode: return puleString("decode");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelAttribute const e) {
  switch (e) {
    case PuleAssetModelAttribute_origin: return puleString("origin");
    case PuleAssetModelAttribute_normal: return puleString("normal");
    case PuleAssetModelAttribute_tangent: return puleString("tangent");
    case PuleAssetModelAttribute_uvcoord_0: return puleString("uvcoord_0");
    case PuleAssetModelAttribute_uvcoord_1: return puleString("uvcoord_1");
    case PuleAssetModelAttribute_uvcoord_2: return puleString("uvcoord_2");
    case PuleAssetModelAttribute_uvcoord_3: return puleString("uvcoord_3");
    case PuleAssetModelAttribute_color_0: return puleString("color_0");
    case PuleAssetModelAttribute_color_1: return puleString("color_1");
    case PuleAssetModelAttribute_color_2: return puleString("color_2");
    case PuleAssetModelAttribute_color_3: return puleString("color_3");
    case PuleAssetModelAttribute_joints_0: return puleString("joints_0");
    case PuleAssetModelAttribute_joints_1: return puleString("joints_1");
    case PuleAssetModelAttribute_joints_2: return puleString("joints_2");
    case PuleAssetModelAttribute_joints_3: return puleString("joints_3");
    case PuleAssetModelAttribute_weights_0: return puleString("weights_0");
    case PuleAssetModelAttribute_weights_1: return puleString("weights_1");
    case PuleAssetModelAttribute_weights_2: return puleString("weights_2");
    case PuleAssetModelAttribute_weights_3: return puleString("weights_3");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelElementType const e) {
  switch (e) {
    case PuleAssetModelElementType_scalar: return puleString("scalar");
    case PuleAssetModelElementType_vec2: return puleString("vec2");
    case PuleAssetModelElementType_vec3: return puleString("vec3");
    case PuleAssetModelElementType_vec4: return puleString("vec4");
    case PuleAssetModelElementType_mat2: return puleString("mat2");
    case PuleAssetModelElementType_mat3: return puleString("mat3");
    case PuleAssetModelElementType_mat4: return puleString("mat4");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelBufferViewUsage const e) {
  switch (e) {
    case PuleAssetModelBufferViewUsage_none: return puleString("none");
    case PuleAssetModelBufferViewUsage_attribute: return puleString("attribute");
    case PuleAssetModelBufferViewUsage_elementIdx: return puleString("elementIdx");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelAnimationInterpolation const e) {
  switch (e) {
    case PuleAssetModelAnimationInterpolation_linear: return puleString("linear");
    case PuleAssetModelAnimationInterpolation_step: return puleString("step");
    case PuleAssetModelAnimationInterpolation_cubicspline: return puleString("cubicspline");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelAnimationTarget const e) {
  switch (e) {
    case PuleAssetModelAnimationTarget_translation: return puleString("translation");
    case PuleAssetModelAnimationTarget_rotation: return puleString("rotation");
    case PuleAssetModelAnimationTarget_scale: return puleString("scale");
    case PuleAssetModelAnimationTarget_weights: return puleString("weights");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelFilterMag const e) {
  switch (e) {
    case PuleAssetModelFilterMag_nearest: return puleString("nearest");
    case PuleAssetModelFilterMag_linear: return puleString("linear");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelFilterMin const e) {
  switch (e) {
    case PuleAssetModelFilterMin_nearest: return puleString("nearest");
    case PuleAssetModelFilterMin_nearestMipmapNearest: return puleString("nearestMipmapNearest");
    case PuleAssetModelFilterMin_nearestMipmapLinear: return puleString("nearestMipmapLinear");
    case PuleAssetModelFilterMin_linear: return puleString("linear");
    case PuleAssetModelFilterMin_linearMipmapNearest: return puleString("linearMipmapNearest");
    case PuleAssetModelFilterMin_linearMipmapLinear: return puleString("linearMipmapLinear");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetModelWrap const e) {
  switch (e) {
    case PuleAssetModelWrap_clampToEdge: return puleString("clampToEdge");
    case PuleAssetModelWrap_repeat: return puleString("repeat");
    case PuleAssetModelWrap_repeatMirrored: return puleString("repeatMirrored");
    default: return puleString("N/A");
  }
}
}
