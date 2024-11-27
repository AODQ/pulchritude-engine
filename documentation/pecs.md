# pecs

## structs
### PulePecsComponentCreateInfo
```c
struct {
  label : PuleStringView;
  byteLength : size_t;
  byteAlignment : size_t;
  serializeComponentCallback : @fnptr(void, void const ptr, PuleDsValue);
  deserializeComponentCallback : @fnptr(void, void ptr, PuleDsValue);
  imguiOverviewCallbackOptional : @fnptr(void) = nullptr;
  imguiEntityCallbackOptional : @fnptr(void, PulePecsScene, PulePecsEntity, PulePecsComponent) = nullptr;
};
```
### PulePecsSystemCreateInfo
```c
struct {
  scene : PulePecsScene;
  label : PuleStringView;
  components : PulePecsComponent const ptr;
  componentCount : size_t;
  callbackFrequency : PulePecsSystemCallbackFrequency;
  /* if frequency is fixed */
  callbackFrequencyFixed : PuleMicrosecond;
  userdata : void ptr;
  callback : @fnptr(int32_t, PulePecsScene, PulePecsSystem, PuleMicrosecond, void ptr);
};
```
### PulePecsQueryEntityCreateInfo
```c
struct {
  scene : PulePecsScene;
  components : PulePecsComponent const ptr;
  componentCount : size_t;
};
```
### PulePecsEntityTemplateCreateInfo
```c
struct {
  scene : PulePecsScene;
  label : PuleStringView;
  components : PulePecsComponent const ptr;
  componentCount : size_t;
};
```
### PulePecsEntityCreateInfo
```c
struct {
  scene : PulePecsScene;
  label : PuleStringView;
  entityTemplate : PulePecsEntityTemplate;
};
```
### PulePecsComponent_Transform2D
```c
struct {
  origin : PuleF32v2;
  rotation : float;
  scale : PuleF32v2;
};
```
### PulePecsComponent_Camera
```c
struct {
  fov : float;
  near : float;
  far : float;
};
```
### PulePecsComponent_2dSprite
```c
struct {
  spriteInstance : PuleAssetSpriteInstance;
};
```

## enums
### PulePecsSystemCallbackFrequency
```c
enum {
  vsync,
  unlimited,
  fixed,
}
```

## entities
### PulePecsScene
### PulePecsEntity
### PulePecsComponent
### PulePecsSystem
### PulePecsQueryEntity
### PulePecsQueryEntityIterator
### PulePecsEntityTemplate

## functions
### pulePecsSceneCreate
```c
pulePecsSceneCreate() PulePecsScene;
```
### pulePecsSceneDestroy
```c
pulePecsSceneDestroy(
  scene : PulePecsScene
) void;
```
### pulePecsSceneAdvance
```c
pulePecsSceneAdvance(
  scene : PulePecsScene,
  deltaTime : PuleMicrosecond
) void;
```
### pulePecsComponentCreate
```c
pulePecsComponentCreate(
  scene : PulePecsScene,
  createInfo : PulePecsComponentCreateInfo
) PulePecsComponent;
```
### pulePecsComponentFetch
```c
pulePecsComponentFetch(
  scene : PulePecsScene,
  label : PuleStringView
) PulePecsComponent;
```
### pulePecsSystemCreate
```c
pulePecsSystemCreate(
  createInfo : PulePecsSystemCreateInfo
) PulePecsSystem;
```
### pulePecsSystemDestroy
```c
pulePecsSystemDestroy(
  scene : PulePecsScene,
  system : PulePecsSystem
) void;
```
### pulePecsSystemAdvance
```c
pulePecsSystemAdvance(
  scene : PulePecsScene,
  system : PulePecsSystem,
  deltaTime : PuleMicrosecond
) void;
```
### pulePecsQueryEntityCreate
```c
pulePecsQueryEntityCreate(
  createInfo : PulePecsQueryEntityCreateInfo
) PulePecsQueryEntity;
```
### pulePecsQueryEntityDestroy
```c
pulePecsQueryEntityDestroy(
  scene : PulePecsScene,
  query : PulePecsQueryEntity
) void;
```
### pulePecsQueryEntityIteratorCreate
```c
pulePecsQueryEntityIteratorCreate(
  scene : PulePecsScene,
  query : PulePecsQueryEntity
) PulePecsQueryEntityIterator;
```
### pulePecsQueryEntityIteratorDestroy
```c
pulePecsQueryEntityIteratorDestroy(
  scene : PulePecsScene,
  iterator : PulePecsQueryEntityIterator
) void;
```
### pulePecsQueryEntityIteratorFront
```c
pulePecsQueryEntityIteratorFront(
  scene : PulePecsScene,
  iterator : PulePecsQueryEntityIterator
) PulePecsEntity;
```
### pulePecsQueryEntityIteratorPop
```c
pulePecsQueryEntityIteratorPop(
  scene : PulePecsScene,
  iterator : PulePecsQueryEntityIterator
) void;
```
### pulePecsQueryEntityIteratorIsEmpty
```c
pulePecsQueryEntityIteratorIsEmpty(
  scene : PulePecsScene,
  iterator : PulePecsQueryEntityIterator
) bool;
```
### pulePecsEntityTemplateCreate
```c
pulePecsEntityTemplateCreate(
  createInfo : PulePecsEntityTemplateCreateInfo
) PulePecsEntityTemplate;
```
### pulePecsEntityTemplateDestroy
```c
pulePecsEntityTemplateDestroy(
  scene : PulePecsScene,
  entityTemplate : PulePecsEntityTemplate
) void;
```
### pulePecsEntityCreate

  Entities have a relatively large overhead, so it's best to not use them
  for things that have a large number of instances, e.g. particles. A particle
  system and all of its particles should be implemented as a single entity to
  avoid overhead.


```c
pulePecsEntityCreate(
  createInfo : PulePecsEntityCreateInfo
) PulePecsEntity;
```
### pulePecsEntityDestroy
```c
pulePecsEntityDestroy(
  scene : PulePecsScene,
  entity : PulePecsEntity
) void;
```
### pulePecsEntityLabel
```c
pulePecsEntityLabel(
  scene : PulePecsScene,
  entity : PulePecsEntity
) PuleStringView;
```
### pulePecsEntityComponentData
```c
pulePecsEntityComponentData(
  scene : PulePecsScene,
  entity : PulePecsEntity,
  component : PulePecsComponent
) void ptr;
```
### pulePecsComponentEngine_Transform2D
```c
pulePecsComponentEngine_Transform2D(
  scene : PulePecsScene
) PulePecsComponent;
```
### pulePecsComponentEngine_Camera
```c
pulePecsComponentEngine_Camera(
  scene : PulePecsScene
) PulePecsComponent;
```
### pulePecsComponentEngine_2dSprite

  2D sprite component

```c
pulePecsComponentEngine_2dSprite(
  scene : PulePecsScene
) PulePecsComponent;
```
