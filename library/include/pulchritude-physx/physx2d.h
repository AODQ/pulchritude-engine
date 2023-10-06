#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

// 2D physics simulator api, heavily derived from box2d's existing api

typedef struct {
  uint64_t id;
} PulePhysx2DWorld;

typedef struct {
  uint64_t id;
} PulePhysx2DBody;

typedef enum {
  PulePhysx2DShapeType_convexPolygon,
} PulePhysx2DShapeType;

typedef struct PulePhysx2DShape {
  PulePhysx2DShapeType type;
  union {
    struct {
      PuleF32v2 centroid;
      PuleF32v2 origins[8];
      PuleF32v2 normals[8];
      uint32_t vertexCount;
    } convexPolygon;
  };
} PulePhysx2DShape;

PULE_exportFn PulePhysx2DWorld pulePhysx2DWorldCreate();
PULE_exportFn void pulePhysx2DWorldDestroy(PulePhysx2DWorld const world);

PULE_exportFn void pulePhysx2DWorldGravitySet(
  PulePhysx2DWorld const world,
  PuleF32v2 const gravity
);

PULE_exportFn void pulePhysx2DWorldStep(
  PulePhysx2DWorld const world,
  float const timeStep,
  int const velocityIterations,
  int const positionIterations
);

typedef enum {
  PulePhysx2DBodyType_static,
  PulePhysx2DBodyType_kinematic,
  PulePhysx2DBodyType_dynamic,
} PulePhysx2DBodyType;

typedef struct PulePhysx2DBodyCreateInfo {
  PulePhysx2DBodyType type PULE_param(PulePhysx2DBodyType_static);
  PuleF32v2 origin PULE_param({0.0f, 0.0f});
  PuleF32v2 originVelocity PULE_param({0.0f, 0.0f});
  float originDamping PULE_param(0.0f);
  float angle PULE_param(0.0f);
  float angleVelocity PULE_param(0.0f);
  float angleDamping PULE_param(0.0f);
  float gravityScale PULE_param(1.0f);
  bool allowSleep PULE_param(true);
  bool startAwake PULE_param(true);
  bool startEnabled PULE_param(true);
  bool fixedRotation PULE_param(false);
  bool bullet PULE_param(false);
  void * userData PULE_param(nullptr);
} PulePhysx2DBodyCreateInfo;

PULE_exportFn PulePhysx2DBody pulePhysx2DBodyCreate(
  PulePhysx2DWorld const world,
  PulePhysx2DBodyCreateInfo const createInfo
);
PULE_exportFn void pulePhysx2DBodyDestroy(
  PulePhysx2DWorld const world,
  PulePhysx2DBody const body
);

PULE_exportFn PuleF32v2 pulePhysx2DBodyPosition(
  PulePhysx2DWorld const world,
  PulePhysx2DBody const body
);
PULE_exportFn float pulePhysx2DBodyAngle(
  PulePhysx2DWorld const world,
  PulePhysx2DBody const body
);

typedef struct PulePhysx2DBodyAttachShapeCreateInfo {
  float friction PULE_param(0.2f);
  float density PULE_param(0.0f);
  float restitution PULE_param(0.0f);
  float restitutionThreshold PULE_param(1.0f);
  bool isSensor PULE_param(false);
  void * userData PULE_param(nullptr);
  uint16_t collisionCategoryBits PULE_param(0x0001);
  uint16_t collisionMaskBits PULE_param(0xFFFF);
  uint16_t collisionGroup PULE_param(0);
} PulePhysx2DBodyAttachShapeCreateInfo;

PULE_exportFn void pulePhysx2DBodyAttachShape(
  PulePhysx2DWorld const world,
  PulePhysx2DBody const body,
  PulePhysx2DShape const shape,
  PulePhysx2DBodyAttachShapeCreateInfo const createInfo
);

PULE_exportFn PulePhysx2DShape pulePhysx2DShapeCreateConvexPolygonAsBox(
  PuleF32v2 const origin,
  PuleF32v2 const halfExtents,
  float const angle
);

#ifdef __cplusplus
}
#endif
