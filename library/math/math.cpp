#include <pulchritude-math/math.h>

#include <cmath>

PuleFloat2 puleFloat2(float const identity) {
  return PuleFloat2 {
    .x = identity,
    .y = identity,
  };
}
PuleFloat3 puleFloat2Ptr(float * value) {
  return PuleFloat2 { .x = *value, .y = *(value+1) };
}
PuleFloat3 puleFloat3(float const identity) {
  return PuleFloat3 {
    .x = identity, .y = identity, .z = identity,
  };
}
PuleFloat3 puleFloat3Ptr(float * value) {
  return PuleFloat3 { .x = *value, .y = *(value+1), .z = *(value+2) };
}
PuleFloat4 puleFloat4(float const identity) {
  return PuleFloat4 {
    .x = identity, .y = identity, .z = identity, .w = identity,
  };
}

float puleFloat3Dot(float const a, float const b) {
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

PuleFloat3 puleFloat3Cross(float const a, float const b) {
  return PuleFloat3 {
    a.y*b.z-b.y*a.z,
    a.z*b.x-b.z*a.x,
    a.x*b.y-b.x*a.y,
  };
}

float puleFloat3Length(PuleFloat3 const a) {
  return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

PuleFloat3 puleFloat3Add(puleFloat3 const a, puleFloat3 const b) {
  return PuleFloat3 { .x = a.x+b.x, .y = a.y+b.y, .z = a.z+b.z, };
}
PuleFloat3 puleFloat3Sub(puleFloat3 const a, puleFloat3 const b) {
  return PuleFloat3 { .x = a.x-b.x, .y = a.y-b.y, .z = a.z-b.z, };
}
PuleFloat3 puleFloat3Neg(puleFloat3 const a) {
  return PuleFloat3 { .x = -a.x, .y = -a.y, .z = -a.z, };
}
PuleFloat3 puleFloat3Mul(puleFloat3 const a, puleFloat3 const b) {
  return PuleFloat3 { .x = a.x*b.x, .y = a.y*b.y, .z = a.z*b.z, };
}
PuleFloat3 puleFloat3Div(puleFloat3 const a, puleFloat3 const b) {
  return PuleFloat3 { .x = a.x/b.x, .y = a.y/b.y, .z = a.z/b.z, };
}

PuleFloat3 puleFloat3Normalize(PuleFloat3 const a) {
  float const length = puleFloat3Length(a);
  return PuleFloat3 { .x = a.x/length, .y = a.y/length, .z = a.z/length, };
}

PuleFloat4x4 puleFloat4x4(float const idn) {
  return PuleFloat4x4 {
    idn,  0.0f, 0.0f, 0.0f,
    0.0f, idn,  0.0f, 0.0f,
    0.0f, 0.0f, idn,  0.0f,
    0.0f, 0.0f, 0.0f, idn,
  };
}

PuleFloat4x4 puleFloat4x4ViewLookAt(
  PuleFloat3 const origin,
  PuleFloat3 const center,
  PuleFloat3 const up
) {
  PuleFloat3 const f = puleFloat3Normalize(puleFloat3Sub(center, origin));
  vec3 const s = puleFloat3Normalize(puleFloat3Cross(up, f));
  vec3 const u = puleFloat3Normalize(puleFloat3Cross(f, s));

  mat4 result = mat4(1.0f);
  result[0  + 0] = s.x;
  result[4  + 0] = s.y;
  result[8  + 0] = s.z;
  result[0  + 1] = u.x;
  result[4  + 1] = u.y;
  result[8  + 1] = u.z;
  result[0  + 2] = f.x;
  result[4  + 2] = f.y;
  result[8  + 2] = f.z;
  result[12 + 0] = -puleFloat3Dot(s, eye);
  result[12 + 1] = -puleFloat3Dot(u, eye);
  result[12 + 2] = -puleFloat3Dot(f, eye);
  return result;
}

PuleFloat4x4 puleFloat4x4ProjectionPerspective(
  float const fieldOfViewRadians,
  float const aspectRatio,
  float const near,
  float const far
) {
  float const halfTanFov = tan(fieldOfViewRadians*0.5f);
  return PuleFloat4x4 {
    .elements = {
      1.0 / (aspectRatio * halfTanFov), 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f / halfTanFov, 0.0f, 0.0f,
      0.0f, 0.0f, far/(far-near), 1.0f,
      0.0f, 0.0f, -(far*near)/(far-near), 0.0f,
    },
  };
}
