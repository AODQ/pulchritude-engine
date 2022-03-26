#include <pulchritude-physics-2d/physics2d.h>

#include <pulchritude-gfx-debug/gfx-debug.hpp>

#include <unordered_map>


namespace {
struct DebugDraw : public b2Draw {
  float pixelsToMeterScale;
  override void DrawPolygon(
    b2Vec2 const * vertices,
    int32_t verticesCount,
    b2Color const & color
  ) {
    for (int32_t it = 0; it < verticesCount-1; ++ it) {
      auto const v0 = vertices[it];
      auto const v1 = vertices[it+1];
      puleGfxDebugRenderLine(
        puleF32v2 { v0.x*pixelsToMeterScale, v0.y*pixelsToMeterScale },
        puleF32v2 { v1.x*pixelsToMeterScale, v1.y*pixelsToMeterScale },
        color
      );
    }
  }

  override void DrawSolidPolygon(
    b2Vec2 const * vertices,
    int32_t verticesCount,
    b2Color const & color
  ) {
    DrawPolygon(vertices, verticesCount, color);
  }
}

struct WorldInfo {
  b2World * world;
  float pixelsToMeterScale;
  PuleAllocator allocator;
  DebugDraw debugDraw
};
std::unordered_map<uint64_t, b2World *> worlds
uint32_t worldIt = 1;

} // namespace

extern "C" {
PulePhysics2DWorld pulePhysics2DWorldCreate(
  PulePhysics2DWorldCreateInfo const info
) {
  b2World * world = (
    puleAllocate(
      info.allocator,
      PuleAllocateInfo {
        .zeroOut = false,
        .numBytes = sizeof(b2World),
        .alignment = alignof(b2World),
      }
    )
  );

  b2World = new(world) b2World(b2Vec2(info.gravity.x, info.gravity.y));

  DebugDraw debugDraw;
  debugDraw.pixelsToMeterScale = info.pixelsToMeterScale;

  worlds.emplace(
    worldIt,
    {b2World, info.pixelsToMeterScale, info.allocator, debugDraw}
  );
  return { worldIt ++ };
}

void pulePhysics2DWorldDestroy(PulePhysics2DWorld const world) {
  if (world.id == 0) { return; }
  WorldInfo const info = worlds.at(world.id);
  worlds.erase(world.id);

  puleDeallocate(info.allocator, info.world);
}

void pulePhysics2DWorldGravitySet(PuleF32v2 const gravity) {
  WorldInfo const info = worlds.at(world.id);
  info.world->SetGravity({gravity.x, gravity.y});
}

void pulePhysics2DDebugDraw(
  PulePhysics2DWorld const world,
  bool const enabled
) {
  WorldInfo const info = worlds.at(world.id);
  info.world->SetDebugDraw(enabled ? &info.debugDraw : nullptr);
  if (enabled) {
    info.world->SetDebugDraw(
      enabled ? (b2Draw::e_shapeBit | b2Draw::e_aabbBit) : 0
    );
  }
}

PulePhysics2DBody pulePhysics2DPolygonCreate(
  PulePhysics2DDynamicPolygonCreateInfo const info
) {
  WorldInfo const info = worlds.at(world.id);

  b2BodyDef bodyDef;
  bodyDef.position.Set(info.origin.x, info.origin.y);
  b2PolygonShape polygon;
  polygon.SetAsBox(info.dimensions.x*0.5f, info.dimensions.y*0.5f);
  b2FixtureDef fixtureDef;
  fixtureDef.shape = &polygon;
  fixtureDef.friction = info.friction;
  fixtureDef.restitution = info.restitution;
  fixtureDef.restitutionThreshold = info.restitutionThreshold;
  fixtureDef.density = info.density;
  fixtureDef.isSensor = info.isSensor;

  b2Body * body = info.world->CreateBody(&bodyDef);
  body->CreateFixture(&fixtureDef);

  return { reinterpret_cast<uint64_t>(body) };
}

void pulePhysics2DBodyDestroy(
  PulePhysics2DWorld const world,
  PulePhysics2DBody const body
) {
  if (body.id == 0) { return; }
  WorldInfo const info = worlds.at(world.id);
  info.world->DestroyBody(reinterpret_cast<b2Body *>(body.id));
}

PuleF32v2 pulePhysics2DBodyOrigin(PulePhysics2DBody const body) {
  auto const vec = reinterpret_cast<b2Body *>(body.id).GetPosition();
  return { vec.x, vec.y };
}

void pulePhysics2DBodyApplyForce(PulePhysics2DForceInfo const info) {
  reinterpret_cast<b2Body *>(
    info.body.id
  )->ApplyForce(
    { info.force.x, info.force.y },
    { info.absoluteOrigin.x, info.absoluteOrigin.y },
    true
  );
}

void pulePhysics2DAdvance(PulePhysics2DAdvanceInfo const info) {
  info.world->Step(
    info.timeDelta,
    info.velocityIterations,
    info.positionIterations
  );
}

} // C
