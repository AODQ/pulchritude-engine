#include <pulchritude-ecs/ecs.h>

#include <string>
#include <unordered_map>

#include <flecs.h>

// TODO all these need to be scoped by world
namespace {
  struct ComponentInfo {
    std::string label;
    size_t byteLength;
    void (*imguiOverviewCallback)();
    void (*imguiEntityCallback)(
      PuleEcsWorld const world,
      PuleEcsEntity const entity,
      PuleEcsComponent const component
    );
    void (*serializeComponentCallback)(
      PuleEcsEntity const entity,
      void const * const componentData,
      PuleDsValue const writeObjectPds,
      PuleAllocator const allocator
    );
    void (*deserializeComponentCallback)(
      PuleEcsWorld const world,
      PuleEcsEntity const entity,
      PuleEcsComponent const component,
      PuleDsValue const readObjectPds,
      PuleAllocator const allocator
    );
  };

  std::unordered_map<uint64_t, ComponentInfo> componentInfos;

  // this tag gets attached to every entity to allow for queries
  ecs_entity_t puEntityTag = 0;

  struct InternalQuery {
    ecs_query_t * query;
  };
  std::unordered_map<uint64_t, InternalQuery> internalQueries;
  size_t internalQueryCount = 1;

  struct InternalIter {
    ecs_iter_t iter;
    size_t childIt = 0;
    bool consumed = false;
  };
  std::unordered_map<uint64_t, InternalIter> internalIters;
  size_t internalIterCount = 1;
}

extern "C" {

PuleEcsWorld puleEcsWorldCreate() {
  auto world = ecs_init();
  ecs_set_threads(world, 8);
  return { reinterpret_cast<uint64_t>(world) };
}

void puleEcsWorldDestroy(PuleEcsWorld const world) {
  if (world.id == 0) {
    return;
  }
  ecs_quit(reinterpret_cast<ecs_world_t *>(world.id));
}

void puleEcsWorldAdvance(
  PuleEcsWorld const world, float const msDelta
) {
  ecs_set_threads(reinterpret_cast<ecs_world_t *>(world.id), 8);
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

  ::componentInfos.emplace(
    componentEntity,
    ComponentInfo {
      .label = std::string(info.label),
      .byteLength = info.byteLength,
      .imguiOverviewCallback = info.imguiOverviewCallbackOptional,
      .imguiEntityCallback = info.imguiEntityCallbackOptional,
      .serializeComponentCallback = info.serializeComponentCallback,
      .deserializeComponentCallback = info.deserializeComponentCallback,
    }
  );
  return { componentEntity };
}

void puleEcsEntityIterateComponents(
  PuleEcsEntityIterateComponentsInfo const info
) {
  PULE_assert(info.callback);
  PULE_assert(info.entity.id != 0);

  ecs_world_t * const world = reinterpret_cast<ecs_world_t *>(info.world.id);
  ecs_entity_t const entity = info.entity.id;

  ecs_type_t const type = ecs_get_type(world, info.entity.id);
  ecs_id_t * typeIds = ecs_vector_first(type, ecs_id_t);
  int32_t const count = ecs_vector_count(type);

  for (size_t it = 0; it < count; ++ it) {
    ecs_id_t const id = typeIds[it];
    ecs_entity_t const component = ecs_get_typeid(world, id);
    if ((id & ECS_PAIR) == ECS_PAIR || component == 0) continue;
    info.callback(PuleEcsComponent{.id=component,}, info.userdata);
  }
}

PuleEcsComponent puleEcsComponentFetchByLabel(
  PuleEcsWorld const puWorld,
  PuleStringView const label
) {
  // TODO create a hash-string map, instead of iterating
  for (auto const & componentIt : ::componentInfos) {
    ecs_world_t * const world = reinterpret_cast<ecs_world_t *>(puWorld.id);
    if (
      !puleStringViewEq(
        puleCStr(ecs_get_name(world, componentIt.first)),
        label
      )
    ) {
      continue;
    }
    return {.id = componentIt.first,};
  }
  puleLogError("failed to find component '%s'", label.contents);
  return { .id = 0, };
}

PuleStringView puleEcsComponentLabel(
  PuleEcsWorld const world,
  PuleEcsComponent const component
) {
  auto infoPtr = componentInfos.find(component.id);
  if (infoPtr == componentInfos.end()) {
    puleLogError("component %zu does not exist", component.id);
    return puleCStr("");
  }
  return puleCStr(infoPtr->second.label.c_str());
}

PuleEcsComponentSerializer puleEcsComponentSerializer(
  PuleEcsWorld const world,
  PuleEcsComponent const component
) {
  (void)world;
  auto infoPtr = componentInfos.find(component.id);
  if (infoPtr == componentInfos.end()) {
    puleLogError("component %zu does not exist", component.id);
    return {
      .serializeComponentCallback = nullptr,
      .deserializeComponentCallback = nullptr,
    };
  }
  return (
    PuleEcsComponentSerializer {
      .serializeComponentCallback = (
        infoPtr->second.serializeComponentCallback
      ),
      .deserializeComponentCallback = (
        infoPtr->second.deserializeComponentCallback
      ),
    }
  );
}

PuleStringView puleEcsEntityName(
  PuleEcsWorld const puWorld,
  PuleEcsEntity const puEntity
) {
  ecs_world_t * const world = reinterpret_cast<ecs_world_t *>(puWorld.id);
  return puleCStr(ecs_get_name(world, puEntity.id));
}

PuleEcsComponentInfo puleEcsComponentInfo(
  PuleEcsWorld const world,
  PuleEcsComponent const component
) {
  ecs_world_t * const worldEcs = reinterpret_cast<ecs_world_t *>(world.id);
  auto const componentInfo = ::componentInfos.at(component.id);
  return PuleEcsComponentInfo {
    .label = puleCStr(ecs_get_name(worldEcs, component.id)),
    .byteLength = componentInfo.byteLength,
    .imguiOverviewCallback = componentInfo.imguiOverviewCallback,
    .imguiEntityCallback = componentInfo.imguiEntityCallback,
  };
}

void puleEcsComponentIterateAll(
  PuleEcsComponentIterateAllCallback const callback
) {
  for (auto const & componentInfoItr : ::componentInfos) {
    callback.fn(
      PuleEcsComponent{.id=componentInfoItr.first,},
      callback.userdata
    );
  }
}

size_t puleEcsIteratorEntityCount(PuleEcsIterator const iterator) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.id);
  return static_cast<size_t>(iter->count);
}

size_t puleEcsIteratorRelativeOffset(PuleEcsIterator const iterator) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.id);
  return static_cast<size_t>(iter->frame_offset);
}

void * puleEcsIteratorQueryComponents(
  PuleEcsIterator const iterator,
  size_t const componentIndex,
  size_t const componentByteLength
) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.id);
  return ecs_term_w_size(iter, componentByteLength, componentIndex+1);
}

PuleEcsEntity * puleEcsIteratorQueryEntities(
  PuleEcsIterator const iterator
) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.id);
  // the return of uint64_t matches ecs_entity_t, so pointer should work
  return reinterpret_cast<PuleEcsEntity *>(iter->entities);
}

PuleEcsWorld puleEcsIteratorWorld(
  PuleEcsIterator const iterator
) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.id);
  return { reinterpret_cast<uint64_t>(iter->world) };
}

PuleEcsSystem puleEcsIteratorSystem(
  PuleEcsIterator const iterator
) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.id);
  return { reinterpret_cast<uint64_t>(iter->system) };
}

void * puleEcsIteratorUserData(PuleEcsIterator const iterator) {
  auto iter = reinterpret_cast<ecs_iter_t *>(iterator.id);
  return iter->param;
}

static ecs_entity_t callbackFrequencyToFlecs(
  PuleEcsSystemCallbackFrequency const frequency
) {
  switch (frequency) {
    default: PULE_assert(false);
    case PuleEcsSystemCallbackFrequency_none:       return EcsInactive;
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

void puleEcsSystemAdvance(
  PuleEcsWorld const world,
  PuleEcsSystem const system,
  float const deltaTime,
  void * const userdata
) {
  ecs_run(
    reinterpret_cast<ecs_world_t *>(world.id),
    system.id,
    deltaTime,
    userdata
  );
}

PuleEcsEntity puleEcsEntityCreate(
  PuleEcsWorld const puWorld,
  PuleStringView const label
) {
  ecs_world_t * const world = reinterpret_cast<ecs_world_t *>(puWorld.id);
  ecs_entity_t const entity = ecs_new_id(world);
  // TODO verify name doesn't already exist...
  ecs_set_name(world, entity, label.contents);

  if (::puEntityTag == 0) {
    ::puEntityTag = ecs_new_id(world);
  }
  ecs_add_id(world, entity, ::puEntityTag);

  return {.id = entity,};
}

void puleEcsEntityDestroy(
  PuleEcsWorld const world,
  PuleEcsEntity const entity
) {
  ecs_delete(reinterpret_cast<ecs_world_t *>(world.id), entity.id);
}

void puleEcsEntityAttachComponent(
  PuleEcsWorld const world,
  PuleEcsEntity const entity,
  PuleEcsComponent const component,
  void const * const nullableInitialData
) {
  ecs_world_t * const worldEcs = reinterpret_cast<ecs_world_t *>(world.id);
  auto const componentInfo = ::componentInfos.at(component.id);
  ecs_set_id(
    worldEcs,
    entity.id, component.id,
    componentInfo.byteLength, nullableInitialData
  );
}

void const * puleEcsEntityComponentData(
  PuleEcsWorld const world,
  PuleEcsEntity const entity,
  PuleEcsComponent const component
) {
  return (
    ecs_get_id(
      reinterpret_cast<ecs_world_t *>(world.id),
      entity.id,
      component.id
    )
  );
}

PuleEcsQuery puleEcsQueryByComponent(
  PuleEcsWorld const puWorld,
  PuleEcsComponent * const puComponentList,
  size_t const puComponentListSize,
  PuleError * const error
) {
  ecs_world_t * const world = reinterpret_cast<ecs_world_t *>(puWorld.id);
  ecs_query_desc_t queryDescription = {};
  if (puComponentListSize < ECS_TERM_DESC_CACHE_SIZE) {
    queryDescription.filter.terms_buffer_count = 0;
    queryDescription.filter.terms_buffer = nullptr;
    for (size_t it = 0; it < puComponentListSize; ++ it) {
      queryDescription.filter.terms[it].id = (
        static_cast<ecs_entity_t>(puComponentList[it].id)
      );
    }
  } else {
    PULE_error(
      PuleErrorEcs_queryFailed,
      "query failed, too many terms, behavior not yet implemented"
    );
    return {.id = 0,};
  }
  ecs_query_t * query = ecs_query_init(world, &queryDescription);
  if (!query) {
    PULE_error(PuleErrorEcs_queryFailed, "failed to query for components");
    return {.id = 0,};
  }
  ::internalQueries.emplace(
    ::internalQueryCount,
    InternalQuery{.query = query,}
  );
  return { .id = ::internalQueryCount ++, };
}

PuleEcsQuery puleEcsQueryAllEntities(
  PuleEcsWorld const puWorld,
  PuleError * const error
) {
  ecs_world_t * const world = reinterpret_cast<ecs_world_t *>(puWorld.id);
  ecs_query_desc_t queryDescription = {};
  queryDescription.filter.terms[0].id = ::puEntityTag;
  ecs_query_t * query = ecs_query_init(world, &queryDescription);
  if (!query) {
    PULE_error(PuleErrorEcs_queryFailed, "failed to query for components");
    return {.id = 0,};
  }
  ::internalQueries.emplace(
    ::internalQueryCount,
    InternalQuery{.query = query,}
  );
  return { .id = ::internalQueryCount ++, };
}

#define getQuery(pquery)                                   \
  ({                                                       \
    auto m_queryPtr = ::internalQueries.find(pquery.id);   \
    ecs_query_t * m_ret = nullptr;                         \
    if (m_queryPtr == ::internalQueries.end()) {           \
      puleLogError("query %zu does not exist", pquery.id); \
    } else {                                               \
      m_ret = m_queryPtr->second.query;                    \
    }                                                      \
    m_ret;                                                 \
  })

PuleEcsQueryIterator puleEcsQueryIterator(
  PuleEcsWorld const puWorld,
  PuleEcsQuery const puQuery
) {
  ecs_world_t * const world = reinterpret_cast<ecs_world_t *>(puWorld.id);
  ecs_query_t * const query = getQuery(puQuery);
  if (!query) return { .id = 0, };
  ecs_iter_t iter = ecs_query_iter(world, query);
  bool const consumed = !ecs_query_next(&iter);
  ::internalIters.emplace(
    ::internalIterCount,
    InternalIter { .iter = iter, .childIt = 0, .consumed = consumed, }
  );
  return { .id = ::internalIterCount ++, };
}

PuleEcsIterator puleEcsQueryIteratorNext(PuleEcsQueryIterator const puIter) {
  auto queryPtr = ::internalIters.find(puIter.id);
  if (queryPtr == ::internalIters.end()) {
    puleLogError("query iter %zu does not exist", puIter.id);
    return { .id = 0, };
  }
  ::InternalIter & iter = queryPtr->second;
  if (iter.consumed) {
    return { .id = 0, };
  }
  iter.consumed = !ecs_query_next(&iter.iter);
  return { .id = reinterpret_cast<uint64_t>(&iter.iter), };
}

void puleEcsQueryIteratorDestroy(
  PuleEcsQueryIterator const iter
) {
  ::internalIters.erase(iter.id);
}

void puleEcsQueryDestroy(PuleEcsQuery const puQuery) {
  ecs_query_t * query = getQuery(puQuery);
  if (query == nullptr) {
    return;
  }
  ecs_query_fini(query);
  ::internalQueries.erase(puQuery.id);
}

} // C
