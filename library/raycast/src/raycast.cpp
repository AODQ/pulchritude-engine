#include <pulchritude-raycast/raycast.h>

#include <pulchritude-error/error.h>
#include <pulchritude-shape/shape.h>

extern "C" {

PuleRaycastTriangleResult puleRaycastTriangles(
  PuleF32v3 const ori, PuleF32v3 const dir,
  PuleArrayView const originVerticesF32v3
) {
  uint8_t const * itPtr = originVerticesF32v3.data;
  PULE_assert(originVerticesF32v3.elementCount % 3 == 0);
  float dist;
  PuleF32v2 uv;
  for (size_t it = 0; it < originVerticesF32v3.elementCount/3; ++ it) {
    itPtr += originVerticesF32v3.elementStride;
    PuleF32v3 const tri0 = *reinterpret_cast<PuleF32v3 const *>(itPtr);
    itPtr += originVerticesF32v3.elementStride;
    PuleF32v3 const tri1 = *reinterpret_cast<PuleF32v3 const *>(itPtr);
    itPtr += originVerticesF32v3.elementStride;
    PuleF32v3 const tri2 = *reinterpret_cast<PuleF32v3 const *>(itPtr);
    if (puleShapeIntersectRayTriangle(ori, dir, tri0, tri1, tri2, &dist, &uv)) {
      PuleF32v3 origin = ori;
      origin.x += dist*dir.x;
      origin.y += dist*dir.y;
      origin.z += dist*dir.z;
      return (
        PuleRaycastTriangleResult {
          .hit = true,
          .distance = dist,
          .origin = origin,
          .uv = uv,
        }
      );
    }
  }

  return (
    PuleRaycastTriangleResult {
      .hit = false,
      .distance = -1.0f,
      .origin = puleF32v3(0.0f),
      .uv = puleF32v2(-1.0f),
    }
  );
}

}
