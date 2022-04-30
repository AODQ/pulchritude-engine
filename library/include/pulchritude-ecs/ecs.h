#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

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
} PuleEcsComponentCreateInfo;

PULE_exportFn PuleEcsComponent puleEcsComponentCreate(
  PuleEcsWorld const world,
  PuleEcsComponentCreateInfo const info
);

typedef struct {
  void * data;
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
  PuleEcsSystemCallbackFrequency_none,
  PuleEcsSystemCallbackFrequency_preUpdate,
  PuleEcsSystemCallbackFrequency_onUpdate,
  PuleEcsSystemCallbackFrequency_postUpdate,
} PuleEcsSystemCallbackFrequency;

typedef struct {
  char const * label;
  char const * commaSeparatedComponentLabels;
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

PULE_exportFn void * puleEcsSystemUserData();

PULE_exportFn PuleEcsEntity puleEcsEntityCreate(PuleEcsWorld const world);
PULE_exportFn void puleEcsEntityDestroy(
  PuleEcsWorld const world,
  PuleEcsEntity const entity
);
PULE_exportFn void puleEcsEntityAttachComponent(
 PuleEcsWorld const world,
 PuleEcsEntity const entity,
 PuleEcsComponent const component,
 void * const nullableInitialData
);

// TODO destroy entities and systems

#ifdef __cplusplus
}
#endif
