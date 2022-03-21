#include <pulchritude-ecs/ecs.h>

#include <unordered_map>

#include <flecs.h>

namespace {
  struct ComponentInfo {
    size_t byteLength;
  };

  std::unordered_map<uint64_t, ComponentInfo> componentInfos;
}

extern "C" {

PuleEcsWorld puleEcsWorldCreate() {
  return { reinterpret_cast<uint64_t>(ecs_init()) };
}

void puleEcsWorldDestroy(PuleEcsWorld const world) {
  if (world.id == 0) {
    return;
  }
  ecs_quit(reinterpret_cast<ecs_world_t *>(world.id));
}

PULE_exportFn void puleEcsWorldAdvance(
  PuleEcsWorld const world, float const msDelta
) {
  ecs_progress(reinterpret_cast<ecs_world_t *>(world.id), msDelta);
}

PuleEcsComponent puleEcsComponentCreate(
  PuleEcsWorld const world,
  PuleEcsComponentCreateInfo const info
) {
  ecs_component_desc_t desc = { 0 };
  desc.entity.name = info.label;
  desc.entity.symbol = info.label;
  desc.size = info.byteLength;
  desc.alignment = info.byteAlignment;

  ecs_entity_t componentEntity = (
    ecs_component_init(
      reinterpret_cast<ecs_world_t *>(world.id),
      &desc
    )
  );
  PULE_assert(componentEntity != 0);
  ::componentInfos.emplace(componentEntity, info.byteLength);
  return { componentEntity };
}

size_t puleEcsIteratorEntityCount(PuleEcsIterator const iterator) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.data);
  return static_cast<size_t>(iter->count);
}

size_t puleEcsIteratorRelativeOffset(PuleEcsIterator const iterator) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.data);
  return static_cast<size_t>(iter->frame_offset);
}

void * puleEcsIteratorQueryComponents(
  PuleEcsIterator const iterator,
  size_t const componentIndex,
  size_t const componentByteLength
) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.data);
  return ecs_term_w_size(iter, componentByteLength, componentIndex+1);
}

static ecs_entity_t callbackFrequencyToFlecs(
  PuleEcsSystemCallbackFrequency const frequency
) {
  switch (frequency) {
    default: PULE_assert(false);
    case PuleEcsSystemCallbackFrequency_preUpdate:  return EcsPreUpdate;
    case PuleEcsSystemCallbackFrequency_onUpdate:   return EcsOnUpdate;
    case PuleEcsSystemCallbackFrequency_postUpdate: return EcsPostUpdate;
  }
}

PuleEcsSystem puleEcsSystemCreate(
  PuleEcsWorld const world,
  PuleEcsSystemCreateInfo const info
) {
  ecs_system_desc_t desc = { 0 };
  desc.entity.name = info.label;
  desc.entity.add[0] = callbackFrequencyToFlecs(info.callbackFrequency);
  desc.query.filter.expr = info.commaSeparatedComponentLabels;
  desc.callback = reinterpret_cast<void (*)(ecs_iter_t *)>(info.callback);
  ecs_entity_t system = (
    ecs_system_init(
      reinterpret_cast<ecs_world_t *>(world.id),
      &desc
    )
  );
  PULE_assert(system != 0);
  return {system};
}

PuleEcsEntity puleEcsEntityCreate(PuleEcsWorld const world) {
  return { ecs_new_id(reinterpret_cast<ecs_world_t *>(world.id)) };
}

void puleEcsEntityAttachComponent(
  PuleEcsWorld const world,
  PuleEcsEntity const entity,
  PuleEcsComponent const component,
  void * const nullableInitialData
) {
  auto componentInfo = ::componentInfos.at(component.id);
  ecs_set_id(
    reinterpret_cast<ecs_world_t *>(world.id),
    entity.id, component.id,
    componentInfo.byteLength, nullableInitialData
  );
}

} // C

