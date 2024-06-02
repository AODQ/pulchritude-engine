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
using AssetModelSampler = PuleAssetModelSampler;
}
namespace pule {
inline char const * toStr(PuleErrorAssetModel const e) {
  switch (e) {
    case PuleErrorAssetModel_none: return "none";
    case PuleErrorAssetModel_decode: return "decode";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAssetModelAttribute const e) {
  switch (e) {
    case PuleAssetModelAttribute_index: return "index";
    case PuleAssetModelAttribute_origin: return "origin";
    case PuleAssetModelAttribute_normal: return "normal";
    case PuleAssetModelAttribute_tangent: return "tangent";
    case PuleAssetModelAttribute_uvcoord_0: return "uvcoord_0";
    case PuleAssetModelAttribute_uvcoord_1: return "uvcoord_1";
    case PuleAssetModelAttribute_uvcoord_2: return "uvcoord_2";
    case PuleAssetModelAttribute_uvcoord_3: return "uvcoord_3";
    case PuleAssetModelAttribute_color_0: return "color_0";
    case PuleAssetModelAttribute_color_1: return "color_1";
    case PuleAssetModelAttribute_color_2: return "color_2";
    case PuleAssetModelAttribute_color_3: return "color_3";
    case PuleAssetModelAttribute_joints_0: return "joints_0";
    case PuleAssetModelAttribute_joints_1: return "joints_1";
    case PuleAssetModelAttribute_joints_2: return "joints_2";
    case PuleAssetModelAttribute_joints_3: return "joints_3";
    case PuleAssetModelAttribute_weights_0: return "weights_0";
    case PuleAssetModelAttribute_weights_1: return "weights_1";
    case PuleAssetModelAttribute_weights_2: return "weights_2";
    case PuleAssetModelAttribute_weights_3: return "weights_3";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAssetModelElementType const e) {
  switch (e) {
    case PuleAssetModelElementType_scalar: return "scalar";
    case PuleAssetModelElementType_vec2: return "vec2";
    case PuleAssetModelElementType_vec3: return "vec3";
    case PuleAssetModelElementType_vec4: return "vec4";
    case PuleAssetModelElementType_mat2: return "mat2";
    case PuleAssetModelElementType_mat3: return "mat3";
    case PuleAssetModelElementType_mat4: return "mat4";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAssetModelFilterMag const e) {
  switch (e) {
    case PuleAssetModelFilterMag_nearest: return "nearest";
    case PuleAssetModelFilterMag_linear: return "linear";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAssetModelFilterMin const e) {
  switch (e) {
    case PuleAssetModelFilterMin_nearest: return "nearest";
    case PuleAssetModelFilterMin_nearestMipmapNearest: return "nearestMipmapNearest";
    case PuleAssetModelFilterMin_nearestMipmapLinear: return "nearestMipmapLinear";
    case PuleAssetModelFilterMin_linear: return "linear";
    case PuleAssetModelFilterMin_linearMipmapNearest: return "linearMipmapNearest";
    case PuleAssetModelFilterMin_linearMipmapLinear: return "linearMipmapLinear";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAssetModelWrap const e) {
  switch (e) {
    case PuleAssetModelWrap_clampToEdge: return "clampToEdge";
    case PuleAssetModelWrap_repeat: return "repeat";
    case PuleAssetModelWrap_repeatMirrored: return "repeatMirrored";
    default: return "N/A";
  }
}
}
