#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleWindowVsyncMode_none,
  PuleWindowVsyncMode_single,
  PuleWindowVsyncMode_triple,
  PuleWindowVsyncMode_End,
  PuleWindowVsyncMode_MaxEnum = 0x7FFFFFFF,
} PuleWindowVsyncMode;

typedef enum {
  PuleErrorWindow_windowCreationFailed,
  PuleErrorWindow_invalidConfiguration,
} PuleErrorWindow;

typedef struct {
  PuleStringView name;
  size_t width; // <= 7680, can leave at 0 for full screen
  size_t height; // <= 4320, can leave at 0 for full screen
  PuleWindowVsyncMode vsyncMode;
} PuleWindowCreateInfo;

typedef struct {
  void * data;
} PuleWindow;

PULE_exportFn PuleWindow puleWindowCreate(
  PuleWindowCreateInfo const info, PuleError * const error
);
PULE_exportFn void puleWindowDestroy(
  PuleWindow const window
);

PULE_exportFn void puleWindowPollEvents(PuleWindow const window);
PULE_exportFn void puleWindowSwapFramebuffer(PuleWindow const window);

#ifdef __cplusplus
} // extern C
#endif
