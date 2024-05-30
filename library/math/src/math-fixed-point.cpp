#include <pulchritude/math.h>

#include <pulchritude/error.h>
#include <pulchritude/log.h>

#include <cmath>

// higher means more fractional bits, but at the cost of lower range
//constexpr int64_t fix64BitPrecision = PULE_fix64BitPrecision;
//constexpr int64_t fix64Scale = PULE_fix64Limit;
//#if PULE_fixedPointAssert
//constexpr int64_t fix64Range = PULE_fix64Range;
//#endif
//
//// this allows to construct fixed-point from scaled integer, but requires
////   fpEquiv to be defined for the assert-mode tracking fp equivalence
//#if PULE_fixedPointAssert
//#define fix64FpEquiv(v) double const fpEquiv = pint::clampFpRange(v)
//#define fix64Construct(v) \
//  PuleFix64 { .value = (v), .fpEquiv = fpEquiv, }
//#else
//#define fix64FpEquiv(v) (void)0
//#define fix64Construct(v) \
//  PuleFix64 { .value = (v), }
//#endif
//
//namespace pint {
//
//// there is no underflow/overflow in this implementation, things will always
////   be clamped, so assert-mode fp equiv needs to be clamped as well
//#if PULE_fixedPointAssert
//double clampFpRange(double a) {
//  constexpr auto x = -fix64Range;
//  constexpr auto y = +fix64Range;
//  return (a < x ? x : (a > y ? y : a));
//}
//#endif
//
//// this is to contruct fixed-point from scaled integer, but not exposed for now
//PuleFix64 puleFix64FromI64(int64_t in) {
//  fix64FpEquiv((double)in / (double)fix64Scale);
//  return fix64Construct(in);
//}
//
//} // namespace pint
//
//extern "C" {
//
//PuleFix64 puleFix64FromF64(double in) {
//  fix64FpEquiv((double)in);
//  // convert value from [min, max] to [uintmin, uintmax] w/ a mix interp
//  //   mix(int64_min, int64_max, (in - -range)/(range*2))
//  // but in a way that tries to preserve precision
//  double const alpha = in * (double)fix64Scale;
//  return fix64Construct((int64_t)alpha);
//}
//
//double puleFix64ToF64(PuleFix64 const in) {
//  // convert value from [uintmin, uintmax] to [min, max] w/ a mix interp
//  //   mix(-r, +r, (in - MIN)/(MAX - MIN)
//  // but in an optimal way that tries to perserve precision
//  double const alpha = (double)in.value / (double)fix64Scale;
//  return alpha;
//}
//
//PuleFix64 puleFix64Add(PuleFix64 const lhs, PuleFix64 const rhs) {
//  fix64FpEquiv(lhs.fpEquiv + rhs.fpEquiv);
//  int64_t value = lhs.value + rhs.value;
//  // check overflow, if LHS & RHS are same sign and newValue is diff sign
//  if (!((rhs.value < 0) ^ (lhs.value < 0)) && ((value < 0) ^ (lhs.value < 0))) {
//    // clamp
//    value = (rhs.value > 0 ? INT64_MAX : INT64_MIN);
//  }
//  auto const v = fix64Construct(value);
//#if PULE_fixedPointAssert
//  double afeq = v.fpEquiv;
//  double af = puleFix64ToF64(v);
//  if (afeq - 0.001 > af || afeq + 0.001 < af) {
//    puleLogError(
//      "fpEquiv mismatch, %f+%f, fpEquiv: %f result: %f",
//      lhs.fpEquiv, rhs.fpEquiv, afeq, af
//    );
//  }
//#endif
//  return v;
//}
//
//PuleFix64 puleFix64Sub(PuleFix64 const lhs, PuleFix64 rhs) {
//  rhs.value = -rhs.value;
//  rhs.fpEquiv = -rhs.fpEquiv;
//  return puleFix64Add(lhs, rhs);
//}
//
//PuleFix64 puleFix64Mul(PuleFix64 const lhs, PuleFix64 const rhs) {
//  fix64FpEquiv(lhs.fpEquiv * rhs.fpEquiv);
//  // where L = S*x, R = S*y, where P is the scaling factor,
//  //   the value of the product is S^2 * x * y, which will overflow on
//  //   the majority of values in 64-bit space, so need to split the
//  //   operands into 32-bit high and low to emulate 128-bit multiplication
//  //   as the product of each high and low will be in 64-bit space
//
//  uint64_t const lhsLow = (int64_t)lhs.value &  0xFFFFFFFF;
//  int64_t  const lhsHi  = (int64_t)lhs.value >> fix64BitPrecision;
//  uint64_t const rhsLow = (int64_t)rhs.value &  0xFFFFFFFF;
//  int64_t  const rhsHi  = (int64_t)rhs.value >> fix64BitPrecision;
//
//  uint64_t const productLowLow = lhsLow * rhsLow;
//  int64_t const productLowHi  = (int64_t)lhsLow * (int64_t)rhsHi;
//  int64_t const productHiLow  = (int64_t)lhsHi  * (int64_t)rhsLow;
//  int64_t const productHiHi   = (int64_t)lhsHi  * (int64_t)rhsHi;
//
//  uint64_t const lowResult = productLowLow >> fix64BitPrecision;
//  int64_t const hiResult  = productHiHi << fix64BitPrecision;
//
//  uint64_t const sum = productLowHi + productHiLow + lowResult + hiResult;
//
//  auto v = fix64Construct((int64_t)sum);
//  // TODO handle overflow, it's a bit tricky
//#if PULE_fixedPointAssert
//  double afeq = v.fpEquiv;
//  double af = puleFix64ToF64(v);
//  if (afeq - 1.0 > af || afeq + 1.0 < af) {
//    puleLogError(
//      "fpEquiv mismatch, %f*%f (%llu, %llu) (%f, %f),"
//      "fpEquiv: %f result: %f (%llu) (%f)",
//      lhs.fpEquiv, rhs.fpEquiv,
//      lhs.value, rhs.value,
//      puleFix64ToF64(lhs), puleFix64ToF64(rhs),
//      afeq, af, v.value,
//      puleFix64ToF64(v)
//    );
//    PULE_assert(false);
//  }
//#endif
//  return v;
//}
//
//static int32_t countLeadingZeroes(uint64_t value) {
//  int32_t result = 0;
//  while ((value & 0xF000000000000000) == 0) { value <<= 4; result += 4; }
//  while ((value & 0x8000000000000000) == 0) { value <<= 1; result += 1; }
//  return result;
//}
//
//PuleFix64 puleFix64Div(PuleFix64 const lhs, PuleFix64 const rhs) {
//  fix64FpEquiv(lhs.fpEquiv / rhs.fpEquiv);
//  // TODO
//  // in real world cases, zero is a tiny number, so providing an approximate to
//  //   infinity is fine instead of fucking segfaulting ¯\_(x)_/¯
//  if (rhs.value == 0)
//    return PuleFix64 { lhs.value < 0 ? INT64_MIN : INT64_MAX };
//
//  uint64_t remainder = (uint64_t)(lhs.value >= 0 ? lhs.value : -lhs.value);
//  uint64_t divider   = (uint64_t)(rhs.value >= 0 ? rhs.value : -rhs.value);
//  uint64_t quotient = 0ull;
//  int32_t bitPos = 33;
//
//  // perf improvement if divider is divisble by 2^n
//  while ((divider & 0xF) == 0 && bitPos >= 4) {
//    divider >>= 4;
//    bitPos -= 4;
//  }
//
//  // loop over
//  while (remainder != 0 && bitPos >= 0) {
//    int32_t shift = countLeadingZeroes(remainder);
//    if (shift > bitPos) \
//      shift = bitPos;
//
//    remainder <<= shift;
//    bitPos -= shift;
//
//    auto div = remainder / divider;
//    remainder = remainder % divider;
//    quotient += div << bitPos;
//
//    // detect overflow
//    if ((div & ~(0xFFFFFFFFFFFFFFFF >> bitPos)) != 0) {
//      return fix64Construct(
//        ((lhs.value > 0) ^ (rhs.value > 0)) ? INT64_MAX : INT64_MIN
//      );
//    }
//
//    remainder <<= 1;
//    -- bitPos;
//  }
//
//  // rounding
//  ++ quotient;
//  int64_t result = quotient >> 1;
//
//  // handle sign
//  if ((lhs.value > 0) ^ (rhs.value > 0)) \
//    result = -result;
//
//  return fix64Construct((int64_t)result);
//}
//
//PuleFix64 puleFix64Neg(PuleFix64 const in) {
//  fix64FpEquiv(-in.fpEquiv);
//  return fix64Construct(-in.value);
//}
//
//
//#include <cstdio>
//PuleFix64 puleFix64Sqrt(PuleFix64 const in) {
//  fix64FpEquiv(std::sqrt(in.fpEquiv));
//  // they say sqrt is undefined for negative numbers, I say fuck it it's
//  //   well defined here
//  uint64_t num = puleFix64Abs(in).value;
//  auto result = 0ull;
//
//  // second MSB
//  auto bit = 1ull << 62;
//  while (bit > num) bit >>= 2;
//
//  // perform division in 128 bit space by a lo and hi loop
//  for (auto lohiIt = 0; lohiIt < 2; ++ lohiIt) {
//    // top 48 bits
//    while (bit != 0) {
//      if (num >= result + bit) {
//        num -= result + bit;
//        result = (result >> 1) + bit;
//      } else {
//        result = result >> 1;
//      }
//      bit >>= 2;
//    }
//    if (lohiIt != 0) continue;
//    // get lowest 16 bits, then set up next loop
//    if (num > (1ul << 32) - 1) {
//      // shift left by 32, but we know it's too large to do naively
//      num -= result;
//      num = (num << 32) - 0x80000000UL;
//      result = (result << 32) + 0x80000000UL;
//    } else {
//      num <<= 32;
//      result <<= 32;
//    }
//
//    bit = 1ul << 30;
//  }
//  // round
//  if (num > result) ++ result;
//  return fix64Construct((in.value < 0 ? -(int64_t)result : (int64_t)result));
//}
//
//PuleFix64 puleFix64Abs(PuleFix64 const in) {
//#if PULE_fixedPointAssert
//  fix64FpEquiv(std::abs(in.fpEquiv));
//#endif
//  return fix64Construct(std::abs(in.value));
//}
//
//PuleFix64 puleFix64Min(PuleFix64 const lhs, PuleFix64 const rhs) {
//#if PULE_fixedPointAssert
//  fix64FpEquiv(std::min(lhs.fpEquiv, rhs.fpEquiv));
//#endif
//  return fix64Construct(std::min(lhs.value, rhs.value));
//}
//
//PuleFix64 puleFix64Max(PuleFix64 const lhs, PuleFix64 const rhs) {
//#if PULE_fixedPointAssert
//  fix64FpEquiv(std::max(lhs.fpEquiv, rhs.fpEquiv));
//#endif
//  return fix64Construct(std::max(lhs.value, rhs.value));
//}
//
//PULE_exportFn PuleFix64 puleFix64Sin(PuleFix64 a) {
//  // TODO implement sine using deterministic fixed-point arithmetic
//  // for now just use the floating-point version
//  auto f64 = puleFix64ToF64(a);
//  f64 = std::sin(f64);
//  return puleFix64FromF64(f64);
//}
//PULE_exportFn PuleFix64 puleFix64Cos(PuleFix64 a) {
//  auto f64 = puleFix64ToF64(a);
//  f64 = std::cos(f64);
//  return puleFix64FromF64(f64);
//}
//PULE_exportFn PuleFix64 puleFix64Atan2(PuleFix64 a, PuleFix64 b) {
//  auto f64 = std::atan2(puleFix64ToF64(a), puleFix64ToF64(b));
//  return puleFix64FromF64(f64);
//}
//PULE_exportFn PuleFix64 puleFix64Asin(PuleFix64 a) {
//  auto f64 = std::asin(puleFix64ToF64(a));
//  return puleFix64FromF64(f64);
//}
//PULE_exportFn PuleFix64 puleFix64Acos(PuleFix64 a) {
//  auto f64 = std::acos(puleFix64ToF64(a));
//  return puleFix64FromF64(f64);
//}
//
//} // extern "C"
//
//pule::Fix64::Fix64() {
//  this->cabi.value = 0;
//#if PULE_fixedPointAssert
//  this->cabi.fpEquiv = 0.0;
//#endif
//}
//
//pule::Fix64::Fix64(double a) {
//  this->cabi = puleFix64FromF64(a);
//}
//
//pule::Fix64::Fix64(PuleFix64 const & a) {
//  this->cabi = a;
//}
//
//double pule::Fix64::toF64() const {
//  return puleFix64ToF64(this->cabi);
//}
//
//
//bool pule::Fix64::operator==(pule::Fix64 const & rhs) const {
//  return this->cabi.value == rhs.cabi.value;
//}
//bool pule::Fix64::operator<(pule::Fix64 const & rhs) const {
//  return this->cabi.value < rhs.cabi.value;
//}
//bool pule::Fix64::operator>(pule::Fix64 const & rhs) const {
//  return this->cabi.value > rhs.cabi.value;
//}
//pule::Fix64 pule::Fix64::operator+(pule::Fix64 const & rhs) const {
//  return puleFix64Add(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator-(pule::Fix64 const & rhs) const {
//  return puleFix64Sub(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator*(pule::Fix64 const & rhs) const {
//  return puleFix64Mul(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator/(pule::Fix64 const & rhs) const {
//  return puleFix64Div(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator-() const {
//  return puleFix64Neg(*this);
//}
//pule::Fix64 pule::Fix64::operator+=(Fix64 const & rhs) {
//  this->cabi = puleFix64Add(*this, rhs.cabi);
//  return *this;
//}
//pule::Fix64 pule::Fix64::operator-=(Fix64 const & rhs) {
//  this->cabi = puleFix64Sub(*this, rhs.cabi);
//  return *this;
//}
//pule::Fix64 pule::Fix64::operator/=(Fix64 const & rhs) {
//  this->cabi = puleFix64Div(*this, rhs.cabi);
//  return *this;
//}
//
//pule::Fix64 pule::Fix64::operator+(PuleFix64 const & rhs) const {
//  return puleFix64Add(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator-(PuleFix64 const & rhs) const {
//  return puleFix64Sub(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator*(PuleFix64 const & rhs) const {
//  return puleFix64Mul(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator/(PuleFix64 const & rhs) const {
//  return puleFix64Div(*this, rhs);
//}
//pule::Fix64 pule::Fix64::operator+=(PuleFix64 const & rhs) {
//  this->cabi = puleFix64Add(*this, rhs);
//  return *this;
//}
//
//pule::Fix64 operator "" _fix(long double a) {
//  return pule::Fix64(a);
//}
//
//pule::Fix64 sqrt(pule::Fix64 const & a) { return puleFix64Sqrt(a); }
//pule::Fix64 abs(pule::Fix64 const & a) { return puleFix64Abs(a); }
//pule::Fix64 sin(pule::Fix64 const & a) { return puleFix64Sin(a); }
//pule::Fix64 cos(pule::Fix64 const & a) { return puleFix64Cos(a); }
//
//pule::Fix64 min(pule::Fix64 const & a, pule::Fix64 const & b) {
//  return puleFix64Min(a, b);
//}
//
//pule::Fix64 max(pule::Fix64 const & a, pule::Fix64 const & b) {
//  return puleFix64Max(a, b);
//}
//
//// -- vector/matrices/quats ----------------------------------------------------
//extern "C" {
//
//PuleFix64v3 puleFix64v3(PuleFix64 const identity) {
//  return PuleFix64v3 { .x = identity, .y = identity, .z = identity };
//}
//
//PuleFix64v3 puleFix64v3Ptr(PuleFix64  const * const values) {
//  return PuleFix64v3 { .x = values[0], .y = values[1], .z = values[2] };
//}
//PuleFix64v3 puleFix64v3Add(PuleFix64v3 a, PuleFix64v3 b) {
//  return PuleFix64v3 {
//    .x = puleFix64Add(a.x, b.x),
//    .y = puleFix64Add(a.y, b.y),
//    .z = puleFix64Add(a.z, b.z)
//  };
//}
//PuleFix64v3 puleFix64v3Sub(PuleFix64v3 a, PuleFix64v3 b) {
//  return PuleFix64v3 {
//    .x = puleFix64Sub(a.x, b.x),
//    .y = puleFix64Sub(a.y, b.y),
//    .z = puleFix64Sub(a.z, b.z)
//  };
//}
//PuleFix64v3 puleFix64v3Neg(PuleFix64v3 a) {
//  return PuleFix64v3 {
//    .x = puleFix64Neg(a.x),
//    .y = puleFix64Neg(a.y),
//    .z = puleFix64Neg(a.z)
//  };
//}
//PuleFix64v3 puleFix64v3Mul(PuleFix64v3 a, PuleFix64v3 b) {
//  return PuleFix64v3 {
//    .x = puleFix64Mul(a.x, b.x),
//    .y = puleFix64Mul(a.y, b.y),
//    .z = puleFix64Mul(a.z, b.z)
//  };
//}
//PuleFix64v3 puleFix64v3Div(PuleFix64v3 a, PuleFix64v3 b) {
//  return PuleFix64v3 {
//    .x = puleFix64Div(a.x, b.x),
//    .y = puleFix64Div(a.y, b.y),
//    .z = puleFix64Div(a.z, b.z)
//  };
//}
//PuleFix64 puleFix64v3Dot(PuleFix64v3 a, PuleFix64v3 b) {
//  return (
//    puleFix64Add(
//      puleFix64Mul(a.x, b.x),
//      puleFix64Add(puleFix64Mul(a.y, b.y), puleFix64Mul(a.z, b.z))
//    )
//  );
//}
//PuleFix64 puleFix64v3Length(PuleFix64v3 a) {
//  return puleFix64Sqrt(puleFix64v3Dot(a, a));
//}
//PuleFix64v3 puleFix64v3Normalize(PuleFix64v3 b) {
//  PuleFix64 const length = puleFix64v3Length(b);
//  if (length.value == 0) { return puleFix64v3(0.0_fix); }
//  return puleFix64v3Div(b, puleFix64v3(length));
//}
//PuleFix64v3 puleFix64v3Cross(PuleFix64v3 a, PuleFix64v3 b) {
//  return PuleFix64v3 {
//    .x = puleFix64Sub(puleFix64Mul(a.y, b.z), puleFix64Mul(a.z, b.y)),
//    .y = puleFix64Sub(puleFix64Mul(a.z, b.x), puleFix64Mul(a.x, b.z)),
//    .z = puleFix64Sub(puleFix64Mul(a.x, b.y), puleFix64Mul(a.y, b.x))
//  };
//}
//
//PuleF32v3 puleFix64v3ToF32(PuleFix64v3 a) {
//  return PuleF32v3 {
//    .x = (float)puleFix64ToF64(a.x),
//    .y = (float)puleFix64ToF64(a.y),
//    .z = (float)puleFix64ToF64(a.z)
//  };
//}
//
//PuleFix64v4 puleFix64v4(PuleFix64 const identity) {
//  return PuleFix64v4 {
//    .x = identity, .y = identity, .z = identity, .w = identity
//  };
//}
//
//PuleFix64q puleFix64qIdentity() {
//  return PuleFix64q {
//    .x = 0.0_fix,
//    .y = 0.0_fix,
//    .z = 0.0_fix,
//    .w = 1.0_fix,
//  };
//}
//
//PuleFix64q puleFix64qPlane(PuleFix64v3 const normal, PuleFix64 const theta) {
//  pule::Fix64 halfTheta = pule::Fix64(theta) * 0.5_fix;
//  pule::Fix64 const sinHalfTheta = sin(halfTheta);
//  return PuleFix64q {
//    .x = sinHalfTheta * normal.x,
//    .y = sinHalfTheta * normal.y,
//    .z = sinHalfTheta * normal.z,
//    .w = cos(halfTheta),
//  };
//}
//
//PuleFix64q puleFix64qNormalize(PuleFix64q const a) {
//  PuleFix64 const magnitude = puleFix64qMagnitude(a);
//  if (magnitude.value == 0) { return puleFix64qIdentity(); }
//  return PuleFix64q {
//    .x = puleFix64Div(a.x, magnitude),
//    .y = puleFix64Div(a.y, magnitude),
//    .z = puleFix64Div(a.z, magnitude),
//    .w = puleFix64Div(a.w, magnitude),
//  };
//}
//
//PuleFix64q puleFix64qInvert(PuleFix64q const a) {
//  pule::Fix64q const inv = (
//    pule::Fix64q(a) * (1.0_fix / puleFix64qMagnitude(a))
//  );
//  return PuleFix64q {
//    .x = -inv.v4.x,
//    .y = -inv.v4.y,
//    .z = -inv.v4.z,
//    .w =  inv.v4.w
//  };
//}
//
//PuleFix64 puleFix64qMagnitude(PuleFix64q const a) {
//  return puleFix64Sqrt(puleFix64qMagnitudeSqr(a));
//}
//
//PuleFix64 puleFix64qMagnitudeSqr(PuleFix64q const a) {
//  pule::Fix64v4 const v4 = pule::Fix64v4(a.x, a.y, a.z, a.w);
//  return (v4.x*v4.x + v4.y*v4.y + v4.z*v4.z + v4.w*v4.w);
//}
//
//PuleFix64v3 puleFix64qRotateV3(
//  PuleFix64q const a, PuleFix64v3 const b
//) {
//  PuleFix64v3 bNor = puleFix64v3Normalize(b);
//  PuleFix64q q = PuleFix64q{bNor.x, bNor.y, bNor.z, 0.0_fix};
//  q = puleFix64qMul(a, q);
//  q = puleFix64qMul(puleFix64qInvert(a), q);
//  return PuleFix64v3{q.x, q.y, q.z};
//}
//
//PuleFix64m33 puleFix64qRotateM33(
//  PuleFix64q const a, PuleFix64m33 const b
//) {
//  PuleFix64v3 const rows[3] = {
//    puleFix64qRotateV3(a, PuleFix64v3{b.elem[0], b.elem[1], b.elem[2]}),
//    puleFix64qRotateV3(a, PuleFix64v3{b.elem[3], b.elem[4], b.elem[5]}),
//    puleFix64qRotateV3(a, PuleFix64v3{b.elem[6], b.elem[7], b.elem[8]})
//  };
//  return {
//    PuleFix64m33 {
//      .elem = {
//        rows[0].x, rows[0].y, rows[0].z,
//        rows[1].x, rows[1].y, rows[1].z,
//        rows[2].x, rows[2].y, rows[2].z
//      },
//    }
//  };
//}
//PuleFix64v3 puleFix64qAxis(PuleFix64q const a) {
//  return PuleFix64v3{a.x, a.y, a.z};
//}
//
//PuleFix64m33 puleFix64qAsM33(PuleFix64q const id) {
//  PuleFix64v3 const rows[3] = {
//    puleFix64qRotateV3(id, PuleFix64v3{1.0_fix, 0.0_fix, 0.0_fix}),
//    puleFix64qRotateV3(id, PuleFix64v3{0.0_fix, 1.0_fix, 0.0_fix}),
//    puleFix64qRotateV3(id, PuleFix64v3{0.0_fix, 0.0_fix, 1.0_fix}),
//  };
//  return (
//    PuleFix64m33 {
//      .elem = {
//        rows[0].x, rows[0].y, rows[0].z,
//        rows[1].x, rows[1].y, rows[1].z,
//        rows[2].x, rows[2].y, rows[2].z,
//      },
//    }
//  );
//}
//PuleFix64v4 puleFix64qAsV4(PuleFix64q const id) {
//  return PuleFix64v4 { .x = id.x, .y = id.y, .z = id.z, .w = id.w };
//}
//
//PuleFix64q puleFix64qFromEuler(PuleFix64v3 const euler) {
//  PuleFix64q const qX = (
//    puleFix64qPlane(PuleFix64v3{1.0_fix, 0.0_fix, 0.0_fix}, euler.x)
//  );
//  PuleFix64q const qY = (
//    puleFix64qPlane(PuleFix64v3{0.0_fix, 1.0_fix, 0.0_fix}, euler.y)
//  );
//  PuleFix64q const qZ = (
//    puleFix64qPlane(PuleFix64v3{0.0_fix, 0.0_fix, 1.0_fix}, euler.z)
//  );
//
//  return puleFix64qMul(puleFix64qMul(qX, qY), qZ);
//}
//
//PuleFix64q puleFix64qMul(PuleFix64q const a, PuleFix64q const b) {
//  // sigh just ezier to do c++
//  pule::Fix64v4 const l = pule::Fix64q(a).v4;
//  pule::Fix64v4 const r = pule::Fix64q(b).v4;
//  return PuleFix64q {
//    .x = l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z,
//    .y = l.x*r.w + l.w*r.x + l.y*r.z - l.z*r.y,
//    .z = l.y*r.w + l.w*r.y + l.z*r.x - l.x*r.z,
//    .w = l.z*r.w + l.w*r.z + l.x*r.y - l.y*r.x,
//  };
//}
//PuleFix64q puleFix64qMulF(PuleFix64q pa, PuleFix64 pb) {
//  pule::Fix64v4 const a = pule::Fix64q(pa).v4;
//  pule::Fix64 const b = pule::Fix64(pb);
//  return PuleFix64q { .x = (a.x*b), .y = a.y*b, .z = a.z*b, .w = a.w*b, };
//}
//
//} // extern "C"
//
//// -- c++ binding --------------------------------------------------------------
//
//pule::Fix64v3::Fix64v3(PuleFix64v3 const & a) {
//  this->x = a.x; this->y = a.y; this->z = a.z;
//}
//pule::Fix64v3::Fix64v3() {
//  this->x = 0.0_fix; this->y = 0.0_fix; this->z = 0.0_fix;
//}
//pule::Fix64v3::Fix64v3(pule::Fix64 x, pule::Fix64 y, pule::Fix64 z) {
//  this->x = x; this->y = y; this->z = z;
//}
//PuleF32v3 pule::Fix64v3::toFloat() const {
//  return PuleF32v3 {
//    .x = (float)this->x.toF64(),
//    .y = (float)this->y.toF64(),
//    .z = (float)this->z.toF64(),
//  };
//}
//pule::Fix64v3 pule::Fix64v3::operator+(pule::Fix64v3 const & rhs) const {
//  return puleFix64v3Add(*this, rhs);
//}
//pule::Fix64v3 pule::Fix64v3::operator-(pule::Fix64v3 const & rhs) const {
//  return puleFix64v3Sub(*this, rhs);
//}
//pule::Fix64v3 pule::Fix64v3::operator-() const {
//  return puleFix64v3Neg(*this);
//}
//pule::Fix64v3 pule::Fix64v3::operator*(pule::Fix64v3 const & rhs) const {
//  return puleFix64v3Mul(*this, rhs);
//}
//pule::Fix64v3 pule::Fix64v3::operator/(pule::Fix64v3 const & rhs) const {
//  return puleFix64v3Div(*this, rhs);
//}
//pule::Fix64v3 pule::Fix64v3::operator/(pule::Fix64 const & rhs) const {
//  return { this->x / rhs, this->y / rhs, this->z / rhs };
//}
//pule::Fix64v3 pule::Fix64v3::operator*(pule::Fix64 const & rhs) const {
//  return { this->x * rhs, this->y * rhs, this->z * rhs };
//}
//pule::Fix64v3 pule::Fix64v3::operator+=(Fix64v3 const & rhs) {
//  this->x += rhs.x; this->y += rhs.y; this->z += rhs.z;
//  return *this;
//}
//pule::Fix64v3 pule::Fix64v3::operator-=(pule::Fix64v3 const & rhs) {
//  this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z;
//  return *this;
//}
//pule::Fix64 dot(pule::Fix64v3 const & a, pule::Fix64v3 const & b) {
//  return puleFix64v3Dot(a, b);
//}
//pule::Fix64 length(pule::Fix64v3 const & a) {
//  return puleFix64v3Length(a);
//}
//pule::Fix64v3 cross(pule::Fix64v3 const & a, pule::Fix64v3 const & b) {
//  return puleFix64v3Cross(a, b);
//}
//pule::Fix64v3 normalize(pule::Fix64v3 const & a) {
//  return puleFix64v3Normalize(a);
//}
//
//pule::Fix64v4::Fix64v4() {
//  this->x = 0.0_fix; this->y = 0.0_fix;
//  this->z = 0.0_fix; this->w = 0.0_fix;
//}
//pule::Fix64v4::Fix64v4(PuleFix64v4 const & a) {
//  this->x = a.x; this->y = a.y; this->z = a.z; this->w = a.w;
//}
//pule::Fix64v4::Fix64v4(
//  pule::Fix64 x, pule::Fix64 y,
//  pule::Fix64 z, pule::Fix64 w
//) {
//  this->x = x; this->y = y; this->z = z; this->w = w;
//}
//
//pule::Fix64q::Fix64q() {
//  this->v4 = {0.0_fix, 0.0_fix, 0.0_fix, 1.0_fix};
//}
//pule::Fix64q::Fix64q(pule::Fix64v3 const & axis, pule::Fix64 angle) {
//  auto cabi = puleFix64qPlane(axis, angle);
//  this->v4 = { cabi.x, cabi.y, cabi.z, cabi.w };
//}
//pule::Fix64q::Fix64q(
//  pule::Fix64 x, pule::Fix64 y, pule::Fix64 z, pule::Fix64 w
//) {
//  this->v4 = {x, y, z, w};
//}
//pule::Fix64q::Fix64q(PuleFix64q const & a) {
//  this->v4 = { a.x, a.y, a.z, a.w };
//}
//
//pule::Fix64q pule::Fix64q::operator*(pule::Fix64q const & a) const {
//  return puleFix64qMul(*this, a);
//}
//pule::Fix64q pule::Fix64q::operator*(pule::Fix64 const & a) const {
//  return puleFix64qMulF(*this, a);
//}
//
//PuleFix64m33 pule::Fix64q::asM33() const {
//  return puleFix64qAsM33(*this);
//}
//pule::Fix64v3 pule::Fix64q::axis() const {
//  return puleFix64qAxis(*this);
//}
//
//pule::Fix64q normalize(pule::Fix64q const a) {
//  return puleFix64qNormalize(a);
//}
//pule::Fix64q invert(pule::Fix64q const a) {
//  return puleFix64qInvert(a);
//}
//pule::Fix64 magnitude(pule::Fix64q const a) {
//  return puleFix64qMagnitude(a);
//}
//pule::Fix64 magnitudeSqr(pule::Fix64q const a) {
//  return puleFix64qMagnitudeSqr(a);
//}
//pule::Fix64v3 rotateV3(pule::Fix64q const a, pule::Fix64v3 const b) {
//  return puleFix64qRotateV3(a, b);
//}
//PuleFix64m33 rotateM33(pule::Fix64q const a, PuleFix64m33 const b) {
//  return puleFix64qRotateM33(a, b);
//}
//
//// -- matrix
//
//extern "C" {
//
//PuleFix64v4 puleFix64m44Mulv4(PuleFix64m44 const a, PuleFix64v4 bp) {
//  pule::Fix64v4 const b = pule::Fix64v4(bp);
//  return PuleFix64v4 {
//    .x = b.x*a.elem[0] + b.y*a.elem[4] + b.z*a.elem[8] + b.w*a.elem[12],
//    .y = b.x*a.elem[1] + b.y*a.elem[5] + b.z*a.elem[9] + b.w*a.elem[13],
//    .z = b.x*a.elem[2] + b.y*a.elem[6] + b.z*a.elem[10] + b.w*a.elem[14],
//    .w = b.x*a.elem[3] + b.y*a.elem[7] + b.z*a.elem[11] + b.w*a.elem[15],
//  };
//}
//
//PuleF32m33 puleFix64m33ToF32(PuleFix64m33 a) {
//  return PuleF32m33 {
//    .elem = {
//      (float)puleFix64ToF64(a.elem[0]),
//      (float)puleFix64ToF64(a.elem[1]),
//      (float)puleFix64ToF64(a.elem[2]),
//
//      (float)puleFix64ToF64(a.elem[3]),
//      (float)puleFix64ToF64(a.elem[4]),
//      (float)puleFix64ToF64(a.elem[5]),
//
//      (float)puleFix64ToF64(a.elem[6]),
//      (float)puleFix64ToF64(a.elem[7]),
//      (float)puleFix64ToF64(a.elem[8]),
//    }
//  };
//}
//
//}
//
//// AT SOME POINT, I will be able to run unit tests on all the modules
////    puledit unittest
//// or something.
//#include <pulchritude/log.h>
//namespace {
//template <typename Fn> bool run(char const * label, Fn fn) {
//  auto const ret = fn();
//  double af = ret.toF64();
//  if (af - 0.001 > ret.cabi.fpEquiv || af + 0.001 < ret.cabi.fpEquiv) {
//    puleLogError(
//      "failed '%s', got %.5f expect %.4f", label, af, ret.cabi.fpEquiv
//    );
//    return false;
//  } else {
//    puleLog("passed '%s', got %.5f", label, af);
//    return true;
//  }
//}
//}
//extern "C" {
//
//bool pulcUnitTest() {
//  bool passed = true;
//  #define runMacro(a) run(#a, a)
//  passed &= runMacro([](){ return 1.0_fix; });
//  passed &= runMacro([](){ return 0.0_fix; });
//  passed &= runMacro([](){ return -1.0_fix; });
//  passed &= runMacro([](){ return 200.0_fix; });
//  passed &= runMacro([](){ return 200.0_fix * 1024.0_fix * 1024._fix; });
//  passed &= runMacro([](){ return -200.0_fix; });
//
//  passed &= runMacro([](){ return 1.0_fix + 1.0_fix; });
//  passed &= runMacro([](){ return 1.0_fix + 0.0_fix; });
//  passed &= runMacro([](){ return 5.0_fix + 5.0_fix; });
//  passed &= runMacro([](){ return 1.2349_fix + 3.23_fix; });
//  passed &= runMacro([](){ return 8.0_fix + 3.23_fix; });
//
//  passed &= runMacro([](){ return 1.0_fix + 1.0_fix; });
//  passed &= runMacro([](){ return 1.0_fix + 0.0_fix; });
//  passed &= runMacro([](){ return 5.0_fix + 5.0_fix; });
//  passed &= runMacro([](){ return 1.2349_fix + -3.23_fix; });
//  passed &= runMacro([](){ return 8.0_fix + 3.23_fix; });
//
//  passed &= runMacro([](){ return 1.0_fix * 1.0_fix; });
//  passed &= runMacro([](){ return 1.0_fix * 0.0_fix; });
//  passed &= runMacro([](){ return 5.0_fix * 5.0_fix; });
//  passed &= runMacro([](){ return 0.298_fix * 3.32923_fix; });
//  passed &= runMacro([](){ return 8.0_fix * -3.23_fix; });
//
//  passed &= runMacro([](){ return 1.0_fix / 1.0_fix; });
//  passed &= runMacro([](){ return 5.0_fix / 5.0_fix; });
//  passed &= runMacro([](){ return 5.0_fix / 1.0_fix; });
//  passed &= runMacro([](){ return 1.0_fix / 5.0_fix; });
//  passed &= runMacro([](){ return 10.0_fix / 0.01_fix; });
//  passed &= runMacro([](){ return 1.0_fix / 20.0_fix; });
//  passed &= runMacro([](){ return 1.0_fix / 1000.0_fix; });
//  passed &= runMacro([](){ return 0.1_fix / 10.0_fix; });
//  passed &= runMacro([](){ return 100.0_fix; });
//  passed &= runMacro([](){ return 10.0_fix / 1.0_fix; });
//  passed &= runMacro([](){ return 1000.0_fix / 0.83923_fix; });
//  passed &= runMacro([](){ return 10.0_fix / 0.1_fix; });
//  passed &= runMacro([](){ return 0.1_fix / 10.1_fix; });
//  passed &= runMacro([](){ return 850.0_fix / 5.2823_fix; });
//
//  passed &= runMacro([](){ return sqrt(850.0_fix); });
//  passed &= runMacro([](){ return sqrt(30'000.0_fix); });
//  passed &= runMacro([](){ return sqrt(1.0_fix); });
//  passed &= runMacro([](){ return sqrt(0.005_fix); });
//  passed &= runMacro([](){ return sqrt(9.5_fix); });
//  passed &= runMacro([](){ return sqrt(29.91_fix); });
//  passed &= runMacro([](){ return sqrt(0.21_fix); });
//  return passed;
//}
//} // extern C
//
//// -- c++ binding 3x3 matrix ---------------------------------------------------
//pule::Fix64m33::Fix64m33(pule::Fix64 identity) {
//  this->elem[0] = identity; this->elem[1] = 0.0_fix; this->elem[2] = 0.0_fix;
//  this->elem[3] = 0.0_fix; this->elem[4] = identity; this->elem[5] = 0.0_fix;
//  this->elem[6] = 0.0_fix; this->elem[7] = 0.0_fix; this->elem[8] = identity;
//}
//
//pule::Fix64m33::Fix64m33(PuleFix64m33 const & a) {
//  for (size_t i = 0; i < 9; ++ i) this->elem[i] = a.elem[i];
//}
//
//pule::Fix64m33::Fix64m33(std::initializer_list<pule::Fix64> const & list) {
//  size_t i = 0;
//  for (auto const & elem : list) {
//    this->elem[i] = elem;
//    ++ i;
//    if (i >= 9) break;
//  }
//  for (; i < 9; ++ i) this->elem[i] = 0.0_fix;
//}
//
//pule::Fix64 trace(pule::Fix64m33 const & m) {
//  auto const xx = m.elem[0] * m.elem[0];
//  auto const yy = m.elem[4] * m.elem[4];
//  auto const zz = m.elem[8] * m.elem[8];
//  return xx + yy + zz;
//}
//
//pule::Fix64 determinant(pule::Fix64m33 const & m) {
//  auto const & elem = m.elem;
//  return (
//      elem[0] * (elem[4] * elem[8] - elem[5] * elem[7])
//    - elem[1] * (elem[3] * elem[8] - elem[5] * elem[6])
//    + elem[2] * (elem[3] * elem[7] - elem[4] * elem[6])
//  );
//}
//
//pule::Fix64m33 transpose(pule::Fix64m33 const & m) {
//  pule::Fix64m33 ret;
//  for (size_t i = 0; i < 3; ++ i) \
//  for (size_t j = 0; j < 3; ++ j) {
//    ret.elem[i*3 + j] = m.elem[j*3 + i];
//  }
//  return ret;
//}
//
//
//pule::Fix64m33 inverse(pule::Fix64m33 const & m) {
//  pule::Fix64m33 inv;
//  for (size_t i = 0; i < 3; ++ i) \
//  for (size_t j = 0; j < 3; ++ j) {
//    inv.elem[i*3 + j] = cofactor(m, i, j);
//  }
//  auto det = determinant(m);
//  // if determinant is too low, then clamp
//  if (det < 0.01_fix) det = 0.01_fix;
//  inv *= (1.0_fix / det);
//  return inv;
//}
//
//pule::Fix64 cofactor(pule::Fix64m33 const & m, size_t i, size_t j) {
//  // first compute the minor
//  pule::Fix64 minor[4];
//  size_t yy = 0;
//  for (size_t y = 0; y < 3; ++ y) {
//    if (y == i) continue;
//    size_t xx = 0;
//    for (size_t x = 0; x < 3; ++ x) {
//      if (x == j) continue;
//      minor[yy*2 + xx] = m.elem[y*3 + x];
//      ++ xx;
//    }
//    ++ yy;
//  }
//
//  // then compute determinant
//  pule::Fix64 det = minor[0]*minor[3] - minor[1]*minor[2];
//  return ((i+j) % 2 == 0) ? det : -det;
//}
//
//pule::Fix64v3 pule::Fix64m33::operator *(pule::Fix64v3 const & rhs) const {
//  auto xyz = pule::Fix64v3();
//  xyz.x = rhs.x*this->elem[0] + rhs.y*this->elem[3] + rhs.z*this->elem[6];
//  xyz.y = rhs.x*this->elem[1] + rhs.y*this->elem[4] + rhs.z*this->elem[7];
//  xyz.z = rhs.x*this->elem[2] + rhs.y*this->elem[5] + rhs.z*this->elem[8];
//  return xyz;
//}
//pule::Fix64m33 pule::Fix64m33::operator *(pule::Fix64 const & rhs) const {
//  pule::Fix64m33 ret;
//  for (size_t i = 0; i < 9; ++ i) ret.elem[i] = this->elem[i] * rhs;
//  return ret;
//}
//pule::Fix64m33 pule::Fix64m33::operator *(pule::Fix64m33 const & rhs) const {
//  pule::Fix64m33 ret;
//  for (size_t i = 0; i < 3; ++ i) \
//  for (size_t j = 0; j < 3; ++ j) {
//    auto x = pule::Fix64v3 {
//      this->elem[i*3], this->elem[i*3 + 1], this->elem[i*3 + 2]
//    };
//    auto y = pule::Fix64v3 {
//      rhs.elem[j], rhs.elem[j + 3], rhs.elem[j + 6]
//    };
//    ret.elem[i*3 + j] = dot(x, y);
//  }
//  return ret;
//}
//pule::Fix64m33 pule::Fix64m33::operator +(pule::Fix64m33 const & rhs) const {
//  pule::Fix64m33 ret;
//  for (size_t i = 0; i < 9; ++ i) ret.elem[i] = this->elem[i] + rhs.elem[i];
//  return ret;
//}
//
//pule::Fix64m33 const & pule::Fix64m33::operator *=(pule::Fix64m33 const & rhs) {
//  *this = *this * rhs;
//  return *this;
//}
//pule::Fix64m33 const & pule::Fix64m33::operator *=(pule::Fix64 const & rhs) {
//  *this = *this * rhs;
//  return *this;
//}
