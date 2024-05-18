#pragma once

// a scene is just a representation of a simple world of all the engine plugins,
//   to help get started

#include <pulchritude-camera/camera.h>
#include <pulchritude-core/core.h>
#include <pulchritude-ecs/ecs.h>
#include <pulchritude-gpu/module.h>
#include <pulchritude-physx/physx2d.h>
#include <pulchritude-physx/physx3d.h>
#include <pulchritude-platform/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- scene -------------------------------------------------------------------

typedef struct { uint64_t id; } PuleScene;

typedef enum {
  PuleSceneDimension_2d,
  PuleSceneDimension_3d,
} PuleSceneDimension;

typedef struct {
  PulePlatform platform;
  PuleSceneDimension dimension;
  //bool createEcsWorld; i think i have to always create ecs world for update
  bool createPhysxWorld;
} PuleSceneCreateInfo;

PULE_exportFn PuleScene puleSceneCreate(PuleSceneCreateInfo const info);

// this will destroy the ecs & physx world if create*World was set to true
PULE_exportFn void puleSceneDestroy(PuleScene const scene);

// by default the scene renders to the screen, but can be set to image
PULE_exportFn void puleSceneImageAttachmentsSet(
  PuleScene const scene,
  PuleI32v2 const viewportMin,
  PuleI32v2 const viewportMax,
  PuleGpuImage const colorAttachment,
  PuleGpuImage const depthAttachment
);

// by default the clear color is gray
PULE_exportFn void puleSceneClearColorSet(
  PuleScene const scene,
  PuleF32v4 const color
);

// default (based on set on dimension) is perspective or orthographic camera
PULE_exportFn void puleSceneCameraSet(
  PuleScene const scene,
  PuleCamera const camera
);

// default camera controller is a simple orbit controller
PULE_exportFn void puleSceneCameraControllerSet(
  PuleScene const scene,
  PuleCameraController const controller
);

// only valid if createEcsWorld was set to false
PULE_exportFn void puleSceneEcsWorldSet(
  PuleScene const scene,
  PuleEcsWorld const world
);
PULE_exportFn PuleEcsWorld puleSceneEcsWorld(PuleScene const scene);

// this must be consistent with what the scene was created with
typedef union PuleScenePhysxWorld {
  PulePhysx2DWorld i2D;
  PulePhysx3DWorld i3D;
} PuleScenePhysxWorld;
PULE_exportFn PuleEcsWorld puleScenePhysxWorldSet(
  PuleScene const scene,
  PuleScenePhysxWorld const world
);
PULE_exportFn PuleScenePhysxWorld puleScenePhysxWorld(PuleScene const scene);

// advance the scene, with optional updates
typedef struct PuleSceneAdvanceInfo {
  PuleScene scene;
  float msDelta;
  bool advanceEcsWorld PULE_defaultField(true);
  bool advancePhysxWorld PULE_defaultField(true);
  PuleGpuSemaphore const waitSemaphore PULE_defaultField({0});
} PuleSceneAdvanceInfo;
PULE_exportFn void puleSceneAdvance(PuleSceneAdvanceInfo const info);

#ifdef __cplusplus
}
#endif

// --- components --------------------------------------------------------------

// utility scene components

#include <pulchritude-ecs/ecs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleSceneComponentModelType_none,
  PuleSceneComponentModelType_cube,
  PuleSceneComponentModelType_sphere,
  PuleSceneComponentModelType_plane,
} PuleSceneComponentModelType;

typedef struct {
  PuleSceneComponentModelType type;
  // TODO material
  // TODO texture
  // TODO mesh support
  // TODO animation support?
} PuleSceneComponentModelData;

typedef struct {
  PulePhysx3DBody body;
} PuleSceneComponentPhysicsData;

PULE_exportFn PuleEcsComponent puleSceneComponentModel(PuleScene const scene);
PULE_exportFn PuleEcsComponent puleSceneComponentPhysics(PuleScene const scene);

typedef struct {
  PuleEcsEntity entity;
  PuleScene scene;
  PuleSceneComponentModelType modelType;
  PuleSceneComponentPhysicsData physicsData;
} PuleSceneNodeCreateInfo;

PULE_exportFn void puleSceneNodeAttachComponents(
  PuleSceneNodeCreateInfo const info
);

#ifdef __cplusplus
}
#endif
