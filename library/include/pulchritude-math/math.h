#pragma once

// math libraries can be kind of clunky with a C ABI, so this only really
// exists to send vector/matrix data around, as well as some very simple
// utilities such as creating projection/view matrices. This might change in
// the future to support SSE operations.
//
// you should probably prefer to use a dedicated math library for the language
// you use, as it will provide better syntax. Especially if your language
// supports operator overloading. I recommend sticking to libraries that stick
// close to GLSL syntax of vectors, have support for SSE vectors, and don't
// noticably decrease compile times.

#include <pulchritude-core/core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { float x; float y; } PuleF32v2;
typedef struct { float x; float y; float z; } PuleF32v3;
typedef struct { float x; float y; float z; float w; } PuleF32v4;

typedef struct { int32_t x; int32_t y; } PuleI32v2;
typedef struct { int32_t x; int32_t y; int32_t z; } PuleI32v3;
typedef struct { int32_t x; int32_t y; int32_t z; int32_t w; } PuleI32v4;

typedef struct { uint32_t x; uint32_t y; } PuleU32v2;
typedef struct { uint32_t x; uint32_t y; uint32_t z; } PuleU32v3;
typedef struct { uint32_t x; uint32_t y; uint32_t z; uint32_t w; } PuleU32v4;

PULE_exportFn PuleF32v2 puleF32v2(float const identity);
PULE_exportFn PuleF32v2 puleF32v2Ptr(float const * const values);
PULE_exportFn PuleF32v2 puleF32v2Add(PuleF32v2 const a, PuleF32v2 const b);
PULE_exportFn PuleF32v2 puleF32v2Sub(PuleF32v2 const a, PuleF32v2 const b);
PULE_exportFn PuleF32v2 puleF32v2Abs(PuleF32v2 const a);
//PULE_exportFn PuleF32v2 puleF32v2Mul(PuleF32v2 const a, PuleF32v2 const b);
//PULE_exportFn PuleF32v2 puleF32v2Div(PuleF32v2 const a, PuleF32v2 const b);
//PULE_exportFn PuleF32v2 puleF32v2Dot(PuleF32v2 const a, PuleF32v2 const b);
//
PULE_exportFn PuleF32v3 puleF32v3(float const identity);
PULE_exportFn PuleF32v3 puleF32v3Ptr(float const * const values);
PULE_exportFn PuleF32v3 puleF32v3Add(PuleF32v3 const a, PuleF32v3 const b);
PULE_exportFn PuleF32v3 puleF32v3Sub(PuleF32v3 const a, PuleF32v3 const b);
PULE_exportFn PuleF32v3 puleF32v3Neg(PuleF32v3 const a);
PULE_exportFn PuleF32v3 puleF32v3Mul(PuleF32v3 const a, PuleF32v3 const b);
PULE_exportFn PuleF32v3 puleF32v3Div(PuleF32v3 const a, PuleF32v3 const b);
PULE_exportFn float puleF32v3Dot(PuleF32v3 const a, PuleF32v3 const b);
PULE_exportFn float puleF32v3Length(PuleF32v3 const a);
PULE_exportFn PuleF32v3 puleF32v3Normalize(PuleF32v3 const b);
PULE_exportFn PuleF32v3 puleF32v3Cross(PuleF32v3 const a, PuleF32v3 const b);

PULE_exportFn PuleF32v4 puleF32v4(float const identity);
/* PuleF32v4 pulef32v4Add(pulef32v4 const a, pulef32v4 const b); */
/* PuleF32v4 pulef32v4Sub(pulef32v4 const a, pulef32v4 const b); */
/* PuleF32v4 pulef32v4Mul(pulef32v4 const a, pulef32v4 const b); */
/* PuleF32v4 pulef32v4Div(pulef32v4 const a, pulef32v4 const b); */
/* PuleF32v4 pulef32v4Dot(pulef32v4 const a, pulef32v4 const b); */

// matrices are 'column major', the indices look like
//
//    |  0  4  8 12 |
//    |  1  5  9 13 |
//    |  2  6 10 14 |
//    |  3  7 11 15 |
//
// which compose vectors
//   <0 .. 3>, <4 .. 7>, <8 .. 11>, <12 .. 15>
//
// for example, the 'translation' part of a view matrix would be stored at
//   <12, 13, 14, 15>

typedef struct {
  float elements[16];
} PuleF32m44;

PULE_exportFn PuleF32m44 puleF32m44(float const identity);
PULE_exportFn PuleF32m44 puleF32m44Ptr(float const * const data);
PULE_exportFn PuleF32m44 puleF32m44PtrTranspose(float const * const data);

PULE_exportFn PuleF32m44 puleProjectionPerspective(
  float const fieldOfViewRadians,
  float const aspectRatio,
  float const near,
  float const far
);

// if engine has 3D use, then this should always be the preferred perspective
//PuleF32m44 pulef32v4x4ProjectionPerspectiveReverseZ(

PULE_exportFn PuleF32m44 puleViewLookAt(
  PuleF32v3 const origin,
  PuleF32v3 const center,
  PuleF32v3 const up
);

#ifdef __cplusplus
} // C
#endif
