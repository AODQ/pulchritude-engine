#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint64_t id;
} PulePhysics2DWorld;

typedef struct {
  PuleAllocator allocator;
  float pixelsToMeterScale;
  PuleF32v2 gravity;
} PulePhysics2DWorldCreateInfo;
PULE_exportFn PulePhysics2DWorld pulePhysics2DWorldCreate(
  PulePhysics2DWorldCreateInfo const info
);
PULE_exportFn void pulePhysics2DWorldDestroy(PulePhysics2DWorld const world);
PULE_exportFn void pulePhysics2DWorldGravitySet(PuleF32v2 const gravity);
PULE_exportFn void pulePhysics2DDebugDraw(bool const enabled);

typedef enum {
  PulePhysicsShape_polygon,
  PulePhysicsShape_circle,
  PulePhysicsShape_edge,
  PulePhysicsShape_surface,
} PulePhysicsShape;

typedef struct {
  uint64_t id;
} PulePhysics2DBody;

typedef struct {
  PulePhysics2DWorld world;
  PuleF32v2 origin;
  PuleF32v2 dimensions;
  float density;
  float friction;
  float restitution;
  float restitutionThreshold;
  float density
  bool isSensor;
} PulePhysics2DDynamicPolygonCreateInfo;
PULE_exportFn PulePhysics2DBody pulePhysics2DPolygonCreate(
  PulePhysics2DDynamicPolygonCreateInfo const info
);

PULE_exportFn void pulePhysics2DBodyDestroy(
  PulePhysics2DWorld const world,
  PulePhysics2DBody const body
);

PULE_exportFn PuleF32v2 pulePhysics2DBodyOrigin(PulePhysics2DBody const body);

typedef struct {
  PulePhysics2DBody body;
  PuleF32v2 force;
  PuleF32v2 absoluteOrigin;
} PulePhysics2DForceInfo;
PULE_exportFn void pulePhysics2DBodyApplyForce(
  PulePhysics2DForceInfo const info
);

typedef struct {
  PulePhysics2DWorld world;
  float timeDelta;
  size_t velocityIterations;
  size_t positionIterations;
} PulePhysics2DAdvanceInfo;
PULE_exportFn void pulePhysics2DAdvance(PulePhysics2DAdvanceInfo const info);

#ifdef __cplusplus
}
#endif
