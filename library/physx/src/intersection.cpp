#include "intersection.hpp"

bool pint::intersectSphereSphere(
  pint::Body const & bodyA, pint::Body const & bodyB,
  pint::ContactInfo & contact
) {
  pule::Fv3 const ab = bodyB.origin - bodyA.origin;
  contact.normal = normalize(ab);

  contact.ptAWorld = bodyA.origin + contact.normal * bodyA.shape.sphere.radius;
  contact.ptBWorld = bodyB.origin - contact.normal * bodyB.shape.sphere.radius;

  pule::Float const radiusAb = (
    pule::Float(bodyA.shape.sphere.radius) + bodyB.shape.sphere.radius
  );
  contact.separationDistance = length(ab) - radiusAb;
  return contact.separationDistance < 0;
}
