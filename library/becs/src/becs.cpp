#include <pulchritude-becs/becs.h>

#include <string>
#include <vector>

namespace pint {

struct Component {
  std::string label;
  size_t byteLength;
};

struct World {
  std::vector<PuleBecsSystem> systemsInOrder;
  pule::ResourceContainer<Component, PuleBecsComponent> components;
};

pule::ResourceContainer<World, PuleBecsWorld> worlds;

struct QueryEntity {
  std::vector<PuleBecsComponent> components;
  std::vector<PuleBecsEntity> cachedEntities;
};

struct Entity {
  PuleBecsSystem system;
  std::string label;
  std::vector<PuleBecsComponent> components;
};

struct System {
  PuleBecsWorld world;
  std::string label;
  pule::ResourceContainer<Entity, PuleBecsEntity> entities;
  std::vector<std::vector<uint8_t>> componentData;
  std::unordered_map<PuleBecsComponent, size_t> componentDataIndex;
  pule::ResourceContainer<
    pint::QueryEntity,
    PuleBecsQueryEntityIterator
  > queryEntityIterators;
  PuleBecsSystemCallbackFrequency callbackFrequency;
  size_t callbackFrequencyFixed;
  uint8_t threadGroup;
  size_t bundleHistoryCount;

  float msDeltaCounter;

  int32_t (*callback)(
    PuleBecsSystem system,
    PuleBecsBundle writeBundle,
    PuleBecsBundleHistory readBundleHistory,
    PuleBecsBundle * inputSystemReadBundles,
    float msDelta
  );
};

pule::ResourceContainer<System, PuleBecsSystem> systems;

}

// -- world --------------------------------------------------------------------

extern "C" {

PuleBecsWorld puleBecsWorldCreate() {
  return pint::worlds.create({});
}
void puleBecsWorldDestroy(PuleBecsWorld world) {
  if (world.id == 0) { return; }
  pint::worlds.destroy(world);
}
void puleBecsWorldAdvance(PuleBecsWorld puWorld, float const msDelta) {
  auto & world = *pint::worlds.at(puWorld);
  for (auto & puSystem : world.systemsInOrder) {
    puleBecsSystemAdvance(puSystem, msDelta);
  }
}

} // extern C

// -- components ---------------------------------------------------------------

extern "C" {

PuleBecsComponent puleBecsComponentCreate(
  PuleBecsWorld puWorld,
  PuleBecsComponentCreateInfo const * createInfo
) {
  return (
    pint::worlds.at(puWorld)->components.create({
      .label = std::string(createInfo->label.contents),
      .byteLength = createInfo->byteLength,
    })
  );
}

} // extern C

// -- systems ------------------------------------------------------------------
PuleBecsSystem puleBecsSystemCreate(
  PuleBecsWorld world, PuleBecsSystemCreateInfo ci
) {
  pint::System system = {
    .world = world,
    .label = std::string(ci.label.contents, ci.label.len),
    .entities = {},
    .componentDataIndex = {},
    .queryEntityIterators = {},
    .callbackFrequency = ci.callbackFrequency,
    .callbackFrequencyFixed = ci.callbackFrequencyFixed,
    .threadGroup = 0, // set in relation
    .bundleHistoryCount = ci.bundleHistoryCount,
    .callback = ci.callback,
  };
  return pint::systems.create(system);
}
void puleBecsSystemDestroy(PuleBecsSystem system) {
  if (system.id == 0) { return; }
  pint::systems.destroy(system);
}
void puleBecsSystemAdvance(PuleBecsSystem puSystem, float const msDelta) {
  // TODO thread and thread group
  auto & system = *pint::systems.at(puSystem);
  if (system.callbackFrequency == PuleBecsSystemCallbackFrequency_fixed) {
    system.msDeltaCounter += msDelta;
    while (system.msDeltaCounter >= system.callbackFrequencyFixed) {
      system.msDeltaCounter -= system.callbackFrequencyFixed;
      system.callback(puSystem, {}, {}, {}, system.callbackFrequencyFixed);
    }
  } else {
    system.callback(puSystem, {}, {}, {}, msDelta);
  }
}

// -- entities -----------------------------------------------------------------

extern "C" {

PuleBecsQueryEntityIterator puleBecsQueryEntities(PuleBecsEntityQuery query) {
  auto & system = *pint::systems.at(query.system);
  pint::QueryEntity queryEntity;
  queryEntity.components.assign(
    query.components, query.components + query.componentCount
  );
  queryEntity.cachedEntities.reserve(256);
  auto iterator = (
    system.queryEntityIterators.create(queryEntity)
  );
  puleBecsQueryEntitiesUpdate(query.system, iterator);
  return iterator;
}

void puleBecsQueryEntitiesUpdate(
  PuleBecsSystem puSystem, PuleBecsQueryEntityIterator iter
) {
  auto & system = *pint::systems.at(puSystem);
  auto & queryEntity = *system.queryEntityIterators.at(iter);
  queryEntity.cachedEntities.clear();
  for (auto & entity : system.entities) { // all entites
    bool match = true;
    for (auto & component : queryEntity.components) { // query comps
      bool found = false;
      for (auto & entityComponent : entity.second.components) { // entity comps
        if (entityComponent.id == component.id) {
          found = true;
          break;
        }
      }
      if (!found) {
        match = false;
        break;
      }
    }
    if (match) {
      queryEntity.cachedEntities.emplace_back(entity.first);
    }
  }
}

size_t puleBecsQueryEntityIteratorCount(
  PuleBecsSystem puSystem, PuleBecsQueryEntityIterator iter
) {
  auto & system = *pint::systems.at(puSystem);
  return system.queryEntityIterators.at(iter)->cachedEntities.size();
}

PuleBecsEntity puleBecsQueryEntityIteratorAt(
  PuleBecsSystem puSystem, PuleBecsQueryEntityIterator iter, size_t idx
) {
  auto & system = *pint::systems.at(puSystem);
  return system.queryEntityIterators.at(iter)->cachedEntities[idx];
}

} // extern C
