/* auto generated file asset-image */
#pragma once
#include "core.h"

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
  static inline AssetImage loadFromStream(PuleAllocator allocator, PuleStreamRead imageSource, PuleAssetImageFormat requestedFormat, struct PuleError * error) {
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
namespace pule {
inline char const * toStr(PuleErrorAssetImage const e) {
  switch (e) {
    case PuleErrorAssetImage_none: return "none";
    case PuleErrorAssetImage_decode: return "decode";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAssetImageSupportFlag const e) {
  switch (e) {
    case PuleAssetImageSupportFlag_none: return "none";
    case PuleAssetImageSupportFlag_read: return "read";
    case PuleAssetImageSupportFlag_write: return "write";
    case PuleAssetImageSupportFlag_readWrite: return "readWrite";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAssetImageFormat const e) {
  switch (e) {
    case PuleAssetImageFormat_rgbaU8: return "rgbaU8";
    case PuleAssetImageFormat_rgbaU16: return "rgbaU16";
    case PuleAssetImageFormat_rgbU8: return "rgbU8";
    default: return "N/A";
  }
}
}
