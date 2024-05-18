#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-string/string.h>

// bECS is 'bundle' entity-component-system. Systems emit bundles of data
//   (entity IDs and component data), and other systems will process them,
//   to allow for easier parallelism.

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorBecs_none,
} PuleErrorBecs;

typedef struct { uint64_t id; } PuleBecsSystem;
typedef struct { uint64_t id; } PuleBecsEntity;
typedef struct { uint64_t id; } PuleBecsComponent;
typedef struct { uint64_t id; } PuleBecsBundle;
typedef struct { uint64_t id; } PuleBecsBundleHistory;
typedef struct { uint64_t id; } PuleBecsWorld;

// -- world --------------------------------------------------------------------

PULE_exportFn PuleBecsWorld puleBecsWorldCreate();
PULE_exportFn void puleBecsWorldDestroy(PuleBecsWorld world);
PULE_exportFn void puleBecsWorldAdvance(PuleBecsWorld, float const msDelta);

// -- component ----------------------------------------------------------------

typedef struct {
  PuleStringView label;
  size_t byteLength;
} PuleBecsComponentCreateInfo;

PULE_exportFn PuleBecsComponent puleBecsComponentCreate(
  PuleBecsWorld world,
  PuleBecsComponentCreateInfo const * createInfo
);

// -- system -------------------------------------------------------------------

typedef enum {
  PuleBecsSystemCallbackFrequency_vsync,
  PuleBecsSystemCallbackFrequency_unlimited,
  PuleBecsSystemCallbackFrequency_fixed,
} PuleBecsSystemCallbackFrequency;

typedef struct {
  PuleBecsWorld world;
  PuleStringView label;
  PuleBecsComponent const * components;
  size_t componentCount;
  PuleBecsSystemCallbackFrequency callbackFrequency;
  size_t callbackFrequencyFixed; // if callbackFrequency == fixed
  size_t bundleHistoryCount;

  int32_t (*callback)(
    PuleBecsSystem system,
    PuleBecsBundle writeBundle,
    PuleBecsBundleHistory readBundleHistory,
    PuleBecsBundle * inputSystemReadBundles
  );
} PuleBecsSystemCreateInfo;

PULE_exportFn PuleBecsSystem puleBecsSystemCreate(
  PuleBecsWorld world, PuleBecsSystemCreateInfo ci
);
PULE_exportFn void puleBecsSystemDestroy(PuleBecsSystem system);
PULE_exportFn void puleBecsSystemAdvance(PuleBecsSystem, float const msDelta);

// -- queries ------------------------------------------------------------------

typedef struct {
  PuleBecsSystem system;
  PuleBecsComponent const * components;
  size_t componentCount;
} PuleBecsEntityQuery;

typedef struct {
  uint64_t id;
} PuleBecsQueryEntityIterator;

// returns an iterator to entities that match query, the iterator must be
//   updated manually if entities are added or removed that match the query
// TODO for now only create this once per query and call update manually
//      do not create multiple iterators for the same query
PULE_exportFn PuleBecsQueryEntityIterator puleBecsQueryEntities(
  PuleBecsEntityQuery query
);

PULE_exportFn void puleBecsQueryEntitiesUpdate(
  PuleBecsSystem system,
  PuleBecsQueryEntityIterator iter
);

size_t puleBecsQueryEntityIteratorCount(
  PuleBecsSystem system, PuleBecsQueryEntityIterator iter
);
PuleBecsEntity puleBecsQueryEntityIteratorAt(
  PuleBecsSystem system, PuleBecsQueryEntityIterator iter, size_t idx
);

typedef enum {
  PuleBecsSystemInputRelation_current, // system will read from current frame
  PuleBecsSystemInputRelation_previous, // system will read from previous frame
} PuleBecsSystemInputRelation;

typedef struct {
  PuleBecsSystem system;
  PuleBecsSystem * inputSystems;
  PuleBecsSystemInputRelation * inputSystemRelations;
  size_t inputSystemCount;
  uint8_t threadGroup;
} PuleBecsSystemInputRelationsCreateInfo;

PULE_exportFn void puleBecsSystemInputRelationsSet(
  PuleBecsSystemInputRelationsCreateInfo createInfo
);

// -- entity -------------------------------------------------------------------

typedef struct {
  PuleBecsSystem system;
  PuleStringView label;
  PuleBecsComponent const * components;
  void const * const * componentData;
  size_t componentCount;
} PuleBecsEntityCreateInfo;

PULE_exportFn PuleBecsEntity puleBecsEntityCreate(
  PuleBecsEntityCreateInfo createInfo
);
void puleBecsEntityDestroy(PuleBecsEntity entity);
void * puleBecsEntityComponentData(
  PuleBecsEntity entity,
  PuleBecsComponent component
);



#ifdef __cplusplus
} // extern "C"
#endif
