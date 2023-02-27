#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-gfx/commands.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-gfx/pipeline.h>
#include <pulchritude-math/math.h>

// Most basic rendering primitive, which can be represented in a single
// draw call under even the most basic rendering procedure
// More sophisticated renderers will take the mesh and batch them

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleGfxMeshAttribute_origin,
  PuleGfxMeshAttribute_uv0, PuleGfxMeshAttribute_uv1,
  PuleGfxMeshAttribute_uv2, PuleGfxMeshAttribute_uv3,
  PuleGfxMeshAttribute_normal,
  PuleGfxMeshAttribute_tangent,
  PuleGfxMeshAttribute_color,
  PuleGfxMeshAttribute_joint0, PuleGfxMeshAttribute_joint1,
  PuleGfxMeshAttribute_joint2, PuleGfxMeshAttribute_joint3,
  PuleGfxMeshAttribute_weight0, PuleGfxMeshAttribute_weight1,
  PuleGfxMeshAttribute_weight2, PuleGfxMeshAttribute_weight3,
  PuleGfxMeshAttributeSize,
} PuleGfxMeshAttribute;

typedef enum {
  // the attribute won't be used/stored
  PuleGfxMeshVertexAttributeStorage_none = 0x0,

  // stores attributes on GPU for rendering,
  // .gpuOnly is incompatible with other GPUs,
  // but all GPU operations compatible with CPU
  PuleGfxMeshVertexAttributeStorage_gpuOnly = 0x1,
  PuleGfxMeshVertexAttributeStorage_gpuWrite = 0x2,
  PuleGfxMeshVertexAttributeStorage_gpuRead = 0x4,

  // makes a copy on CPU (for operations like raycasts)
  PuleGfxMeshVertexAttributeStorage_cpuCopy = 0x8,
} PuleGfxMeshVertexAttributeStorage;

typedef struct {
  PuleGfxGpuBuffer buffer;
  size_t byteOffset;
  size_t byteStride;
  void * mappedMemory; // need to use stride
  void * cpuMemory; // doesn't use stride
  PuleGfxMeshVertexAttributeStorage storage;
} PuleGfxMeshAttributeMemory;

typedef struct {
  PuleGfxMeshAttributeMemory attributes[PuleGfxMeshAttributeSize];
  PuleGfxMeshAttributeMemory indices;
  PuleGfxGpuImage textures[4];
  PuleGfxPipeline pipeline;
} PuleGfxMesh;

PULE_exportFn void puleGfxMeshRender(
  PuleGfxMesh const * const mesh,
  PuleGfxGpuBuffer const cameraUniformBuffer,
  PuleGfxFramebuffer const targetFramebuffer,
  PuleGfxCommandListRecorder const recorder
);

// * TODO * generate descriptor set from mesh

#ifdef __cplusplus
} // extern C
#endif
