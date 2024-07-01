/* auto generated file imgui */
#pragma once
#include "core.hpp"

#include "imgui.h"
#include "error.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "platform.hpp"
#include "render-graph.hpp"

namespace pule {
using ImguiCallbackRegistry = PuleImguiCallbackRegistry;
}
namespace pule {
struct ImguiCallback {
  PuleImguiCallback _handle;
  inline operator PuleImguiCallback() const {
    return _handle;
  }
  inline PuleImguiCallbackRegistry register() {
    return puleImguiCallbackRegister(this->_handle);
  }
};
}
  inline PuleImguiCallbackRegistry register(pule::ImguiCallback self) {
    return puleImguiCallbackRegister(self._handle);
  }
