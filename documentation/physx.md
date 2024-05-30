# physx

## structs
### PulePhysxCollisionResultShape
```c
struct {
  hit : bool;
  distance : float;
  origin : PuleF32v3;
};
```
### PulePhysxCollisionResultMesh
```c
struct {
  hit : bool;
  distance : float;
  origin : PuleF32v3;
  triangleIndex : uint32_t;
  barycentric : PuleF32v2;
};
```
### PulePhysxRay
```c
struct {
  origin : PuleF32v3;
  direction : PuleF32v3;
};
```
### PulePhysxCollisionShapeSphere
```c
struct {
  radius : float;
};
```
### PulePhysxCollisionShapeBox
```c
struct {
  halfExtent : PuleF32v3;
};
```
### PulePhysxCollisionShapeCapsule
```c
struct {
  radius : float;
  height : float;
};
```
### PulePhysxCollisionShapePlane
```c
struct {
  normal : PuleF32v3;
  distance : float;
};
```
### PulePhysxCollisionShapeTriangle
```c
struct {
  origins : PuleF32v3 [3];
};
```
### PulePhysxCollisionShapeUnion
```c
union {
  sphere : PulePhysxCollisionShapeSphere;
  box : PulePhysxCollisionShapeBox;
  capsule : PulePhysxCollisionShapeCapsule;
  plane : PulePhysxCollisionShapePlane;
  triangle : PulePhysxCollisionShapeTriangle;
};
```
### PulePhysxCollisionShape
```c
struct {
  type : PulePhysxCollisionShapeType;
  shape : PulePhysxCollisionShapeUnion;
};
```
### PulePhysxCollisionMesh
```c
struct {
  origins : PuleF32v3 ptr;
  indices : uint32_t ptr;
  originCount : size_t;
  indexCount : size_t;
};
```
### PulePhysx2DShapeConvexPolygon
```c
struct {
  centroid : PuleF32v2;
  origins : PuleF32v2 [8];
  normals : PuleF32v2 [8];
  vertexCount : uint32_t;
};
```
### PulePhysx2DShapeUnion
```c
union {
  convexPolygon : PulePhysx2DShapeConvexPolygon;
};
```
### PulePhysx2DShape
```c
struct {
  type : PulePhysx2DShapeType;
  shape : PulePhysx2DShapeUnion;
};
```
### PulePhysx2DBodyCreateInfo
```c
struct {
  type : PulePhysx2DBodyType = PulePhysx2DBodyType_static;
  /*  TODO = {0.0f, 0.0f}  */
  origin : PuleF32v2;
  /*  = {0.0f, 0.0f}  */
  originVelocity : PuleF32v2;
  originDamping : float = 0.000000;
  angle : float = 0.000000;
  angleVelocity : float = 0.000000;
  angleDamping : float = 0.000000;
  gravityScale : float = 1.000000;
  allowSleep : bool = false;
  startAwake : bool = false;
  startEnabled : bool = false;
  fixedRotation : bool = false;
  bullet : bool = false;
  userData : void ptr = nullptr;
};
```
### PulePhysx2DBodyAttachShapeCreateInfo
```c
struct {
  friction : float = 0.200000;
  density : float = 0.000000;
  restitution : float = 0.000000;
  restitutionThreshold : float = 1.000000;
  isSensor : bool = false;
  userData : void ptr = nullptr;
  collisionCategoryBits : uint16_t = 1;
  collisionMaskBits : uint16_t = 65535;
  collisionGroup : uint16_t = 0;
};
```

## enums
### PulePhysxCollisionShapeType
```c
enum {
  sphere,
  box,
  capsule,
  plane,
  triangle,
}
```
### PulePhysxMode
```c
enum {
  i2d,
  i3d,
}
```
### PulePhysx2DShapeType
```c
enum {
  convexPolygon,
}
```
### PulePhysx2DBodyType
```c
enum {
  static,
  kinematic,
  dynamic,
}
```

## entities
### PulePhysx2DWorld
### PulePhysx2DBody
### PulePhysx3DWorld
### PulePhysx3DBody

## functions
### pulePhysxRaycastShape
```c
pulePhysxRaycastShape(
  mode : PulePhysxMode,
  ray : PulePhysxRay,
  shapeTransform : PuleF32m44,
  shape : PulePhysxCollisionShape
) PulePhysxCollisionResultShape;
```
### pulePhysxRaycastMesh
```c
pulePhysxRaycastMesh(
  mode : PulePhysxMode,
  ray : PulePhysxRay,
  meshTransform : PuleF32m44,
  mesh : PulePhysxCollisionMesh
) PulePhysxCollisionResultMesh;
```
### pulePhysxShapeOverlapShape
```c
pulePhysxShapeOverlapShape(
  mode : PulePhysxMode,
  shape1Transform : PuleF32m44,
  shape1 : PulePhysxCollisionShape,
  shape2Transform : PuleF32m44,
  shape2 : PulePhysxCollisionShape
) bool;
```
### pulePhysxShapeOverlapMesh
```c
pulePhysxShapeOverlapMesh(
  mode : PulePhysxMode,
  shapeTransform : PuleF32m44,
  shape : PulePhysxCollisionShape,
  meshTransform : PuleF32m44,
  mesh : PulePhysxCollisionMesh
) bool;
```
### pulePhysx2DWorldCreate
```c
pulePhysx2DWorldCreate() PulePhysx2DWorld;
```
### pulePhysx2DWorldDestroy
```c
pulePhysx2DWorldDestroy(
  world : PulePhysx2DWorld
) void;
```
### pulePhysx2DWorldGravitySet
```c
pulePhysx2DWorldGravitySet(
  world : PulePhysx2DWorld,
  gravity : PuleF32v2
) void;
```
### pulePhysx2DWorldStep
```c
pulePhysx2DWorldStep(
  world : PulePhysx2DWorld,
  timeStep : float,
  velocityIterations : int,
  positionIterations : int
) void;
```
### pulePhysx2DBodyCreate
```c
pulePhysx2DBodyCreate(
  world : PulePhysx2DWorld,
  createInfo : PulePhysx2DBodyCreateInfo
) PulePhysx2DBody;
```
### pulePhysx2DBodyDestroy
```c
pulePhysx2DBodyDestroy(
  world : PulePhysx2DWorld,
  body : PulePhysx2DBody
) void;
```
### pulePhysx2DBodyPosition
```c
pulePhysx2DBodyPosition(
  world : PulePhysx2DWorld,
  body : PulePhysx2DBody
) PuleF32v2;
```
### pulePhysx2DBodyAngle
```c
pulePhysx2DBodyAngle(
  world : PulePhysx2DWorld,
  body : PulePhysx2DBody
) float;
```
### pulePhysx2DBodyAttachShape
```c
pulePhysx2DBodyAttachShape(
  world : PulePhysx2DWorld,
  body : PulePhysx2DBody,
  shape : PulePhysx2DShape,
  createInfo : PulePhysx2DBodyAttachShapeCreateInfo
) void;
```
### pulePhysx2DShapeCreateConvexPolygonAsBox
```c
pulePhysx2DShapeCreateConvexPolygonAsBox(
  origin : PuleF32v2,
  halfExtents : PuleF32v2,
  angle : float
) PulePhysx2DShape;
```
### pulePhysx3DWorldCreate
```c
pulePhysx3DWorldCreate() PulePhysx3DWorld;
```
