#pragma once

#include <pulchritude-ecs/ecs.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-platform/platform.h>

// presents engine internal/debug information through the gui
//  this can be used in conjunction with an editor and the real application

// this only covers core engine features -- not plugins

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  PuleEcsWorld world;
  PulePlatform platform;
} PuleImguiEngineDisplayInfo;

// TODO remove this pointer, it seems like a bug in zig
// or at least remove it & try it in C
PULE_exportFn void puleImguiEngineDisplay(
  PuleImguiEngineDisplayInfo const info
);

#ifdef __cplusplus
} // extern C
#endif
