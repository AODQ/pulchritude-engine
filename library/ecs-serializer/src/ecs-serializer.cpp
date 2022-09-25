#include <pulchritude-ecs-serializer/ecs-serializer.h>

// essentially want to have plugins register their components,
//   so they are serializable

// Thus the proper way to create a scene is to create all the components
//   up-front, which then gives us information on how to serialize and
//   deserialize
//
// This is important because we will store the label per-component of each
//   entity.

/*

serialized-world: {
  # components: [ ] , probably optional ?
  entities: [
    {
      label: "some-unique-identifying-label",
      components: [
        "Position": {
          # serialized position info
        },
        "Direction": {
          # serialized direction info
        },
      ],
    },
  ],
}

thus iterate each entity, get the label, look up component, perform the
  serialize/deserialize callback

It might be possible that we could have generic serialize/deserializers, this
  must only be for POD, and involves just convert the raw binary data to
  base-64

*/

// -----------------------------------------------------------------------------
// -- SERIALIZE ----------------------------------------------------------------
// -----------------------------------------------------------------------------

namespace {

struct SerializeEntityPerComponentInfo {
  PuleEcsWorld world;
  PuleEcsEntity entity;
  PuleDsValue writeArrayPds;
  PuleAllocator allocator;
};

// iterated over by puleEcsEntityIterateComponents
void serializeEntityPerComponent(
  PuleEcsComponent const component,
  void const * const userdata
) {
  auto const info = (
    *reinterpret_cast<SerializeEntityPerComponentInfo const *>(userdata)
  );

  PuleDsValue const componentValue = puleDsCreateObject(info.allocator);
  puleDsAppendArray(info.writeArrayPds, componentValue);

  puleDsAssignObjectMember(
    componentValue,
    puleCStr("component-label"),
    puleDsCreateString(puleEcsComponentLabel(info.world, component))
  );

  PuleEcsComponentSerializer const serializer = (
    puleEcsComponentSerializer(info.world, component)
  );

  if (serializer.serializeComponentCallback) {
    serializer.serializeComponentCallback(
      info.entity,
      puleEcsEntityComponentData(info.world, info.entity, component),
      componentValue,
      info.allocator
    );
  }
}

} // namespace

extern "C" {

PuleDsValue puleEcsSerializeWorld(PuleEcsSerializeWorldInfo const info) {
  PuleDsValue const serializedWorld = puleDsCreateObject(info.allocator);
  PuleError err = puleError();
  PuleEcsQuery const allEntityQuery = puleEcsQueryAllEntities(info.world, &err);
  if (puleErrorConsume(&err)) {
    return { .id = 0, };
  }

  PuleDsValue const entityListValue = puleDsCreateArray(info.allocator);
  puleDsAssignObjectMember(
    serializedWorld,
    puleCStr("entity-list"),
    entityListValue
  );

  PuleEcsQueryIterator const queryIter = (
    puleEcsQueryIterator(info.world, allEntityQuery)
  );
  for (
    PuleEcsIterator iter = puleEcsQueryIteratorNext(queryIter);
    iter.id != 0;
    iter = puleEcsQueryIteratorNext(queryIter)
  ) {
    size_t const entityCount = puleEcsIteratorEntityCount(iter);
    PuleEcsEntity * const entities = puleEcsIteratorQueryEntities(iter);
    for (size_t it = 0; it < entityCount; ++ it) {
      PuleDsValue const entityValue = puleDsCreateObject(info.allocator);
      puleDsAppendArray(entityListValue, entityValue);
      puleDsAssignObjectMember(
        entityValue,
        puleCStr("label"),
        puleDsCreateString(puleEcsEntityName(info.world, entities[it]))
      );

      PuleDsValue const entityComponentListValue = (
        puleDsCreateArray(info.allocator)
      );
      puleDsAssignObjectMember(
        entityValue,
        puleCStr("components"),
        entityComponentListValue
      );

      auto const componentWorldInfo = SerializeEntityPerComponentInfo {
        .world = info.world,
        .entity = entities[it],
        .writeArrayPds = entityComponentListValue,
        .allocator = info.allocator,
      };

      puleEcsEntityIterateComponents(
        PuleEcsEntityIterateComponentsInfo {
          .world = info.world,
          .entity = entities[it],
          .userdata = (
            const_cast<void *>(
              reinterpret_cast<void const *>(&componentWorldInfo)
            )
          ),
          .callback = ::serializeEntityPerComponent,
        }
      );
    }
  }

  return serializedWorld;
}

} // extern C

// -----------------------------------------------------------------------------
// -- DESERIALIZE --------------------------------------------------------------
// -----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

void puleEcsDeserializeWorld(PuleEcsDeserializeWorldInfo const info) {
  // iterate through each entity of the entity list, then each component,
  //   creating each component as they were serialized

  PuleDsValueArray const entityListValues = (
    puleDsAsArray(puleDsObjectMember(info.readObjectPds, "entity-list"))
  );

  // each entity
  for (size_t entityIt = 0; entityIt < entityListValues.length; ++ entityIt) {
    PuleDsValue const entityValue = entityListValues.values[entityIt];
    PuleEcsEntity const entity = (
      puleEcsEntityCreate(
        info.world,
        puleDsAsString(puleDsObjectMember(entityValue, "label"))
      )
    );

    // each component of entity
    PuleDsValueArray const componentListValues = (
      puleDsAsArray(puleDsObjectMember(entityValue, "components"))
    );
    for (
      size_t componentIt = 0;
      componentIt < componentListValues.length;
      ++ componentIt
    ) {
      PuleDsValue const componentValue = (
        componentListValues.values[componentIt]
      );
      PuleStringView const componentLabel = (
        puleDsAsString(puleDsObjectMember(componentValue, "component-label"))
      );
      PuleEcsComponent const component = (
        puleEcsComponentFetchByLabel(info.world, componentLabel)
      );
      if (component.id == 0) {
        puleLogError(
          "failed to deserialize: could not fetch component by label '%s'",
          componentLabel.contents
        );
        continue;
      }
      PuleEcsComponentSerializer const serializer = (
        puleEcsComponentSerializer(info.world, component)
      );
      if (serializer.deserializeComponentCallback) {
        serializer.deserializeComponentCallback(
          info.world, entity, component, componentValue, info.allocator
        );
      }
    }
  }
}

#ifdef __cplusplus
} // extern C
#endif
