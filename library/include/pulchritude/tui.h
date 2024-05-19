/* auto generated file tui */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/math.h>
#include <pulchritude/string.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs

// enum

// entities
typedef struct { uint64_t id; } PuleTuiWindow;

// functions
PULE_exportFn PuleTuiWindow puleTuiInitialize();
PULE_exportFn void puleTuiDestroy(PuleTuiWindow _);
PULE_exportFn void puleTuiClear(PuleTuiWindow _);
PULE_exportFn void puleTuiRefresh(PuleTuiWindow _);
PULE_exportFn void puleTuiRenderString(PuleTuiWindow _, PuleStringView content, PuleF32v3 rgb);
PULE_exportFn int32_t puleTuiReadInputCharacterBlocking(PuleTuiWindow window);
PULE_exportFn void puleTuiMoveCursor(PuleTuiWindow window, PuleI32v2 position);
PULE_exportFn PuleI32v2 puleTuiWindowDim(PuleTuiWindow window);

#ifdef __cplusplus
} // extern C
#endif
