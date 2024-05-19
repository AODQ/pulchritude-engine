# camera

## structs
### PuleCameraPerspective
```c
struct {
  nearCutoff : float;
  farCutoff : float;
  aspectRatio : float;
  fieldOfViewRadians : float;
};
```
### PuleCameraSetArray
```c
struct {
  cameras : PuleCamera ptr;
  cameraCount : size_t;
};
```

## entities
### PuleCamera
### PuleCameraSet
### PuleCameraController

## functions
### puleCameraCreate
```c
puleCameraCreate() PuleCamera;
```
### puleCameraDestroy
```c
puleCameraDestroy(
  camera : PuleCamera
) void;
```
### puleCameraView
```c
puleCameraView(
  camera : PuleCamera
) PuleF32m44;
```
### puleCameraProj
```c
puleCameraProj(
  camera : PuleCamera
) PuleF32m44;
```
### puleCameraLookAt
```c
puleCameraLookAt(
  camera : PuleCamera,
  origin : PuleF32v3,
  normalizedTarget : PuleF32v3,
  up : PuleF32v3
) void;
```
### puleCameraPerspective
```c
puleCameraPerspective(
  camera : PuleCamera
) PuleCameraPerspective;
```
### puleCameraPerspectiveSet
```c
puleCameraPerspectiveSet(
  camera : PuleCamera,
  perspective : PuleCameraPerspective
) void;
```
### puleCameraSetCreate
```c
puleCameraSetCreate(
  label : PuleStringView
) PuleCameraSet;
```
### puleCameraSetDestroy
```c
puleCameraSetDestroy(
  camera : PuleCameraSet
) void;
```
### puleCameraSetAdd
```c
puleCameraSetAdd(
  set : PuleCameraSet,
  camera : PuleCamera
) void;
```
### puleCameraSetRemove
```c
puleCameraSetRemove(
  set : PuleCameraSet,
  camera : PuleCamera
) void;
```
### puleCameraSetArray
```c
puleCameraSetArray(
  set : PuleCameraSet
) PuleCameraSetArray;
```
### puleCameraSetUniformBuffer
```c
puleCameraSetUniformBuffer(
  set : PuleCameraSet
) PuleGpuBuffer;
```
### puleCameraSetRefresh

  must be on main thread in case buffer needs to be reallocated

```c
puleCameraSetRefresh(
  set : PuleCameraSet
) PuleGpuFence;
```
### puleCameraControllerFirstPerson
```c
puleCameraControllerFirstPerson(
  platform : PulePlatform,
  camera : PuleCamera
) PuleCameraController;
```
### puleCameraControllerOrbit
```c
puleCameraControllerOrbit(
  platform : PulePlatform,
  camera : PuleCamera,
  origin : PuleF32v3,
  radius : float
) PuleCameraController;
```
### puleCameraControllerDestroy
```c
puleCameraControllerDestroy(
  controller : PuleCameraController
) void;
```
### puleCameraControllerPollEvents
```c
puleCameraControllerPollEvents() void;
```
