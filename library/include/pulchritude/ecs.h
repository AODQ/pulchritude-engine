/* auto generated file ecs */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/data-serializer.h>
#include <pulchritude/error.h>
#include <pulchritude/math.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
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
typedef struct {
  PuleEcsWorld world;
  PuleEcsEntity entity;
  void const * userdata;
  void(* callback)(PuleEcsComponent, void const *);
} PuleEcsEntityIterateComponentsInfo;
typedef struct {
  void(* serializeComponentCallback)(PuleEcsEntity, void const *, PuleDsValue, PuleAllocator);
  void(* deserializeComponentCallback)(PuleEcsWorld, PuleEcsEntity, PuleEcsComponent, PuleDsValue, PuleAllocator);
} PuleEcsComponentSerializer;
typedef struct {
  PuleStringView label;
  size_t byteLength;
  /* 
      callback for an overview of the component metadata
   */
  void(* imguiOverviewCallback)();
  /*  callback to display information on specified entity's component  */
  void(* imguiEntityCallback)(PuleEcsWorld, PuleEcsEntity, PuleEcsComponent);
} PuleEcsComponentInfo;
typedef struct {
  PuleEcsWorld world;
  void * userdata;
  void(* fn)(PuleEcsComponent, void *);
} PuleEcsComponentIterateAllCallback;
typedef struct {
  PuleEcsComponent component;
  PuleAccess access;
} PuleEcsSystemComponentDesc;
typedef struct {
  char const * label;
  size_t componentCount;
  PuleEcsSystemComponentDesc * components;
  void(* callback)(PuleEcsIterator);
  PuleEcsSystemCallbackFrequency callbackFrequency;
} PuleEcsSystemCreateInfo;
typedef struct {
  uint64_t id;
} PuleEcsQuery;
typedef struct {
  uint64_t id;
} PuleEcsQueryIterator;

// enum
typedef enum {
  PuleErrorEcs_none,
  PuleErrorEcs_queryFailed,
} PuleErrorEcs;
typedef enum {
  PuleEcsSystemCallbackFrequency_preUpdate,
  PuleEcsSystemCallbackFrequency_onUpdate,
  PuleEcsSystemCallbackFrequency_postUpdate,
} PuleEcsSystemCallbackFrequency;

// entities
typedef struct { uint64_t id; } PuleEcsWorld;
typedef struct { uint64_t id; } PuleEcsComponent;
typedef struct { uint64_t id; } PuleEcsSystem;
typedef struct { uint64_t id; } PuleEcsEntity;
typedef struct { uint64_t id; } PuleEcsIterator;

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
PULE_exportFn ptr const puleEcsEntityComponentData(PuleEcsWorld world, PuleEcsEntity entity, PuleEcsComponent component);
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
