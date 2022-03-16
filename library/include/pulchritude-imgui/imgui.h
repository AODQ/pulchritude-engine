// this is an immediate mode GUI, most likely only to be used as a debug GUI
//   during development, and hidden/unused in production builds. it's meant to
//   provide a clean C ABI that lets you quickly prototype and create a lot of
//   development tools.

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-platform/platform.h>


#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn void puleImguiInitialize(PulePlatform const window);
PULE_exportFn void puleImguiShutdown();
PULE_exportFn void puleImguiNewFrame();
PULE_exportFn void puleImguiRender();

#ifdef __cplusplus
} // extern C
#endif
