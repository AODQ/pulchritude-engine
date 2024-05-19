/* auto generated file imgui */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/gpu.h>
#include <pulchritude/math.h>
#include <pulchritude/platform.h>
#include <pulchritude/render-graph.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  void(* callback)();
} PuleImguiCallback;

// enum

// entities
/* 
usually this isn't necessary, but since everything is so heavily
module-based, it's easier to register callbacks to display information
from many different callbacks
 */
typedef struct { uint64_t id; } PuleImguiCallbackRegistry;

// functions
PULE_exportFn void puleImguiInitialize(PulePlatform window);
PULE_exportFn void puleImguiShutdown();
PULE_exportFn void puleImguiNewFrame();
PULE_exportFn void puleImguiRender(PuleRenderGraph renderGraph);
PULE_exportFn void puleImguiJoinNext();
PULE_exportFn bool puleImguiSliderF32(char const * label, float * data, float min, float max);
PULE_exportFn bool puleImguiSliderZu(char const * label, size_t * data, size_t min, size_t max);
PULE_exportFn void puleImguiWindowBegin(char const * labelFormat, bool * openNullable);
PULE_exportFn bool puleImguiSectionBegin(char const * label);
PULE_exportFn void puleImguiSectionEnd();
PULE_exportFn void puleImguiText(char const * format, ...);
PULE_exportFn void puleImguiImage(PuleGpuImage image, PuleF32v2 size, PuleF32v2 uvMin, PuleF32v2 uvMax, PuleF32v4 borderColor);
PULE_exportFn bool puleImguiLastItemHovered();
PULE_exportFn PuleI32v2 puleImguiCurrentOrigin();
PULE_exportFn bool puleImguiToggle(char const * label, bool * data);
PULE_exportFn bool puleImguiButton(char const * label);
PULE_exportFn PuleImguiCallbackRegistry puleImguiCallbackRegister(PuleImguiCallback callback);
PULE_exportFn void puleImguiCallbackUnregister(PuleImguiCallbackRegistry registry);
PULE_exportFn void puleImguiCallbackShowAll();

#ifdef __cplusplus
} // extern C
#endif
