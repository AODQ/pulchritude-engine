#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-gfx/barrier.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-math/math.h>
#include <pulchritude-platform/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

// -- camera -------------------------------------------------------------------

typedef struct { uint64_t id; } PuleCamera;

PULE_exportFn PuleCamera puleCameraCreate();
PULE_exportFn void puleCameraDestroy(PuleCamera const);

PULE_exportFn PuleDsValue puleCameraSerialize();
PULE_exportFn PuleCamera puleCameraDeserialize(PuleDsValue const);

PULE_exportFn PuleF32m44 puleCameraView(PuleCamera const);
PULE_exportFn PuleF32m44 puleCameraProj(PuleCamera const);

PULE_exportFn void puleCameraLookAt(
  PuleCamera const,
  PuleF32v3 const origin, PuleF32v3 const normalizedTarget, PuleF32v3 const up
);

typedef struct {
  float nearCutoff;
  float farCutoff;
  float aspectRatio;
  float fieldOfViewRadians;
} PuleCameraPerspective;

PULE_exportFn PuleCameraPerspective puleCameraPerspective(PuleCamera const);
PULE_exportFn void puleCameraPerspectiveSet(
  PuleCamera const, PuleCameraPerspective const
);

/* PULE_exportFn void puleCameraTransformLockToEcsNode( */
  /* PuleCamera const, */
  /* PuleF44m4 const relativeTransform, */
  /* PuleEcsNode const node */
/* ); */
/* PULE_exportFn void puleCameraTransformLockToOriginPtr( */
  /* PuleCamera const, */
  /* PuleF32v3 * const originPtr */
/* ); */

/* PULE_exportFn void puleCameraTransformReset(PuleCamera const); */
/* PULE_exportFn void puleCameraTransformUnlock(PuleCamera const); */

// -- camera set ---------------------------------------------------------------

typedef struct { uint64_t id; } PuleCameraSet;

PULE_exportFn PuleCameraSet puleCameraSetCreate(PuleStringView const label);
PULE_exportFn void puleCameraSetDestroy(PuleCameraSet const);

PULE_exportFn void puleCameraSetAdd(PuleCameraSet const, PuleCamera const);
PULE_exportFn void puleCameraSetRemove(PuleCameraSet const, PuleCamera const);

typedef struct {
  PuleCamera * cameras;
  size_t cameraCount;
} PuleCameraSetArray;
PULE_exportFn PuleCameraSetArray puleCameraSetArray(PuleCameraSet const);

PULE_exportFn PuleGfxGpuBuffer puleCameraSetGfxUniformBuffer(
  PuleCameraSet const
);
// must be on main thread in case buffer needs to be reallocated
PULE_exportFn PuleGfxFence puleCameraSetRefresh(PuleCameraSet const);

// -- camera controller --------------------------------------------------------

typedef struct { uint64_t id; } PuleCameraController;
PULE_exportFn PuleCameraController puleCameraControllerCreate(
  void (*update)(void * const userdata),
  void const * const controllerData,
  size_t controllerDataSize
);
PULE_exportFn PuleCameraController puleCameraControllerFirstPerson(
  PulePlatform const platform,
  PuleCamera const
);
PULE_exportFn void puleCameraControllerDestroy(PuleCameraController const);

PULE_exportFn void puleCameraControllerPollEvents();

#ifdef __cplusplus
} // extern C
#endif
