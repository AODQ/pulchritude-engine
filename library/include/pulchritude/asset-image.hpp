/* auto generated file asset-image */
#pragma once
#include "core.hpp"

#include "asset-image.h"
#include "stream.hpp"
#include "allocator.hpp"
#include "error.hpp"

namespace pule {
struct AssetImage {
  PuleAssetImage _handle;
  inline operator PuleAssetImage() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetImageDestroy(this->_handle);
  }
  inline void * decodedData() {
    return puleAssetImageDecodedData(this->_handle);
  }
  inline size_t decodedDataLength() {
    return puleAssetImageDecodedDataLength(this->_handle);
  }
  inline uint32_t width() {
    return puleAssetImageWidth(this->_handle);
  }
  inline uint32_t height() {
    return puleAssetImageHeight(this->_handle);
  }
  static inline AssetImage loadFromStream(PuleAllocator allocator, PuleStreamRead imageSource, PuleAssetImageFormat requestedFormat, PuleError * error) {
    return { ._handle = puleAssetImageLoadFromStream(allocator, imageSource, requestedFormat, error),};
  }
};
}
  inline void destroy(pule::AssetImage self) {
    return puleAssetImageDestroy(self._handle);
  }
  inline void * decodedData(pule::AssetImage self) {
    return puleAssetImageDecodedData(self._handle);
  }
  inline size_t decodedDataLength(pule::AssetImage self) {
    return puleAssetImageDecodedDataLength(self._handle);
  }
  inline uint32_t width(pule::AssetImage self) {
    return puleAssetImageWidth(self._handle);
  }
  inline uint32_t height(pule::AssetImage self) {
    return puleAssetImageHeight(self._handle);
  }
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorAssetImage const e) {
  switch (e) {
    case PuleErrorAssetImage_none: return puleString("none");
    case PuleErrorAssetImage_decode: return puleString("decode");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetImageSupportFlag const e) {
  switch (e) {
    case PuleAssetImageSupportFlag_none: return puleString("none");
    case PuleAssetImageSupportFlag_read: return puleString("read");
    case PuleAssetImageSupportFlag_write: return puleString("write");
    case PuleAssetImageSupportFlag_readWrite: return puleString("readWrite");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleAssetImageFormat const e) {
  switch (e) {
    case PuleAssetImageFormat_rgbaU8: return puleString("rgbaU8");
    case PuleAssetImageFormat_rgbaU16: return puleString("rgbaU16");
    case PuleAssetImageFormat_rgbU8: return puleString("rgbU8");
    default: return puleString("N/A");
  }
}
}
