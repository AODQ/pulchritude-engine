/* auto generated file becs */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/string.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleStringView label;
  size_t byteLength;
} PuleBecsComponentCreateInfo;
typedef struct {
  PuleBecsWorld world;
  PuleStringView label;
  PuleBecsComponent const * components;
  size_t componentCount;
  PuleBecsSystemCallbackFrequency callbackFrequency;
  /* if callbackFrequency == fixed */
  size_t callbackFrequencyFixed;
  size_t bundleHistoryCount;
  int32_t(* callback)(PuleBecsSystem, system, PuleBecsBundle, writeBundle, PuleBecsBundleHistory, readBundleHistory, PuleBecsBundle *, inputSystemReadBundles);
} PuleBecsSystemCreateInfo;
typedef struct {
  PuleBecsSystem system;
  PuleBecsComponent const * components;
  size_t componentCount;
} PuleBecsEntityQuery;
typedef struct {
  PuleBecsSystem system;
  PuleBecsSystem * inputSystems;
  PuleBecsSystemInputRelation * inputSystemRelations;
  size_t inputSystemCount;
  uint8_t threadGroup;
} PuleBecsSystemInputRelationsCreateInfo;
typedef struct {
  PuleBecsSystem system;
  PuleStringView label;
  PuleBecsComponent const * components;
  void const * const * componentData;
  size_t componentCount;
} PuleBecsEntityCreateInfo;

// enum
typedef enum {
  PuleErrorBecs_none,
} PuleErrorBecs;
typedef enum {
  PuleBecsSystemCallbackFrequency_vsync,
  PuleBecsSystemCallbackFrequency_unlimited,
  PuleBecsSystemCallbackFrequency_fixed,
} PuleBecsSystemCallbackFrequency;
typedef enum {
  /*  system uses current frame  */
  PuleBecsSystemInputRelation_current,
  /*  system uses previous frame  */
  PuleBecsSystemInputRelation_previous,
} PuleBecsSystemInputRelation;

// entities
typedef struct { uint64_t id; } PuleBecsSystem;
typedef struct { uint64_t id; } PuleBecsEntity;
typedef struct { uint64_t id; } PuleBecsComponent;
typedef struct { uint64_t id; } PuleBecsBundle;
typedef struct { uint64_t id; } PuleBecsBundleHistory;
typedef struct { uint64_t id; } PuleBecsWorld;
typedef struct { uint64_t id; } PuleBecsQueryEntityIterator;

// functions
PULE_exportFn PuleBecsWorld puleBecsWorldCreate();
PULE_exportFn void puleBecsWorldDestroy(PuleBecsWorld world);
PULE_exportFn void puleBecsWorldAdvance(PuleBecsWorld world, float msDelta);
PULE_exportFn PuleBecsComponent puleBecsComponentCreate(PuleBecsWorld world, PuleBecsComponentCreateInfo const * createInfo);
PULE_exportFn PuleBecsSystem puleBecsSystemCreate(PuleBecsWorld world, PuleBecsSystemCreateInfo ci);
PULE_exportFn void puleBecsSystemDestroy(PuleBecsSystem system);
PULE_exportFn void puleBecsSystemAdvance(PuleBecsSystem system, float msDelta);
/* 
    returns an iterator to entities that match query, the iterator must be
    updated manually if entities are added or removed that match the query
    TODO for now only create this once per query and call update manually do
    not create multiple iterators for the same query
 */
PULE_exportFn PuleBecsQueryEntityIterator puleBecsQueryEntities(PuleBecsEntityQuery query);
PULE_exportFn void puleBecsQueryEntitiesUpdate(PuleBecsSystem system, PuleBecsQueryEntityIterator iter);
PULE_exportFn size_t puleBecsQueryEntityIteratorCount(PuleBecsSystem system, PuleBecsQueryEntityIterator iter);
PULE_exportFn PuleBecsEntity puleBecsQueryEntityIteratorAt(PuleBecsSystem system, PuleBecsQueryEntityIterator iter, size_t idx);
PULE_exportFn void puleBecsSystemInputRelationsSet(PuleBecsSystemInputRelationsCreateInfo createInfo);
PULE_exportFn PuleBecsEntity puleBecsEntityCreate(PuleBecsEntityCreateInfo createInfo);
PULE_exportFn void puleBecsEntityDestroy(PuleBecsEntity entity);
PULE_exportFn void * puleBecsEntityComponentData(PuleBecsEntity entity, PuleBecsComponent component);

#ifdef __cplusplus
} // extern C
#endif
