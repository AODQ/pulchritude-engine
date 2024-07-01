/* auto generated file gfx-debug */
#pragma once
#include "core.hpp"

#include "gfx-debug.h"
#include "error.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "platform.hpp"

namespace pule {
using GfxDebugRecorder = PuleGfxDebugRecorder;
}
namespace pule {
using GfxDebugSubmitInfo = PuleGfxDebugSubmitInfo;
}
namespace pule {
using GfxDebugRenderLine = PuleGfxDebugRenderLine;
}
namespace pule {
using GfxDebugRenderQuad = PuleGfxDebugRenderQuad;
}
namespace pule {
using GfxDebugRenderCube = PuleGfxDebugRenderCube;
}
namespace pule {
using GfxDebugRenderSphere = PuleGfxDebugRenderSphere;
}
namespace pule {
using GfxDebugRenderPlane = PuleGfxDebugRenderPlane;
}
namespace pule {
using GfxDebugRenderParam = PuleGfxDebugRenderParam;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleGfxDebugRenderType const e) {
  switch (e) {
    case PuleGfxDebugRenderType_line: return puleString("line");
    case PuleGfxDebugRenderType_quad: return puleString("quad");
    case PuleGfxDebugRenderType_cube: return puleString("cube");
    case PuleGfxDebugRenderType_sphere: return puleString("sphere");
    case PuleGfxDebugRenderType_plane: return puleString("plane");
    default: return puleString("N/A");
  }
}
}
