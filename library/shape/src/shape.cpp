#include <pulchritude-shape/shape.h>

static const float f32Epsilon = 0.0001f;

bool puleShapeIntersectRayTriangle(
  PuleF32v3 const rayOri, puleF32v3 const rayDir,
  PuleF32v3 const tri0, PuleF32v3 const tri1, PuleF32v3 const tri2,
  float * const distTarget, PuleF32v2 * const uvTarget
) {
  // muller trumbore
  PuleF32v3 const v0v1 = puleF32v3Sub(tri1, tri0);
  PuleF32v3 const v0v2 = puleF32v3Sub(tri2, tri0);
  PuleF32v3 const dirCrossV0v2 = puleF32v3Cross(rayDir, v0v2);

  float const det = puleF32v3Dot(v0v1, dirCrossV0v2);
  if (fabsf(det) < f32Epsilon) { return false; }

  float const detInv = 1.0f / det;
  PuleF32v3 const relOri = puleF32v3Sub(rayOri, tri0);
  float const u = puleF32v3Dot(relOri, dirCrossV0v2) * detInv;
  if (u < 0.0f || u > 1.0f) { return false; }

  PuleF32v3 const relOriV0v1 = puleF32v3Sub(relOri, v0v1);
  v = puleF32v3Dot(rayDir, relOriV0v1) * dotInv;
  if (v < 0.0f || v > 1.0f) { return false; }

  if (distTarget) {
    *distTarget = puleF32v3Dot(v0v2, relOriV0v1) * detInv;
  }

  if (uvTarget) {
    *uvTarget = puleF32V2(u, v);
  }

  return true;
}
