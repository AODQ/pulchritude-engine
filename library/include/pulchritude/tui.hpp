/* auto generated file tui */
#pragma once
#include "core.h"

#include "tui.h"
#include "math.hpp"
#include "string.hpp"

namespace pule {
struct TuiWindow {
  PuleTuiWindow _handle;
  inline operator PuleTuiWindow() const {
    return _handle;
  }
  inline PuleI32v2 dim() {
    return puleTuiWindowDim(this->_handle);
  }
};
}
  inline PuleI32v2 dim(pule::TuiWindow self) {
    return puleTuiWindowDim(self._handle);
  }
namespace pule {
}
