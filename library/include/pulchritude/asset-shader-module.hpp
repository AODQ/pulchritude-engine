/* auto generated file asset-shader-module */
#pragma once
#include "core.h"

#include "asset-shader-module.h"
#include "gpu.hpp"
#include "string.hpp"
#include "error.hpp"
#include "core.hpp"

namespace pule {
struct AssetShaderModule {
  PuleAssetShaderModule _handle;
  inline operator PuleAssetShaderModule() const {
    return _handle;
  }
  inline void destroy() {
    return puleAssetShaderModuleDestroy(this->_handle);
  }
  inline PuleStringView label() {
    return puleAssetShaderModuleLabel(this->_handle);
  }
  inline PuleGpuShaderModule handle() {
    return puleAssetShaderModuleHandle(this->_handle);
  }
  static inline AssetShaderModule createFromPaths(PuleStringView shaderModuleLabel, PuleStringView vertexPath, PuleStringView fragmentPath) {
    return { ._handle = puleAssetShaderModuleCreateFromPaths(shaderModuleLabel, vertexPath, fragmentPath),};
  }
};
}
  inline void destroy(pule::AssetShaderModule self) {
    return puleAssetShaderModuleDestroy(self._handle);
  }
  inline PuleStringView label(pule::AssetShaderModule self) {
    return puleAssetShaderModuleLabel(self._handle);
  }
  inline PuleGpuShaderModule handle(pule::AssetShaderModule self) {
    return puleAssetShaderModuleHandle(self._handle);
  }
namespace pule {
}
