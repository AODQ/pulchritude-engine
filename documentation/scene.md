# scene

## structs
### PuleSceneCreateInfo
```c
struct {
  platform : PulePlatform;
  dimension : PuleSceneDimension;
  createPhysxWorld : bool;
};
```
### PuleScenePhysxWorld
this must be consistent with what the scene was created with
```c
union {
  i2D : PulePhysx2DWorld;
  i3D : PulePhysx3DWorld;
};
```
### PuleSceneAdvanceInfo
```c
struct {
  scene : PuleScene;
  msDelta : float;
  advanceEcsWorld : bool = false;
  advancePhysxWorld : bool = false;
  waitSemaphore : PuleGpuSemaphore = 0;
};
```
### PuleSceneComponentModelData
```c
struct {
  type : PuleSceneComponentModelType;
};
```
### PuleSceneComponentPhysicsData
```c
struct {
  body : PulePhysx3DBody;
};
```
### PuleSceneNodeCreateInfo
```c
struct {
  entity : PuleEcsEntity;
  scene : PuleScene;
  modelType : PuleSceneComponentModelType;
  physicsData : PuleSceneComponentPhysicsData;
};
```

## enums
### PuleSceneDimension
```c
enum {
  i2d,
  i3d,
}
```
### PuleSceneComponentModelType
```c
enum {
  none,
  cube,
  sphere,
  plane,
}
```

## entities
### PuleScene

## functions
### puleSceneCreate
```c
puleSceneCreate(
  info : PuleSceneCreateInfo
) PuleScene;
```
### puleSceneDestroy

  this will destroy the ecs & physx world if create*World was set to true

```c
puleSceneDestroy(
  scene : PuleScene
) void;
```
### puleSceneImageAttachmentsSet

  by default the scene renders to the screen, but can be set to image

```c
puleSceneImageAttachmentsSet(
  scene : PuleScene,
  viewportMin : PuleI32v2,
  viewportMax : PuleI32v2,
  colorAttachment : PuleGpuImage,
  depthAttachment : PuleGpuImage
) void;
```
### puleSceneClearColorSet

  by default the clear color is gray

```c
puleSceneClearColorSet(
  scene : PuleScene,
  color : PuleF32v4
) void;
```
### puleSceneCameraSet

  default (based on set on dimension) is perspective or orthographic camera

```c
puleSceneCameraSet(
  scene : PuleScene,
  camera : PuleCamera
) void;
```
### puleSceneCameraControllerSet

  default camera controller is a simple orbit controller

```c
puleSceneCameraControllerSet(
  scene : PuleScene,
  controller : PuleCameraController
) void;
```
### puleSceneEcsWorldSet

  only valid if createEcsWorld was set to false

```c
puleSceneEcsWorldSet(
  scene : PuleScene,
  world : PuleEcsWorld
) void;
```
### puleSceneEcsWorld
```c
puleSceneEcsWorld(
  scene : PuleScene
) PuleEcsWorld;
```
### puleScenePhysxWorldSet
```c
puleScenePhysxWorldSet(
  scene : PuleScene,
  world : PuleScenePhysxWorld
) PuleEcsWorld;
```
### puleScenePhysxWorld
```c
puleScenePhysxWorld(
  scene : PuleScene
) PuleScenePhysxWorld;
```
### puleSceneAdvance
```c
puleSceneAdvance(
  info : PuleSceneAdvanceInfo
) void;
```
### puleSceneComponentModel
```c
puleSceneComponentModel(
  scene : PuleScene
) PuleEcsComponent;
```
### puleSceneComponentPhysics
```c
puleSceneComponentPhysics(
  scene : PuleScene
) PuleEcsComponent;
```
### puleSceneNodeAttachComponents
```c
puleSceneNodeAttachComponents(
  info : PuleSceneNodeCreateInfo
) void;
```
