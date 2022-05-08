#include <pulchritude-math/math.h>

#include <cmath>

PuleF32v2 puleF32v2(float const identity) {
  return PuleF32v2 {
    .x = identity,
    .y = identity,
  };
}
PuleF32v2 puleF32v2Ptr(float const * const value) {
  return PuleF32v2 { .x = *value, .y = *(value+1) };
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
PuleF32v3 puleF32v3Div(PuleF32v3 const a, PuleF32v3 const b) {
  return PuleF32v3 { .x = a.x/b.x, .y = a.y/b.y, .z = a.z/b.z, };
}

PuleF32v3 puleF32v3Normalize(PuleF32v3 const a) {
  float const length = puleF32v3Length(a);
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
PULE_exportFn PuleF32m44 puleF32m44PtrTranspose(float const * const data) {
  return PuleF32m44 {
    data[0],  data[1],  data[2],  data[3],
    data[4],  data[5],  data[6],  data[7],
    data[8],  data[9],  data[10], data[11],
    data[12], data[13], data[14], data[15],
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
  result.elements[0  + 0] = s.x;
  result.elements[4  + 0] = s.y;
  result.elements[8  + 0] = s.z;
  result.elements[0  + 1] = u.x;
  result.elements[4  + 1] = u.y;
  result.elements[8  + 1] = u.z;
  result.elements[0  + 2] = f.x;
  result.elements[4  + 2] = f.y;
  result.elements[8  + 2] = f.z;
  result.elements[12 + 0] = -puleF32v3Dot(s, origin);
  result.elements[12 + 1] = -puleF32v3Dot(u, origin);
  result.elements[12 + 2] = -puleF32v3Dot(f, origin);
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
    .elements = {
      1.0f / (aspectRatio * halfTanFov), 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f / halfTanFov, 0.0f, 0.0f,
      0.0f, 0.0f, far/(far-near), 1.0f,
      0.0f, 0.0f, -(far*near)/(far-near), 0.0f,
    },
  };
}
