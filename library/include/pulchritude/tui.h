/* auto generated file tui */
#pragma once
#include "core.h"

#include "math.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities
typedef struct PuleTuiWindow { uint64_t id; } PuleTuiWindow;

// structs


// functions
PULE_exportFn PuleTuiWindow puleTuiInitialize();
PULE_exportFn void puleTuiDestroy(PuleTuiWindow);
PULE_exportFn void puleTuiClear(PuleTuiWindow);
PULE_exportFn void puleTuiRefresh(PuleTuiWindow);
PULE_exportFn void puleTuiRenderString(PuleTuiWindow, PuleStringView content, PuleF32v3 rgb);
PULE_exportFn int32_t puleTuiReadInputCharacterBlocking(PuleTuiWindow window);
PULE_exportFn void puleTuiMoveCursor(PuleTuiWindow window, PuleI32v2 position);
PULE_exportFn PuleI32v2 puleTuiWindowDim(PuleTuiWindow window);

#ifdef __cplusplus
} // extern C
#endif
