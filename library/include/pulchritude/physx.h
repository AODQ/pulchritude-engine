/* auto generated file physx */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/math.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
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
typedef struct {
  float radius;
} PulePhysxCollisionShapeSphere;
typedef struct {
  PuleF32v3 halfExtent;
} PulePhysxCollisionShapeBox;
typedef struct {
  float radius;
  float height;
} PulePhysxCollisionShapeCapsule;
typedef struct {
  PuleF32v3 normal;
  float distance;
} PulePhysxCollisionShapePlane;
typedef struct {
  PuleF32v3 [3] origins;
} PulePhysxCollisionShapeTriangle;
typedef union {
  PulePhysxCollisionShapeSphere sphere;
  PulePhysxCollisionShapeBox box;
  PulePhysxCollisionShapeCapsule capsule;
  PulePhysxCollisionShapePlane plane;
  PulePhysxCollisionShapeTriangle triangle;
} PulePhysxCollisionShapeUnion;
typedef struct {
  PulePhysxCollisionShapeType type;
  PulePhysxCollisionShapeUnion shape;
} PulePhysxCollisionShape;
typedef struct {
  PuleF32v3 * origins;
  uint32_t * indices;
  size_t originCount;
  size_t indexCount;
} PulePhysxCollisionMesh;
typedef struct {
  PuleF32v2 centroid;
  PuleF32v2 [8] origins;
  PuleF32v2 [8] normals;
  uint32_t vertexCount;
} PulePhysx2DShapeConvexPolygon;
typedef union {
  PulePhysx2DShapeConvexPolygon convexPolygon;
} PulePhysx2DShapeUnion;
typedef struct {
  PulePhysx2DShapeType type;
  PulePhysx2DShapeUnion shape;
} PulePhysx2DShape;
typedef struct {
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
typedef struct {
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

// enum
typedef enum {
  PulePhysxCollisionShapeType_sphere,
  PulePhysxCollisionShapeType_box,
  PulePhysxCollisionShapeType_capsule,
  PulePhysxCollisionShapeType_plane,
  PulePhysxCollisionShapeType_triangle,
} PulePhysxCollisionShapeType;
typedef enum {
  PulePhysxMode_2d,
  PulePhysxMode_3d,
} PulePhysxMode;
typedef enum {
  PulePhysx2DShapeType_convexPolygon,
} PulePhysx2DShapeType;
typedef enum {
  PulePhysx2DBodyType_static,
  PulePhysx2DBodyType_kinematic,
  PulePhysx2DBodyType_dynamic,
} PulePhysx2DBodyType;

// entities
typedef struct { uint64_t id; } PulePhysx2DWorld;
typedef struct { uint64_t id; } PulePhysx2DBody;

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

#ifdef __cplusplus
} // extern C
#endif
