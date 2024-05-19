# shape

## functions
### puleShapeCreateHexahedron
```c
puleShapeCreateHexahedron() PuleBuffer;
```
### puleShapeCreateTetrahedron
```c
puleShapeCreateTetrahedron() PuleBuffer;
```
### puleShapeCreateOctahedron
```c
puleShapeCreateOctahedron() PuleBuffer;
```
### puleShapeCreateIcosahedron
```c
puleShapeCreateIcosahedron() PuleBuffer;
```
### puleShapeCreateDodecahedron
```c
puleShapeCreateDodecahedron() PuleBuffer;
```
### puleShapeCreateIcosphere
```c
puleShapeCreateIcosphere(
  subdivisions : size_t
) PuleBuffer;
```
### puleShapeIntersectRayTriangle
```c
puleShapeIntersectRayTriangle(
  rayOri : PuleF32v3,
  rayDir : PuleF32v3,
  tri0 : PuleF32v3,
  tri1 : PuleF32v3,
  tri2 : PuleF32v3,
  distanceNullable : float ptr,
  uvNullable : PuleF32v2 ptr
) bool;
```
### puleShapeIntersectSphereSphere
```c
puleShapeIntersectSphereSphere(
  sphere0Center : PuleF32v3,
  sphere0Radius : float,
  sphere1Center : PuleF32v3,
  sphere1Radius : float
) bool;
```
