/* auto generated file imgui-engine */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/ecs.h>
#include <pulchritude/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
/* 
  presents engine internal/debug information through the gui this can be used
  in conjunction with an editor and the real application
 */
typedef struct {
  PuleEcsWorld world;
  PulePlatform platform;
} PuleImguiEngineDisplayInfo;

// enum

// entities

// functions
PULE_exportFn void puleImguiEngineDisplay(PuleImguiEngineDisplayInfo info);

#ifdef __cplusplus
} // extern C
#endif
