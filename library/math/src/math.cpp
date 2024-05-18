#include <pulchritude-math/math.h>

#include <pulchritude-log/log.h>

#include <cmath>

extern "C" {

PuleF32v2 puleF32v2(float const identity) {
  return PuleF32v2 {
    .x = identity,
    .y = identity,
  };
}
PuleF32v2 puleF32v2Ptr(float const * const value) {
  return PuleF32v2 { .x = *value, .y = *(value+1) };
}
PuleF32v2 puleF32v2Add(PuleF32v2 const a, PuleF32v2 const b) {
  return PuleF32v2 { .x = a.x+b.x, .y = a.y+b.y };
}
PuleF32v2 puleF32v2Sub(PuleF32v2 const a, PuleF32v2 const b) {
  return PuleF32v2 { .x = a.x-b.x, .y = a.y-b.y };
}
PuleF32v2 puleF32v2Abs(PuleF32v2 const a) {
  return PuleF32v2 { .x = fabsf(a.x), .y = fabsf(a.y) };
}
PuleF32v2 puleF32v2Mul(PuleF32v2 const a, PuleF32v2 const b) {
  return PuleF32v2 { .x = a.x*b.x, .y = a.y*b.y };
}
PuleF32v2 puleF32v2MulScalar(PuleF32v2 const a, float const b) {
  return PuleF32v2 { .x = a.x*b, .y = a.y*b };
}

PuleI32v2 puleI32v2(int32_t const identity) {
  return PuleI32v2 {
    .x = identity,
    .y = identity,
  };
}
PuleI32v2 puleI32v2Ptr(int32_t const * const value) {
  return PuleI32v2 { .x = *value, .y = *(value+1) };
}
PuleI32v2 puleI32v2Add(PuleI32v2 const a, PuleI32v2 const b) {
  return PuleI32v2 { .x = a.x+b.x, .y = a.y+b.y };
}
PuleI32v2 puleI32v2Sub(PuleI32v2 const a, PuleI32v2 const b) {
  return PuleI32v2 { .x = a.x-b.x, .y = a.y-b.y };
}
PuleI32v2 puleI32v2Abs(PuleI32v2 const a) {
  return (
    PuleI32v2 {
      .x = a.x > 0 ? a.x : -a.x,
      .y = a.y > 0 ? a.y : -a.y,
    }
  );
}

PuleF32v3 puleF32v3(float const identity) {
  return PuleF32v3 {
    .x = identity, .y = identity, .z = identity,
  };
}
PuleF32v3 puleF32v3Ptr(float const * const value) {
  return PuleF32v3 { .x = *value, .y = *(value+1), .z = *(value+2) };
}
PuleF32v4 puleF32v4(float const identity) {
  return PuleF32v4 {
    .x = identity, .y = identity, .z = identity, .w = identity,
  };
}
PuleF32v4 puleF32v3to4(PuleF32v3 const a, float const w) {
  return PuleF32v4 { .x = a.x, .y = a.y, .z = a.z, .w = w };
}

PuleF32v4 puleF32v4Add(PuleF32v4 const a, PuleF32v4 const b) {
  return PuleF32v4 { .x = a.x+b.x, .y = a.y+b.y, .z = a.z+b.z, .w = a.w+b.w };
}
PuleF32v4 puleF32v4Sub(PuleF32v4 const a, PuleF32v4 const b) {
  return PuleF32v4 { .x = a.x-b.x, .y = a.y-b.y, .z = a.z-b.z, .w = a.w-b.w };
}
PuleF32v4 puleF32v4Neg(PuleF32v4 const a) {
  return PuleF32v4 { .x = -a.x, .y = -a.y, .z = -a.z, .w = -a.w };
}
PuleF32v4 puleF32v4Mul(PuleF32v4 const a, PuleF32v4 const b) {
  return PuleF32v4 { .x = a.x*b.x, .y = a.y*b.y, .z = a.z*b.z, .w = a.w*b.w };
}
PuleF32v4 puleF32v4Div(PuleF32v4 const a, PuleF32v4 const b) {
  return PuleF32v4 { .x = a.x/b.x, .y = a.y/b.y, .z = a.z/b.z, .w = a.w/b.w };
}

float puleF32v3Dot(PuleF32v3 const a, PuleF32v3 const b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

PuleF32v3 puleF32v3Cross(PuleF32v3 const a, PuleF32v3 const b) {
  return PuleF32v3 {
    a.y*b.z-b.y*a.z,
    a.z*b.x-b.z*a.x,
    a.x*b.y-b.x*a.y,
  };
}

float puleF32v3Length(PuleF32v3 const a) {
  return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
}

PuleF32v3 puleF32v3Add(PuleF32v3 const a, PuleF32v3 const b) {
  return PuleF32v3 { .x = a.x+b.x, .y = a.y+b.y, .z = a.z+b.z, };
}
PuleF32v3 puleF32v3Sub(PuleF32v3 const a, PuleF32v3 const b) {
  return PuleF32v3 { .x = a.x-b.x, .y = a.y-b.y, .z = a.z-b.z, };
}
PuleF32v3 puleF32v3Neg(PuleF32v3 const a) {
  return PuleF32v3 { .x = -a.x, .y = -a.y, .z = -a.z, };
}
PuleF32v3 puleF32v3Mul(PuleF32v3 const a, PuleF32v3 const b) {
  return PuleF32v3 { .x = a.x*b.x, .y = a.y*b.y, .z = a.z*b.z, };
}
PuleF32v3 puleF32v3MulScalar(PuleF32v3 const a, float const b) {
  return PuleF32v3 { .x = a.x*b, .y = a.y*b, .z = a.z*b, };
}
PuleF32v3 puleF32v3Div(PuleF32v3 const a, PuleF32v3 const b) {
  return PuleF32v3 { .x = a.x/b.x, .y = a.y/b.y, .z = a.z/b.z, };
}

PuleF32v3 puleF32v3Normalize(PuleF32v3 a) {
  float const length = puleF32v3Length(a);
  if (length == 0.0f) { return PuleF32v3 { 1.0f, 0.0f, 0.0f, }; }
  return PuleF32v3 { .x = a.x/length, .y = a.y/length, .z = a.z/length, };
}

PuleF32m44 puleF32m44(float const idn) {
  return PuleF32m44 {
    idn,  0.0f, 0.0f, 0.0f,
    0.0f, idn,  0.0f, 0.0f,
    0.0f, 0.0f, idn,  0.0f,
    0.0f, 0.0f, 0.0f, idn,
  };
}
PuleF32m44 puleF32m44Ptr(float const * const data) {
  return PuleF32m44 {
    data[0], data[4], data[8],  data[12],
    data[1], data[5], data[9],  data[13],
    data[2], data[6], data[10], data[14],
    data[3], data[7], data[11], data[15],
  };
}
PuleF32m44 puleF32m44PtrTranspose(float const * const data) {
  return PuleF32m44 {
    data[0],  data[1],  data[2],  data[3],
    data[4],  data[5],  data[6],  data[7],
    data[8],  data[9],  data[10], data[11],
    data[12], data[13], data[14], data[15],
  };
}
PULE_exportFn PuleF32m44 puleF32m33AsM44(PuleF32m33 const a) {
  return PuleF32m44 {
    a.elem[0], a.elem[3], a.elem[6], 0.0f,
    a.elem[1], a.elem[4], a.elem[7], 0.0f,
    a.elem[2], a.elem[5], a.elem[8], 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
  };
}

PuleF32m44 puleViewLookAt(
  PuleF32v3 const origin,
  PuleF32v3 const center,
  PuleF32v3 const up
) {
  PuleF32v3 const f = puleF32v3Normalize(puleF32v3Sub(center, origin));
  PuleF32v3 const s = puleF32v3Normalize(puleF32v3Cross(up, f));
  PuleF32v3 const u = puleF32v3Normalize(puleF32v3Cross(f, s));

  PuleF32m44 result = puleF32m44(1.0f);
  result.elem[0  + 0] = s.x;
  result.elem[4  + 0] = s.y;
  result.elem[8  + 0] = s.z;
  result.elem[0  + 1] = u.x;
  result.elem[4  + 1] = u.y;
  result.elem[8  + 1] = u.z;
  result.elem[0  + 2] = f.x;
  result.elem[4  + 2] = f.y;
  result.elem[8  + 2] = f.z;
  result.elem[12 + 0] = -puleF32v3Dot(s, origin);
  result.elem[12 + 1] = -puleF32v3Dot(u, origin);
  result.elem[12 + 2] = -puleF32v3Dot(f, origin);
  return result;
}

PuleF32m44 puleProjectionPerspective(
  float const fieldOfViewRadians,
  float const aspectRatio,
  float const near,
  float const far
) {
  float const halfTanFov = tanf(fieldOfViewRadians*0.5f);
  return PuleF32m44 {
    .elem = {
      1.0f / (aspectRatio * halfTanFov), 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f / halfTanFov, 0.0f, 0.0f,
      0.0f, 0.0f, far/(far-near), 1.0f,
      0.0f, 0.0f, -(far*near)/(far-near), 0.0f,
    },
  };
}

PuleF32v4 puleF32m44MulV4(PuleF32m44 const a, PuleF32v4 const b) {
  return PuleF32v4 {
    .x = a.elem[0]*b.x + a.elem[4]*b.y + a.elem[8]*b.z + a.elem[12]*b.w,
    .y = a.elem[1]*b.x + a.elem[5]*b.y + a.elem[9]*b.z + a.elem[13]*b.w,
    .z = a.elem[2]*b.x + a.elem[6]*b.y + a.elem[10]*b.z + a.elem[14]*b.w,
    .w = a.elem[3]*b.x + a.elem[7]*b.y + a.elem[11]*b.z + a.elem[15]*b.w,
  };
}

void puleF32m44DumpToStdout(PuleF32m44 const m) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdouble-promotion"
  puleLogRaw(
    "\n\t| %.2f , %.2f , %.2f , %.2f |"
    "\n\t| %.2f , %.2f , %.2f , %.2f |"
    "\n\t| %.2f , %.2f , %.2f , %.2f |"
    "\n\t| %.2f , %.2f , %.2f , %.2f |\n",
    m.elem[0],  m.elem[1],  m.elem[2],  m.elem[3],
    m.elem[4],  m.elem[5],  m.elem[6],  m.elem[7],
    m.elem[8],  m.elem[9],  m.elem[10], m.elem[11],
    m.elem[12], m.elem[13], m.elem[14], m.elem[15]
  );
#pragma GCC diagnostic pop
}

PuleF32m44 puleF32m44Viewport(float const width, float const height) {
  float const scaleX =  width / 2.0f;
  float const scaleY = -height / 2.0f;
  float const translateX = -width / 2.0f;
  float const translateY = height / 2.0f;
  return PuleF32m44 {
    .elem = {
      scaleX, 0.0f,   0.0f, 0.0f,
      0.0f,   scaleY, 0.0f, 0.0f,
      0.0f,   0.0f,   1.0f, 0.0f,
      translateX, translateY, 0.0f, 1.0f,
    },
  };
}

PuleF32m44 puleF32m44Mul(PuleF32m44 const a, PuleF32m44 const b) {
  PuleF32m44 result = puleF32m44(0.0f);
  for (size_t x = 0; x < 4; ++ x)
  for (size_t y = 0; y < 4; ++ y) {
    for (size_t z = 0; z < 4; ++ z) {
      result.elem[x*4 + y] += a.elem[x*4 + z] * b.elem[z*4 + y];
    }
  }
  return result;
}

PuleF32m44 puleF32m44Translate(PuleF32v3 const translate) {
  PuleF32m44 result = puleF32m44(1.0f);
  result.elem[12] = translate.x;
  result.elem[13] = translate.y;
  result.elem[14] = translate.z;
  return result;
}

PuleF32m44 puleF32m44Rotation(
  float const radians,
  PuleF32v3 const axis
) {
  return PuleF32m44 {
    .elem = {
      cosf(radians) + axis.x*axis.x*(1.0f-cosf(radians)),
      axis.x*axis.y*(1.0f-cosf(radians)) - axis.z*sinf(radians),
      axis.x*axis.z*(1.0f-cosf(radians)) + axis.y*sinf(radians),
      0.0f,
      axis.y*axis.x*(1.0f-cosf(radians)) + axis.z*sinf(radians),
      cosf(radians) + axis.y*axis.y*(1.0f-cosf(radians)),
      axis.y*axis.z*(1.0f-cosf(radians)) - axis.x*sinf(radians),
      0.0f,
      axis.z*axis.x*(1.0f-cosf(radians)) - axis.y*sinf(radians),
      axis.z*axis.y*(1.0f-cosf(radians)) + axis.x*sinf(radians),
      cosf(radians) + axis.z*axis.z*(1.0f-cosf(radians)),
      0.0f,
      0.0f, 0.0f, 0.0f, 1.0f,
    },
  };
}

// -- quat ---------------------------------------------------------------------

PuleF32q puleF32qIdentity() {
  return PuleF32q { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f, };
}

PuleF32q puleF32qPlane(
  PuleF32v3 const normal, float const theta
) {
  float const halfTheta = theta * 0.5f;
  float const sinHalfTheta = sinf(halfTheta);
  return PuleF32q {
    .x = normal.x * sinHalfTheta,
    .y = normal.y * sinHalfTheta,
    .z = normal.z * sinHalfTheta,
    .w = cosf(halfTheta),
  };
}

PuleF32q puleF32qFromEuler(PuleF32v3 const euler) {
  PuleF32q const qX = puleF32qPlane(PuleF32v3{1.0f, 0.0f, 0.0f}, euler.x);
  PuleF32q const qY = puleF32qPlane(PuleF32v3{0.0f, 1.0f, 0.0f}, euler.y);
  PuleF32q const qZ = puleF32qPlane(PuleF32v3{0.0f, 0.0f, 1.0f}, euler.z);
  return puleF32qMul(puleF32qMul(qX, qY), qZ);
}

PuleF32q puleF32qMul(PuleF32q const l, PuleF32q const r) {
  return PuleF32q {
    .x = l.w*r.w - l.x*r.x - l.y*r.y - l.z*r.z,
    .y = l.x*r.w + l.w*r.x + l.y*r.z - l.z*r.y,
    .z = l.y*r.w + l.w*r.y + l.z*r.x - l.x*r.z,
    .w = l.z*r.w + l.w*r.z + l.x*r.y - l.y*r.x,
  };
}

PuleF32q puleF32qMulF(PuleF32q const a, float const b) {
  return PuleF32q { .x = a.x*b, .y = a.y*b, .z = a.z*b, .w = a.w*b, };
}

PuleF32q puleF32qNormalize(PuleF32q const a) {
  float const magnitude = puleF32qMagnitude(a);
  if (magnitude == 0.0f) { return puleF32qIdentity(); }
  return PuleF32q {
    .x = a.x / magnitude,
    .y = a.y / magnitude,
    .z = a.z / magnitude,
    .w = a.w / magnitude,
  };
}
PuleF32q puleF32qInvert(PuleF32q const a) {
  PuleF32q const inv = puleF32qMulF(a, 1.0f / puleF32qMagnitude(a));
  return PuleF32q { .x = -inv.x, .y = -inv.y, .z = -inv.z, .w = inv.w };
}
float puleF32qMagnitude(PuleF32q const a) {
  return sqrtf(puleF32qMagnitudeSqr(a));
}
float puleF32qMagnitudeSqr(PuleF32q const a) {
  return a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w;
}
PuleF32v3 puleF32qRotateV3(
  PuleF32q const a, PuleF32v3 const b
) {
  PuleF32v3 bNor = puleF32v3Normalize(b);
  PuleF32q q = PuleF32q{bNor.x, bNor.y, bNor.z, 0.0f};
  q = puleF32qMul(a, q);
  q = puleF32qMul(puleF32qInvert(a), q);
  return PuleF32v3{q.x, q.y, q.z};
}
PuleF32m33 puleF32qRotateM33(
  PuleF32q const a, PuleF32m33 const b
) {
  PuleF32v3 const rows[3] = {
    puleF32qRotateV3(a, PuleF32v3{b.elem[0], b.elem[1], b.elem[2]}),
    puleF32qRotateV3(a, PuleF32v3{b.elem[3], b.elem[4], b.elem[5]}),
    puleF32qRotateV3(a, PuleF32v3{b.elem[6], b.elem[7], b.elem[8]}),
  };
  return (
    PuleF32m33 {
      .elem = {
        rows[0].x, rows[0].y, rows[0].z,
        rows[1].x, rows[1].y, rows[1].z,
        rows[2].x, rows[2].y, rows[2].z,
      },
    }
  );
}
PuleF32v3 puleF32qAxis(PuleF32q const a) {
  return PuleF32v3 { .x = a.x, .y = a.y, .z = a.z };
}
bool puleF32qValid(PuleF32q const a) {
  return (
       (a.x*0.0f == a.x*0.0f)
    && (a.y*0.0f == a.y*0.0f)
    && (a.z*0.0f == a.z*0.0f)
    && (a.w*0.0f == a.w*0.0f)
  );
}

PuleF32m33 puleF32qAsM33(PuleF32q const id) {
  PuleF32v3 const rows[3] = {
    puleF32qRotateV3(id, PuleF32v3{1.0f, 0.0f, 0.0f}),
    puleF32qRotateV3(id, PuleF32v3{0.0f, 1.0f, 0.0f}),
    puleF32qRotateV3(id, PuleF32v3{0.0f, 0.0f, 1.0f}),
  };
  return (
    PuleF32m33 {
      .elem = {
        rows[0].x, rows[0].y, rows[0].z,
        rows[1].x, rows[1].y, rows[1].z,
        rows[2].x, rows[2].y, rows[2].z,
      },
    }
  );
}
PuleF32v4 puleF32qAsV4(PuleF32q const id) {
  return PuleF32v4 { .x = id.x, .y = id.y, .z = id.z, .w = id.w };
}

} // extern C


// -- c++ bindings -------------------------------------------------------------

#include <pulchritude-math/math.hpp>

pule::F32v3::F32v3() { x = 0.0f; y = 0.0f; z = 0.0f; }

pule::F32v3::F32v3(PuleF32v3 const & a) { x = a.x; y = a.y; z = a.z; }
pule::F32v3::F32v3(float x, float y, float z) {
  this->x = x; this->y = y; this->z = z;
}

PuleF32v3 pule::F32v3::toFloat() const { return {x,y,z}; }

pule::F32v3 pule::F32v3::operator+(F32v3 const & rhs) const {
  return F32v3 { x+rhs.x, y+rhs.y, z+rhs.z };
}
pule::F32v3 pule::F32v3::operator-(F32v3 const & rhs) const {
  return F32v3 { x-rhs.x, y-rhs.y, z-rhs.z };
}
pule::F32v3 pule::F32v3::operator/(F32v3 const & rhs) const {
  return F32v3 { x/rhs.x, y/rhs.y, z/rhs.z };
}
pule::F32v3 pule::F32v3::operator*(F32v3 const & rhs) const {
  return F32v3 { x*rhs.x, y*rhs.y, z*rhs.z };
}
pule::F32v3 pule::F32v3::operator-() const {
  return F32v3 { -x, -y, -z };
}

pule::F32v3 pule::F32v3::operator-=(F32v3 const & rhs) {
  x -= rhs.x; y -= rhs.y; z -= rhs.z;
  return *this;
}

pule::F32v3 pule::F32v3::operator/(F32 const & rhs) const {
  return F32v3 { x/rhs, y/rhs, z/rhs };
}
pule::F32v3 pule::F32v3::operator*(F32 const & rhs) const {
  return F32v3 { x*rhs, y*rhs, z*rhs };
}

pule::F32v3 pule::F32v3::operator+=(F32v3 const & rhs) {
  x += rhs.x; y += rhs.y; z += rhs.z;
  return *this;
}

pule::F32v4::F32v4() { x = 0.0f; y = 0.0f; z = 0.0f; w = 0.0f; }

pule::F32v4::F32v4(PuleF32v4 const & a) { x = a.x; y = a.y; z = a.z; w = a.w; }
pule::F32v4::F32v4(float x, float y, float z, float w) {
  this->x = x; this->y = y; this->z = z; this->w = w;
}

pule::F32m33::F32m33(pule::F32 identity) {
  elem[0] = identity; elem[1] = 0.0f; elem[2] = 0.0f;
  elem[3] = 0.0f; elem[4] = identity; elem[5] = 0.0f;
  elem[6] = 0.0f; elem[7] = 0.0f; elem[8] = identity;
}
pule::F32m33::F32m33(PuleF32m33 const & a) {
  elem[0] = a.elem[0]; elem[1] = a.elem[1]; elem[2] = a.elem[2];
  elem[3] = a.elem[3]; elem[4] = a.elem[4]; elem[5] = a.elem[5];
  elem[6] = a.elem[6]; elem[7] = a.elem[7]; elem[8] = a.elem[8];
}
pule::F32m33::F32m33(std::initializer_list<pule::F32> const & list) {
  size_t i = 0;
  for (auto const & elem : list) {
    this->elem[i] = elem;
    ++ i;
    if (i == 9) { break; }
  }
  for (; i < 9; ++ i) { this->elem[i] = 0.0f; }
}
pule::F32v3 pule::F32m33::operator*(pule::F32v3 const & rhs) const {
  return pule::F32v3 {
    elem[0]*rhs.x + elem[3]*rhs.y + elem[6]*rhs.z,
    elem[1]*rhs.x + elem[4]*rhs.y + elem[7]*rhs.z,
    elem[2]*rhs.x + elem[5]*rhs.y + elem[8]*rhs.z,
  };
}
pule::F32m33 pule::F32m33::operator*(pule::F32 const & rhs) const {
  return pule::F32m33 {
    elem[0]*rhs, elem[1]*rhs, elem[2]*rhs,
    elem[3]*rhs, elem[4]*rhs, elem[5]*rhs,
    elem[6]*rhs, elem[7]*rhs, elem[8]*rhs,
  };
}
pule::F32m33 pule::F32m33::operator*(F32m33 const & rhs) const {
  pule::F32m33 ret;
  for (size_t i = 0; i < 3; ++ i) \
  for (size_t j = 0; j < 3; ++ j) {
    auto x = pule::F32v3 {
      this->elem[i*3], this->elem[i*3 + 1], this->elem[i*3 + 2]
    };
    auto y = pule::F32v3 {
      rhs.elem[j], rhs.elem[j + 3], rhs.elem[j + 6]
    };
    ret.elem[i*3 + j] = dot(x, y);
  }
  return ret;
}
pule::F32m33 pule::F32m33::operator+(F32m33 const & rhs) const {
  return pule::F32m33 {
    elem[0]+rhs.elem[0], elem[1]+rhs.elem[1], elem[2]+rhs.elem[2],
    elem[3]+rhs.elem[3], elem[4]+rhs.elem[4], elem[5]+rhs.elem[5],
    elem[6]+rhs.elem[6], elem[7]+rhs.elem[7], elem[8]+rhs.elem[8],
  };
}

pule::F32m33 const & pule::F32m33::operator*=(F32m33 const & rhs) {
  *this = *this * rhs;
  return *this;
}
pule::F32m33 const & pule::F32m33::operator*=(pule::F32 const & rhs) {
  *this = *this * rhs;
  return *this;
}

pule::F32q::F32q() { v4.x = 0.0f; v4.y = 0.0f; v4.z = 0.0f; v4.w = 1.0f; }
pule::F32q::F32q(float x, float y, float z, float w) {
  v4.x = x; v4.y = y; v4.z = z; v4.w = w;
}
pule::F32q::F32q(pule::F32v3 const & axis, float angle) {
  auto cabi = puleF32qPlane(axis, angle);
  this->v4 = { cabi.x, cabi.y, cabi.z, cabi.w };
}
pule::F32q::F32q(PuleF32q const & a) {
  v4.x = a.x; v4.y = a.y; v4.z = a.z; v4.w = a.w;
}

pule::F32q pule::F32q::operator*(F32q const & rhs) const {
  return F32q {
    v4.w*rhs.v4.x + v4.x*rhs.v4.w + v4.y*rhs.v4.z - v4.z*rhs.v4.y,
    v4.w*rhs.v4.y + v4.y*rhs.v4.w + v4.z*rhs.v4.x - v4.x*rhs.v4.z,
    v4.w*rhs.v4.z + v4.z*rhs.v4.w + v4.x*rhs.v4.y - v4.y*rhs.v4.x,
    v4.w*rhs.v4.w - v4.x*rhs.v4.x - v4.y*rhs.v4.y - v4.z*rhs.v4.z,
  };
}
pule::F32q pule::F32q::operator*(float const & rhs) const {
  return F32q { v4.x*rhs, v4.y*rhs, v4.z*rhs, v4.w*rhs };
}

PuleF32m33 pule::F32q::asM33() const {
  return puleF32qAsM33(*this);
}
pule::F32v3 pule::F32q::axis() const {
  return puleF32qAxis(*this);
}

pule::F32q pule::F32q::plane(F32v3 const normal, float const theta) {
  return puleF32qPlane(normal, theta);
}

pule::F32v3 pule::F32q::rotate(F32v3 const & a) const {
  return puleF32qRotateV3(*this, a);
}
pule::F32m33 pule::F32q::rotate(F32m33 const & a) const {
  return puleF32qRotateM33(*this, a);
}

// free-form operators
pule::F32 dot(pule::F32v3 const & a, pule::F32v3 const & b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}
pule::F32 lengthSqr(pule::F32v3 const & a) {
  return a.x*a.x + a.y*a.y + a.z*a.z;
}
pule::F32 length(pule::F32v3 const & a) {
  return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
}
pule::F32v3 cross(pule::F32v3 const & a, pule::F32v3 const & b) {
  return pule::F32v3 {
    a.y*b.z-b.y*a.z,
    a.z*b.x-b.z*a.x,
    a.x*b.y-b.x*a.y,
  };
}
pule::F32v3 normalize(pule::F32v3 const & a) {
  float const length = (a.x*a.x + a.y*a.y + a.z*a.z);
  if (length == 0.0f) { return pule::F32v3 { 1.0f, 0.0f, 0.0f, }; }
  return pule::F32v3 { a.x/length, a.y/length, a.z/length, };
}

pule::F32q normalize(pule::F32q const a) {
  return puleF32qNormalize(a);
}
pule::F32q inverse(pule::F32q const a) {
  return puleF32qInvert(a);
}
pule::F32 magnitude(pule::F32q const a) {
  return puleF32qMagnitude(a);
}
pule::F32 magnitudeSqr(pule::F32q const a) {
  return puleF32qMagnitudeSqr(a);
}

pule::F32 magnitude(pule::F32v3 const a) {
  return puleF32v3Length(a);
}
pule::F32v3 rotate(
  pule::F32q const a, pule::F32v3 const b
) {
  return puleF32qRotateV3(a, b);
}
PULE_exportFn PuleF32m33 rotate(
  pule::F32q const a, PuleF32m33 const b
) {
  return puleF32qRotateM33(a, b);
}

pule::F32 trace(pule::F32m33 const & m) {
  auto const xx = m.elem[0] * m.elem[0];
  auto const yy = m.elem[4] * m.elem[4];
  auto const zz = m.elem[8] * m.elem[8];
  return xx + yy + zz;
}
pule::F32 determinant(pule::F32m33 const & m) {
  auto const & elem = m.elem;
  return (
      elem[0] * (elem[4] * elem[8] - elem[5] * elem[7])
    - elem[1] * (elem[3] * elem[8] - elem[5] * elem[6])
    + elem[2] * (elem[3] * elem[7] - elem[4] * elem[6])
  );
}
pule::F32m33 transpose(pule::F32m33 const & m) {
  pule::F32m33 ret;
  for (size_t i = 0; i < 3; ++ i) \
  for (size_t j = 0; j < 3; ++ j) {
    ret.elem[i*3 + j] = m.elem[j*3 + i];
  }
  return ret;
}
pule::F32m33 inverse(pule::F32m33 const & m) {
  pule::F32m33 inv;
  for (size_t i = 0; i < 3; ++ i) \
  for (size_t j = 0; j < 3; ++ j) {
    inv.elem[i*3 + j] = cofactor(m, i, j);
  }
  auto det = determinant(m);
  // if determinant is too low, then clamp
  if (det < 0.01f) det = 0.01f;
  inv *= (1.0f / det);
  return inv;
}
pule::F32 cofactor(pule::F32m33 const & m, size_t i, size_t j) {
  // first compute the minor
  pule::F32 minor[4];
  size_t yy = 0;
  for (size_t y = 0; y < 3; ++ y) {
    if (y == i) continue;
    size_t xx = 0;
    for (size_t x = 0; x < 3; ++ x) {
      if (x == j) continue;
      minor[yy*2 + xx] = m.elem[y*3 + x];
      ++ xx;
    }
    ++ yy;
  }

  // then compute determinant
  pule::F32 det = minor[0]*minor[3] - minor[1]*minor[2];
  return ((i+j) % 2 == 0) ? det : -det;
}
