/* auto generated file imgui-engine */
#pragma once
#include "core.h"

#include "ecs.h"
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs
struct PuleImguiEngineDisplayInfo;

/* 
  presents engine internal/debug information through the gui this can be used
  in conjunction with an editor and the real application
 */
typedef struct PuleImguiEngineDisplayInfo {
  PuleEcsWorld world;
  PulePlatform platform;
} PuleImguiEngineDisplayInfo;

// functions
PULE_exportFn void puleImguiEngineDisplay(PuleImguiEngineDisplayInfo info);

#ifdef __cplusplus
} // extern C
#endif
