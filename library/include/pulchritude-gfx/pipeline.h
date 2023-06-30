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
  PuleGfxAttributeDataType dataType;
  size_t bufferIndex;
  size_t numComponents;
  bool convertFixedDataTypeToNormalizedFloating;
  size_t offsetIntoBuffer;
} PuleGfxPipelineAttributeDescriptorBinding;

typedef struct {
  size_t stridePerElement; // must be non-zero
  // TODO input rate
} PuleGfxPipelineAttributeBufferDescriptorBinding;


typedef enum {
  puleGfxPipelineDescriptorMax_uniform = 16,
  puleGfxPipelineDescriptorMax_storage = 16,
  puleGfxPipelineDescriptorMax_attribute = 16,
  puleGfxPipelineDescriptorMax_texture = 8,
} PuleGfxPipelineDescriptorMax;

typedef enum {
  PuleGfxDescriptorStage_unused = 0x0,
  PuleGfxDescriptorStage_vertex = 0x1,
  PuleGfxDescriptorStage_fragment = 0x2,
} PuleGfxDescriptorStage;

// here are some known & fixable limitations with the current model:
//   - can't reference the same buffer in separate elements
//   - can't reference relative offset strides (related to above)
//   - of course, maximum 16 attributes (I think this is fine though)
typedef struct {
  // TODO change 'uniform' to just 'smallStorage' in this context
  // !!!!!!!!!!!!!!
  // TODO.... DONT BIND BUFFERS HERE!!!!!!!
  // Need to just bind what stages they will be used!!!
  // !!!!!!!!!!!!!!
  PuleGfxDescriptorStage bufferUniformBindings[puleGfxPipelineDescriptorMax_uniform];
  // TODO use range
  PuleGfxDescriptorStage bufferStorageBindings[puleGfxPipelineDescriptorMax_storage];
  PuleGfxPipelineAttributeDescriptorBinding attributeBindings[puleGfxPipelineDescriptorMax_attribute];
  PuleGfxPipelineAttributeBufferDescriptorBinding attributeBufferBindings[puleGfxPipelineDescriptorMax_attribute];
  // TODO can store metadata but not texture
  PuleGfxDescriptorStage textureBindings[puleGfxPipelineDescriptorMax_texture];
  // TODO I guess pair with an optional sampler?
} PuleGfxPipelineDescriptorSetLayout;

PULE_exportFn PuleGfxPipelineDescriptorSetLayout
  puleGfxPipelineDescriptorSetLayout();

typedef struct {
  bool depthTestEnabled;
  bool blendEnabled;
  bool scissorTestEnabled;
  // TODO viewport/scissor like this shouldn't be in a viewport
  PuleI32v2 viewportUl; // default 0, 0
  PuleI32v2 viewportLr; // default 1, 1
  PuleI32v2 scissorUl; // default 0, 0
  PuleI32v2 scissorLr; // default 1, 1
} PuleGfxPipelineConfig;

typedef struct {
  PuleGfxShaderModule shaderModule;
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
