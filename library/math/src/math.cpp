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

PULE_exportFn PuleF32m44 puleF32m44Rotation(
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

} // extern C
