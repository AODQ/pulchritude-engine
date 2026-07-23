#include <pulchritude/pecs.h>

#include <pulchritude/core.hpp>
#include <pulchritude/string.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#include "autogen-serializer.inl"

namespace pint {

struct Component {
  std::string label;
  size_t byteLength;
  size_t byteAlignment;
};

struct QueryEntity {
  std::vector<PulePecsComponent> components;
};
struct QueryEntityIterator {
  std::vector<PulePecsEntity> cachedEntities;
};

struct EntityTemplate {
  std::string label;
  struct ComponentInfo {
    uint64_t offset;
  };
  std::unordered_map<uint64_t, ComponentInfo> componentInfo;
  std::vector<PulePecsComponent> components;
  size_t componentTotalByteLength;
};

struct Entity {
  std::string label;
  PulePecsScene scene;
  PulePecsEntityTemplate entityTemplate;
  std::vector<uint8_t> componentData;
};

struct System {
  PulePecsScene scene;
  std::string label;
  PulePecsSystemCallbackFrequency callbackFrequency;
  int64_t callbackFrequencyFixed;

  PuleMicrosecond usDeltaCounter;

  void * userdata;

  int32_t (*callback)(
    PulePecsScene scene,
    PulePecsSystem system,
    PuleMicrosecond usDelta,
    void * userdata
  );
};

pule::ResourceContainer<System, PulePecsSystem> systems;

struct Scene {
  pule::ResourceContainer<Entity, PulePecsEntity> entities;
  pule::ResourceContainer<System, PulePecsSystem> systems;
  pule::ResourceContainer<
    EntityTemplate, PulePecsEntityTemplate
  > entityTemplates;
  pule::ResourceContainer<Component, PulePecsComponent> components;
  pule::ResourceContainer<
    pint::QueryEntity,
    PulePecsQueryEntity
  > queryEntities;
  pule::ResourceContainer<
    pint::QueryEntityIterator,
    PulePecsQueryEntityIterator
  > queryEntityIterators;
};

pule::ResourceContainer<Scene, PulePecsScene> scenes;

} // namespace pint

// -- scene --------------------------------------------------------------------

extern "C" {

PulePecsScene pulePecsSceneCreate() {
  return pint::scenes.create({});
}
void pulePecsSceneDestroy(PulePecsScene scene) {
  if (scene.id == 0) { return; }
  pint::scenes.destroy(scene);
}

void pulePecsSceneAdvance(
  PulePecsScene const puScene,
  PuleMicrosecond const usDelta
) {
}

} // extern C

// -- entity template ----------------------------------------------------------

extern "C" {

PulePecsEntityTemplate pulePecsEntityTemplateCreate(
  PulePecsEntityTemplateCreateInfo const createInfo
) {
  auto & scene = *pint::scenes.at(createInfo.scene);
  auto entityTemplate = pint::EntityTemplate {
    .label = std::string(createInfo.label.contents, createInfo.label.len),
    .componentInfo = {},
    .components = {},
    .componentTotalByteLength = 0,
  };
  size_t offset = 0;
  for (size_t i = 0; i < createInfo.componentCount; ++i) {
    auto puComponent = createInfo.components[i];
    auto & component = *scene.components.at(puComponent);
    // pad out to the component's alignment; 0 is fine here
    offset = (
        (offset + component.byteAlignment-1)
      & ~(component.byteAlignment-1)
    );
    // create a component info struct with the offset
    puleLogDev("creating component info at offset %zu for ID %zu", offset, puComponent.id);
    entityTemplate.componentInfo.insert({puComponent.id, {offset}});
    entityTemplate.components.emplace_back(puComponent);
    // then add the component's byte length for next component
    offset += component.byteLength;
  }
  entityTemplate.componentTotalByteLength = offset;
  return scene.entityTemplates.create(entityTemplate);
}

} // extern "C"

// -- entity -------------------------------------------------------------------

extern "C" {

PulePecsEntity pulePecsEntityCreate(PulePecsEntityCreateInfo const createInfo) {
  auto & scene = *pint::scenes.at(createInfo.scene);
  auto entity = pint::Entity {
    .label = std::string(createInfo.label.contents, createInfo.label.len),
    .scene = createInfo.scene,
    .entityTemplate = createInfo.entityTemplate,
    .componentData = {},
  };
  auto & entityTemplate = *scene.entityTemplates.at(createInfo.entityTemplate);
  entity.componentData.resize(entityTemplate.componentTotalByteLength);
  return scene.entities.create(entity);
}

void pulePecsEntityDestroy(PulePecsScene puScene, PulePecsEntity const entity) {
  if (entity.id == 0) { return; }
  auto & scene = *pint::scenes.at(puScene);
  scene.entities.destroy(entity);
}

PuleStringView pulePecsEntityLabel(
  PulePecsScene const puScene, PulePecsEntity const puEntity
) {
  auto & scene = *pint::scenes.at(puScene);
  auto & entity = *scene.entities.at(puEntity);
  return PuleStringView {
    .contents = entity.label.data(),
    .len = entity.label.size(),
  };
}

void * pulePecsEntityComponentData(
  PulePecsScene puScene, PulePecsEntity const puEntity,
  PulePecsComponent const puComponent
) {
  auto & scene = *pint::scenes.at(puScene);
  auto & entity = *scene.entities.at(puEntity);
  auto & entityTemplate = *scene.entityTemplates.at(entity.entityTemplate);
  puleLogDev("component id %zu template %zu", puComponent.id, entity.entityTemplate.id);
  auto & componentInfo = entityTemplate.componentInfo.at(puComponent.id);
  return entity.componentData.data() + componentInfo.offset;
}

} // extern C

// -- component ----------------------------------------------------------------

extern "C" {

PulePecsComponent pulePecsComponentCreate(
  PulePecsScene const puScene, PulePecsComponentCreateInfo const createInfo
) {
  auto & scene = *pint::scenes.at(puScene);
  auto component = pint::Component {
    .label = std::string(createInfo.label.contents, createInfo.label.len),
    .byteLength = createInfo.byteLength,
    .byteAlignment = createInfo.byteAlignment,
  };
  return scene.components.create(component);
}

PulePecsComponent pulePecsComponentFetch(
  PulePecsScene const puScene, PuleStringView const label
) {
  auto & scene = *pint::scenes.at(puScene);
  for (auto const & componentPair : scene.components) {
    auto & component = componentPair.second;
    if (component.label == std::string(label.contents, label.len)) {
      return {componentPair.first};
    }
  }
  return PulePecsComponent {0};
}

} // extern "C"

// -- system -------------------------------------------------------------------

extern "C" {

PulePecsSystem pulePecsSystemCreate(
  PulePecsSystemCreateInfo const createInfo
) {
  auto & scene = *pint::scenes.at(createInfo.scene);
  auto system = pint::System {
    .scene = createInfo.scene,
    .label = std::string(createInfo.label.contents, createInfo.label.len),
    .callbackFrequency = createInfo.callbackFrequency,
    .callbackFrequencyFixed = (
      (int64_t)createInfo.callbackFrequencyFixed.us
    ),
    .usDeltaCounter = { .us = 0 },
    .userdata = createInfo.userdata,
    .callback = createInfo.callback,
  };
  return scene.systems.create(system);
}

void pulePecsSystemDestroy(
  PulePecsScene puScene, PulePecsSystem const puSystem
) {
  if (puSystem.id == 0) { return; }
  auto & scene = *pint::scenes.at(puScene);
  scene.systems.destroy(puSystem);
}

void pulePecsSystemAdvance(
  PulePecsScene const puScene,
  PulePecsSystem const puSystem,
  PuleMicrosecond const usDelta
) {
  auto & scene = *pint::scenes.at(puScene);
  auto & system = *scene.systems.at(puSystem);
  system.usDeltaCounter.us += usDelta.us;
  switch (system.callbackFrequency) {
    case PulePecsSystemCallbackFrequency_fixed: {
      if (system.usDeltaCounter.us >= system.callbackFrequencyFixed) {
        system.callback(
          puScene, puSystem, system.usDeltaCounter, system.userdata
        );
        system.usDeltaCounter.us = (
          system.usDeltaCounter.us - system.callbackFrequencyFixed
        );
      }
    } break;
    case PulePecsSystemCallbackFrequency_unlimited:
    case PulePecsSystemCallbackFrequency_vsync: {
      system.callback(puScene, puSystem, usDelta, system.userdata);
    } break;
  }
}

} // extern "C"

// -- query entity -------------------------------------------------------------
extern "C" {

PulePecsQueryEntity pulePecsQueryEntityCreate(
  PulePecsQueryEntityCreateInfo const createInfo
) {
  auto & scene = *pint::scenes.at(createInfo.scene);
  auto queryEntity = pint::QueryEntity {
    .components = {},
  };
  queryEntity.components.reserve(createInfo.componentCount);
  for (size_t i = 0; i < createInfo.componentCount; ++i) {
    queryEntity.components.emplace_back(createInfo.components[i]);
  }
  return scene.queryEntities.create(queryEntity);
}
void pulePecsQueryEntityDestroy(
  PulePecsScene puScene, PulePecsQueryEntity const puQueryEntity
) {
  if (puQueryEntity.id == 0) { return; }
  auto & scene = *pint::scenes.at(puScene);
  scene.queryEntities.destroy(puQueryEntity);
}

} // extern "C"

// -- query entity iterator ----------------------------------------------------
extern "C" {

PulePecsQueryEntityIterator pulePecsQueryEntityIteratorCreate(
  PulePecsScene const puScene,
  PulePecsQueryEntity const puQueryEntity
) {
  auto & scene = *pint::scenes.at(puScene);
  auto & queryEntity = *scene.queryEntities.at(puQueryEntity);
  auto queryEntityIterator = pint::QueryEntityIterator {
    .cachedEntities = {},
  };
  // this is a ridiculously naive implementation
  for (auto const & entityPair : scene.entities) {
    auto & entity = entityPair.second;
    auto & entityTemplate = *scene.entityTemplates.at(entity.entityTemplate);
    bool hasAllComponents = true;
    for (auto const & component : queryEntity.components) {
      bool hasComponent = false;
      for (auto const & entityComponent : entityTemplate.components) {
        if (component.id == entityComponent.id) {
          hasComponent = true;
          break;
        }
      }
      if (!hasComponent) {
        hasAllComponents = false;
        break;
      }
    }
    if (hasAllComponents) {
      queryEntityIterator.cachedEntities.emplace_back(entityPair.first);
    }
  }
  return scene.queryEntityIterators.create(queryEntityIterator);
}
void pulePecsQueryEntityIteratorDestroy(
  PulePecsScene const puScene,
  PulePecsQueryEntityIterator const puQueryEntityIterator
) {
  if (puQueryEntityIterator.id == 0) { return; }
  auto & scene = *pint::scenes.at(puScene);
  scene.queryEntityIterators.destroy(puQueryEntityIterator);
}

PulePecsEntity pulePecsQueryEntityIteratorFront(
  PulePecsScene const puScene,
  PulePecsQueryEntityIterator const puQueryEntityIterator
) {
  auto & scene = *pint::scenes.at(puScene);
  auto & queryEntityIterator = (
    *scene.queryEntityIterators.at(puQueryEntityIterator)
  );
  return queryEntityIterator.cachedEntities.back();
}

void pulePecsQueryEntityIteratorPop(
  PulePecsScene const puScene,
  PulePecsQueryEntityIterator const puQueryEntityIterator
) {
  auto & scene = *pint::scenes.at(puScene);
  auto & queryEntityIterator = (
    *scene.queryEntityIterators.at(puQueryEntityIterator)
  );
  queryEntityIterator.cachedEntities.pop_back();
}

bool pulePecsQueryEntityIteratorIsEmpty(
  PulePecsScene const puScene,
  PulePecsQueryEntityIterator const puQueryEntityIterator
) {
  auto & scene = *pint::scenes.at(puScene);
  auto & queryEntityIterator = (
    *scene.queryEntityIterators.at(puQueryEntityIterator)
  );
  return queryEntityIterator.cachedEntities.empty();
}

} // extern "C"

// -- asset sprite -------------------------------------------------------------

extern "C" {

#define PulePecsComponentDefinition(functionLabel, componentLabel) \
  PulePecsComponent pulePecsComponentEngine_ ## functionLabel ( \
    PulePecsScene const puScene \
  ) { \
    return ( \
      pulePecsComponentCreate( \
        puScene, \
        PulePecsComponentCreateInfo { \
          .label = componentLabel ## _psv, \
          .byteLength = sizeof(PulePecsComponent_ ## functionLabel), \
          .byteAlignment = alignof(PulePecsComponent_ ## functionLabel), \
          .serializeComponentCallback = pulePecsComponent_ ## functionLabel ## Serialize, \
          .deserializeComponentCallback = pulePecsComponent_ ## functionLabel ## Deserialize, \
        } \
      ) \
    ); \
  }

#define PulePecsComponentDefinitionNoSerialize(functionLabel, componentLabel) \
  PulePecsComponent pulePecsComponentEngine_ ## functionLabel ( \
    PulePecsScene const puScene \
  ) { \
    return ( \
      pulePecsComponentCreate( \
        puScene, \
        PulePecsComponentCreateInfo { \
          .label = componentLabel ## _psv, \
          .byteLength = sizeof(PulePecsComponent_ ## functionLabel), \
          .byteAlignment = alignof(PulePecsComponent_ ## functionLabel), \
        } \
      ) \
    ); \
  }

PulePecsComponentDefinition(Transform2D, "transform2D")
PulePecsComponentDefinition(Camera, "camera")
PulePecsComponentDefinitionNoSerialize(2dSprite, "2dSprite")

} // extern "C"
