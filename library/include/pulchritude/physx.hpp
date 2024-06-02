/* auto generated file physx */
#pragma once
#include "core.hpp"

#include "physx.h"
#include "math.hpp"

namespace pule {
struct Physx2DWorld {
  PulePhysx2DWorld _handle;
  inline operator PulePhysx2DWorld() const {
    return _handle;
  }
  inline void destroy() {
    return pulePhysx2DWorldDestroy(this->_handle);
  }
  inline void gravitySet(PuleF32v2 gravity) {
    return pulePhysx2DWorldGravitySet(this->_handle, gravity);
  }
  inline void step(float timeStep, int velocityIterations, int positionIterations) {
    return pulePhysx2DWorldStep(this->_handle, timeStep, velocityIterations, positionIterations);
  }
  static inline Physx2DWorld create() {
    return { ._handle = pulePhysx2DWorldCreate(),};
  }
};
}
  inline void destroy(pule::Physx2DWorld self) {
    return pulePhysx2DWorldDestroy(self._handle);
  }
  inline void gravitySet(pule::Physx2DWorld self, PuleF32v2 gravity) {
    return pulePhysx2DWorldGravitySet(self._handle, gravity);
  }
  inline void step(pule::Physx2DWorld self, float timeStep, int velocityIterations, int positionIterations) {
    return pulePhysx2DWorldStep(self._handle, timeStep, velocityIterations, positionIterations);
  }
namespace pule {
struct Physx2DBody {
  PulePhysx2DBody _handle;
  inline operator PulePhysx2DBody() const {
    return _handle;
  }
  static inline Physx2DBody create(PulePhysx2DWorld world, PulePhysx2DBodyCreateInfo createInfo) {
    return { ._handle = pulePhysx2DBodyCreate(world, createInfo),};
  }
};
}
namespace pule {
struct Physx3DWorld {
  PulePhysx3DWorld _handle;
  inline operator PulePhysx3DWorld() const {
    return _handle;
  }
  static inline Physx3DWorld create() {
    return { ._handle = pulePhysx3DWorldCreate(),};
  }
};
}
namespace pule {
using Physx3DBody = PulePhysx3DBody;
}
namespace pule {
using PhysxCollisionResultShape = PulePhysxCollisionResultShape;
}
namespace pule {
using PhysxCollisionResultMesh = PulePhysxCollisionResultMesh;
}
namespace pule {
using PhysxRay = PulePhysxRay;
}
namespace pule {
using PhysxCollisionShapeSphere = PulePhysxCollisionShapeSphere;
}
namespace pule {
using PhysxCollisionShapeBox = PulePhysxCollisionShapeBox;
}
namespace pule {
using PhysxCollisionShapeCapsule = PulePhysxCollisionShapeCapsule;
}
namespace pule {
using PhysxCollisionShapePlane = PulePhysxCollisionShapePlane;
}
namespace pule {
using PhysxCollisionShapeTriangle = PulePhysxCollisionShapeTriangle;
}
namespace pule {
using PhysxCollisionShapeUnion = PulePhysxCollisionShapeUnion;
}
namespace pule {
using PhysxCollisionShape = PulePhysxCollisionShape;
}
namespace pule {
using PhysxCollisionMesh = PulePhysxCollisionMesh;
}
namespace pule {
using Physx2DShapeConvexPolygon = PulePhysx2DShapeConvexPolygon;
}
namespace pule {
using Physx2DShapeUnion = PulePhysx2DShapeUnion;
}
namespace pule {
struct Physx2DShape {
  PulePhysx2DShape _handle;
  inline operator PulePhysx2DShape() const {
    return _handle;
  }
  static inline Physx2DShape createConvexPolygonAsBox(PuleF32v2 origin, PuleF32v2 halfExtents, float angle) {
    return { ._handle = pulePhysx2DShapeCreateConvexPolygonAsBox(origin, halfExtents, angle),};
  }
};
}
namespace pule {
using Physx2DBodyCreateInfo = PulePhysx2DBodyCreateInfo;
}
namespace pule {
using Physx2DBodyAttachShapeCreateInfo = PulePhysx2DBodyAttachShapeCreateInfo;
}
namespace pule {
inline char const * toStr(PulePhysxCollisionShapeType const e) {
  switch (e) {
    case PulePhysxCollisionShapeType_sphere: return "sphere";
    case PulePhysxCollisionShapeType_box: return "box";
    case PulePhysxCollisionShapeType_capsule: return "capsule";
    case PulePhysxCollisionShapeType_plane: return "plane";
    case PulePhysxCollisionShapeType_triangle: return "triangle";
    default: return "N/A";
  }
}
inline char const * toStr(PulePhysxMode const e) {
  switch (e) {
    case PulePhysxMode_i2d: return "i2d";
    case PulePhysxMode_i3d: return "i3d";
    default: return "N/A";
  }
}
inline char const * toStr(PulePhysx2DShapeType const e) {
  switch (e) {
    case PulePhysx2DShapeType_convexPolygon: return "convexPolygon";
    default: return "N/A";
  }
}
inline char const * toStr(PulePhysx2DBodyType const e) {
  switch (e) {
    case PulePhysx2DBodyType_static: return "static";
    case PulePhysx2DBodyType_kinematic: return "kinematic";
    case PulePhysx2DBodyType_dynamic: return "dynamic";
    default: return "N/A";
  }
}
}
