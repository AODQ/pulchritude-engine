/* auto generated file shape */
#pragma once
#include "core.h"

#include "array.h"
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs


// functions
PULE_exportFn PuleBuffer puleShapeCreateHexahedron();
PULE_exportFn PuleBuffer puleShapeCreateTetrahedron();
PULE_exportFn PuleBuffer puleShapeCreateOctahedron();
PULE_exportFn PuleBuffer puleShapeCreateIcosahedron();
PULE_exportFn PuleBuffer puleShapeCreateDodecahedron();
PULE_exportFn PuleBuffer puleShapeCreateIcosphere(size_t subdivisions);
PULE_exportFn bool puleShapeIntersectRayTriangle(PuleF32v3 rayOri, PuleF32v3 rayDir, PuleF32v3 tri0, PuleF32v3 tri1, PuleF32v3 tri2, float * distanceNullable, PuleF32v2 * uvNullable);
PULE_exportFn bool puleShapeIntersectSphereSphere(PuleF32v3 sphere0Center, float sphere0Radius, PuleF32v3 sphere1Center, float sphere1Radius);

#ifdef __cplusplus
} // extern C
#endif
