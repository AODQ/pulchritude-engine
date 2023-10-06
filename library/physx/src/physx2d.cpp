#include <pulchritude-physx/physx2d.h>

#include <pulchritude-log/log.h>

#include <box2d/box2d.h>

namespace in {

pule::ResourceContainer<b2World *> worlds;

b2Vec2 vec2(PuleF32v2 const v) {
  return b2Vec2(v.x, v.y);
}

PuleF32v2 vec2(b2Vec2 const v) {
  return PuleF32v2{v.x, v.y};
}

} // in

extern "C" {

PulePhysx2DWorld pulePhysx2DWorldCreate() {
  // I have to allocate b2World to heap, as it is not easily placed into
  // standard containers due to its non-copyable constructors
  b2World * world = new b2World(b2Vec2(0.0f, -10.0f));
  return { .id = in::worlds.create(std::move(world)), };
}
void pulePhysx2DWorldDestroy(PulePhysx2DWorld const puWorld) {
  in::worlds.destroy(puWorld.id);
}

void pulePhysx2DWorldGravitySet(
  PulePhysx2DWorld const world,
  PuleF32v2 const gravity
) {
  in::worlds.at(world.id)->SetGravity(in::vec2(gravity));
}

void pulePhysx2DWorldStep(
  PulePhysx2DWorld const world,
  float const timeStep,
  int const velocityIterations,
  int const positionIterations
) {
  (
    in::worlds.at(world.id)
      ->Step(timeStep, velocityIterations, positionIterations)
  );
}

PulePhysx2DBody pulePhysx2DBodyCreate(
  PulePhysx2DWorld const world,
  PulePhysx2DBodyCreateInfo const createInfo
) {
  b2BodyType bodyType;
  switch (createInfo.type) {
    case PulePhysx2DBodyType_static:
      bodyType = b2BodyType::b2_staticBody;
    break;
    case PulePhysx2DBodyType_kinematic:
      bodyType = b2BodyType::b2_kinematicBody;
    break;
    case PulePhysx2DBodyType_dynamic:
      bodyType = b2BodyType::b2_dynamicBody;
    break;
  }
  b2BodyDef bodyDefinition; // userData cons prevents struct aggregate init
  bodyDefinition.type = bodyType;
  bodyDefinition.position = in::vec2(createInfo.origin);
  bodyDefinition.angle = createInfo.angle;
  bodyDefinition.linearVelocity = in::vec2(createInfo.originVelocity);
  bodyDefinition.angularVelocity = createInfo.angleVelocity;
  bodyDefinition.linearDamping = createInfo.originDamping;
  bodyDefinition.angularDamping = createInfo.angleDamping;
  bodyDefinition.allowSleep = createInfo.allowSleep;
  bodyDefinition.awake = createInfo.startAwake;
  bodyDefinition.fixedRotation = createInfo.fixedRotation;
  bodyDefinition.bullet = createInfo.bullet;
  bodyDefinition.enabled = createInfo.startEnabled;
  bodyDefinition.userData.pointer = (
    reinterpret_cast<uintptr_t>(createInfo.userData)
  );
  bodyDefinition.gravityScale = createInfo.gravityScale;

  return {
    .id = (
      reinterpret_cast<uint64_t>(
        in::worlds.at(world.id)->CreateBody(&bodyDefinition)
      )
    ),
  };
}

void pulePhysx2DBodyDestroy(
  PulePhysx2DWorld const world,
  PulePhysx2DBody const body
) {
  in::worlds.at(world.id)->DestroyBody(reinterpret_cast<b2Body *>(body.id));
}

PuleF32v2 pulePhysx2DBodyPosition(
  [[maybe_unused]] PulePhysx2DWorld const world,
  PulePhysx2DBody const body
) {
  return in::vec2(reinterpret_cast<b2Body *>(body.id)->GetPosition());
}
float pulePhysx2DBodyAngle(
  [[maybe_unused]] PulePhysx2DWorld const world,
  PulePhysx2DBody const body
) {
  return reinterpret_cast<b2Body *>(body.id)->GetAngle();
}

void pulePhysx2DBodyAttachShape(
  [[maybe_unused]] PulePhysx2DWorld const world,
  PulePhysx2DBody const body,
  PulePhysx2DShape const shape,
  PulePhysx2DBodyAttachShapeCreateInfo const createInfo
) {
  b2FixtureDef fixtureDefinition; // userData cons prevents struct aggregate init
  b2PolygonShape allocPolygon;
  switch (shape.type) {
    case PulePhysx2DShapeType_convexPolygon:
      fixtureDefinition.shape = &allocPolygon;
      b2Vec2 vertices[8];
      for (size_t i = 0; i < shape.convexPolygon.vertexCount; ++ i) {
        vertices[i] = in::vec2(shape.convexPolygon.origins[i]);
      }
      allocPolygon.Set(vertices, shape.convexPolygon.vertexCount);
    break;
  }
  fixtureDefinition.friction = createInfo.friction;
  fixtureDefinition.density = createInfo.density;
  fixtureDefinition.restitution = createInfo.restitution;
  fixtureDefinition.restitutionThreshold = createInfo.restitutionThreshold;
  fixtureDefinition.isSensor = createInfo.isSensor;
  fixtureDefinition.userData.pointer = (
    reinterpret_cast<uintptr_t>(createInfo.userData)
  );
  fixtureDefinition.filter.categoryBits = createInfo.collisionCategoryBits;
  fixtureDefinition.filter.maskBits = createInfo.collisionMaskBits;
  fixtureDefinition.filter.groupIndex = createInfo.collisionGroup;

  reinterpret_cast<b2Body *>(body.id)->CreateFixture(&fixtureDefinition);
}

PulePhysx2DShape pulePhysx2DShapeCreateConvexPolygonAsBox(
  PuleF32v2 const origin,
  PuleF32v2 const halfExtents,
  float const angle
) {
  // this is a copy of b2PolygonShape::SetAsBox
  PulePhysx2DShape shape;
  b2PolygonShape allocPolygon;
  //allocPolygon.SetAsBox(halfExtents.x, halfExtents.y, in::vec2(origin), angle);
  allocPolygon.SetAsBox(halfExtents.x, halfExtents.y);
  puleLog("count: %zu", allocPolygon.m_count);
  shape.type = PulePhysx2DShapeType_convexPolygon;
  shape.convexPolygon.vertexCount = allocPolygon.m_count;
  shape.convexPolygon.centroid = in::vec2(allocPolygon.m_centroid);
  puleLog("count: %zu", allocPolygon.m_count);
  for (size_t it = 0; it < shape.convexPolygon.vertexCount; ++ it) {
    shape.convexPolygon.normals[it] = in::vec2(allocPolygon.m_normals[it]);
    shape.convexPolygon.origins[it] = in::vec2(allocPolygon.m_vertices[it]);
    puleLog(
      "new origin @ %d: %f, %f\nnew normal: %f, %f",
      it,
      shape.convexPolygon.origins[it].x,
      shape.convexPolygon.origins[it].y,
      shape.convexPolygon.normals[it].x,
      shape.convexPolygon.normals[it].y
    );
  }

  return shape;
}

#ifdef __cplusplus
} // extern C
#endif
