#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-error/error.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorEcs_none,
  PuleErrorEcs_queryFailed,
} PuleErrorEcs;

typedef struct {
  uint64_t id;
} PuleEcsWorld;

typedef struct {
  uint64_t id;
} PuleEcsComponent;

typedef struct {
  uint64_t id;
} PuleEcsSystem;

typedef struct {
  uint64_t id;
} PuleEcsEntity;

PULE_exportFn PuleEcsWorld puleEcsWorldCreate();
PULE_exportFn void puleEcsWorldDestroy(PuleEcsWorld const world);
PULE_exportFn void puleEcsWorldAdvance(
  PuleEcsWorld const world,
  float const msDelta
);

typedef struct {
  char const * label;
  size_t byteLength;
  size_t byteAlignment;
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
  void (*imguiOverviewCallbackOptional)() PULE_defaultField(nullptr);
  void (*imguiEntityCallbackOptional)(
    PuleEcsWorld const world,
    PuleEcsEntity const entity,
    PuleEcsComponent const component
  ) PULE_defaultField(nullptr);
} PuleEcsComponentCreateInfo;

PULE_exportFn PuleEcsComponent puleEcsComponentCreate(
  PuleEcsWorld const world,
  PuleEcsComponentCreateInfo const info
);

// this is a relatively expensive operation that should only be done
//   during one-off cases (like serializing, debugging, etc)
typedef struct {
  PuleEcsWorld world;
  PuleEcsEntity entity;
  void const * userdata;
  void (*callback)(
    PuleEcsComponent const component,
    void const * const userdata
  );
} PuleEcsEntityIterateComponentsInfo;
PULE_exportFn void puleEcsEntityIterateComponents(
  PuleEcsEntityIterateComponentsInfo const info
);

PULE_exportFn PuleEcsComponent puleEcsComponentFetchByLabel(
  PuleEcsWorld const world,
  PuleStringView const label
);

PULE_exportFn PuleStringView puleEcsComponentLabel(
  PuleEcsWorld const world,
  PuleEcsComponent const component
);

typedef struct {
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
} PuleEcsComponentSerializer;
PULE_exportFn PuleEcsComponentSerializer puleEcsComponentSerializer(
  PuleEcsWorld const world,
  PuleEcsComponent const component
);

typedef struct {
  PuleStringView label;
  size_t byteLength;
  // callback for an overview of the component metadata
  void (*imguiOverviewCallback)();

  // callback to display information on the specified entity's component
  void (*imguiEntityCallback)(
    PuleEcsWorld const world,
    PuleEcsEntity const entity,
    PuleEcsComponent const component
  );
} PuleEcsComponentInfo;

PULE_exportFn PuleEcsComponentInfo puleEcsComponentInfo(
  PuleEcsWorld const world,
  PuleEcsComponent const component
);

typedef struct {
  PuleEcsWorld world;
  void * userdata;
  void (*fn)(PuleEcsComponent const component, void * const userdata);
} PuleEcsComponentIterateAllCallback;

PULE_exportFn void puleEcsComponentIterateAll(
  PuleEcsComponentIterateAllCallback const callback
);

typedef struct {
  uint64_t id;
} PuleEcsIterator;

PULE_exportFn size_t puleEcsIteratorEntityCount(
  PuleEcsIterator const iterator
);
PULE_exportFn size_t puleEcsIteratorRelativeOffset(
  PuleEcsIterator const iterator
);
PULE_exportFn void * puleEcsIteratorQueryComponents(
  PuleEcsIterator const iterator,
  size_t const componentIndex,
  size_t const componentByteLength
);
PULE_exportFn PuleEcsEntity * puleEcsIteratorQueryEntities(
  PuleEcsIterator const iterator
);
PULE_exportFn PuleEcsWorld puleEcsIteratorWorld(PuleEcsIterator const iterator);
PULE_exportFn PuleEcsSystem puleEcsIteratorSystem(
  PuleEcsIterator const iterator
);
PULE_exportFn void * puleEcsIteratorUserData(PuleEcsIterator const iterator);

typedef enum {
  PuleEcsSystemCallbackFrequency_preUpdate,
  PuleEcsSystemCallbackFrequency_onUpdate,
  PuleEcsSystemCallbackFrequency_postUpdate,
} PuleEcsSystemCallbackFrequency;

typedef struct PuleEcsSystemComponentDesc {
  PuleEcsComponent component;
  PuleAccess access;
} PuleEcsSystemComponentDesc;

typedef struct {
  char const * label;
  size_t componentCount;
  PuleEcsSystemComponentDesc * components;
  void (* callback)(PuleEcsIterator const iterator);
  PuleEcsSystemCallbackFrequency callbackFrequency;
} PuleEcsSystemCreateInfo;

PULE_exportFn PuleEcsSystem puleEcsSystemCreate(
  PuleEcsWorld const world,
  PuleEcsSystemCreateInfo const info
);

PULE_exportFn void puleEcsSystemAdvance(
  PuleEcsWorld const world,
  PuleEcsSystem const system,
  float const deltaTime,
  void * const userdata
);

PULE_exportFn PuleEcsEntity puleEcsEntityCreate(
  PuleEcsWorld const world,
  PuleStringView const label
);
PULE_exportFn PuleStringView puleEcsEntityName(
  PuleEcsWorld const world,
  PuleEcsEntity const entity
);
PULE_exportFn void puleEcsEntityDestroy(
  PuleEcsWorld const world,
  PuleEcsEntity const entity
);

PULE_exportFn void puleEcsEntityAttachComponent(
 PuleEcsWorld const world,
 PuleEcsEntity const entity,
 PuleEcsComponent const component,
 void const * const nullableInitialData
);

// fetches the underlying-data for the component of an entity
PULE_exportFn void const * puleEcsEntityComponentData(
  PuleEcsWorld const world,
  PuleEcsEntity const entity,
  PuleEcsComponent const component
);

typedef struct {
  uint64_t id;
} PuleEcsQuery;

PULE_exportFn PuleEcsQuery puleEcsQueryByComponent(
  PuleEcsWorld const world,
  PuleEcsComponent * const componentList,
  size_t const componentListSize,
  PuleError * const error
);

// these below are very silly, just can't think of a better way for serializing
PULE_exportFn PuleEcsQuery puleEcsQueryAllEntities(
  PuleEcsWorld const world,
  PuleError * const error
);

PULE_exportFn void puleEcsQueryDestroy(PuleEcsQuery const query);

typedef struct {
  uint64_t id;
} PuleEcsQueryIterator;
PULE_exportFn PuleEcsQueryIterator puleEcsQueryIterator(
  PuleEcsWorld const world,
  PuleEcsQuery const query
);
// returns .id=0 if finished
PULE_exportFn PuleEcsIterator puleEcsQueryIteratorNext(
  PuleEcsQueryIterator const iter
);
PULE_exportFn void puleEcsQueryIteratorDestroy(
  PuleEcsQueryIterator const iter
);

// TODO destroy entities and systems

// -- these are universal / global / default entities
//    to override these is kind of impossible

typedef PuleF32v3 PuleEcsComponentOriginF32v3;
PuleEcsComponent puleEcsComponentOriginF32v3();

#ifdef __cplusplus
}
#endif
