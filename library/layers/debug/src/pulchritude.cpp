#include <pulchritude-ecs/ecs.h>

#include "../include/debug.hpp"

#include <pulchritude-log/log.h>
#include <pulchritude-math/math.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-tui/tui.h>
#include <pulchritude-ecs-serializer/ecs-serializer.h>
#include <pulchritude-ecs/ecs.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-asset/model.h>
#include <pulchritude-asset/tiled.h>
#include <pulchritude-asset/font.h>
#include <pulchritude-asset/image.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-stream/stream.h>
#include <pulchritude-array/array.h>
#include <pulchritude-allocator/allocator.h>
#include <pulchritude-imgui-engine/imgui-engine.h>
#include <pulchritude-gfx-debug/gfx-debug.h>
#include <pulchritude-error/error.h>
#include <pulchritude-string/string.h>
#include <pulchritude-renderer-3d/renderer-3d.h>
#include <pulchritude-file/file.h>
#include <pulchritude-script/script.h>
#include <pulchritude-plugin/plugin.h>
#include <pulchritude-plugin/plugin-payload.h>
#include <pulchritude-time/time.h>
#include <pulchritude-gpu/pipeline.h>
#include <pulchritude-gpu/shader-module.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/synchronization.h>

//#include <volk.h>

#include <GLFW/glfw3.h>

#include <string>
#include <thread>
#include <unordered_map>

static PuleEngineLayer pul;
static std::thread::id renderThread;

extern "C" {
void puleEngineLayerEntry(PuleEngineLayer * const parentLayer) {
  puleLog("Using layer entry: %s", parentLayer->layerName.contents);
  pul = *parentLayer;
}
// log
bool * puleLogDebugEnabled() {
  debugLayerFnEntry();
  return pul.logDebugEnabled();
}

// no variadic generation: puleLog
// no variadic generation: puleLogDebug
// no variadic generation: puleLogError
// no variadic generation: puleLogLn
// no variadic generation: puleLogRaw
}
