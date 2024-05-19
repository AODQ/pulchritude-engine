/* auto generated file scene */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/ecs.h>
#include <pulchritude/gpu.h>
#include <pulchritude/physx.h>
#include <pulchritude/platform.h>
#include <pulchritude/camera.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PulePlatform platform;
  PuleSceneDimension dimension;
  bool createPhysxWorld;
} PuleSceneCreateInfo;
/* this must be consistent with what the scene was created with */
typedef union {
  PulePhysx2DWorld i2D;
  PulePhysx3DWorld i3D;
} PuleScenePhysxWorld;
typedef struct {
  PuleScene scene;
  float msDelta;
  bool advanceEcsWorld;
  bool advancePhysxWorld;
  PuleGpuSemaphore waitSemaphore;
} PuleSceneAdvanceInfo;
typedef struct {
  PuleSceneComponentModelType type;
} PuleSceneComponentModelData;
typedef struct {
  PulePhysx3DBody body;
} PuleSceneComponentPhysicsData;
typedef struct {
  PuleEcsEntity entity;
  PuleScene scene;
  PuleSceneComponentModelType modelType;
  PuleSceneComponentPhysicsData physicsData;
} PuleSceneNodeCreateInfo;

// enum
typedef enum {
  PuleSceneDimension_2d,
  PuleSceneDimension_3d,
} PuleSceneDimension;
typedef enum {
  PuleSceneComponentModelType_none,
  PuleSceneComponentModelType_cube,
  PuleSceneComponentModelType_sphere,
  PuleSceneComponentModelType_plane,
} PuleSceneComponentModelType;

// entities
typedef struct { uint64_t id; } PuleScene;

// functions
PULE_exportFn PuleScene puleSceneCreate(PuleSceneCreateInfo info);
/* 
  this will destroy the ecs & physx world if create*World was set to true
 */
PULE_exportFn void puleSceneDestroy(PuleScene scene);
/* 
  by default the scene renders to the screen, but can be set to image
 */
PULE_exportFn void puleSceneImageAttachmentsSet(PuleScene scene, PuleI32v2 viewportMin, PuleI32v2 viewportMax, PuleGpuImage colorAttachment, PuleGpuImage depthAttachment);
/* 
  by default the clear color is gray
 */
PULE_exportFn void puleSceneClearColorSet(PuleScene scene, PuleF32v4 color);
/* 
  default (based on set on dimension) is perspective or orthographic camera
 */
PULE_exportFn void puleSceneCameraSet(PuleScene scene, PuleCamera camera);
/* 
  default camera controller is a simple orbit controller
 */
PULE_exportFn void puleSceneCameraControllerSet(PuleScene scene, PuleCameraController controller);
/* 
  only valid if createEcsWorld was set to false
 */
PULE_exportFn void puleSceneEcsWorldSet(PuleScene scene, PuleEcsWorld world);
PULE_exportFn PuleEcsWorld puleSceneEcsWorld(PuleScene scene);
PULE_exportFn PuleEcsWorld puleScenePhysxWorldSet(PuleScene scene, PuleScenePhysxWorld world);
PULE_exportFn PuleScenePhysxWorld puleScenePhysxWorld(PuleScene scene);
PULE_exportFn void puleSceneAdvance(PuleSceneAdvanceInfo info);
PULE_exportFn PuleEcsComponent puleSceneComponentModel(PuleScene scene);
PULE_exportFn PuleEcsComponent puleSceneComponentPhysics(PuleScene scene);
PULE_exportFn void puleSceneNodeAttachComponents(PuleSceneNodeCreateInfo info);

#ifdef __cplusplus
} // extern C
#endif
