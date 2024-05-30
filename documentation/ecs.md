# ecs

## structs
### PuleEcsComponentCreateInfo
```c
struct {
  label : char const ptr;
  byteLength : size_t;
  byteAlignment : size_t;
  serializeComponentCallback : @fnptr(void, PuleEcsEntity, void const ptr, PuleDsValue, PuleAllocator);
  deserializeComponentCallback : @fnptr(void, PuleEcsWorld, PuleEcsEntity, PuleEcsComponent, PuleDsValue, PuleAllocator);
  imguiOverviewCallbackOptional : @fnptr(void) = nullptr;
  imguiEntityCallbackOptional : @fnptr(void, PuleEcsWorld, PuleEcsEntity, PuleEcsComponent) = nullptr;
};
```
### PuleEcsEntityIterateComponentsInfo

    this is a relatively expensive operation that should only be done during
    one-off cases (like serializing, debugging, etc)
  
```c
struct {
  world : PuleEcsWorld;
  entity : PuleEcsEntity;
  userdata : void const ptr;
  callback : @fnptr(void, PuleEcsComponent, void const ptr);
};
```
### PuleEcsComponentSerializer
```c
struct {
  serializeComponentCallback : @fnptr(void, PuleEcsEntity, void const ptr, PuleDsValue, PuleAllocator);
  deserializeComponentCallback : @fnptr(void, PuleEcsWorld, PuleEcsEntity, PuleEcsComponent, PuleDsValue, PuleAllocator);
};
```
### PuleEcsComponentInfo
```c
struct {
  label : PuleStringView;
  byteLength : size_t;
  /* 
      callback for an overview of the component metadata
   */
  imguiOverviewCallback : @fnptr(void);
  /*  callback to display information on specified entity's component  */
  imguiEntityCallback : @fnptr(void, PuleEcsWorld, PuleEcsEntity, PuleEcsComponent);
};
```
### PuleEcsComponentIterateAllCallback
```c
struct {
  world : PuleEcsWorld;
  userdata : void ptr;
  fn : @fnptr(void, PuleEcsComponent, void ptr);
};
```
### PuleEcsSystemComponentDesc
```c
struct {
  component : PuleEcsComponent;
  access : PuleAccess;
};
```
### PuleEcsSystemCreateInfo
```c
struct {
  label : char const ptr;
  componentCount : size_t;
  components : PuleEcsSystemComponentDesc ptr;
  callback : @fnptr(void, PuleEcsIterator);
  callbackFrequency : PuleEcsSystemCallbackFrequency;
};
```
### PuleEcsQuery
```c
struct {
  id : uint64_t;
};
```
### PuleEcsQueryIterator
```c
struct {
  id : uint64_t;
};
```

## enums
### PuleErrorEcs
```c
enum {
  none,
  queryFailed,
}
```
### PuleEcsSystemCallbackFrequency
```c
enum {
  preUpdate,
  onUpdate,
  postUpdate,
}
```

## entities
### PuleEcsWorld
### PuleEcsComponent
### PuleEcsSystem
### PuleEcsEntity
### PuleEcsIterator

## functions
### puleEcsWorldCreate
```c
puleEcsWorldCreate() PuleEcsWorld;
```
### puleEcsWorldDestroy
```c
puleEcsWorldDestroy(
  world : PuleEcsWorld
) void;
```
### puleEcsWorldAdvance
```c
puleEcsWorldAdvance(
  world : PuleEcsWorld,
  msDelta : float
) void;
```
### puleEcsComponentCreate
```c
puleEcsComponentCreate(
  world : PuleEcsWorld,
  info : PuleEcsComponentCreateInfo
) PuleEcsComponent;
```
### puleEcsEntityIterateComponents
```c
puleEcsEntityIterateComponents(
  info : PuleEcsEntityIterateComponentsInfo
) void;
```
### puleEcsComponentFetchByLabel
```c
puleEcsComponentFetchByLabel(
  world : PuleEcsWorld,
  label : PuleStringView
) PuleEcsComponent;
```
### puleEcsComponentLabel
```c
puleEcsComponentLabel(
  world : PuleEcsWorld,
  component : PuleEcsComponent
) PuleStringView;
```
### puleEcsComponentSerializer
```c
puleEcsComponentSerializer(
  world : PuleEcsWorld,
  component : PuleEcsComponent
) PuleEcsComponentSerializer;
```
### puleEcsComponentInfo
```c
puleEcsComponentInfo(
  world : PuleEcsWorld,
  component : PuleEcsComponent
) PuleEcsComponentInfo;
```
### puleEcsComponentIterateAll
```c
puleEcsComponentIterateAll(
  callback : PuleEcsComponentIterateAllCallback
) void;
```
### puleEcsIteratorEntityCount
```c
puleEcsIteratorEntityCount(
  iterator : PuleEcsIterator
) size_t;
```
### puleEcsIteratorRelativeOffset
```c
puleEcsIteratorRelativeOffset(
  iterator : PuleEcsIterator
) size_t;
```
### puleEcsIteratorQueryComponents
```c
puleEcsIteratorQueryComponents(
  iterator : PuleEcsIterator,
  componentIndex : size_t,
  componentByteLength : size_t
) void ptr;
```
### puleEcsIteratorQueryEntities
```c
puleEcsIteratorQueryEntities(
  iterator : PuleEcsIterator
) PuleEcsEntity ptr;
```
### puleEcsIteratorWorld
```c
puleEcsIteratorWorld(
  iterator : PuleEcsIterator
) PuleEcsWorld;
```
### puleEcsIteratorSystem
```c
puleEcsIteratorSystem(
  iterator : PuleEcsIterator
) PuleEcsSystem;
```
### puleEcsIteratorUserData
```c
puleEcsIteratorUserData(
  iterator : PuleEcsIterator
) void ptr;
```
### puleEcsSystemCreate
```c
puleEcsSystemCreate(
  world : PuleEcsWorld,
  info : PuleEcsSystemCreateInfo
) PuleEcsSystem;
```
### puleEcsSystemAdvance
```c
puleEcsSystemAdvance(
  world : PuleEcsWorld,
  system : PuleEcsSystem,
  deltaTime : float,
  userdata : void ptr
) void;
```
### puleEcsEntityCreate
```c
puleEcsEntityCreate(
  world : PuleEcsWorld,
  label : PuleStringView
) PuleEcsEntity;
```
### puleEcsEntityName
```c
puleEcsEntityName(
  world : PuleEcsWorld,
  entity : PuleEcsEntity
) PuleStringView;
```
### puleEcsEntityDestroy
```c
puleEcsEntityDestroy(
  world : PuleEcsWorld,
  entity : PuleEcsEntity
) void;
```
### puleEcsEntityAttachComponent
```c
puleEcsEntityAttachComponent(
  world : PuleEcsWorld,
  entity : PuleEcsEntity,
  component : PuleEcsComponent,
  nullableInitialData : void const ptr
) void;
```
### puleEcsEntityComponentData
fetches the underlying-data for the component of an entity
```c
puleEcsEntityComponentData(
  world : PuleEcsWorld,
  entity : PuleEcsEntity,
  component : PuleEcsComponent
) void const ptr;
```
### puleEcsQueryByComponent
```c
puleEcsQueryByComponent(
  world : PuleEcsWorld,
  componentList : PuleEcsComponent ptr,
  componentListSize : size_t,
  error : PuleError ptr
) PuleEcsQuery;
```
### puleEcsQueryAllEntities
```c
puleEcsQueryAllEntities(
  world : PuleEcsWorld,
  error : PuleError ptr
) PuleEcsQuery;
```
### puleEcsQueryDestroy
```c
puleEcsQueryDestroy(
  query : PuleEcsQuery
) void;
```
### puleEcsQueryIterator
```c
puleEcsQueryIterator(
  world : PuleEcsWorld,
  query : PuleEcsQuery
) PuleEcsQueryIterator;
```
### puleEcsQueryIteratorNext
 returns .id=0 if finished 
```c
puleEcsQueryIteratorNext(
  iter : PuleEcsQueryIterator
) PuleEcsIterator;
```
### puleEcsQueryIteratorDestroy
```c
puleEcsQueryIteratorDestroy(
  iter : PuleEcsQueryIterator
) void;
```
### puleEcsComponentOriginF32v3
```c
puleEcsComponentOriginF32v3() PuleEcsComponent;
```
