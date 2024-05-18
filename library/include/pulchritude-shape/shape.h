#pragma once

#include <pulchritude-array/array.h>
#include <pulchritude-core/core.h>
#include <pulchritude-math/math.h>

// -- surface mesh --

#ifdef __cplusplus
extern "C" {
#endif

// all shapes are created at origin {0, 0, 0} scaled to a unit sphere
// returns a list of triangles, each vertex 3 floats

PULE_exportFn PuleBuffer puleShapeCreateHexahedron();
PULE_exportFn PuleBuffer puleShapeCreateTetrahedron();
PULE_exportFn PuleBuffer puleShapeCreateOctahedron();
PULE_exportFn PuleBuffer puleShapeCreateIcosahedron();
PULE_exportFn PuleBuffer puleShapeCreateDodecahedron();
PULE_exportFn PuleBuffer puleShapeCreateIcosphere(size_t subdivisions);


#ifdef __cplusplus
} // extern C
#endif

// -- intersections --

#ifdef __cplusplus
extern "C" {
#endif

bool puleShapeIntersectRayTriangle(
  PuleF32v3 const rayOri, PuleF32v3 const rayDir,
  PuleF32v3 const tri0, PuleF32v3 const tri1, PuleF32v3 const tri2,
  float * const distanceNullable, PuleF32v2 * const uvNullable
);

bool puleShapeIntersectSphereSphere(
  PuleF32v3 const sphere0Center, float const sphere0Radius,
  PuleF32v3 const sphere1Center, float const sphere1Radius
);

#ifdef __cplusplus
} // extern C
#endif
