#pragma once

#include <pulchritude/physx.h>

//#include <pulchritude-math/math-fixed-point.h>
#include <pulchritude/math.hpp>

namespace pule {
  using Fv3 = pule::F32v3;
  using Float = pule::F32;
  using Fquat = pule::F32q;
}
//
//namespace pint {
//
//struct ContactInfo {
//  pule::Fv3 ptAWorld;
//  pule::Fv3 ptBWorld;
//  pule::Fv3 ptALocal;
//  pule::Fv3 ptBLocal;
//
//  pule::Fv3 normal;
//
//  pule::Float separationDistance;
//  float timeOfImpact;
//};
//
//struct Body {
//  pule::Fv3 origin;
//  pule::Fv3 velocity;
//  pule::Fv3 velocityAngle;
//  pule::Float mass;
//  pule::Float invMass;
//  pule::Fv3 centerOfMass;
//  pule::Float friction;
//  pule::Float elasticity;
//  pule::Fquat orientation;
//  PulePhysx3DShape shape;
//  PulePhysxBodyType type;
//  PulePhysx3DBody id;
//};
//
//bool intersectSphereSphere(
//  Body const & body0, Body const & body1, ContactInfo & contactInfo
//);

//} // namespace pint
