#pragma once

#include <pulchritude/core.h>

#include <pulchritude/math.h>

// provides Q31.32 fixed floating-point 64-bit deterministic arithmetic

// Performance is poor, should be avoided if determinism is unnecessary

// tracks floating-point equivalent of a fixed-point value
#ifndef PULE_fixedPointAssert
#define PULE_fixedPointAssert 1
#endif

// precision is 2^-32, which is approx 0.000'000'000'2
#define PULE_fix64BitPrecision 32ll
#define PULE_fix64Limit (1ll<<PULE_fix64BitPrecision)
#define PULE_fix64Range (1ll<<(63ll-PULE_fix64BitPrecision))

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PuleFix64 {
  int64_t value;
#if PULE_fixedPointAssert
  double fpEquiv;
#endif
} PuleFix64;

PULE_exportFn PuleFix64 puleFix64FromF64(double f);
PULE_exportFn double puleFix64ToF64(PuleFix64 f);

PULE_exportFn PuleFix64 puleFix64Add(PuleFix64 a, PuleFix64 b);
PULE_exportFn PuleFix64 puleFix64Sub(PuleFix64 a, PuleFix64 b);
PULE_exportFn PuleFix64 puleFix64Mul(PuleFix64 a, PuleFix64 b);
PULE_exportFn PuleFix64 puleFix64Div(PuleFix64 a, PuleFix64 b);
PULE_exportFn PuleFix64 puleFix64Neg(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Sqrt(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Abs(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Min(PuleFix64 a, PuleFix64 b);
PULE_exportFn PuleFix64 puleFix64Max(PuleFix64 a, PuleFix64 b);

// below functions are not implemented yet, or not deterministic
PULE_exportFn PuleFix64 puleFix64Sin(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Cos(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Tan(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Asin(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Acos(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Atan(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Atan2(PuleFix64 a, PuleFix64 b);
PULE_exportFn PuleFix64 puleFix64Exp(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Log(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Pow(PuleFix64 a, PuleFix64 b);
PULE_exportFn PuleFix64 puleFix64Ceil(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Floor(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Round(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Trunc(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Frac(PuleFix64 a);
PULE_exportFn PuleFix64 puleFix64Mod(PuleFix64 a, PuleFix64 b);

#ifdef __cplusplus
}
#endif

// -- c++ binding --------------------------------------------------------------

#ifdef __cplusplus
namespace pule {

struct Fix64 {
  PuleFix64 cabi;

  Fix64();
  Fix64(double a);
  Fix64(PuleFix64 const & a);
  ~Fix64() = default;
  Fix64(Fix64 && ff) = default;
  Fix64(Fix64 const & ff) = default;
  Fix64 & operator=(Fix64 && ff) = default;
  Fix64 & operator=(Fix64 const & ff) = default;

  inline operator PuleFix64() const { return this->cabi; }

  double toF64() const;

  bool operator==(Fix64 const & rhs) const;
  bool operator<(Fix64 const & rhs) const;
  bool operator>(Fix64 const & rhs) const;
  Fix64 operator+(Fix64 const & rhs) const;
  Fix64 operator-(Fix64 const & rhs) const;
  Fix64 operator/(Fix64 const & rhs) const;
  Fix64 operator*(Fix64 const & rhs) const;
  Fix64 operator-() const;
  Fix64 operator+=(Fix64 const & rhs);
  Fix64 operator-=(Fix64 const & rhs);
  Fix64 operator/=(Fix64 const & rhs);

  Fix64 operator+(PuleFix64 const & rhs) const;
  Fix64 operator-(PuleFix64 const & rhs) const;
  Fix64 operator/(PuleFix64 const & rhs) const;
  Fix64 operator*(PuleFix64 const & rhs) const;
  Fix64 operator+=(PuleFix64 const & rhs);

};

} // namespace pule

// free-form operators
pule::Fix64 operator "" _fix(double long a);
pule::Fix64 sqrt(pule::Fix64 const & a);
pule::Fix64 abs(pule::Fix64 const & a);
pule::Fix64 sin(pule::Fix64 const & a);
pule::Fix64 cos(pule::Fix64 const & a);
pule::Fix64 min(pule::Fix64 const & a, pule::Fix64 const & b);
pule::Fix64 max(pule::Fix64 const & a, pule::Fix64 const & b);

#endif // __cplusplus


// -- vectors/matrices/quats ---------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct { PuleFix64 x; PuleFix64 y; } PuleFix64v2;
typedef struct {
  PuleFix64 x; PuleFix64 y; PuleFix64 z;
} PuleFix64v3;
typedef struct {
  PuleFix64 x; PuleFix64 y; PuleFix64 z; PuleFix64 w;
} PuleFix64v4;

PULE_exportFn PuleFix64v3 puleFix64v3(PuleFix64 identity);
PULE_exportFn PuleFix64v3 puleFix64v3Ptr(PuleFix64 const * values);
PULE_exportFn PuleFix64v3 puleFix64v3Add(PuleFix64v3 a, PuleFix64v3 b);
PULE_exportFn PuleFix64v3 puleFix64v3Sub(PuleFix64v3 a, PuleFix64v3 b);
PULE_exportFn PuleFix64v3 puleFix64v3Neg(PuleFix64v3 a);
PULE_exportFn PuleFix64v3 puleFix64v3Mul(PuleFix64v3 a, PuleFix64v3 b);
PULE_exportFn PuleFix64v3 puleFix64v3Div(PuleFix64v3 a, PuleFix64v3 b);
PULE_exportFn PuleFix64 puleFix64v3Dot(PuleFix64v3 a, PuleFix64v3 b);
PULE_exportFn PuleFix64 puleFix64v3Length(PuleFix64v3 a);
PULE_exportFn PuleFix64v3 puleFix64v3Normalize(PuleFix64v3 b);
PULE_exportFn PuleFix64v3 puleFix64v3Cross(PuleFix64v3 a, PuleFix64v3 b);

PULE_exportFn PuleF32v3 puleFix64v3ToF32(PuleFix64v3 a);

PULE_exportFn PuleFix64v4 puleFix64v4(PuleFix64 identity);

typedef struct {
  PuleFix64 elem[16];
} PuleFix64m44;

PuleFix64v4 puleFix64m44Mulv4(PuleFix64m44 const a, PuleFix64v4 const b);

typedef struct {
  PuleFix64 elem[9];
} PuleFix64m33;

PuleF32m33 puleFix64m33ToF32(PuleFix64m33 a);

typedef struct {
  PuleFix64 x; PuleFix64 y; PuleFix64 z; PuleFix64 w;
} PuleFix64q;

PULE_exportFn PuleFix64q puleFix64qIdentity();
PULE_exportFn PuleFix64q puleFix64qPlane(
  PuleFix64v3 const normal, PuleFix64 const theta
);
PULE_exportFn PuleFix64q puleFix64qFromEuler(PuleFix64v3 const euler);
PULE_exportFn PuleFix64m44 puleFix64qToM44(PuleFix64q const q);

PULE_exportFn PuleFix64q puleFix64qNormalize(PuleFix64q const a);
PULE_exportFn PuleFix64q puleFix64qInvert(PuleFix64q const a);
PULE_exportFn PuleFix64 puleFix64qMagnitude(PuleFix64q const a);
PULE_exportFn PuleFix64 puleFix64qMagnitudeSqr(PuleFix64q const a);
PULE_exportFn PuleFix64v3 puleFix64qRotateV3(
  PuleFix64q const a, PuleFix64v3 const b
);
PULE_exportFn PuleFix64m33 puleFix64qRotateM33(
  PuleFix64q const a, PuleFix64m33 const b
);
PULE_exportFn PuleFix64v3 puleFix64qAxis(PuleFix64q const a);

PULE_exportFn PuleFix64m33 puleFix64qAsM33(PuleFix64q const identity);
PULE_exportFn PuleFix64v4 puleFix64qAsV4(PuleFix64q const identity);

PULE_exportFn PuleFix64q puleFix64qMul(PuleFix64q a, PuleFix64q b);
PULE_exportFn PuleFix64q puleFix64qMulF(PuleFix64q a, PuleFix64 b);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

// -- vectors/matrices/quats C++ bindings --------------------------------------

#ifdef __cplusplus
namespace pule {

struct Fix64v3 {
  pule::Fix64 x, y, z;

  Fix64v3();
  ~Fix64v3() = default;
  Fix64v3(Fix64v3 && ff) = default;
  Fix64v3(Fix64v3 const & ff) = default;
  Fix64v3 & operator=(Fix64v3 && ff) = default;
  Fix64v3 & operator=(Fix64v3 const & ff) = default;

  Fix64v3(PuleFix64v3 const & a);
  Fix64v3(pule::Fix64 x, pule::Fix64 y, pule::Fix64 z);

  inline operator PuleFix64v3() const { return {x,y,z}; }

  PuleF32v3 toFloat() const;

  Fix64v3 operator+(Fix64v3 const & rhs) const;
  Fix64v3 operator-(Fix64v3 const & rhs) const;
  Fix64v3 operator/(Fix64v3 const & rhs) const;
  Fix64v3 operator*(Fix64v3 const & rhs) const;
  Fix64v3 operator-() const;

  Fix64v3 operator-=(Fix64v3 const & rhs);

  Fix64v3 operator/(Fix64 const & rhs) const;
  Fix64v3 operator*(Fix64 const & rhs) const;

  Fix64v3 operator+=(Fix64v3 const & rhs);

  // TODO many missing operators, just fill in as needed
  // -=, *=, /=, etc
  // as well as comparison operators
  // as well as their equivalents for scalars
  // as well as CABI-equivalents
};

struct Fix64v4 {
  pule::Fix64 x, y, z, w;

  Fix64v4();
  ~Fix64v4() = default;
  Fix64v4(Fix64v4 && ff) = default;
  Fix64v4(Fix64v4 const & ff) = default;
  Fix64v4 & operator=(Fix64v4 && ff) = default;
  Fix64v4 & operator=(Fix64v4 const & ff) = default;

  inline operator PuleFix64v4() const { return {x,y,z,w,}; }

  Fix64v4(PuleFix64v4 const & a);
  Fix64v4(pule::Fix64 x, pule::Fix64 y, pule::Fix64 z, pule::Fix64 w);
};

} // namespace pule

// free-form operators
pule::Fix64 dot(pule::Fix64v3 const & a, pule::Fix64v3 const & b);
pule::Fix64 length(pule::Fix64v3 const & a);
pule::Fix64v3 cross(pule::Fix64v3 const & a, pule::Fix64v3 const & b);
pule::Fix64v3 normalize(pule::Fix64v3 const & a);

namespace pule {

struct Fix64q {
  pule::Fix64v4 v4;

  Fix64q();
  Fix64q(pule::Fix64 x, pule::Fix64 y, pule::Fix64 z, pule::Fix64 w);
  Fix64q(pule::Fix64v3 const & axis, pule::Fix64 angle);
  Fix64q(PuleFix64q const & a);

  inline operator PuleFix64q() const { return {v4.x,v4.y,v4.z,v4.w,}; }

  ~Fix64q() = default;
  Fix64q(Fix64q && ff) = default;
  Fix64q(Fix64q const & ff) = default;
  Fix64q & operator=(Fix64q && ff) = default;
  Fix64q & operator=(Fix64q const & ff) = default;

  Fix64q operator *(Fix64q const & rhs) const;
  Fix64q operator *(pule::Fix64 const & rhs) const;

  PuleFix64m33 asM33() const;
  pule::Fix64v3 axis() const;
};

} // namespace pule

// free-form operators
pule::Fix64q normalize(pule::Fix64q const a);
pule::Fix64q invert(pule::Fix64q const a);
pule::Fix64 magnitude(pule::Fix64q const a);
pule::Fix64 magnitudeSqr(pule::Fix64q const a);
pule::Fix64 magnitude(pule::Fix64v3 const a);
pule::Fix64v3 rotateV3(
  pule::Fix64q const a, pule::Fix64 const b
);
PULE_exportFn PuleFix64m33 rotateM33(
  pule::Fix64q const a, PuleFix64m33 const b
);

#endif // __cplusplus

// -- c++ binding 3x3 matrix ---------------------------------------------------
#ifdef __cplusplus

#include <initializer_list>

namespace pule {

struct Fix64m33 {
  pule::Fix64 elem[9];

  Fix64m33(pule::Fix64 identity = 0.0_fix);
  Fix64m33(PuleFix64m33 const & a);
  Fix64m33(std::initializer_list<pule::Fix64> const & list);
  ~Fix64m33() = default;
  Fix64m33(Fix64m33 && ff) = default;
  Fix64m33(Fix64m33 const & ff) = default;
  Fix64m33 & operator=(Fix64m33 && ff) = default;
  Fix64m33 & operator=(Fix64m33 const & ff) = default;

  inline operator PuleFix64m33() const {
    return {
      elem[0], elem[1], elem[2],
      elem[3], elem[4], elem[5],
      elem[6], elem[7], elem[8],
    };
  }

  Fix64v3 operator *(pule::Fix64v3 const & rhs) const;
  Fix64m33 operator *(pule::Fix64 const & rhs) const;
  Fix64m33 operator *(Fix64m33 const & rhs) const;
  Fix64m33 operator +(Fix64m33 const & rhs) const;

  Fix64m33 const & operator *=(Fix64m33 const & rhs);
  Fix64m33 const & operator *=(pule::Fix64 const & rhs);
};

} // pule

pule::Fix64 trace(pule::Fix64m33 const &);
pule::Fix64 determinant(pule::Fix64m33 const &);
pule::Fix64m33 transpose(pule::Fix64m33 const &);
pule::Fix64m33 inverse(pule::Fix64m33 const &);
pule::Fix64 cofactor(pule::Fix64m33 const &, size_t row, size_t col);

#endif // __cplusplus
