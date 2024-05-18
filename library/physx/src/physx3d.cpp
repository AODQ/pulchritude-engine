#include <pulchritude-physx/physx3d.h>

#include "intersection.hpp"

#include <pulchritude-log/log.h>

#if PULE_physx3dUseFixedPoint
pule::Fix64 operator ""_fp(double long a) { return pule::Fix64(a); }
#else
constexpr float operator ""_fp(double long a) { return (float)a; }
#endif


// TODO allow for multiple worlds ¯\_(ツ)_/¯
extern "C" {
PulePhysx3DWorld pulePhysx3DWorldCreate() { return { 1 }; }
void pulePhysx3DWorldDestroy(PulePhysx3DWorld const world) { }
} // extern c

namespace pint {


pule::ResourceContainer<pint::Body, PulePhysx3DBody> bodies;
} // namespace

extern "C" {

PulePhysx3DBody pulePhysx3DBodyCreate(
  PulePhysx3DBodyCreateInfo const info
) {
  auto orientation = info.orientationEuler;
  if (
       orientation.x == 0.0_fp
    && orientation.y == 0.0_fp
    && orientation.z == 0.0_fp
  ) {
    orientation = pule::Fv3 { 0, 1, 0 };
  }
  pint::Body body = {
    .origin = info.origin,
    .velocity = pule::Fv3 { 0, 0, 0 },
    .velocityAngle = pule::Fv3 { 0, 0, 0 },
    .mass = info.mass,
    .invMass = info.mass == 0.0_fp ? 0.0_fp : 1.0_fp / info.mass,
    .centerOfMass = pule::Fv3 { 0, 0, 0 },
    .friction = info.friction,
    .elasticity = info.elasticity,
    .orientation = PuleFquat(),//info.orientationEuler,
    .shape = info.shape,
    .type = info.type,
  };
  body.id = pint::bodies.create(body);
  pint::bodies.at(body.id)->id = body.id;
  return { body.id };
}
void pulePhysx3DBodyDestroy(PulePhysx3DBody const body) {
  if (body.id == 0) { return; }
  pint::bodies.destroy(body);
}

PulePhysx3DShape pulePhysx3DShape(PulePhysx3DBody const body) {
  return pint::bodies.at(body)->shape;
}

PuleFv3 pulePhysx3DBodyOrigin(PulePhysx3DBody const body) {
  return pint::bodies.at(body)->origin;
}
PuleFm33 pulePhysx3DBodyOrientation(PulePhysx3DBody const body) {
  return pint::bodies.at(body)->orientation.asM33();
}
PuleFv3 pulePhysx3DBodyVelocity(PulePhysx3DBody const body) {
  return pint::bodies.at(body)->velocity;
}

} // extern c

// -- physics integrator simulation --------------------------------------------

namespace pint {

pule::Fm33 inertiaTensor(pint::Body & b) {
  auto const r = b.shape.sphere.radius;
  auto const m = b.mass;
  auto const i = (2.0_fp / 5.0_fp) * m * r * r;
  return pule::Fm33 {
    i,       0.0_fp, 0.0_fp,
    0.0_fp, i,       0.0_fp,
    0.0_fp, 0.0_fp, i,
  };
}

pule::Fm33 inverseInertiaTensorBodySpace(pint::Body & b) {
  auto inertiaTensor = pint::inertiaTensor(b);
  auto invInertiaTensor = inverse(inertiaTensor) * b.invMass;
  return invInertiaTensor;
}

pule::Fm33 inverseInertiaTensorWorldSpace(pint::Body & b) {
  auto inertiaTensor = pint::inertiaTensor(b);
  auto invInertiaTensor = inverse(inertiaTensor) * b.invMass;
  auto orient = pule::Fm33(b.orientation.asM33());
  invInertiaTensor = orient * invInertiaTensor * transpose(orient);
  return invInertiaTensor;
}

void applyImpulseAngle(
  pint::Body & body, pule::Fv3 const impulse
) {
  if (body.invMass == 0.0_fp) { return; }

  body.velocityAngle += (
    inverseInertiaTensorWorldSpace(body) * impulse
  );

  auto const maxAngularVelocity = 30.0_fp;
  if (lengthSqr(body.velocityAngle) > maxAngularVelocity*maxAngularVelocity) {
    body.velocityAngle = normalize(body.velocityAngle) * maxAngularVelocity;
  }
}

void applyImpulseLinear(
  pint::Body & body, pule::Fv3 const impulse
) {
  if (body.invMass == 0.0_fp) { return; }
  body.velocity += impulse;
}

void applyImpulse(
  pint::Body & body, pule::Fv3 impulsePoint, pule::Fv3 impulse
) {
  // apply impulse to center of mass
  auto position = pulePhysx3DBodyCenterOfMassWorldSpace(body.id);
  applyImpulseLinear(body, impulse);
  auto r = pule::Fv3(impulsePoint) - position;
  auto dL = cross(r, impulse);
  applyImpulseAngle(body, dL);
}

auto centerOfMassWorld(pint::Body & b) {
  return b.origin + b.orientation.rotate(b.centerOfMass);
}

auto centerOfMassModel(pint::Body & b) {
  return b.centerOfMass;
}

void resolveContact(
  pint::Body & bodyA, pint::Body & bodyB, pint::ContactInfo & contact
) {
  auto ptOnA = contact.ptAWorld;
  auto ptOnB = contact.ptBWorld;

  auto const elasticity = bodyA.elasticity * bodyB.elasticity;

  // getInverseInertiaTensorWorldSpace
  auto invWorldInertiaA = inverseInertiaTensorWorldSpace(bodyA);
  auto invWorldInertiaB = inverseInertiaTensorWorldSpace(bodyB);

  auto const n = contact.normal;

  auto const ra = ptOnA - centerOfMassWorld(bodyA);
  auto const rb = ptOnB - centerOfMassWorld(bodyB);

  auto const angularJA = cross(invWorldInertiaA * cross(ra, n), ra);
  auto const angularJB = cross(invWorldInertiaB * cross(rb, n), rb);
  auto const angularFactor = dot(angularJA + angularJB, n);


  auto const velA = bodyA.velocity + cross(bodyA.velocityAngle, ra);
  auto const velB = bodyB.velocity + cross(bodyB.velocityAngle, rb);

  // calculate collision impulse
  auto const vab = velA - velB;
  auto const impulseJ = (
      (1.0_fp + elasticity)*dot(vab, n)
    / (bodyA.invMass+bodyB.invMass+angularFactor)
  );
  auto const vectorImpulseJ = n * impulseJ;

  //applyImpulse(bodyA, ptOnA, -vectorImpulseJ);
  //applyImpulse(bodyB, ptOnB, vectorImpulseJ);

  // calculate impulse caused by friction

  auto const frictionA = bodyA.friction;
  auto const frictionB = bodyB.friction;
  auto const friction = frictionA * frictionB;

  auto const velNorm = n * dot(vab, n);
  auto const velTang = vab - velNorm;
  auto const relativeVelTang = normalize(velTang);

  auto const inertiaA = (
    cross(invWorldInertiaA * cross(ra, relativeVelTang), ra)
  );
  auto const inertiaB = (
    cross(invWorldInertiaB * cross(rb, relativeVelTang), rb)
  );
  auto const invInertia = dot(inertiaA + inertiaB, relativeVelTang);

  // calculate tangential impulse for friction
  auto const reducedMass = 1.0_fp / (bodyA.invMass+bodyB.invMass+invInertia);
  auto const impulseFriction = velTang * reducedMass * friction;

  // apply kinetic friction
  //applyImpulse(bodyA, ptOnA, -impulseFriction);
  //applyImpulse(bodyB, ptOnB, impulseFriction);

  // move colliding objects to outside of bounds
  auto const ta = bodyA.invMass / (bodyA.invMass + bodyB.invMass);
  auto const tb = bodyB.invMass / (bodyA.invMass + bodyB.invMass);

  puleLog("inv mas %f | %f", ta, tb);

  auto const ds = contact.ptBWorld - contact.ptAWorld;
  bodyA.origin += ds * ta;
  bodyB.origin -= ds * tb;
  puleLog("collision resolved: |%f, %f, %f| |%f, %f, %f|",
    bodyA.origin.x, bodyA.origin.y, bodyA.origin.z,
    bodyB.origin.x, bodyB.origin.y, bodyB.origin.z
  );
}

} // pint

extern "C" {

void pulePhysx3DWorldAdvance(
  PulePhysx3DWorld const, float const msDeltaFp
) {
  auto const msDelta = pule::Float { msDeltaFp };
  auto const gravity = pule::Fv3 { 0.0_fp, 1.0_fp, 0.0_fp };

  // update gravity
  for (auto & [id, body] : pint::bodies) {
    if (body.type == PulePhysxBodyType::PulePhysx3DBodyType_dynamic) {
      pule::Fv3 const gravityImpulse = (
        gravity * body.mass * msDelta
      );
      body.velocity += gravityImpulse;
    }
  }

  // check for collisions
  for (auto & [idA, bodyA] : pint::bodies) {
    for (auto & [idB, bodyB] : pint::bodies) {
      if (idA == idB) { continue; }
      if (bodyA.invMass == 0.0_fp && bodyB.invMass == 0.0_fp) { continue; }
      pint::ContactInfo contact;
      if (!pint::intersectSphereSphere(bodyA, bodyB, contact)) {
        continue;
      }
      pint::resolveContact(bodyA, bodyB, contact);
    }
  }

  // update positions
  for (auto & [id, body] : pint::bodies) {
    if (body.mass == 0.0_fp) { continue; }
    body.origin += body.velocity * msDelta;
    auto originCM = pule::Fv3(pulePhysx3DBodyCenterOfMassWorldSpace({id}));
    auto cmToPos = body.origin - originCM;

    // total torque is equal to external + internal torque
    auto orientation = pule::Fm33(body.orientation.asM33());
    auto inertiaTensor = pint::inertiaTensor(body);
    auto oinertiaTensor = orientation * inertiaTensor;
    auto tinertiaTensor = oinertiaTensor * transpose(orientation);
    auto invInertiaTensor = inverse(tinertiaTensor);
    auto alphaCross = (
      cross(body.velocityAngle, inertiaTensor * body.velocityAngle)
    );
    auto alpha = invInertiaTensor * alphaCross;
    body.velocityAngle += pule::Fv3(alpha) * msDelta;

    // update orientation
    auto orientationDelta = (body.velocityAngle * msDelta);
    auto dq = pule::F32q::plane(orientationDelta, length(orientationDelta));
    body.orientation = normalize(body.orientation * dq);

    // get new position
    //body.origin = originCM + body.orientation.rotate(cmToPos);
  }
}


} // extern c

// -- public utility functions -------------------------------------------------
extern "C" {

PuleFv3 pulePhysx3DBodyCenterOfMassWorldSpace(PulePhysx3DBody const body) {
  auto const & b = *pint::bodies.at(body);
  return b.origin + b.orientation.rotate(b.centerOfMass);
}
PuleFv3 pulePhysx3DBodyCenterOfMassModelSpace(PulePhysx3DBody const body) {
  return pint::bodies.at(body)->centerOfMass;
}
PuleFv3 pulePhysx3DBodyWorldSpaceToBodySpace(
  PulePhysx3DBody const body, PuleFv3 const worldSpace
) {
  auto const & b = *pint::bodies.at(body);
  return inverse(b.orientation).rotate(pule::Fv3(worldSpace) - b.origin);
}
PuleFv3 pulePhysx3DBodyBodySpaceToWorldSpace(
  PulePhysx3DBody const body, PuleFv3 const bodySpace
) {
  auto const & b = *pint::bodies.at(body);
  return b.origin + b.orientation.rotate(bodySpace);
}

void pulePhysx3DBodyApplyImpulse(
  PulePhysx3DBody body, PuleFv3 impulsePoint, PuleFv3 impulse
) {
  auto & b = *pint::bodies.at(body);

  pint::applyImpulse(b, impulsePoint, impulse);
}

} // extern C
