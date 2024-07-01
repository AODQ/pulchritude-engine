/* auto generated file text */
#pragma once
#include "core.hpp"

#include "text.h"
#include "asset-font.hpp"
#include "gpu.hpp"
#include "math.hpp"

namespace pule {
struct TextRenderer {
  PuleTextRenderer _handle;
  inline operator PuleTextRenderer() const {
    return _handle;
  }
  inline void destroy() {
    return puleTextRendererDestroy(this->_handle);
  }
  static inline TextRenderer create(PuleAssetFont font, PuleTextType type) {
    return { ._handle = puleTextRendererCreate(font, type),};
  }
};
}
  inline void destroy(pule::TextRenderer self) {
    return puleTextRendererDestroy(self._handle);
  }
namespace pule {
using TextRenderInfo = PuleTextRenderInfo;
}
namespace pule {
using TextRender2DInfo = PuleTextRender2DInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleTextType const e) {
  switch (e) {
    case PuleTextType_bitmap: return puleString("bitmap");
    case PuleTextType_sdf: return puleString("sdf");
    default: return puleString("N/A");
  }
}
}
