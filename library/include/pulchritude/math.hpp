/* auto generated file math */
#pragma once
#include "core.hpp"

#include "math.h"
#include <initializer_list>

namespace pule {

using F32 = float;

struct F32v2 {
  float x, y;
};

struct F32v3 {
  float x, y, z;

  F32v3();
  ~F32v3() = default;
  F32v3(F32v3 && ff) = default;
  F32v3(F32v3 const & ff) = default;
  F32v3 & operator=(F32v3 && ff) = default;
  F32v3 & operator=(F32v3 const & ff) = default;

  F32v3(PuleF32v3 const & a);
  F32v3(float x, float y, float z);

  inline operator PuleF32v3() const { return {x,y,z}; }

  PuleF32v3 toFloat() const;

  F32v3 operator+(F32v3 const & rhs) const;
  F32v3 operator-(F32v3 const & rhs) const;
  F32v3 operator/(F32v3 const & rhs) const;
  F32v3 operator*(F32v3 const & rhs) const;
  F32v3 operator-() const;

  F32v3 operator-=(F32v3 const & rhs);

  F32v3 operator/(F32 const & rhs) const;
  F32v3 operator*(F32 const & rhs) const;

  F32v3 operator+=(F32v3 const & rhs);

  // TODO many missing operators, just fill in as needed
  // -=, *=, /=, etc
  // as well as comparison operators
  // as well as their equivalents for scalars
  // as well as CABI-equivalents
};

struct F32v4 {
  float x, y, z, w;

  F32v4();
  ~F32v4() = default;
  F32v4(F32v4 && ff) = default;
  F32v4(F32v4 const & ff) = default;
  F32v4 & operator=(F32v4 && ff) = default;
  F32v4 & operator=(F32v4 const & ff) = default;

  inline operator PuleF32v4() const { return {x,y,z,w,}; }

  F32v4(PuleF32v4 const & a);
  F32v4(float x, float y, float z, float w);
};

struct F32m33 {
  pule::F32 elem[9];

  F32m33(pule::F32 identity = 0.0f);
  F32m33(PuleF32m33 const & a);
  F32m33(std::initializer_list<pule::F32> const & list);
  ~F32m33() = default;
  F32m33(F32m33 && ff) = default;
  F32m33(F32m33 const & ff) = default;
  F32m33 & operator=(F32m33 && ff) = default;
  F32m33 & operator=(F32m33 const & ff) = default;

  inline operator PuleF32m33() const {
    return {
      elem[0], elem[1], elem[2],
      elem[3], elem[4], elem[5],
      elem[6], elem[7], elem[8],
    };
  }

  F32v3 operator *(pule::F32v3 const & rhs) const;
  F32m33 operator *(pule::F32 const & rhs) const;
  F32m33 operator *(F32m33 const & rhs) const;
  F32m33 operator +(F32m33 const & rhs) const;

  F32m33 const & operator *=(F32m33 const & rhs);
  F32m33 const & operator *=(pule::F32 const & rhs);
};

} // namespace pule

// free-form operators
pule::F32 dot(pule::F32v3 const & a, pule::F32v3 const & b);
pule::F32 length(pule::F32v3 const & a);
pule::F32 lengthSqr(pule::F32v3 const & a);
pule::F32v3 cross(pule::F32v3 const & a, pule::F32v3 const & b);
pule::F32v3 normalize(pule::F32v3 const & a);

pule::F32 magnitude(pule::F32v3 const a);

pule::F32 trace(pule::F32m33 const &);
pule::F32 determinant(pule::F32m33 const &);
pule::F32m33 transpose(pule::F32m33 const &);
pule::F32m33 inverse(pule::F32m33 const &);
pule::F32 cofactor(pule::F32m33 const &, size_t row, size_t col);

