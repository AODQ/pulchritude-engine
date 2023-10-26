#pragma once

#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-gpu/shader-module.h>

#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleGpuPipelineStage_top = 0x01,
  PuleGpuPipelineStage_drawIndirect = 0x02,
  PuleGpuPipelineStage_vertexInput = 0x04,
  PuleGpuPipelineStage_vertexShader = 0x08,
  PuleGpuPipelineStage_fragmentShader = 0x10,
  PuleGpuPipelineStage_colorAttachmentOutput = 0x20,
  PuleGpuPipelineStage_computeShader = 0x40,
  PuleGpuPipelineStage_transfer = 0x80,
  PuleGpuPipelineStage_bottom = 0x100,
} PuleGpuPipelineStage;

typedef enum {
  PuleGpuDescriptorType_sampler,
  PuleGpuDescriptorType_uniformBuffer,
  PuleGpuDescriptorType_storageBuffer, // not supported
} PuleGpuDescriptorType;

typedef enum {
  PuleGpuAttributeDataType_float,
  PuleGpuAttributeDataType_unsignedByte,
} PuleGpuAttributeDataType;

typedef struct {
  PuleGpuAttributeDataType dataType;
  size_t bufferIndex;
  size_t numComponents;
  bool convertFixedDataTypeToNormalizedFloating;
  size_t offsetIntoBuffer;
} PuleGpuPipelineAttributeDescriptorBinding;

typedef struct {
  size_t stridePerElement; // must be non-zero
  // TODO input rate
} PuleGpuPipelineAttributeBufferDescriptorBinding;


typedef enum {
  puleGpuPipelineDescriptorMax_uniform = 16,
  puleGpuPipelineDescriptorMax_storage = 16,
  puleGpuPipelineDescriptorMax_attribute = 16,
  puleGpuPipelineDescriptorMax_texture = 8,
} PuleGpuPipelineDescriptorMax;

typedef enum {
  PuleGpuDescriptorStage_unused = 0x0,
  PuleGpuDescriptorStage_vertex = 0x1,
  PuleGpuDescriptorStage_fragment = 0x2,
} PuleGpuDescriptorStage;

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
  PuleGpuDescriptorStage bufferUniformBindings[puleGpuPipelineDescriptorMax_uniform];
  // TODO use range
  PuleGpuDescriptorStage bufferStorageBindings[puleGpuPipelineDescriptorMax_storage];
  PuleGpuPipelineAttributeDescriptorBinding attributeBindings[puleGpuPipelineDescriptorMax_attribute];
  PuleGpuPipelineAttributeBufferDescriptorBinding attributeBufferBindings[puleGpuPipelineDescriptorMax_attribute];
  // TODO can store metadata but not texture
  PuleGpuDescriptorStage textureBindings[puleGpuPipelineDescriptorMax_texture];
  // TODO I guess pair with an optional sampler?
} PuleGpuPipelineLayoutDescriptorSet;

PULE_exportFn PuleGpuPipelineLayoutDescriptorSet
  puleGpuPipelineDescriptorSetLayout();

typedef struct {
  PuleGpuDescriptorStage stage;
  size_t byteLength;
  size_t byteOffset;
} PuleGpuPipelineLayoutPushConstant;

typedef enum {
  PuleGpuDrawPrimitive_triangle,
  PuleGpuDrawPrimitive_triangleStrip,
  PuleGpuDrawPrimitive_point,
  PuleGpuDrawPrimitive_line,
} PuleGpuDrawPrimitive;

typedef struct {
  bool depthTestEnabled;
  bool blendEnabled;
  bool scissorTestEnabled;
  // TODO viewport/scissor like this shouldn't be in a viewport
  PuleI32v2 viewportMin; // default 0, 0
  PuleI32v2 viewportMax; // default 1, 1
  PuleI32v2 scissorMin; // default 0, 0
  PuleI32v2 scissorMax; // default 1, 1
  PuleGpuDrawPrimitive drawPrimitive;
} PuleGpuPipelineConfig;

typedef struct {
  PuleGpuShaderModule shaderModule;
  PuleGpuPipelineLayoutDescriptorSet const * layoutDescriptorSet;
  PuleGpuPipelineLayoutPushConstant const * layoutPushConstants;
  size_t layoutPushConstantsCount;
  PuleGpuPipelineConfig config;
} PuleGpuPipelineCreateInfo;

typedef struct {
  uint64_t id;
} PuleGpuPipeline;

PULE_exportFn PuleGpuPipeline puleGpuPipelineCreate(
  PuleGpuPipelineCreateInfo const * const createInfo,
  PuleError * const error
);

PULE_exportFn void puleGpuPipelineDestroy(PuleGpuPipeline const pipeline);

#ifdef __cplusplus
} // C
#endif
