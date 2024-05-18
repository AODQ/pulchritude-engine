#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-math/math-fixed-point.h>

#define PULE_physx3dUseFixedPoint 0

#if PULE_physx3dUseFixedPoint // uses fixed point
typedef PuleFix64 PuleFloat;
typedef PuleFix64v2 PuleFv2;
typedef PuleFix64v3 PuleFv3;
typedef PuleFix64v4 PuleFv4;
typedef PuleFix64m33 PuleFm33;
typedef PuleFix64m44 PuleFm44;
typedef PuleFix64q PuleFquat;
typedef PuleFix64m33 PuleFmee;

#ifdef __cplusplus
namespace pule {
using Float = pule::Fix64;
using Fv2 = pule::Fix64v2;
using Fv3 = pule::Fix64v3;
using Fv4 = pule::Fix64v4;
using Fquat = pule::Fix64q;
using Fm33 = pule::Fix64m33;
} // namespace pule
#endif // __cplusplus
#else // uses floating point
typedef float PuleFloat;
typedef PuleF32v2 PuleFv2;
typedef PuleF32v3 PuleFv3;
typedef PuleF32v4 PuleFv4;
typedef PuleF32m33 PuleFm33;
typedef PuleF32m44 PuleFm44;
typedef PuleF32q PuleFquat;
typedef PuleF32m33 PuleFm33;

#ifdef __cplusplus
#include <pulchritude-math/math.hpp>
namespace pule {
using Float = float;
using Fv2 = pule::F32v2;
using Fv3 = pule::F32v3;
using Fv4 = pule::F32v4;
using Fquat = pule::F32q;
using Fm33 = pule::F32m33;
} // namespace pule
#endif // __cplusplus
#endif // PULE_physx3dUseFixedPoint

#ifdef __cplusplus
extern "C" {
#endif

// 3D physics simulator api
// default implementation uses fixed-point deterministic physics

typedef struct {
  uint64_t id;
} PulePhysx3DWorld;

PULE_exportFn PulePhysx3DWorld pulePhysx3DWorldCreate();
PULE_exportFn void pulePhysx3DWorldDestroy(PulePhysx3DWorld const world);

PULE_exportFn void pulePhysx3DWorldAdvance(
  PulePhysx3DWorld const world, float const msDelta
);

typedef enum {
  PulePhysx3DShape_sphere,
  PulePhysx3DShape_plane,
} PulePhysx3DShapeType;

typedef struct PulePhysx3DShapeSphere {
  PulePhysx3DShapeType type PULE_defaultField(PulePhysx3DShape_sphere);
  PuleFloat radius;
} PulePhysx3DShapeSphere;

typedef struct PulePhysx3DShapePlane {
  PulePhysx3DShapeType type PULE_defaultField(PulePhysx3DShape_plane);
  PuleFv3 normal;
} PulePhysx3DShapePlane;

typedef union {
  PulePhysx3DShapeType type;
  PulePhysx3DShapeSphere sphere;
  PulePhysx3DShapePlane plane;
} PulePhysx3DShape;

typedef struct { uint64_t id; } PulePhysx3DBody;

typedef enum PulePhysxBodyType {
  PulePhysx3DBodyType_dynamic,
  PulePhysx3DBodyType_kinematic,
} PulePhysx3DBodyType;

typedef struct {
  PulePhysx3DWorld world;
  PulePhysx3DShape shape;
  PuleFv3 origin;
  PuleFv3 orientationEuler;
  PuleFloat mass; // 0 => infinite mass (static)
  PuleFloat friction;
  PuleFloat elasticity;
  PulePhysx3DBodyType type;
} PulePhysx3DBodyCreateInfo;

PULE_exportFn PulePhysx3DBody pulePhysx3DBodyCreate(
  PulePhysx3DBodyCreateInfo const info
);
PULE_exportFn void pulePhysx3DBodyDestroy(PulePhysx3DBody const body);

PulePhysx3DShape pulePhysx3DShape(PulePhysx3DBody const body);
PuleFv3 pulePhysx3DBodyOrigin(PulePhysx3DBody const body);
PuleFm33 pulePhysx3DBodyOrientation(PulePhysx3DBody const body);

PuleFv3 pulePhysx3DBodyVelocity(PulePhysx3DBody const body);

void pulePhysx3DBodyOriginSet(PulePhysx3DBody body, PuleFv3 origin);
void pulePhysx3DBodyOrientationSet(
  PulePhysx3DBody body, PuleFv3 orientation
);

void pulePhysx3DBodyApplyImpulse(
  PulePhysx3DBody body, PuleFv3 impulsePoint, PuleFv3 impulse
);

#ifdef __cplusplus
} // extern C
#endif


// -- utility functions --------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// model space => centered around model origin
// world space => centered around world origin
// body space => centered around center of mass

PuleFv3 pulePhysx3DBodyCenterOfMassWorldSpace(PulePhysx3DBody const body);
PuleFv3 pulePhysx3DBodyCenterOfMassModelSpace(PulePhysx3DBody const body);
PuleFv3 pulePhysx3DBodyWorldSpaceToBodySpace(
  PulePhysx3DBody const body, PuleFv3 const worldSpace
);
PuleFv3 pulePhysx3DBodyBodySpaceToWorldSpace(
  PulePhysx3DBody const body, PuleFv3 const bodySpace
);

#ifdef __cplusplus
} // extern C
#endif
