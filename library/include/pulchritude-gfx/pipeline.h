#pragma once

#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-gfx/shader-module.h>

#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleGfxDescriptorType_sampler,
  PuleGfxDescriptorType_uniformBuffer,
  PuleGfxDescriptorType_storageBuffer, // not supported
} PuleGfxDescriptorType;

typedef enum {
  PuleGfxAttributeDataType_float,
  PuleGfxAttributeDataType_unsignedByte,
} PuleGfxAttributeDataType;

typedef struct {
  PuleGfxGpuBuffer buffer;
  size_t numComponents;
  PuleGfxAttributeDataType dataType;
  bool normalizeFixedDataTypeToNormalizedFloating;
  size_t stridePerElement; // zero implies same element; does not pack for you
  size_t offsetIntoBuffer;
} PuleGfxPipelineAttributeDescriptorBinding;

// here are some known & fixable limitations with the current model:
//   - can't reference the same buffer in separate elements
//   - can't reference relative offset strides (related to above)
//   - of course, maximum 16 attributes (I think this is fine though)
typedef struct {
  PuleGfxGpuBuffer bufferUniformBindings[16];
  PuleGfxGpuBuffer bufferStorageBindings[16]; // TODO use range
  PuleGfxPipelineAttributeDescriptorBinding bufferAttributeBindings[16];
  PuleGfxGpuImage textureBindings[8];
  // TODO I guess pair with an optional sampler?
  PuleGfxGpuBuffer bufferElementBinding;
} PuleGfxPipelineDescriptorSetLayout;

PULE_exportFn PuleGfxPipelineDescriptorSetLayout
  puleGfxPipelineDescriptorSetLayout();

typedef struct {
  bool blendEnabled;
  bool scissorTestEnabled;
  PuleI32v2 viewportUl; // default 0, 0
  PuleI32v2 viewportLr; // default 1, 1
  PuleI32v2 scissorUl; // default 0, 0
  PuleI32v2 scissorLr; // default 1, 1
} PuleGfxPipelineConfig;

typedef struct {
  PuleGfxShaderModule shaderModule;
  PuleGfxFramebuffer framebuffer;
  PuleGfxPipelineDescriptorSetLayout const * layout;
  PuleGfxPipelineConfig config;
} PuleGfxPipelineCreateInfo;

typedef struct {
  uint64_t id;
} PuleGfxPipeline;

PULE_exportFn PuleGfxPipeline puleGfxPipelineCreate(
  PuleGfxPipelineCreateInfo const * const createInfo,
  PuleError * const error
);
PULE_exportFn void puleGfxPipelineUpdate(
  PuleGfxPipeline const pipeline,
  PuleGfxPipelineCreateInfo  const * const updateInfo,
  PuleError * const error
);
PULE_exportFn void puleGfxPipelineDestroy(PuleGfxPipeline const pipeline);

typedef struct {
  size_t index;
  void * data;
} PuleGfxPushConstant;

#ifdef __cplusplus
} // C
#endif
