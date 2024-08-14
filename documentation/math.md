# math

## structs
### PuleF32v2
```c
struct {
  x : float;
  y : float;
};
```
### PuleF32v3
```c
struct {
  x : float;
  y : float;
  z : float;
};
```
### PuleF32v4
```c
struct {
  x : float;
  y : float;
  z : float;
  w : float;
};
```
### PuleI32v2
```c
struct {
  x : int32_t;
  y : int32_t;
};
```
### PuleI32v3
```c
struct {
  x : int32_t;
  y : int32_t;
  z : int32_t;
};
```
### PuleI32v4
```c
struct {
  x : int32_t;
  y : int32_t;
  z : int32_t;
  w : int32_t;
};
```
### PuleU32v2
```c
struct {
  x : uint32_t;
  y : uint32_t;
};
```
### PuleU32v3
```c
struct {
  x : uint32_t;
  y : uint32_t;
  z : uint32_t;
};
```
### PuleU32v4
```c
struct {
  x : uint32_t;
  y : uint32_t;
  z : uint32_t;
  w : uint32_t;
};
```
### PuleF32m33

matrices are 'column major', the indices look like

   |  0  4  8 12 |
   |  1  5  9 13 |
   |  2  6 10 14 |
   |  3  7 11 15 |

which compose vectors
  <0 .. 3>, <4 .. 7>, <8 .. 11>, <12 .. 15>

for example, the 'translation' part of a view matrix would be stored at
   <12, 13, 14, 15>

```c
struct {
  elem : float [9];
};
```
### PuleF32m44
```c
struct {
  elem : float [16];
};
```

## functions
### puleF32Mix
```c
puleF32Mix(
  a : float,
  b : float,
  t : float
) float;
```
### puleF32v2
```c
puleF32v2(
  identity : float
) PuleF32v2;
```
### puleF32v2Ptr
```c
puleF32v2Ptr(
  values : float const ptr
) PuleF32v2;
```
### puleF32v2Add
```c
puleF32v2Add(
  a : PuleF32v2,
  b : PuleF32v2
) PuleF32v2;
```
### puleF32v2Sub
```c
puleF32v2Sub(
  a : PuleF32v2,
  b : PuleF32v2
) PuleF32v2;
```
### puleF32v2Abs
```c
puleF32v2Abs(
  a : PuleF32v2
) PuleF32v2;
```
### puleF32v2Mul
```c
puleF32v2Mul(
  a : PuleF32v2,
  b : PuleF32v2
) PuleF32v2;
```
### puleF32v2MulScalar
```c
puleF32v2MulScalar(
  a : PuleF32v2,
  b : float
) PuleF32v2;
```
### puleI32v2
```c
puleI32v2(
  identity : int32_t
) PuleI32v2;
```
### puleI32v2Ptr
```c
puleI32v2Ptr(
  values : int32_t const ptr
) PuleI32v2;
```
### puleI32v2Add
```c
puleI32v2Add(
  a : PuleI32v2,
  b : PuleI32v2
) PuleI32v2;
```
### puleI32v2Sub
```c
puleI32v2Sub(
  a : PuleI32v2,
  b : PuleI32v2
) PuleI32v2;
```
### puleI32v2Abs
```c
puleI32v2Abs(
  a : PuleI32v2
) PuleI32v2;
```
### puleF32v3
```c
puleF32v3(
  identity : float
) PuleF32v3;
```
### puleF32v3Ptr
```c
puleF32v3Ptr(
  values : float const ptr
) PuleF32v3;
```
### puleF32v3Add
```c
puleF32v3Add(
  a : PuleF32v3,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v3Sub
```c
puleF32v3Sub(
  a : PuleF32v3,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v3Neg
```c
puleF32v3Neg(
  a : PuleF32v3
) PuleF32v3;
```
### puleF32v3Mul
```c
puleF32v3Mul(
  a : PuleF32v3,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v3MulScalar
```c
puleF32v3MulScalar(
  a : PuleF32v3,
  b : float
) PuleF32v3;
```
### puleF32v3Div
```c
puleF32v3Div(
  a : PuleF32v3,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v3Mix
```c
puleF32v3Mix(
  a : PuleF32v3,
  b : PuleF32v3,
  t : float
) PuleF32v3;
```
### puleF32v3Min
```c
puleF32v3Min(
  a : PuleF32v3,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v3Max
```c
puleF32v3Max(
  a : PuleF32v3,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v3Dot
```c
puleF32v3Dot(
  a : PuleF32v3,
  b : PuleF32v3
) float;
```
### puleF32v3Length
```c
puleF32v3Length(
  a : PuleF32v3
) float;
```
### puleF32v3Normalize
```c
puleF32v3Normalize(
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v3Cross
```c
puleF32v3Cross(
  a : PuleF32v3,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32v4
```c
puleF32v4(
  identity : float
) PuleF32v4;
```
### puleF32v3to4
```c
puleF32v3to4(
  a : PuleF32v3,
  identity : float
) PuleF32v4;
```
### puleF32v4Add
```c
puleF32v4Add(
  a : PuleF32v4,
  b : PuleF32v4
) PuleF32v4;
```
### puleF32v4Sub
```c
puleF32v4Sub(
  a : PuleF32v4,
  b : PuleF32v4
) PuleF32v4;
```
### puleF32v4Mul
```c
puleF32v4Mul(
  a : PuleF32v4,
  b : PuleF32v4
) PuleF32v4;
```
### puleF32v4MulScalar
```c
puleF32v4MulScalar(
  a : PuleF32v4,
  b : float
) PuleF32v4;
```
### puleF32v4Div
```c
puleF32v4Div(
  a : PuleF32v4,
  b : PuleF32v4
) PuleF32v4;
```
### puleF32v4Mix
```c
puleF32v4Mix(
  a : PuleF32v4,
  b : PuleF32v4,
  t : float
) PuleF32v4;
```
### puleF32v4Dot
```c
puleF32v4Dot(
  a : PuleF32v4,
  b : PuleF32v4
) float;
```
### puleF32m33
```c
puleF32m33(
  identity : float
) PuleF32m33;
```
### puleF32m33Ptr
```c
puleF32m33Ptr(
  data : float const ptr
) PuleF32m33;
```
### puleF32m33PtrTranspose
```c
puleF32m33PtrTranspose(
  data : float const ptr
) PuleF32m33;
```
### puleF32m33MulV3
```c
puleF32m33MulV3(
  m : PuleF32m33,
  v : PuleF32v3
) PuleF32v3;
```
### puleF32m44
```c
puleF32m44(
  identity : float
) PuleF32m44;
```
### puleF32m44Ptr
```c
puleF32m44Ptr(
  data : float const ptr
) PuleF32m44;
```
### puleF32m44PtrTranspose
```c
puleF32m44PtrTranspose(
  data : float const ptr
) PuleF32m44;
```
### puleF32m33AsM44
```c
puleF32m33AsM44(
  a : PuleF32m33
) PuleF32m44;
```
### puleF32m44Inverse
```c
puleF32m44Inverse(
  a : PuleF32m44
) PuleF32m44;
```
### puleF32m44MulV4
```c
puleF32m44MulV4(
  a : PuleF32m44,
  b : PuleF32v4
) PuleF32v4;
```
### puleF32m44Mul
```c
puleF32m44Mul(
  a : PuleF32m44,
  b : PuleF32m44
) PuleF32m44;
```
### puleF32m44Translate
```c
puleF32m44Translate(
  translate : PuleF32v3
) PuleF32m44;
```
### puleF32m44DumpToStdout
```c
puleF32m44DumpToStdout(
  a : PuleF32m44
) void;
```
### puleProjectionPerspective
```c
puleProjectionPerspective(
  fieldOfViewDegrees : float,
  aspectRatio : float,
  nearCutoff : float,
  farCutoff : float
) PuleF32m44;
```
### puleViewLookAt
```c
puleViewLookAt(
  origin : PuleF32v3,
  center : PuleF32v3,
  up : PuleF32v3
) PuleF32m44;
```
### puleF32m44Viewport
```c
puleF32m44Viewport(
  width : float,
  height : float
) PuleF32m44;
```
### puleF32m44Rotation
```c
puleF32m44Rotation(
  radians : float,
  axis : PuleF32v3
) PuleF32m44;
```
### puleF32qIdentity
```c
puleF32qIdentity() PuleF32q;
```
### puleF32qPlane
```c
puleF32qPlane(
  normal : PuleF32v3,
  theta : float
) PuleF32q;
```
### puleF32qFromEuler
```c
puleF32qFromEuler(
  euler : PuleF32v3
) PuleF32q;
```
### puleF32qNormalize
```c
puleF32qNormalize(
  a : PuleF32q
) PuleF32q;
```
### puleF32qInvert
```c
puleF32qInvert(
  a : PuleF32q
) PuleF32q;
```
### puleF32qMagnitude
```c
puleF32qMagnitude(
  a : PuleF32q
) float;
```
### puleF32qMagnitudeSqr
```c
puleF32qMagnitudeSqr(
  a : PuleF32q
) float;
```
### puleF32qRotateV3
```c
puleF32qRotateV3(
  a : PuleF32q,
  b : PuleF32v3
) PuleF32v3;
```
### puleF32qRotateM33
```c
puleF32qRotateM33(
  a : PuleF32q,
  b : PuleF32m33
) PuleF32m33;
```
### puleF32qAxis
```c
puleF32qAxis(
  a : PuleF32q
) PuleF32v3;
```
### puleF32qValid
```c
puleF32qValid(
  a : PuleF32q
) bool;
```
### puleF32qAsM33
```c
puleF32qAsM33(
  identity : PuleF32q
) PuleF32m33;
```
### puleF32qAsM44
```c
puleF32qAsM44(
  identity : PuleF32q
) PuleF32m44;
```
### puleF32qMul
```c
puleF32qMul(
  a : PuleF32q,
  b : PuleF32q
) PuleF32q;
```
### puleF32qMulF
```c
puleF32qMulF(
  a : PuleF32q,
  b : float
) PuleF32q;
```
### puleF32qSlerp
```c
puleF32qSlerp(
  a : PuleF32q,
  b : PuleF32q,
  t : float
) PuleF32q;
```
