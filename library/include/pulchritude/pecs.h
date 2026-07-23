/* auto generated file pecs */
#pragma once
#include "core.h"

#include "data-serializer.h"
#include "math.h"
#include "time.h"
#include "asset-sprite.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PulePecsSystemCallbackFrequency_vsync = 0,
  PulePecsSystemCallbackFrequency_unlimited = 1,
  PulePecsSystemCallbackFrequency_fixed = 2,
} PulePecsSystemCallbackFrequency;
const uint32_t PulePecsSystemCallbackFrequencySize = 3;

// entities
typedef struct PulePecsScene { uint64_t id; } PulePecsScene;
typedef struct PulePecsEntity { uint64_t id; } PulePecsEntity;
typedef struct PulePecsComponent { uint64_t id; } PulePecsComponent;
typedef struct PulePecsSystem { uint64_t id; } PulePecsSystem;
typedef struct PulePecsQueryEntity { uint64_t id; } PulePecsQueryEntity;
typedef struct PulePecsQueryEntityIterator { uint64_t id; } PulePecsQueryEntityIterator;
typedef struct PulePecsEntityTemplate { uint64_t id; } PulePecsEntityTemplate;

// structs
struct PulePecsComponentCreateInfo;
struct PulePecsSystemCreateInfo;
struct PulePecsQueryEntityCreateInfo;
struct PulePecsEntityTemplateCreateInfo;
struct PulePecsEntityCreateInfo;
struct PulePecsComponent_Transform2D;
struct PulePecsComponent_Camera;
struct PulePecsComponent_2dSprite;

typedef struct PulePecsComponentCreateInfo {
  PuleStringView label;
  size_t byteLength;
  size_t byteAlignment;
  void(* serializeComponentCallback)(void const *, PuleDsValue);
  void(* deserializeComponentCallback)(void *, PuleDsValue);
  void(* imguiOverviewCallbackOptional)() PULE_defaultField(nullptr);
  void(* imguiEntityCallbackOptional)(PulePecsScene, PulePecsEntity, PulePecsComponent) PULE_defaultField(nullptr);
} PulePecsComponentCreateInfo;
typedef struct PulePecsSystemCreateInfo {
  PulePecsScene scene;
  PuleStringView label;
  PulePecsComponent const * components;
  size_t componentCount;
  PulePecsSystemCallbackFrequency callbackFrequency;
  /* if frequency is fixed */
  PuleMicrosecond callbackFrequencyFixed;
  void * userdata;
  int32_t(* callback)(PulePecsScene, PulePecsSystem, PuleMicrosecond, void *);
} PulePecsSystemCreateInfo;
typedef struct PulePecsQueryEntityCreateInfo {
  PulePecsScene scene;
  PulePecsComponent const * components;
  size_t componentCount;
} PulePecsQueryEntityCreateInfo;
typedef struct PulePecsEntityTemplateCreateInfo {
  PulePecsScene scene;
  PuleStringView label;
  PulePecsComponent const * components;
  size_t componentCount;
} PulePecsEntityTemplateCreateInfo;
typedef struct PulePecsEntityCreateInfo {
  PulePecsScene scene;
  PuleStringView label;
  PulePecsEntityTemplate entityTemplate;
} PulePecsEntityCreateInfo;
typedef struct PulePecsComponent_Transform2D {
  PuleF32v2 origin;
  float rotation;
  PuleF32v2 scale;
} PulePecsComponent_Transform2D;
typedef struct PulePecsComponent_Camera {
  float fov;
  float near;
  float far;
} PulePecsComponent_Camera;
typedef struct PulePecsComponent_2dSprite {
  PuleAssetSpriteInstance spriteInstance;
} PulePecsComponent_2dSprite;

// functions
PULE_exportFn PulePecsScene pulePecsSceneCreate();
PULE_exportFn void pulePecsSceneDestroy(PulePecsScene scene);
PULE_exportFn void pulePecsSceneAdvance(PulePecsScene scene, PuleMicrosecond deltaTime);
PULE_exportFn PulePecsComponent pulePecsComponentCreate(PulePecsScene scene, PulePecsComponentCreateInfo createInfo);
PULE_exportFn PulePecsComponent pulePecsComponentFetch(PulePecsScene scene, PuleStringView label);
PULE_exportFn PulePecsSystem pulePecsSystemCreate(PulePecsSystemCreateInfo createInfo);
PULE_exportFn void pulePecsSystemDestroy(PulePecsScene scene, PulePecsSystem system);
PULE_exportFn void pulePecsSystemAdvance(PulePecsScene scene, PulePecsSystem system, PuleMicrosecond deltaTime);
PULE_exportFn PulePecsQueryEntity pulePecsQueryEntityCreate(PulePecsQueryEntityCreateInfo createInfo);
PULE_exportFn void pulePecsQueryEntityDestroy(PulePecsScene scene, PulePecsQueryEntity query);
PULE_exportFn PulePecsQueryEntityIterator pulePecsQueryEntityIteratorCreate(PulePecsScene scene, PulePecsQueryEntity query);
PULE_exportFn void pulePecsQueryEntityIteratorDestroy(PulePecsScene scene, PulePecsQueryEntityIterator iterator);
PULE_exportFn PulePecsEntity pulePecsQueryEntityIteratorFront(PulePecsScene scene, PulePecsQueryEntityIterator iterator);
PULE_exportFn void pulePecsQueryEntityIteratorPop(PulePecsScene scene, PulePecsQueryEntityIterator iterator);
PULE_exportFn bool pulePecsQueryEntityIteratorIsEmpty(PulePecsScene scene, PulePecsQueryEntityIterator iterator);
PULE_exportFn PulePecsEntityTemplate pulePecsEntityTemplateCreate(PulePecsEntityTemplateCreateInfo createInfo);
PULE_exportFn void pulePecsEntityTemplateDestroy(PulePecsScene scene, PulePecsEntityTemplate entityTemplate);
/* 
  Entities have a relatively large overhead, so it's best to not use them
  for things that have a large number of instances, e.g. particles. A particle
  system and all of its particles should be implemented as a single entity to
  avoid overhead.

 */
PULE_exportFn PulePecsEntity pulePecsEntityCreate(PulePecsEntityCreateInfo createInfo);
PULE_exportFn void pulePecsEntityDestroy(PulePecsScene scene, PulePecsEntity entity);
PULE_exportFn PuleStringView pulePecsEntityLabel(PulePecsScene scene, PulePecsEntity entity);
PULE_exportFn void * pulePecsEntityComponentData(PulePecsScene scene, PulePecsEntity entity, PulePecsComponent component);
PULE_exportFn PulePecsComponent pulePecsComponentEngine_Transform2D(PulePecsScene scene);
PULE_exportFn PulePecsComponent pulePecsComponentEngine_Camera(PulePecsScene scene);
/* 
  2D sprite component
 */
PULE_exportFn PulePecsComponent pulePecsComponentEngine_2dSprite(PulePecsScene scene);

PULE_exportFn void pulePecsComponent_Transform2DDeserialize(void *, PuleDsValue);
PULE_exportFn void pulePecsComponent_Transform2DSerialize(void const *, PuleDsValue);
PULE_exportFn void pulePecsComponent_CameraDeserialize(void *, PuleDsValue);
PULE_exportFn void pulePecsComponent_CameraSerialize(void const *, PuleDsValue);
#ifdef __cplusplus
} // extern C
#endif
