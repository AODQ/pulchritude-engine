/* auto generated file ecs */
#pragma once
#include "core.h"

#include "data-serializer.h"
#include "error.h"
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorEcs_none = 0,
  PuleErrorEcs_queryFailed = 1,
} PuleErrorEcs;
const uint32_t PuleErrorEcsSize = 2;
typedef enum {
  PuleEcsSystemCallbackFrequency_preUpdate = 0,
  PuleEcsSystemCallbackFrequency_onUpdate = 1,
  PuleEcsSystemCallbackFrequency_postUpdate = 2,
} PuleEcsSystemCallbackFrequency;
const uint32_t PuleEcsSystemCallbackFrequencySize = 3;

// entities
typedef struct PuleEcsWorld { uint64_t id; } PuleEcsWorld;
typedef struct PuleEcsComponent { uint64_t id; } PuleEcsComponent;
typedef struct PuleEcsSystem { uint64_t id; } PuleEcsSystem;
typedef struct PuleEcsEntity { uint64_t id; } PuleEcsEntity;
typedef struct PuleEcsIterator { uint64_t id; } PuleEcsIterator;

// structs
struct PuleEcsComponentCreateInfo;
struct PuleEcsEntityIterateComponentsInfo;
struct PuleEcsComponentSerializer;
struct PuleEcsComponentInfo;
struct PuleEcsComponentIterateAllCallback;
struct PuleEcsSystemComponentDesc;
struct PuleEcsSystemCreateInfo;
struct PuleEcsQuery;
struct PuleEcsQueryIterator;

typedef struct PuleEcsComponentCreateInfo {
  char const * label;
  size_t byteLength;
  size_t byteAlignment;
  void(* serializeComponentCallback)(PuleEcsEntity, void const *, PuleDsValue, PuleAllocator);
  void(* deserializeComponentCallback)(PuleEcsWorld, PuleEcsEntity, PuleEcsComponent, PuleDsValue, PuleAllocator);
  void(* imguiOverviewCallbackOptional)();
  void(* imguiEntityCallbackOptional)(PuleEcsWorld, PuleEcsEntity, PuleEcsComponent);
} PuleEcsComponentCreateInfo;
/* 
    this is a relatively expensive operation that should only be done during
    one-off cases (like serializing, debugging, etc)
   */
typedef struct PuleEcsEntityIterateComponentsInfo {
  PuleEcsWorld world;
  PuleEcsEntity entity;
  void const * userdata;
  void(* callback)(PuleEcsComponent, void const *);
} PuleEcsEntityIterateComponentsInfo;
typedef struct PuleEcsComponentSerializer {
  void(* serializeComponentCallback)(PuleEcsEntity, void const *, PuleDsValue, PuleAllocator);
  void(* deserializeComponentCallback)(PuleEcsWorld, PuleEcsEntity, PuleEcsComponent, PuleDsValue, PuleAllocator);
} PuleEcsComponentSerializer;
typedef struct PuleEcsComponentInfo {
  PuleStringView label;
  size_t byteLength;
  /* 
      callback for an overview of the component metadata
   */
  void(* imguiOverviewCallback)();
  /*  callback to display information on specified entity's component  */
  void(* imguiEntityCallback)(PuleEcsWorld, PuleEcsEntity, PuleEcsComponent);
} PuleEcsComponentInfo;
typedef struct PuleEcsComponentIterateAllCallback {
  PuleEcsWorld world;
  void * userdata;
  void(* fn)(PuleEcsComponent, void *);
} PuleEcsComponentIterateAllCallback;
typedef struct PuleEcsSystemComponentDesc {
  PuleEcsComponent component;
  PuleAccess access;
} PuleEcsSystemComponentDesc;
typedef struct PuleEcsSystemCreateInfo {
  char const * label;
  size_t componentCount;
  PuleEcsSystemComponentDesc * components;
  void(* callback)(PuleEcsIterator);
  PuleEcsSystemCallbackFrequency callbackFrequency;
} PuleEcsSystemCreateInfo;
typedef struct PuleEcsQuery {
  uint64_t id;
} PuleEcsQuery;
typedef struct PuleEcsQueryIterator {
  uint64_t id;
} PuleEcsQueryIterator;

// functions
PULE_exportFn PuleEcsWorld puleEcsWorldCreate();
PULE_exportFn void puleEcsWorldDestroy(PuleEcsWorld world);
PULE_exportFn void puleEcsWorldAdvance(PuleEcsWorld world, float msDelta);
PULE_exportFn PuleEcsComponent puleEcsComponentCreate(PuleEcsWorld world, PuleEcsComponentCreateInfo info);
PULE_exportFn void puleEcsEntityIterateComponents(PuleEcsEntityIterateComponentsInfo info);
PULE_exportFn PuleEcsComponent puleEcsComponentFetchByLabel(PuleEcsWorld world, PuleStringView label);
PULE_exportFn PuleStringView puleEcsComponentLabel(PuleEcsWorld world, PuleEcsComponent component);
PULE_exportFn PuleEcsComponentSerializer puleEcsComponentSerializer(PuleEcsWorld world, PuleEcsComponent component);
PULE_exportFn PuleEcsComponentInfo puleEcsComponentInfo(PuleEcsWorld world, PuleEcsComponent component);
PULE_exportFn void puleEcsComponentIterateAll(PuleEcsComponentIterateAllCallback callback);
PULE_exportFn size_t puleEcsIteratorEntityCount(PuleEcsIterator iterator);
PULE_exportFn size_t puleEcsIteratorRelativeOffset(PuleEcsIterator iterator);
PULE_exportFn void * puleEcsIteratorQueryComponents(PuleEcsIterator iterator, size_t componentIndex, size_t componentByteLength);
PULE_exportFn PuleEcsEntity * puleEcsIteratorQueryEntities(PuleEcsIterator iterator);
PULE_exportFn PuleEcsWorld puleEcsIteratorWorld(PuleEcsIterator iterator);
PULE_exportFn PuleEcsSystem puleEcsIteratorSystem(PuleEcsIterator iterator);
PULE_exportFn void * puleEcsIteratorUserData(PuleEcsIterator iterator);
PULE_exportFn PuleEcsSystem puleEcsSystemCreate(PuleEcsWorld world, PuleEcsSystemCreateInfo info);
PULE_exportFn void puleEcsSystemAdvance(PuleEcsWorld world, PuleEcsSystem system, float deltaTime, void * userdata);
PULE_exportFn PuleEcsEntity puleEcsEntityCreate(PuleEcsWorld world, PuleStringView label);
PULE_exportFn PuleStringView puleEcsEntityName(PuleEcsWorld world, PuleEcsEntity entity);
PULE_exportFn void puleEcsEntityDestroy(PuleEcsWorld world, PuleEcsEntity entity);
PULE_exportFn void puleEcsEntityAttachComponent(PuleEcsWorld world, PuleEcsEntity entity, PuleEcsComponent component, void const * nullableInitialData);
/* fetches the underlying-data for the component of an entity */
PULE_exportFn void const * puleEcsEntityComponentData(PuleEcsWorld world, PuleEcsEntity entity, PuleEcsComponent component);
PULE_exportFn PuleEcsQuery puleEcsQueryByComponent(PuleEcsWorld world, PuleEcsComponent * componentList, size_t componentListSize, PuleError * error);
PULE_exportFn PuleEcsQuery puleEcsQueryAllEntities(PuleEcsWorld world, PuleError * error);
PULE_exportFn void puleEcsQueryDestroy(PuleEcsQuery query);
PULE_exportFn PuleEcsQueryIterator puleEcsQueryIterator(PuleEcsWorld world, PuleEcsQuery query);
/*  returns .id=0 if finished  */
PULE_exportFn PuleEcsIterator puleEcsQueryIteratorNext(PuleEcsQueryIterator iter);
PULE_exportFn void puleEcsQueryIteratorDestroy(PuleEcsQueryIterator iter);
PULE_exportFn PuleEcsComponent puleEcsComponentOriginF32v3();

#ifdef __cplusplus
} // extern C
#endif
