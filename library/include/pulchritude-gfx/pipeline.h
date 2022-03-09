#pragma once

#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-gfx/shader-module.h>

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
  PuleGfxFramebuffer framebuffer;
  PuleGfxPipelineLayout layout;
} PuleGfxPipeline;

typedef struct {
  size_t index;
  void * data;
} PuleGfxPushConstant;

#ifdef __cplusplus
} // C
#endif
