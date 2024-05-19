/* auto generated file math */
#pragma once
#include <pulchritude/core.h>


#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  float x;
  float y;
} PuleF32v2;
typedef struct {
  float x;
  float y;
  float z;
} PuleF32v3;
typedef struct {
  float x;
  float y;
  float z;
  float w;
} PuleF32v4;
typedef struct {
  int32_t x;
  int32_t y;
} PuleI32v2;
typedef struct {
  int32_t x;
  int32_t y;
  int32_t z;
} PuleI32v3;
typedef struct {
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t w;
} PuleI32v4;
typedef struct {
  uint32_t x;
  uint32_t y;
} PuleU32v2;
typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t z;
} PuleU32v3;
typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t z;
  uint32_t w;
} PuleU32v4;
/* 
matrices are 'column major', the indices look like

   |  0  4  8 12 |
   |  1  5  9 13 |
   |  2  6 10 14 |
   |  3  7 11 15 |

which compose vectors
  <0 .. 3>, <4 .. 7>, <8 .. 11>, <12 .. 15>

for example, the 'translation' part of a view matrix would be stored at
   <12, 13, 14, 15>
 */
typedef struct {
  float [9] elem;
} PuleF32m33;
typedef struct {
  float [16] elem;
} PuleF32m44;
/*  quaternion  */
typedef struct {
  float x;
  float y;
  float z;
  float w;
} PuleF32q;

// enum

// entities

// functions
PULE_exportFn PuleF32v2 puleF32v2(float identity);
PULE_exportFn PuleF32v2 puleF32v2Ptr(float const * values);
PULE_exportFn PuleF32v2 puleF32v2Add(PuleF32v2 a, PuleF32v2 b);
PULE_exportFn PuleF32v2 puleF32v2Sub(PuleF32v2 a, PuleF32v2 b);
PULE_exportFn PuleF32v2 puleF32v2Abs(PuleF32v2 a);
PULE_exportFn PuleF32v2 puleF32v2Mul(PuleF32v2 a, PuleF32v2 b);
PULE_exportFn PuleF32v2 puleF32v2MulScalar(PuleF32v2 a, float b);
PULE_exportFn PuleI32v2 puleI32v2(int32_t identity);
PULE_exportFn PuleI32v2 puleI32v2Ptr(int32_t const * values);
PULE_exportFn PuleI32v2 puleI32v2Add(PuleI32v2 a, PuleI32v2 b);
PULE_exportFn PuleI32v2 puleI32v2Sub(PuleI32v2 a, PuleI32v2 b);
PULE_exportFn PuleI32v2 puleI32v2Abs(PuleI32v2 a);
PULE_exportFn PuleF32v3 puleF32v3(float identity);
PULE_exportFn PuleF32v3 puleF32v3Ptr(float const * values);
PULE_exportFn PuleF32v3 puleF32v3Add(PuleF32v3 a, PuleF32v3 b);
PULE_exportFn PuleF32v3 puleF32v3Sub(PuleF32v3 a, PuleF32v3 b);
PULE_exportFn PuleF32v3 puleF32v3Neg(PuleF32v3 a);
PULE_exportFn PuleF32v3 puleF32v3Mul(PuleF32v3 a, PuleF32v3 b);
PULE_exportFn PuleF32v3 puleF32v3MulScalar(PuleF32v3 a, float b);
PULE_exportFn PuleF32v3 puleF32v3Div(PuleF32v3 a, PuleF32v3 b);
PULE_exportFn float puleF32v3Dot(PuleF32v3 a, PuleF32v3 b);
PULE_exportFn float puleF32v3Length(PuleF32v3 a);
PULE_exportFn PuleF32v3 puleF32v3Normalize(PuleF32v3 b);
PULE_exportFn PuleF32v3 puleF32v3Cross(PuleF32v3 a, PuleF32v3 b);
PULE_exportFn PuleF32v4 puleF32v4(float identity);
PULE_exportFn PuleF32v4 puleF32v3to4(PuleF32v3 a, float identity);
PULE_exportFn PuleF32v4 puleF32v4Add(PuleF32v4 a, PuleF32v4 b);
PULE_exportFn PuleF32v4 puleF32v4Sub(PuleF32v4 a, PuleF32v4 b);
PULE_exportFn PuleF32v4 puleF32v4Mul(PuleF32v4 a, PuleF32v4 b);
PULE_exportFn PuleF32v4 puleF32v4Div(PuleF32v4 a, PuleF32v4 b);
PULE_exportFn PuleF32v4 puleF32v4Dot(PuleF32v4 a, PuleF32v4 b);
PULE_exportFn PuleF32m33 puleF32m33(float identity);
PULE_exportFn PuleF32m33 puleF32m33Ptr(float const * data);
PULE_exportFn PuleF32m33 puleF32m33PtrTranspose(float const * data);
PULE_exportFn PuleF32m44 puleF32m44(float identity);
PULE_exportFn PuleF32m44 puleF32m44Ptr(float const * data);
PULE_exportFn PuleF32m44 puleF32m44PtrTranspose(float const * data);
PULE_exportFn PuleF32m44 puleF32m33AsM44(PuleF32m33 a);
PULE_exportFn PuleF32m44 puleF32m44Inverse(PuleF32m44 a);
PULE_exportFn PuleF32v4 puleF32m44MulV4(PuleF32m44 a, PuleF32v4 b);
PULE_exportFn PuleF32m44 puleF32m44Mul(PuleF32m44 a, PuleF32m44 b);
PULE_exportFn PuleF32m44 puleF32m44Translate(PuleF32v3 translate);
PULE_exportFn void puleF32m44DumpToStdout(PuleF32m44 a);
PULE_exportFn PuleF32m44 puleProjectionPerspective(float fieldOfViewRadians, float aspectRatio, float nearCutoff, float farCutoff);
PULE_exportFn PuleF32m44 puleViewLookAt(PuleF32v3 origin, PuleF32v3 center, PuleF32v3 up);
PULE_exportFn PuleF32m44 puleF32m44Viewport(float width, float height);
PULE_exportFn PuleF32m44 puleF32m44Rotation(float radians, PuleF32v3 axis);
PULE_exportFn PuleF32q puleF32qIdentity();
PULE_exportFn PuleF32q puleF32qPlane(PuleF32v3 normal, float theta);
PULE_exportFn PuleF32q puleF32qFromEuler(PuleF32v3 euler);
PULE_exportFn PuleF32q puleF32qNormalize(PuleF32q a);
PULE_exportFn PuleF32q puleF32qInvert(PuleF32q a);
PULE_exportFn float puleF32qMagnitude(PuleF32q a);
PULE_exportFn float puleF32qMagnitudeSqr(PuleF32q a);
PULE_exportFn PuleF32v3 puleF32qRotateV3(PuleF32q a, PuleF32v3 b);
PULE_exportFn PuleF32m33 puleF32qRotateM33(PuleF32q a, PuleF32m33 b);
PULE_exportFn PuleF32v3 puleF32qAxis(PuleF32q a);
PULE_exportFn bool puleF32qValid(PuleF32q a);
PULE_exportFn PuleF32m33 puleF32qAsM33(PuleF32q identity);
PULE_exportFn PuleF32v4 puleF32qAsV4(PuleF32q identity);
PULE_exportFn PuleF32q puleF32qMul(PuleF32q a, PuleF32q b);
PULE_exportFn PuleF32q puleF32qMulF(PuleF32q a, float b);

#ifdef __cplusplus
} // extern C
#endif
