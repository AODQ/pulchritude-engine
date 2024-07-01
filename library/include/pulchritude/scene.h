/* auto generated file scene */
#pragma once
#include "core.h"

#include "ecs.h"
#include "gpu.h"
#include "physx.h"
#include "platform.h"
#include "camera.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleSceneDimension_i2d = 0,
  PuleSceneDimension_i3d = 1,
} PuleSceneDimension;
const uint32_t PuleSceneDimensionSize = 2;
typedef enum {
  PuleSceneComponentModelType_none = 0,
  PuleSceneComponentModelType_cube = 1,
  PuleSceneComponentModelType_sphere = 2,
  PuleSceneComponentModelType_plane = 3,
} PuleSceneComponentModelType;
const uint32_t PuleSceneComponentModelTypeSize = 4;

// entities
typedef struct PuleScene { uint64_t id; } PuleScene;

// structs
struct PuleSceneCreateInfo;
union PuleScenePhysxWorld;
struct PuleSceneAdvanceInfo;
struct PuleSceneComponentModelData;
struct PuleSceneComponentPhysicsData;
struct PuleSceneNodeCreateInfo;

typedef struct PuleSceneCreateInfo {
  PulePlatform platform;
  PuleSceneDimension dimension;
  bool createPhysxWorld;
} PuleSceneCreateInfo;
/* this must be consistent with what the scene was created with */
typedef union PuleScenePhysxWorld {
  PulePhysx2DWorld i2D;
  PulePhysx3DWorld i3D;
} PuleScenePhysxWorld;
typedef struct PuleSceneAdvanceInfo {
  PuleScene scene;
  float msDelta;
  bool advanceEcsWorld PULE_defaultField(false);
  bool advancePhysxWorld PULE_defaultField(false);
  PuleGpuSemaphore waitSemaphore;
} PuleSceneAdvanceInfo;
typedef struct PuleSceneComponentModelData {
  PuleSceneComponentModelType type;
} PuleSceneComponentModelData;
typedef struct PuleSceneComponentPhysicsData {
  PulePhysx3DBody body;
} PuleSceneComponentPhysicsData;
typedef struct PuleSceneNodeCreateInfo {
  PuleEcsEntity entity;
  PuleScene scene;
  PuleSceneComponentModelType modelType;
  PuleSceneComponentPhysicsData physicsData;
} PuleSceneNodeCreateInfo;

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
