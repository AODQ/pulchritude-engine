#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

// 3D physics simulator api
// default implementation uses fixed-point deterministic physics

typedef struct {
  uint64_t id;
} PulePhysx3DWorld;

PULE_exportFn PulePhysx3DWorld pulePhysx3DWorldCreate();
PULE_exportFn void pulePhysx3DWorldDestroy(PulePhysx3DWorld const world);

PULE_exportFn void pulePhysx3DWorldAdvance(
  PulePhysx3DWorld const world, float const msDelta
);

#ifdef __cplusplus
}
#endif
