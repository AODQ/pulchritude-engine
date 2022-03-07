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

typedef struct { float x; float y; } PuleFloat2;
typedef struct { float x; float y; float z; } PuleFloat3;
typedef struct { float x; float y; float z; float w; } PuleFloat4;

PULE_extern PuleFloat2 puleFloat2(float const identity);
PULE_extern PuleFloat3 puleFloat2Ptr(float * values);
//PuleFloat2 puleFloat2Add(puleFloat2 const a, puleFloat2 const b);
//PuleFloat2 puleFloat2Sub(puleFloat2 const a, puleFloat2 const b);
//PuleFloat2 puleFloat2Mul(puleFloat2 const a, puleFloat2 const b);
//PuleFloat2 puleFloat2Div(puleFloat2 const a, puleFloat2 const b);
//PuleFloat2 puleFloat2Dot(puleFloat2 const a, puleFloat2 const b);
//
PULE_extern PuleFloat3 puleFloat3(float const identity);
PULE_extern PuleFloat3 puleFloat3Ptr(float * values);
PULE_extern PuleFloat3 puleFloat3Add(PuleFloat3 const a, PuleFloat3 const b);
PULE_extern PuleFloat3 puleFloat3Sub(PuleFloat3 const a, PuleFloat3 const b);
PULE_extern PuleFloat3 puleFloat3Neg(PuleFloat3 const a);
PULE_extern PuleFloat3 puleFloat3Mul(PuleFloat3 const a, PuleFloat3 const b);
PULE_extern PuleFloat3 puleFloat3Div(PuleFloat3 const a, PuleFloat3 const b);
PULE_extern float puleFloat3Dot(PuleFloat3 const a, PuleFloat3 const b);
PULE_extern float puleFloat3Length(PuleFloat3 const a);
PULE_extern PuleFloat3 puleFloat3Normalize(PuleFloat3 const b);
PULE_extern PuleFloat3 puleFloat3Cross(PuleFloat3 const a, PuleFloat3 const b);

PULE_extern PuleFloat4 puleFloat4(float const identity);
/* PuleFloat4 puleFloat4Add(puleFloat4 const a, puleFloat4 const b); */
/* PuleFloat4 puleFloat4Sub(puleFloat4 const a, puleFloat4 const b); */
/* PuleFloat4 puleFloat4Mul(puleFloat4 const a, puleFloat4 const b); */
/* PuleFloat4 puleFloat4Div(puleFloat4 const a, puleFloat4 const b); */
/* PuleFloat4 puleFloat4Dot(puleFloat4 const a, puleFloat4 const b); */

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
} PuleFloat4x4;

PULE_extern PuleFloat4x4 puleFloat4x4(float const identity);
/* PuleFloat4x4 puleFloat4x4Mul( */
/*   PuleFloat4x4 const * const a, */
/*   PuleFloat4x4 const * const b */
/* ); */
/* PuleFloat4x4 puleFloat4x4Add( */
/*   PuleFloat4x4 const * const a, */
/*   PuleFloat4x4 const * const b */
/* ); */
/* PuleFloat4x4 puleFloat4x4Sub( */
/*   PuleFloat4x4 const * const a, */
/*   PuleFloat4x4 const * const b */
/* ); */
/* PuleFloat4x4 puleFloat4x4MulFloat4( */
/*   PuleFloat4x4 const * const a, PuleFloat4 const * const b */
/* ); */
/* PuleFloat4x4 puleFloat4x4Transpose(PuleFloat4x4 const * const); */

PULE_extern PuleFloat4x4 puleFloat4x4ProjectionPerspective(
  float const fieldOfViewRadians,
  float const aspectRatio,
  float const near,
  float const far
);

// if engine has 3D use, then this should always be the preferred perspective
//PuleFloat4x4 puleFloat4x4ProjectionPerspectiveReverseZ(

PULE_extern PuleFloat4x4 puleFloat4x4ViewLookAt(
  PuleFloat3 const origin,
  PuleFloat3 const center,
  PuleFloat3 const up
);

#ifdef __cplusplus
} // C
#endif
