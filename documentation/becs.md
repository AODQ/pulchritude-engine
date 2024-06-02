# becs

## structs
### PuleBecsComponentCreateInfo
```c
struct {
  label : PuleStringView;
  byteLength : size_t;
};
```
### PuleBecsSystemCreateInfo
```c
struct {
  world : PuleBecsWorld;
  label : PuleStringView;
  components : PuleBecsComponent const ptr;
  componentCount : size_t;
  callbackFrequency : PuleBecsSystemCallbackFrequency;
  /* if callbackFrequency == fixed */
  callbackFrequencyFixed : size_t;
  bundleHistoryCount : size_t;
  callback : @fnptr(int32_t, PuleBecsSystem, PuleBecsBundle, PuleBecsBundleHistory, PuleBecsBundle ptr, float);
};
```
### PuleBecsEntityQuery
```c
struct {
  system : PuleBecsSystem;
  components : PuleBecsComponent const ptr;
  componentCount : size_t;
};
```
### PuleBecsSystemInputRelationsCreateInfo
```c
struct {
  system : PuleBecsSystem;
  inputSystems : PuleBecsSystem ptr;
  inputSystemRelations : PuleBecsSystemInputRelation ptr;
  inputSystemCount : size_t;
  threadGroup : uint8_t;
};
```
### PuleBecsEntityCreateInfo
```c
struct {
  system : PuleBecsSystem;
  label : PuleStringView;
  components : PuleBecsComponent const ptr;
  componentData : void const ptr const ptr;
  componentCount : size_t;
};
```

## enums
### PuleErrorBecs
```c
enum {
  none,
}
```
### PuleBecsSystemCallbackFrequency
```c
enum {
  vsync,
  unlimited,
  fixed,
}
```
### PuleBecsSystemInputRelation
```c
enum {
  /*  system uses current frame  */
  current,
  /*  system uses previous frame  */
  previous,
}
```

## entities
### PuleBecsSystem
### PuleBecsEntity
### PuleBecsComponent
### PuleBecsBundle
### PuleBecsBundleHistory
### PuleBecsWorld
### PuleBecsQueryEntityIterator

## functions
### puleBecsWorldCreate
```c
puleBecsWorldCreate() PuleBecsWorld;
```
### puleBecsWorldDestroy
```c
puleBecsWorldDestroy(
  world : PuleBecsWorld
) void;
```
### puleBecsWorldAdvance
```c
puleBecsWorldAdvance(
  world : PuleBecsWorld,
  msDelta : float
) void;
```
### puleBecsComponentCreate
```c
puleBecsComponentCreate(
  world : PuleBecsWorld,
  createInfo : PuleBecsComponentCreateInfo const ptr
) PuleBecsComponent;
```
### puleBecsSystemCreate
```c
puleBecsSystemCreate(
  world : PuleBecsWorld,
  ci : PuleBecsSystemCreateInfo
) PuleBecsSystem;
```
### puleBecsSystemDestroy
```c
puleBecsSystemDestroy(
  system : PuleBecsSystem
) void;
```
### puleBecsSystemAdvance
```c
puleBecsSystemAdvance(
  system : PuleBecsSystem,
  msDelta : float
) void;
```
### puleBecsQueryEntities

    returns an iterator to entities that match query, the iterator must be
    updated manually if entities are added or removed that match the query
    TODO for now only create this once per query and call update manually do
    not create multiple iterators for the same query

```c
puleBecsQueryEntities(
  query : PuleBecsEntityQuery
) PuleBecsQueryEntityIterator;
```
### puleBecsQueryEntitiesUpdate
```c
puleBecsQueryEntitiesUpdate(
  system : PuleBecsSystem,
  iter : PuleBecsQueryEntityIterator
) void;
```
### puleBecsQueryEntityIteratorCount
```c
puleBecsQueryEntityIteratorCount(
  system : PuleBecsSystem,
  iter : PuleBecsQueryEntityIterator
) size_t;
```
### puleBecsQueryEntityIteratorAt
```c
puleBecsQueryEntityIteratorAt(
  system : PuleBecsSystem,
  iter : PuleBecsQueryEntityIterator,
  idx : size_t
) PuleBecsEntity;
```
### puleBecsSystemInputRelationsSet
```c
puleBecsSystemInputRelationsSet(
  createInfo : PuleBecsSystemInputRelationsCreateInfo
) void;
```
### puleBecsEntityCreate
```c
puleBecsEntityCreate(
  createInfo : PuleBecsEntityCreateInfo
) PuleBecsEntity;
```
### puleBecsEntityDestroy
```c
puleBecsEntityDestroy(
  entity : PuleBecsEntity
) void;
```
### puleBecsEntityComponentData
```c
puleBecsEntityComponentData(
  entity : PuleBecsEntity,
  component : PuleBecsComponent
) void ptr;
```
