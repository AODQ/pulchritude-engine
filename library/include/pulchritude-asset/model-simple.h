#pragma once

// this is a simple, straight-forward manner to load a model. It makes a lot
// of assumptions about renderer, but you can use it as a starting point or
// fallback for your own scene renderer.

#include <pulchritude-core/core.h>
#include <pulchritude-renderer-3d/renderer-3d.h>
#include <pulchritude-string/string.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn PuleRenderer3DModel puleAssetModelSimpleLoad(
  PuleStringView const path,
  PuleAllocator const allocator,
  PuleError * const error
);

#ifdef __cplusplus
} // extern "C"
#endif
