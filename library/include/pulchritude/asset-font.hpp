/* auto generated file asset-font */
#pragma once
#include "core.hpp"

#include "asset-font.h"
#include "error.hpp"
#include "math.hpp"
#include "array.hpp"

namespace pule {
struct AssetFont {
  PuleAssetFont _handle;
  inline operator PuleAssetFont() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetFontDestroy(this->_handle);
  }
  static inline AssetFont load(PuleBufferView fontSource, PuleError * error) {
    return { ._handle = puleAssetFontLoad(fontSource, error),};
  }
  static inline AssetFont loadFromPath(PuleStringView path, PuleError * error) {
    return { ._handle = puleAssetFontLoadFromPath(path, error),};
  }
};
}
  inline void destroy(pule::AssetFont self) {
    return puleAssetFontDestroy(self._handle);
  }
namespace pule {
using AssetFontRenderInfo = PuleAssetFontRenderInfo;
}
namespace pule {
inline char const * toStr(PuleErrorAssetFont const e) {
  switch (e) {
    case PuleErrorAssetFont_none: return "none";
    case PuleErrorAssetFont_decode: return "decode";
    default: return "N/A";
  }
}
}
