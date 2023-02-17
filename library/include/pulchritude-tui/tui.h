#pragma once

// this provides a TUI, mostly meant to be command-line driven

#include <pulchritude-core/core.h>
#include <pulchritude-math/math.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t id;
} PuleTuiWindow;

PULE_exportFn PuleTuiWindow puleTuiInitialize();
PULE_exportFn void puleTuiDestroy(PuleTuiWindow const window);

PULE_exportFn void puleTuiClear(PuleTuiWindow const window);
PULE_exportFn void puleTuiRefresh(PuleTuiWindow const window);
PULE_exportFn void puleTuiRenderString(
  PuleTuiWindow const window,
  PuleStringView const content,
  PuleF32v3 const rgb
);
//PULE_exportFn int32_t puleTuiReadInputCharacter(PuleTuiWindow const window);
PULE_exportFn int32_t puleTuiReadInputCharacterBlocking(
  PuleTuiWindow const window
);
PULE_exportFn void puleTuiMoveCursor(
  PuleTuiWindow const window, PuleI32v2 const position
);

PULE_exportFn PuleI32v2 puleTuiWindowDim(PuleTuiWindow const window);


#ifdef __cplusplus
}
#endif
