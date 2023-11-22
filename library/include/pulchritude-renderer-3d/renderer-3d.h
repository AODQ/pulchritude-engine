#pragma once

#include <pulchritude-core/core.h>

// 3D renderer api on top of pulegpu

#include <pulchritude-gpu/module.h>
#include <pulchritude-ecs/ecs.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-asset/model.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleRenderer3DMaterial;

PULE_exportFn PuleRenderer3DMaterial puleRenderer3DMaterialCreateDefault();
PULE_exportFn void puleRenderer3DMaterialDestroy(
  PuleRenderer3DMaterial const material
);

typedef struct { uint64_t id; } PuleRenderer3DModel;

typedef struct {
  PuleRenderer3DMaterial material;
  PuleAssetMesh const * meshes;
  size_t meshCount;
  PuleBufferView const * buffers;
  size_t bufferCount;
  PuleStringView label;
} PuleRenderer3DModelCreateInfo;
PULE_exportFn PuleRenderer3DModel puleRenderer3DModelCreate(
  PuleRenderer3DModelCreateInfo const createInfo
);
PULE_exportFn void puleRenderer3DDestroyModel(PuleRenderer3DModel const model);

// naive render
PULE_exportFn void puleRenderer3DModelRender(
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleRenderer3DModel const model,
  PuleF32m44 const transform
);

// TODO instances

#ifdef __cplusplus
} // C
#endif
