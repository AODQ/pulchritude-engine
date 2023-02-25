#pragma once

#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

bool puleShapeIntersectRayTriangle(
  PuleF32v3 const rayOri, puleF32v3 const rayDir,
  PuleF32v3 const tri0, PuleF32v3 const tri1, PuleF32v3 const tri2,
  float * const distanceNullable, PuleF32v2 * const uvNullable
)

#ifdef __cplusplus
} // extern C
#endif
