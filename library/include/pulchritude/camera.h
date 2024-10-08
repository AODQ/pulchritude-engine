/* auto generated file camera */
#pragma once
#include "core.h"

#include "data-serializer.h"
#include "gpu.h"
#include "math.h"
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities
typedef struct PuleCamera { uint64_t id; } PuleCamera;
typedef struct PuleCameraSet { uint64_t id; } PuleCameraSet;
typedef struct PuleCameraController { uint64_t id; } PuleCameraController;

// structs
struct PuleCameraPerspective;
struct PuleCameraSetArray;

typedef struct PuleCameraPerspective {
  float nearCutoff;
  float farCutoff;
  float aspectRatio;
  float fieldOfViewRadians;
} PuleCameraPerspective;
typedef struct PuleCameraSetArray {
  PuleCamera * cameras;
  size_t cameraCount;
} PuleCameraSetArray;

// functions
PULE_exportFn PuleCamera puleCameraCreate();
PULE_exportFn void puleCameraDestroy(PuleCamera camera);
PULE_exportFn PuleF32m44 puleCameraView(PuleCamera camera);
PULE_exportFn PuleF32m44 puleCameraProj(PuleCamera camera);
PULE_exportFn void puleCameraLookAt(PuleCamera camera, PuleF32v3 origin, PuleF32v3 normalizedTarget, PuleF32v3 up);
PULE_exportFn PuleCameraPerspective puleCameraPerspective(PuleCamera camera);
PULE_exportFn void puleCameraPerspectiveSet(PuleCamera camera, PuleCameraPerspective perspective);
PULE_exportFn PuleCameraSet puleCameraSetCreate(PuleStringView label);
PULE_exportFn void puleCameraSetDestroy(PuleCameraSet camera);
PULE_exportFn void puleCameraSetAdd(PuleCameraSet set, PuleCamera camera);
PULE_exportFn void puleCameraSetRemove(PuleCameraSet set, PuleCamera camera);
PULE_exportFn PuleCameraSetArray puleCameraSetArray(PuleCameraSet set);
PULE_exportFn PuleGpuBuffer puleCameraSetUniformBuffer(PuleCameraSet set);
/* 
  must be on main thread in case buffer needs to be reallocated
 */
PULE_exportFn PuleGpuFence puleCameraSetRefresh(PuleCameraSet set);
PULE_exportFn PuleCameraController puleCameraControllerFirstPerson(PulePlatform platform, PuleCamera camera);
PULE_exportFn PuleCameraController puleCameraControllerOrbit(PulePlatform platform, PuleCamera camera, PuleF32v3 origin, float radius);
PULE_exportFn void puleCameraControllerDestroy(PuleCameraController controller);
PULE_exportFn void puleCameraControllerPollEvents();

#ifdef __cplusplus
} // extern C
#endif
