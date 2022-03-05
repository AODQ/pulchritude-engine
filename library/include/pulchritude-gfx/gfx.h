#pragma once

// These bindings are implementable in all platforms/renderers (OpenGL, GLES,
//   Vulkan, DX9, Metal, etc)
//
// Right now only OpenGL4.6 renderer exists. In the future, an OpenGL 3.0
//   renderer will also exist (to target more machines). At some point if this
//   library does well a vulkan backend will also be written.
//
// The intent for 3D games that require more hardware capability is to 'extend'
//   the gfx to support more features, though this is the one exception where
//   there might be incompatilibies between libraries. It's best to try to
//   avoid this, of course. For example, enums could be extended, new functions
//   can be added, and most resources are just 'id' so that the underlying
//   implementation can change.

#include <pulchritude-gfx/shader-module.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorGfx_none,
  PuleErrorGfx_creationFailed,
  PuleErrorGfx_shaderModuleCompilationFailed,
  PuleErrorGfx_invalidDescriptorSet,
} PuleErrorGfx;

typedef struct {
  void * implementation;
} PuleGfxContext;

typedef struct {
  uint64_t id;
} PuleGfxGpuImage;

/* typedef enum { */
/* } PuleGfxImageByteFormat; */

/* typedef enum { */
/* } PuleGfxImageChannel; */

typedef struct {
  uint64_t id;
} PuleGfxGpuFramebuffer;

typedef struct {
  uint64_t id;
} PuleGfxGpuBuffer;

// this should be a bitfield in the future (this only tells us what the buffer
//   may be bound to for use in the future);
typedef enum {
  PuleGfxGpuBufferUsage_bufferAttribute,
  PuleGfxGpuBufferUsage_bufferElement,
  PuleGfxGpuBufferUsage_bufferUniform,
  PuleGfxGpuBufferUsage_bufferStorage, // unsupported GL3.3
  PuleGfxGpuBufferUsage_bufferAccelerationStructure, // unsupported GL3.3
  PuleGfxGpuBufferUsage_bufferIndirect, // unsupported GL3.3
} PuleGfxGpuBufferUsage;

typedef enum {
  PuleGfxGpuBufferVisibilityFlag_deviceOnly = 0x1, // incompatible with rest
  PuleGfxGpuBufferVisibilityFlag_hostVisible = 0x2,
  PuleGfxGpuBufferVisibilityFlag_hostWritable = 0x4,
} PuleGfxGpuBufferVisibilityFlag;

PULE_exportFn PuleGfxGpuBuffer puleGfxGpuBufferCreate(
  void * const nullableInitialData,
  size_t const byteLength, // must be >0
  PuleGfxGpuBufferUsage const usage,
  PuleGfxGpuBufferVisibilityFlag const visibility
);
PULE_exportFn void puleGfxGpuBufferDestroy(PuleGfxGpuBuffer const buffer);

typedef enum {
  PuleGfxDescriptorType_sampler,
  PuleGfxDescriptorType_uniformBuffer,
  PuleGfxDescriptorType_storageBuffer, // not supported
} PuleGfxDescriptorType;

typedef enum {
  PuleGfxAttributeDataType_float,
} PuleGfxAttributeDataType;

typedef struct {
  PuleGfxGpuBuffer buffer;
  size_t numComponents;
  PuleGfxAttributeDataType dataType;
  bool normalizeFixedDataTypeToNormalizedFloating;
  size_t stridePerElement; // zero implies strictly packed
  size_t offsetIntoBuffer;
} PuleGfxPipelineDescriptorAttributeBinding;

// here are some known & fixable limitations with the current model:
//   - can't reference the same buffer in separate elements
//   - can't reference relative offset strides (related to above)
//   - of course, maximum 16 attributes (I think this is fine though)
typedef struct {
  PuleGfxGpuBuffer bufferUniformBindings[16];
  PuleGfxPipelineDescriptorAttributeBinding bufferAttributeBindings[16];
  PuleGfxGpuBuffer bufferElementBinding;
} PuleGfxPipelineDescriptorSetLayout;

PULE_exportFn PuleGfxPipelineDescriptorSetLayout
  puleGfxPipelineDescriptorSetLayout();

typedef struct {
  uint64_t id;
} PuleGfxPipelineLayout;

PULE_exportFn PuleGfxPipelineLayout puleGfxPipelineLayoutCreate(
  PuleGfxPipelineDescriptorSetLayout const * const descriptorSetLayout,
  PuleError * const error
);
PULE_exportFn void puleGfxPipelineLayoutDestroy(
  PuleGfxPipelineLayout const pipelineLayout
);

typedef struct {
  PuleGfxShaderModule shaderModule;
  PuleGfxPipelineLayout layout;
} PuleGfxPipeline;

typedef struct {
  size_t index;
  void * data;
} PuleGfxPushConstant;

PULE_exportFn void puleGfxInitialize(PuleError * const error);
PULE_exportFn void puleGfxShutdown();

PULE_exportFn void puleGfxFrameStart();
PULE_exportFn void puleGfxFrameEnd();

PULE_exportFn void puleGfxGpuImageCreate(
  uint32_t const width,
  uint32_t const height,
  /* PuleGfxImageByteFormat const byteFormat, */
  /* PuleGfxImageChannel const imageChannel, */
  void const * const data
);

#ifdef __cplusplus
} // extern C
#endif
