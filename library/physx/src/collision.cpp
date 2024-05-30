#include <pulchritude/physx.h>

#include <pulchritude/error.h>

namespace in { // primitive collisions

float intersectRaySphere(
  PulePhysxRay const ray,
  float const radius
) {
  PuleF32v3 const m = (
    puleF32v3Sub(
      ray.origin,
      puleF32v3(radius)
    )
  );
  float const b = puleF32v3Dot(m, ray.direction);
  float const c = puleF32v3Dot(m, m) - radius * radius;

  if (c > 0.0f && b > 0.0f) {
    return -1.0f;
  }

  float const discr = b * b - c;
  if (discr < 0.0f) {
    return -1.0f;
  }

  float const t = -b - sqrtf(discr);
  if (t < 0.0f) {
    return -1.0f;
  }

  return t;
}

PulePhysxCollisionResultShape intersectRayAABB(
  PulePhysxRay const ray,
  PuleF32v3 const aabbMin,
  PuleF32v3 const aabbMax
) {
  PULE_assert(false && "not implemented");
}

}

PulePhysxCollisionResultShape pulePhysxRaycastShape(
  PulePhysxMode const mode,
  PulePhysxRay const ray,
  PuleF32m44 const shapeTransform,
  PulePhysxCollisionShape const shape
) {
  PULE_assert(mode == PulePhysxMode_i2d && "no 3d physx support yet");

  // transform ray into shape space
  PuleF32m44 const inverseShapeTransform = puleF32m44Inverse(shapeTransform);
  PuleF32v4 const oriSs = (
    puleF32m44MulV4(
      inverseShapeTransform,
      PuleF32v4{ray.origin.x, ray.origin.y, ray.origin.z, 1.0f}
    )
  );
  PuleF32v4 const dirSs = (
    puleF32m44MulV4(
      inverseShapeTransform,
      PuleF32v4{ray.direction.x, ray.direction.y, ray.direction.z, 0.0f}
    )
  );

  auto const rayShapeSpace = (
    PulePhysxRay {
      .origin = PuleF32v3 { oriSs.x, oriSs.y, oriSs.z, },
      .direction = puleF32v3Normalize(PuleF32v3 { dirSs.x, dirSs.y, dirSs.z, }),
    }
  );

  PulePhysxCollisionResultShape result;

  switch (shape.type) {
    default: {
      PULE_assert(false && "unsupported collision shape type");
    }
    case PulePhysxCollisionShapeType_sphere: {
      result.distance = (
        in::intersectRaySphere(rayShapeSpace, shape.shape.sphere.radius)
      );
    }
  }

  if (result.distance < 0.0f) {
    result.hit = false;
    return result;
  }

  // transform result back into world space
  result.origin = (
    puleF32v3Add(
      ray.origin,
      puleF32v3Mul(ray.direction, puleF32v3(result.distance))
    )
  );
  PuleF32v4 const originWorldSpace = (
    puleF32m44MulV4(
      shapeTransform,
      PuleF32v4{result.origin.x, result.origin.y, result.origin.z, 1.0f}
    )
  );
  result.origin = (
    PuleF32v3{originWorldSpace.x, originWorldSpace.y, originWorldSpace.z}
  );
  result.distance = puleF32v3Length(puleF32v3Sub(result.origin, ray.origin));
  return result;
}
