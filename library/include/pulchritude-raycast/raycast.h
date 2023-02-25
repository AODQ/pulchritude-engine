#pragma once

#include <pulchritude-array/array.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool hit;
  float distance;
  PuleF32v3 origin;
  PuleF32v2 uv;
} PuleRaycastTriangleResult;

PULE_exportFn PuleRaycastTriangleResult puleRaycastTriangles(
  PuleF32v3 const ori, PuleF32v3 const dir,
  PuleArrayView const originVerticesF32v3
);

#ifdef __cplusplus
} // extern C
#endif
