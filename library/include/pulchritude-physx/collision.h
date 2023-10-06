#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

// provides basic collisions/intersections for rays and many shapes, only
// for calculations and not actual simulation

typedef struct {
  bool hit;
  float distance;
  PuleF32v3 origin;
} PulePhysxCollisionResultShape;

typedef struct {
  bool hit;
  float distance;
  PuleF32v3 origin;
  uint32_t triangleIndex;
  PuleF32v2 barycentric;
} PulePhysxCollisionResultMesh;

typedef struct {
  PuleF32v3 origin;
  PuleF32v3 direction;
} PulePhysxRay;

typedef enum {
  PulePhysxCollisionShapeType_sphere,
  PulePhysxCollisionShapeType_box,
  PulePhysxCollisionShapeType_capsule,
  PulePhysxCollisionShapeType_plane,
  PulePhysxCollisionShapeType_triangle,
} PulePhysxCollisionShapeType;

typedef struct {
  PulePhysxCollisionShapeType type;
  union {
    struct {
      float radius;
    } sphere;
    struct {
      PuleF32v3 halfExtent;
    } box;
    struct {
      float radius;
      float height;
    } capsule;
    struct {
      PuleF32v3 normal;
      float distance;
    } plane;
    struct {
      PuleF32v3 origins[3];
    } triangle;
  };
} PulePhysxCollisionShape;

typedef struct {
  PuleF32v3 * origins;
  uint32_t * indices;
  size_t originCount;
  size_t indexCount;
} PulePhysxCollisionMesh;

typedef enum {
  PulePhysxMode_2d,
  PulePhysxMode_3d,
} PulePhysxMode;

PULE_exportFn PulePhysxCollisionResultShape pulePhysxRaycastShape(
  PulePhysxMode const mode,
  PulePhysxRay const ray,
  PuleF32m44 const shapeTransform,
  PulePhysxCollisionShape const shape
);

PULE_exportFn PulePhysxCollisionResultMesh pulePhysxRaycastMesh(
  PulePhysxMode const mode,
  PulePhysxRay const ray,
  PuleF32m44 const meshTransform,
  PulePhysxCollisionMesh const mesh
);

PULE_exportFn bool pulePhysxShapeOverlapShape(
  PulePhysxMode const mode,
  PuleF32m44 const shape1Transform,
  PulePhysxCollisionShape const shape1,
  PuleF32m44 const shape2Transform,
  PulePhysxCollisionShape const shape2
);

PULE_exportFn bool pulePhysxShapeOverlapMesh(
  PulePhysxMode const mode,
  PuleF32m44 const shapeTransform,
  PulePhysxCollisionShape const shape,
  PuleF32m44 const meshTransform,
  PulePhysxCollisionMesh const mesh
);

#ifdef __cplusplus
} // extern C
#endif
