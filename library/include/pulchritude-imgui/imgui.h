#pragma once

// this is an immediate mode GUI, most likely only to be used as a debug GUI
//   during development, and hidden/unused in production builds. it's meant to
//   provide a clean C ABI that lets you quickly prototype and create a lot of
//   development tools.

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-platform/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn void puleImguiInitialize(PulePlatform const window);
PULE_exportFn void puleImguiShutdown();
PULE_exportFn void puleImguiNewFrame();
PULE_exportFn void puleImguiRender();

PULE_exportFn void puleImguiJoinNext();

PULE_exportFn bool puleImguiSliderF32(
  char const * const label,
  float * const data,
  float const min, float const max
);

PULE_exportFn bool puleImguiSliderZu(
  char const * const label,
  size_t * const data,
  size_t const min, size_t const max
);

PULE_exportFn void puleImguiWindowBegin(
  char const * const labelFormat,
  bool * const openNullable
);
PULE_exportFn void puleImguiWindowEnd();

PULE_exportFn bool puleImguiSectionBegin(char const * const label);
PULE_exportFn void puleImguiSectionEnd();

PULE_exportFn void puleImguiText(char const * const format, ...);

PULE_exportFn bool puleImguiToggle(char const * const label, bool * const data);
PULE_exportFn bool puleImguiButton(char const * const label);

// usually this isn't necessary, but since everything is so heavily
//   module-based, it's easier to register callbacks to display information
//   from many different callbacks
typedef struct { uint64_t id; } PuleImguiCallbackRegistry;
typedef struct {
  void (*callback)();
} PuleImguiCallback;
PULE_exportFn PuleImguiCallbackRegistry puleImguiCallbackRegister(
  PuleImguiCallback const callback
);
PULE_exportFn void puleImguiCallbackUnregister(
  PuleImguiCallbackRegistry const registry
);
PULE_exportFn void puleImguiCallbackShowAll();

#ifdef __cplusplus
} // extern C
#endif
