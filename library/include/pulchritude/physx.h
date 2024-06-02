/* auto generated file physx */
#pragma once
#include "core.h"

#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PulePhysxCollisionShapeType_sphere = 0,
  PulePhysxCollisionShapeType_box = 1,
  PulePhysxCollisionShapeType_capsule = 2,
  PulePhysxCollisionShapeType_plane = 3,
  PulePhysxCollisionShapeType_triangle = 4,
} PulePhysxCollisionShapeType;
const uint32_t PulePhysxCollisionShapeTypeSize = 5;
typedef enum {
  PulePhysxMode_i2d = 0,
  PulePhysxMode_i3d = 1,
} PulePhysxMode;
const uint32_t PulePhysxModeSize = 2;
typedef enum {
  PulePhysx2DShapeType_convexPolygon = 0,
} PulePhysx2DShapeType;
const uint32_t PulePhysx2DShapeTypeSize = 1;
typedef enum {
  PulePhysx2DBodyType_static = 0,
  PulePhysx2DBodyType_kinematic = 1,
  PulePhysx2DBodyType_dynamic = 2,
} PulePhysx2DBodyType;
const uint32_t PulePhysx2DBodyTypeSize = 3;

// entities
typedef struct PulePhysx2DWorld { uint64_t id; } PulePhysx2DWorld;
typedef struct PulePhysx2DBody { uint64_t id; } PulePhysx2DBody;
typedef struct PulePhysx3DWorld { uint64_t id; } PulePhysx3DWorld;
typedef struct PulePhysx3DBody { uint64_t id; } PulePhysx3DBody;

// structs
struct PulePhysxCollisionResultShape;
struct PulePhysxCollisionResultMesh;
struct PulePhysxRay;
struct PulePhysxCollisionShapeSphere;
struct PulePhysxCollisionShapeBox;
struct PulePhysxCollisionShapeCapsule;
struct PulePhysxCollisionShapePlane;
struct PulePhysxCollisionShapeTriangle;
union PulePhysxCollisionShapeUnion;
struct PulePhysxCollisionShape;
struct PulePhysxCollisionMesh;
struct PulePhysx2DShapeConvexPolygon;
union PulePhysx2DShapeUnion;
struct PulePhysx2DShape;
struct PulePhysx2DBodyCreateInfo;
struct PulePhysx2DBodyAttachShapeCreateInfo;

typedef struct PulePhysxCollisionResultShape {
  bool hit;
  float distance;
  PuleF32v3 origin;
} PulePhysxCollisionResultShape;
typedef struct PulePhysxCollisionResultMesh {
  bool hit;
  float distance;
  PuleF32v3 origin;
  uint32_t triangleIndex;
  PuleF32v2 barycentric;
} PulePhysxCollisionResultMesh;
typedef struct PulePhysxRay {
  PuleF32v3 origin;
  PuleF32v3 direction;
} PulePhysxRay;
typedef struct PulePhysxCollisionShapeSphere {
  float radius;
} PulePhysxCollisionShapeSphere;
typedef struct PulePhysxCollisionShapeBox {
  PuleF32v3 halfExtent;
} PulePhysxCollisionShapeBox;
typedef struct PulePhysxCollisionShapeCapsule {
  float radius;
  float height;
} PulePhysxCollisionShapeCapsule;
typedef struct PulePhysxCollisionShapePlane {
  PuleF32v3 normal;
  float distance;
} PulePhysxCollisionShapePlane;
typedef struct PulePhysxCollisionShapeTriangle {
  PuleF32v3 origins[3] ;
} PulePhysxCollisionShapeTriangle;
typedef union PulePhysxCollisionShapeUnion {
  PulePhysxCollisionShapeSphere sphere;
  PulePhysxCollisionShapeBox box;
  PulePhysxCollisionShapeCapsule capsule;
  PulePhysxCollisionShapePlane plane;
  PulePhysxCollisionShapeTriangle triangle;
} PulePhysxCollisionShapeUnion;
typedef struct PulePhysxCollisionShape {
  PulePhysxCollisionShapeType type;
  PulePhysxCollisionShapeUnion shape;
} PulePhysxCollisionShape;
typedef struct PulePhysxCollisionMesh {
  PuleF32v3 * origins;
  uint32_t * indices;
  size_t originCount;
  size_t indexCount;
} PulePhysxCollisionMesh;
typedef struct PulePhysx2DShapeConvexPolygon {
  PuleF32v2 centroid;
  PuleF32v2 origins[8] ;
  PuleF32v2 normals[8] ;
  uint32_t vertexCount;
} PulePhysx2DShapeConvexPolygon;
typedef union PulePhysx2DShapeUnion {
  PulePhysx2DShapeConvexPolygon convexPolygon;
} PulePhysx2DShapeUnion;
typedef struct PulePhysx2DShape {
  PulePhysx2DShapeType type;
  PulePhysx2DShapeUnion shape;
} PulePhysx2DShape;
typedef struct PulePhysx2DBodyCreateInfo {
  PulePhysx2DBodyType type;
  /*  TODO = {0.0f, 0.0f}  */
  PuleF32v2 origin;
  /*  = {0.0f, 0.0f}  */
  PuleF32v2 originVelocity;
  float originDamping;
  float angle;
  float angleVelocity;
  float angleDamping;
  float gravityScale;
  bool allowSleep;
  bool startAwake;
  bool startEnabled;
  bool fixedRotation;
  bool bullet;
  void * userData;
} PulePhysx2DBodyCreateInfo;
typedef struct PulePhysx2DBodyAttachShapeCreateInfo {
  float friction;
  float density;
  float restitution;
  float restitutionThreshold;
  bool isSensor;
  void * userData;
  uint16_t collisionCategoryBits;
  uint16_t collisionMaskBits;
  uint16_t collisionGroup;
} PulePhysx2DBodyAttachShapeCreateInfo;

// functions
PULE_exportFn PulePhysxCollisionResultShape pulePhysxRaycastShape(PulePhysxMode mode, PulePhysxRay ray, PuleF32m44 shapeTransform, PulePhysxCollisionShape shape);
PULE_exportFn PulePhysxCollisionResultMesh pulePhysxRaycastMesh(PulePhysxMode mode, PulePhysxRay ray, PuleF32m44 meshTransform, PulePhysxCollisionMesh mesh);
PULE_exportFn bool pulePhysxShapeOverlapShape(PulePhysxMode mode, PuleF32m44 shape1Transform, PulePhysxCollisionShape shape1, PuleF32m44 shape2Transform, PulePhysxCollisionShape shape2);
PULE_exportFn bool pulePhysxShapeOverlapMesh(PulePhysxMode mode, PuleF32m44 shapeTransform, PulePhysxCollisionShape shape, PuleF32m44 meshTransform, PulePhysxCollisionMesh mesh);
PULE_exportFn PulePhysx2DWorld pulePhysx2DWorldCreate();
PULE_exportFn void pulePhysx2DWorldDestroy(PulePhysx2DWorld world);
PULE_exportFn void pulePhysx2DWorldGravitySet(PulePhysx2DWorld world, PuleF32v2 gravity);
PULE_exportFn void pulePhysx2DWorldStep(PulePhysx2DWorld world, float timeStep, int velocityIterations, int positionIterations);
PULE_exportFn PulePhysx2DBody pulePhysx2DBodyCreate(PulePhysx2DWorld world, PulePhysx2DBodyCreateInfo createInfo);
PULE_exportFn void pulePhysx2DBodyDestroy(PulePhysx2DWorld world, PulePhysx2DBody body);
PULE_exportFn PuleF32v2 pulePhysx2DBodyPosition(PulePhysx2DWorld world, PulePhysx2DBody body);
PULE_exportFn float pulePhysx2DBodyAngle(PulePhysx2DWorld world, PulePhysx2DBody body);
PULE_exportFn void pulePhysx2DBodyAttachShape(PulePhysx2DWorld world, PulePhysx2DBody body, PulePhysx2DShape shape, PulePhysx2DBodyAttachShapeCreateInfo createInfo);
PULE_exportFn PulePhysx2DShape pulePhysx2DShapeCreateConvexPolygonAsBox(PuleF32v2 origin, PuleF32v2 halfExtents, float angle);
PULE_exportFn PulePhysx3DWorld pulePhysx3DWorldCreate();

#ifdef __cplusplus
} // extern C
#endif
