/* auto generated file becs */
#pragma once
#include "core.h"

#include "error.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorBecs_none = 0,
} PuleErrorBecs;
const uint32_t PuleErrorBecsSize = 1;
typedef enum {
  PuleBecsSystemCallbackFrequency_vsync = 0,
  PuleBecsSystemCallbackFrequency_unlimited = 1,
  PuleBecsSystemCallbackFrequency_fixed = 2,
} PuleBecsSystemCallbackFrequency;
const uint32_t PuleBecsSystemCallbackFrequencySize = 3;
typedef enum {
  /*  system uses current frame  */
  PuleBecsSystemInputRelation_current = 0,
  /*  system uses previous frame  */
  PuleBecsSystemInputRelation_previous = 1,
} PuleBecsSystemInputRelation;
const uint32_t PuleBecsSystemInputRelationSize = 2;

// entities
typedef struct PuleBecsSystem { uint64_t id; } PuleBecsSystem;
typedef struct PuleBecsEntity { uint64_t id; } PuleBecsEntity;
typedef struct PuleBecsComponent { uint64_t id; } PuleBecsComponent;
typedef struct PuleBecsBundle { uint64_t id; } PuleBecsBundle;
typedef struct PuleBecsBundleHistory { uint64_t id; } PuleBecsBundleHistory;
typedef struct PuleBecsWorld { uint64_t id; } PuleBecsWorld;
typedef struct PuleBecsQueryEntityIterator { uint64_t id; } PuleBecsQueryEntityIterator;

// structs
struct PuleBecsComponentCreateInfo;
struct PuleBecsSystemCreateInfo;
struct PuleBecsEntityQuery;
struct PuleBecsSystemInputRelationsCreateInfo;
struct PuleBecsEntityCreateInfo;

typedef struct PuleBecsComponentCreateInfo {
  PuleStringView label;
  size_t byteLength;
} PuleBecsComponentCreateInfo;
typedef struct PuleBecsSystemCreateInfo {
  PuleBecsWorld world;
  PuleStringView label;
  PuleBecsComponent const * components;
  size_t componentCount;
  PuleBecsSystemCallbackFrequency callbackFrequency;
  /* if callbackFrequency == fixed */
  size_t callbackFrequencyFixed;
  size_t bundleHistoryCount;
  int32_t(* callback)(PuleBecsSystem, system, PuleBecsBundle, writeBundle, PuleBecsBundleHistory, readBundleHistory, struct PuleBecsBundle *, inputSystemReadBundles);
} PuleBecsSystemCreateInfo;
typedef struct PuleBecsEntityQuery {
  PuleBecsSystem system;
  PuleBecsComponent const * components;
  size_t componentCount;
} PuleBecsEntityQuery;
typedef struct PuleBecsSystemInputRelationsCreateInfo {
  PuleBecsSystem system;
  struct PuleBecsSystem * inputSystems;
  struct PuleBecsSystemInputRelation * inputSystemRelations;
  size_t inputSystemCount;
  uint8_t threadGroup;
} PuleBecsSystemInputRelationsCreateInfo;
typedef struct PuleBecsEntityCreateInfo {
  PuleBecsSystem system;
  PuleStringView label;
  PuleBecsComponent const * components;
  void const * const * componentData;
  size_t componentCount;
} PuleBecsEntityCreateInfo;

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
