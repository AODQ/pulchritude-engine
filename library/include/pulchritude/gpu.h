/* auto generated file gpu */
#pragma once
#include "core.h"

#include "platform.h"
#include "error.h"
#include "core.h"
#include "math.h"
#include "array.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleGpuElementType_u8 = 0,
  PuleGpuElementType_u16 = 1,
  PuleGpuElementType_u32 = 2,
} PuleGpuElementType;
const uint32_t PuleGpuElementTypeSize = 3;
/* 
   some amount of action independent of another, such that one complete amount
     of work can be made between the GPU and CPU. Such as blits, allocations,
     initiating a render, etc
   commands are not unique, and as such are treated as primitives and may not
     refer to any specific unique index of a queue
   TODO just rename to GpuCommand_
 */
typedef enum {
  PuleGpuAction_bindPipeline = 0,
  PuleGpuAction_bindBuffer = 1,
  PuleGpuAction_bindTexture = 2,
  PuleGpuAction_resourceBarrier = 3,
  PuleGpuAction_renderPassBegin = 4,
  PuleGpuAction_renderPassEnd = 5,
  PuleGpuAction_bindElementBuffer = 6,
  PuleGpuAction_bindAttributeBuffer = 7,
  PuleGpuAction_bindFramebuffer = 8,
  PuleGpuAction_clearImageColor = 9,
  PuleGpuAction_clearImageDepth = 10,
  PuleGpuAction_dispatchRender = 11,
  PuleGpuAction_dispatchRenderElements = 12,
  PuleGpuAction_dispatchRenderIndirect = 13,
  PuleGpuAction_pushConstants = 14,
  PuleGpuAction_dispatchCommandList = 15,
  PuleGpuAction_setScissor = 16,
  PuleGpuAction_copyImageToImage = 17,
} PuleGpuAction;
const uint32_t PuleGpuActionSize = 18;
typedef enum {
  PuleGpuResourceBarrierStage_top = 1,
  PuleGpuResourceBarrierStage_drawIndirect = 2,
  PuleGpuResourceBarrierStage_vertexInput = 4,
  PuleGpuResourceBarrierStage_shaderFragment = 8,
  PuleGpuResourceBarrierStage_shaderVertex = 16,
  PuleGpuResourceBarrierStage_shaderCompute = 32,
  PuleGpuResourceBarrierStage_outputAttachmentColor = 64,
  PuleGpuResourceBarrierStage_outputAttachmentDepth = 128,
  PuleGpuResourceBarrierStage_transfer = 256,
  PuleGpuResourceBarrierStage_bottom = 512,
} PuleGpuResourceBarrierStage;
const uint32_t PuleGpuResourceBarrierStageSize = 10;
typedef enum {
  PuleGpuResourceAccess_none = 0,
  PuleGpuResourceAccess_indirectCommandRead = 1,
  PuleGpuResourceAccess_indexRead = 2,
  PuleGpuResourceAccess_vertexAttributeRead = 4,
  PuleGpuResourceAccess_uniformRead = 8,
  PuleGpuResourceAccess_inputAttachmentRead = 16,
  PuleGpuResourceAccess_shaderRead = 32,
  PuleGpuResourceAccess_shaderWrite = 64,
  PuleGpuResourceAccess_attachmentColorRead = 128,
  PuleGpuResourceAccess_attachmentColorWrite = 256,
  PuleGpuResourceAccess_attachmentDepthRead = 512,
  PuleGpuResourceAccess_attachmentDepthWrite = 1024,
  PuleGpuResourceAccess_transferRead = 2048,
  PuleGpuResourceAccess_transferWrite = 4096,
  PuleGpuResourceAccess_hostRead = 8192,
  PuleGpuResourceAccess_hostWrite = 16384,
  PuleGpuResourceAccess_memoryRead = 32768,
  PuleGpuResourceAccess_memoryWrite = 65536,
} PuleGpuResourceAccess;
const uint32_t PuleGpuResourceAccessSize = 18;
/*  TODO deprecate this in favor of shader introspection  */
typedef enum {
  PuleGpuConstantTypeTag_f32 = 0,
  PuleGpuConstantTypeTag_f32v2 = 1,
  PuleGpuConstantTypeTag_f32v3 = 2,
  PuleGpuConstantTypeTag_f32v4 = 3,
  PuleGpuConstantTypeTag_i32 = 4,
  PuleGpuConstantTypeTag_i32v2 = 5,
  PuleGpuConstantTypeTag_i32v3 = 6,
  PuleGpuConstantTypeTag_i32v4 = 7,
  PuleGpuConstantTypeTag_f32m44 = 8,
} PuleGpuConstantTypeTag;
const uint32_t PuleGpuConstantTypeTagSize = 9;
typedef enum {
  PuleErrorGfx_none = 0,
  PuleErrorGfx_creationFailed = 1,
  PuleErrorGfx_shaderModuleCompilationFailed = 2,
  PuleErrorGfx_invalidDescriptorSet = 3,
  PuleErrorGfx_invalidCommandList = 4,
  PuleErrorGfx_invalidFramebuffer = 5,
  PuleErrorGfx_submissionFenceWaitFailed = 6,
} PuleErrorGfx;
const uint32_t PuleErrorGfxSize = 7;
typedef enum {
  PuleGpuBufferUsage_attribute = 1,
  PuleGpuBufferUsage_element = 2,
  PuleGpuBufferUsage_uniform = 4,
  PuleGpuBufferUsage_storage = 8,
  PuleGpuBufferUsage_accelerationStructure = 16,
  PuleGpuBufferUsage_indirect = 32,
} PuleGpuBufferUsage;
const uint32_t PuleGpuBufferUsageSize = 6;
typedef enum {
  PuleGpuBufferBindingDescriptor_uniform = 0,
  PuleGpuBufferBindingDescriptor_storage = 1,
  PuleGpuBufferBindingDescriptor_accelerationStructure = 2,
} PuleGpuBufferBindingDescriptor;
const uint32_t PuleGpuBufferBindingDescriptorSize = 3;
typedef enum {
  /*  incompatible with rest  */
  PuleGpuBufferVisibilityFlag_deviceOnly = 1,
  PuleGpuBufferVisibilityFlag_hostVisible = 2,
  PuleGpuBufferVisibilityFlag_hostWritable = 4,
} PuleGpuBufferVisibilityFlag;
const uint32_t PuleGpuBufferVisibilityFlagSize = 3;
typedef enum {
  PuleGpuBufferMapAccess_hostVisible = 1,
  PuleGpuBufferMapAccess_hostWritable = 2,
  PuleGpuBufferMapAccess_invalidate = 4,
} PuleGpuBufferMapAccess;
const uint32_t PuleGpuBufferMapAccessSize = 3;
typedef enum {
  PuleGpuImageMagnification_nearest = 0,
} PuleGpuImageMagnification;
const uint32_t PuleGpuImageMagnificationSize = 1;
typedef enum {
  PuleGpuImageWrap_clampToEdge = 0,
} PuleGpuImageWrap;
const uint32_t PuleGpuImageWrapSize = 1;
typedef enum {
  PuleGpuImageByteFormat_undefined = 0,
  PuleGpuImageByteFormat_bgra8U = 1,
  PuleGpuImageByteFormat_rgba8U = 2,
  PuleGpuImageByteFormat_rgb8U = 3,
  PuleGpuImageByteFormat_r8U = 4,
  PuleGpuImageByteFormat_depth16 = 5,
} PuleGpuImageByteFormat;
const uint32_t PuleGpuImageByteFormatSize = 6;
typedef enum {
  PuleGpuImageTarget_i2D = 0,
} PuleGpuImageTarget;
const uint32_t PuleGpuImageTargetSize = 1;
typedef enum {
  PuleGpuFramebufferAttachment_color0 = 0,
  PuleGpuFramebufferAttachment_color1 = 1,
  PuleGpuFramebufferAttachment_color3 = 2,
  PuleGpuFramebufferAttachment_color4 = 3,
  PuleGpuFramebufferAttachment_depth = 4,
  PuleGpuFramebufferAttachment_stencil = 5,
  PuleGpuFramebufferAttachment_depthStencil = 6,
} PuleGpuFramebufferAttachment;
const uint32_t PuleGpuFramebufferAttachmentSize = 7;
typedef enum {
  PuleGpuFramebufferType_renderStorage = 0,
  PuleGpuFramebufferType_imageStorage = 1,
} PuleGpuFramebufferType;
const uint32_t PuleGpuFramebufferTypeSize = 2;
typedef enum {
  PuleGpuImageAttachmentOpLoad_load = 0,
  PuleGpuImageAttachmentOpLoad_clear = 1,
  PuleGpuImageAttachmentOpLoad_dontCare = 2,
} PuleGpuImageAttachmentOpLoad;
const uint32_t PuleGpuImageAttachmentOpLoadSize = 3;
typedef enum {
  PuleGpuImageAttachmentOpStore_store = 0,
  PuleGpuImageAttachmentOpStore_dontCare = 1,
} PuleGpuImageAttachmentOpStore;
const uint32_t PuleGpuImageAttachmentOpStoreSize = 2;
typedef enum {
  PuleGpuImageLayout_uninitialized = 0,
  PuleGpuImageLayout_storage = 1,
  PuleGpuImageLayout_attachmentColor = 2,
  PuleGpuImageLayout_attachmentDepth = 3,
  PuleGpuImageLayout_transferSrc = 4,
  PuleGpuImageLayout_transferDst = 5,
  PuleGpuImageLayout_presentSrc = 6,
} PuleGpuImageLayout;
const uint32_t PuleGpuImageLayoutSize = 7;
typedef enum {
  PuleGpuPipelineStage_top = 1,
  PuleGpuPipelineStage_drawIndirect = 2,
  PuleGpuPipelineStage_vertexInput = 4,
  PuleGpuPipelineStage_vertexShader = 8,
  PuleGpuPipelineStage_fragmentShader = 16,
  PuleGpuPipelineStage_colorAttachmentOutput = 32,
  PuleGpuPipelineStage_computeShader = 64,
  PuleGpuPipelineStage_transfer = 128,
  PuleGpuPipelineStage_bottom = 256,
} PuleGpuPipelineStage;
const uint32_t PuleGpuPipelineStageSize = 9;
typedef enum {
  PuleGpuDescriptorType_sampler = 0,
  PuleGpuDescriptorType_uniformBuffer = 1,
  /*  not supported  */
  PuleGpuDescriptorType_storageBuffer = 2,
} PuleGpuDescriptorType;
const uint32_t PuleGpuDescriptorTypeSize = 3;
typedef enum {
  PuleGpuAttributeDataType_f32 = 0,
  PuleGpuAttributeDataType_u8 = 1,
} PuleGpuAttributeDataType;
const uint32_t PuleGpuAttributeDataTypeSize = 2;
typedef enum {
  PuleGpuPipelineDescriptorMax_uniform = 16,
  PuleGpuPipelineDescriptorMax_storage = 16,
  PuleGpuPipelineDescriptorMax_attribute = 16,
  PuleGpuPipelineDescriptorMax_texture = 8,
} PuleGpuPipelineDescriptorMax;
const uint32_t PuleGpuPipelineDescriptorMaxSize = 4;
typedef enum {
  PuleGpuDescriptorStage_unused = 0,
  PuleGpuDescriptorStage_vertex = 1,
  PuleGpuDescriptorStage_fragment = 2,
} PuleGpuDescriptorStage;
const uint32_t PuleGpuDescriptorStageSize = 3;
typedef enum {
  PuleGpuDrawPrimitive_triangle = 0,
  PuleGpuDrawPrimitive_triangleStrip = 1,
  PuleGpuDrawPrimitive_point = 2,
  PuleGpuDrawPrimitive_line = 3,
} PuleGpuDrawPrimitive;
const uint32_t PuleGpuDrawPrimitiveSize = 4;
typedef enum {
  PuleGpuFenceConditionFlag_all = 0,
} PuleGpuFenceConditionFlag;
const uint32_t PuleGpuFenceConditionFlagSize = 1;
typedef enum {
  PuleGpuSignalTime_forever = 2000000000,
} PuleGpuSignalTime;
const uint32_t PuleGpuSignalTimeSize = 1;

// entities
typedef struct PuleGpuCommandList { uint64_t id; } PuleGpuCommandList;
typedef struct PuleGpuCommandListRecorder { uint64_t id; } PuleGpuCommandListRecorder;
/* 
  utility to allow recording to a command list while it is being submitted,
  which occurs if double or triple buffering for present is enabled It's an
  implementation detail if command lists are reused or destroyed
 */
typedef struct PuleGpuCommandListChain { uint64_t id; } PuleGpuCommandListChain;
typedef struct PuleGpuBuffer { uint64_t id; } PuleGpuBuffer;
typedef struct PuleGpuSampler { uint64_t id; } PuleGpuSampler;
typedef struct PuleGpuImage { uint64_t id; } PuleGpuImage;
typedef struct PuleGpuRenderStorage { uint64_t id; } PuleGpuRenderStorage;
typedef struct PuleGpuPipeline { uint64_t id; } PuleGpuPipeline;
typedef struct PuleGpuImageChain { uint64_t id; } PuleGpuImageChain;
/* 
   allows you to reference images, even if they're behind a chain, and track
   their lifetimes (e.g. if they get reconstructed)
 */
typedef struct PuleGpuImageReference { uint64_t id; } PuleGpuImageReference;
typedef struct PuleGpuShaderModule { uint64_t id; } PuleGpuShaderModule;
typedef struct PuleGpuSemaphore { uint64_t id; } PuleGpuSemaphore;
typedef struct PuleGpuFence { uint64_t id; } PuleGpuFence;

// structs
struct PuleGpuCommandListSubmitInfo;
struct PuleGpuImageView;
struct PuleGpuResourceBarrierImage;
struct PuleGpuResourceBarrierBuffer;
union PuleGpuConstantValue;
struct PuleGpuConstant;
struct PuleGpuDrawIndirectArrays;
struct PuleGpuBufferMapRange;
struct PuleGpuBufferMappedFlushRange;
struct PuleGpuSamplerCreateInfo;
struct PuleGpuImageCreateInfo;
struct PuleGpuFramebuffer;
struct PuleGpuFramebufferImageAttachment;
union PuleGpuFramebufferAttachments;
struct PuleGpuFramebufferCreateInfo;
union PuleGpuImageAttachmentClear;
struct PuleGpuImageAttachment;
struct PuleGpuPipelineAttributeDescriptorBinding;
struct PuleGpuPipelineAttributeBufferDescriptorBinding;
struct PuleGpuPipelineLayoutDescriptorSet;
struct PuleGpuPipelineLayoutPushConstant;
struct PuleGpuPipelineConfig;
struct PuleGpuPipelineCreateInfo;
struct PuleGpuActionResourceBarrier;
struct PuleGpuActionRenderPassBegin;
struct PuleGpuActionRenderPassEnd;
struct PuleGpuActionBindElementBuffer;
struct PuleGpuActionBindAttributeBuffer;
struct PuleGpuActionBindFramebuffer;
struct PuleGpuActionDispatchRender;
struct PuleGpuActionDispatchRenderIndirect;
struct PuleGpuActionDispatchRenderElements;
struct PuleGpuActionClearImageColor;
struct PuleGpuActionBindPipeline;
struct PuleGpuActionBindBuffer;
struct PuleGpuActionBindTexture;
struct PuleGpuActionClearImageDepth;
struct PuleGpuActionPushConstants;
struct PuleGpuActionDispatchCommandList;
struct PuleGpuActionSetScissor;
struct PuleGpuActionCopyImageToImage;
union PuleGpuCommand;

typedef struct PuleGpuCommandListSubmitInfo {
  PuleGpuCommandList commandList;
  size_t waitSemaphoreCount;
  PuleGpuSemaphore const * waitSemaphores;
  PuleGpuPipelineStage const * waitSemaphoreStages;
  /* 
    will create semaphore if id is 0, otherwise will use existing semaphore
   */
  size_t signalSemaphoreCount;
  PuleGpuSemaphore * signalSemaphores;
  /* 
    at end of command list submission, if this is non-zero, fence will be
    triggered
   */
  PuleGpuFence fenceTargetFinish;
} PuleGpuCommandListSubmitInfo;
typedef struct PuleGpuImageView {
  PuleGpuImage image;
  size_t mipmapLevelStart;
  size_t mipmapLevelCount;
  size_t arrayLayerStart;
  size_t arrayLayerCount;
  PuleGpuImageByteFormat byteFormat;
} PuleGpuImageView;
typedef struct PuleGpuResourceBarrierImage {
  PuleGpuImage image;
  PuleGpuResourceAccess accessSrc;
  PuleGpuResourceAccess accessDst;
  PuleGpuImageLayout layoutSrc;
  PuleGpuImageLayout layoutDst;
  bool isDepthStencil;
} PuleGpuResourceBarrierImage;
typedef struct PuleGpuResourceBarrierBuffer {
  PuleGpuBuffer buffer;
  PuleGpuResourceAccess accessSrc;
  PuleGpuResourceAccess accessDst;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuResourceBarrierBuffer;
/*  TODO deprecate this in favor of shader introspection  */
typedef union PuleGpuConstantValue {
  float constantF32;
  PuleF32v2 constantF32v2;
  PuleF32v3 constantF32v3;
  PuleF32v4 constantF32v4;
  int32_t constantI32;
  PuleI32v2 constantI32v2;
  PuleI32v3 constantI32v3;
  PuleI32v4 constantI32v4;
  PuleF32m44 constantF32m44;
} PuleGpuConstantValue;
typedef struct PuleGpuConstant {
  PuleGpuConstantValue value;
  PuleGpuConstantTypeTag typeTag;
  uint32_t bindingSlot;
} PuleGpuConstant;
typedef struct PuleGpuDrawIndirectArrays {
  uint32_t vertexCount;
  uint32_t instanceCount;
  uint32_t vertexOffset;
  uint32_t instanceOffset;
} PuleGpuDrawIndirectArrays;
typedef struct PuleGpuBufferMapRange {
  PuleGpuBuffer buffer;
  PuleGpuBufferMapAccess access;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuBufferMapRange;
typedef struct PuleGpuBufferMappedFlushRange {
  PuleGpuBuffer buffer;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuBufferMappedFlushRange;
typedef struct PuleGpuSamplerCreateInfo {
  PuleGpuImageMagnification minify;
  PuleGpuImageMagnification magnify;
  PuleGpuImageWrap wrapU;
  PuleGpuImageWrap wrapV;
  /*  only valid on 3D targets  */
  PuleGpuImageWrap wrapW;
} PuleGpuSamplerCreateInfo;
typedef struct PuleGpuImageCreateInfo {
  uint32_t width;
  uint32_t height;
  PuleGpuImageTarget target;
  PuleGpuImageByteFormat byteFormat;
  /*  this should technically be changable  */
  PuleGpuSampler sampler;
  PuleStringView label;
  void const * optionalInitialData;
} PuleGpuImageCreateInfo;
typedef struct PuleGpuFramebuffer {
  uint64_t id;
} PuleGpuFramebuffer;
typedef struct PuleGpuFramebufferImageAttachment {
  PuleGpuImage image;
  uint32_t mipmapLevel;
} PuleGpuFramebufferImageAttachment;
typedef union PuleGpuFramebufferAttachments {
  PuleGpuFramebufferImageAttachment images[8] ;
  PuleGpuRenderStorage renderStorages[8] ;
} PuleGpuFramebufferAttachments;
typedef struct PuleGpuFramebufferCreateInfo {
  PuleGpuFramebufferAttachments attachment;
  PuleGpuFramebufferType attachmentType;
} PuleGpuFramebufferCreateInfo;
typedef union PuleGpuImageAttachmentClear {
  PuleF32v4 color;
  float clearDepth;
} PuleGpuImageAttachmentClear;
typedef struct PuleGpuImageAttachment {
  PuleGpuImageAttachmentOpLoad opLoad;
  PuleGpuImageAttachmentOpStore opStore;
  PuleGpuImageLayout layout;
  PuleGpuImageAttachmentClear clear;
  PuleGpuImageView imageView;
} PuleGpuImageAttachment;
typedef struct PuleGpuPipelineAttributeDescriptorBinding {
  PuleGpuAttributeDataType dataType;
  size_t bufferIndex;
  size_t numComponents;
  bool convertFixedDataTypeToNormalizedFloating;
  size_t offsetIntoBuffer;
} PuleGpuPipelineAttributeDescriptorBinding;
/*  TODO input rate  */
typedef struct PuleGpuPipelineAttributeBufferDescriptorBinding {
  /*  must be non-zeo  */
  size_t stridePerElement;
} PuleGpuPipelineAttributeBufferDescriptorBinding;
/* 
  here are some known & fixable limitations with the current model:
    - can't reference the same buffer in separate elements
    - can't reference relative offset strides (related to above)
    - of course, maximum 16 attributes (I think this is fine though)
 */
typedef struct PuleGpuPipelineLayoutDescriptorSet {
  /* 
     TODO change 'uniform' to just 'smallStorage' in this context
     !!!!!!!!!!!!!!
     TODO.... DONT BIND BUFFERS HERE!!!!!!!
     Need to just bind what stages they will be used!!!
     !!!!!!!!!!!!!!
   */
  PuleGpuDescriptorStage bufferUniformBindings[PuleGpuPipelineDescriptorMax_uniform] ;
  /*  TODO use range  */
  PuleGpuDescriptorStage bufferStorageBindings[PuleGpuPipelineDescriptorMax_storage] ;
  PuleGpuPipelineAttributeDescriptorBinding attributeBindings[PuleGpuPipelineDescriptorMax_attribute] ;
  PuleGpuPipelineAttributeBufferDescriptorBinding attributeBufferBindings[PuleGpuPipelineDescriptorMax_attribute] ;
  PuleGpuDescriptorStage textureBindings[PuleGpuPipelineDescriptorMax_texture] ;
} PuleGpuPipelineLayoutDescriptorSet;
typedef struct PuleGpuPipelineLayoutPushConstant {
  PuleGpuDescriptorStage stage;
  size_t byteLength;
  size_t byteOffset;
} PuleGpuPipelineLayoutPushConstant;
/* 
  TODO viewport/scissor like this shouldn't be in a viewport
   */
typedef struct PuleGpuPipelineConfig {
  bool depthTestEnabled;
  bool blendEnabled;
  bool scissorTestEnabled;
  /*  = 0,0  */
  PuleI32v2 viewportMin;
  /*  = 1, 1  */
  PuleI32v2 viewportMax;
  /*  = 0, 0  */
  PuleI32v2 scissorMin;
  /*  = 1, 1  */
  PuleI32v2 scissorMax;
  PuleGpuDrawPrimitive drawPrimitive;
  size_t colorAttachmentCount;
  PuleGpuImageByteFormat colorAttachmentFormats[8] ;
  PuleGpuImageByteFormat depthAttachmentFormat;
} PuleGpuPipelineConfig;
typedef struct PuleGpuPipelineCreateInfo {
  PuleGpuShaderModule shaderModule;
  PuleGpuPipelineLayoutDescriptorSet layoutDescriptorSet;
  PuleGpuPipelineLayoutPushConstant const * layoutPushConstants;
  size_t layoutPushConstantsCount;
  PuleGpuPipelineConfig config;
} PuleGpuPipelineCreateInfo;
typedef struct PuleGpuActionResourceBarrier {
  PuleGpuAction action;
  PuleGpuResourceBarrierStage stageSrc;
  PuleGpuResourceBarrierStage stageDst;
  size_t resourceImageCount;
  PuleGpuResourceBarrierImage const * resourceImages;
  size_t resourceBufferCount;
  PuleGpuResourceBarrierBuffer const * resourceBuffers;
} PuleGpuActionResourceBarrier;
typedef struct PuleGpuActionRenderPassBegin {
  PuleGpuAction action;
  PuleI32v2 viewportMin;
  PuleI32v2 viewportMax;
  PuleGpuImageAttachment attachmentColor[8] ;
  size_t attachmentColorCount;
  PuleGpuImageAttachment attachmentDepth;
} PuleGpuActionRenderPassBegin;
typedef struct PuleGpuActionRenderPassEnd {
  PuleGpuAction action;
} PuleGpuActionRenderPassEnd;
typedef struct PuleGpuActionBindElementBuffer {
  PuleGpuAction action;
  PuleGpuBuffer buffer;
  size_t offset;
  PuleGpuElementType elementType;
} PuleGpuActionBindElementBuffer;
typedef struct PuleGpuActionBindAttributeBuffer {
  PuleGpuAction action;
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  /* 
    this can be 0 to defer to pipeline's default stride at this indx
   */
  size_t stride;
} PuleGpuActionBindAttributeBuffer;
typedef struct PuleGpuActionBindFramebuffer {
  PuleGpuAction action;
  PuleGpuFramebuffer framebuffer;
} PuleGpuActionBindFramebuffer;
typedef struct PuleGpuActionDispatchRender {
  PuleGpuAction action;
  size_t vertexOffset;
  size_t numVertices;
} PuleGpuActionDispatchRender;
typedef struct PuleGpuActionDispatchRenderIndirect {
  PuleGpuAction action;
  PuleGpuBuffer bufferIndirect;
  size_t byteOffset;
} PuleGpuActionDispatchRenderIndirect;
typedef struct PuleGpuActionDispatchRenderElements {
  PuleGpuAction action;
  size_t numElements;
  /*  can be 0  */
  size_t elementOffset;
  /*  can be 0  */
  size_t baseVertexOffset;
} PuleGpuActionDispatchRenderElements;
typedef struct PuleGpuActionClearImageColor {
  PuleGpuAction action;
  PuleGpuImage image;
  PuleF32v4 color;
} PuleGpuActionClearImageColor;
typedef struct PuleGpuActionBindPipeline {
  PuleGpuAction action;
  PuleGpuPipeline pipeline;
} PuleGpuActionBindPipeline;
typedef struct PuleGpuActionBindBuffer {
  PuleGpuAction action;
  PuleGpuBufferBindingDescriptor bindingDescriptor;
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  size_t byteLen;
} PuleGpuActionBindBuffer;
typedef struct PuleGpuActionBindTexture {
  PuleGpuAction action;
  size_t bindingIndex;
  PuleGpuImageView imageView;
  PuleGpuImageLayout imageLayout;
} PuleGpuActionBindTexture;
typedef struct PuleGpuActionClearImageDepth {
  PuleGpuAction action;
  PuleGpuImage image;
  float depth;
} PuleGpuActionClearImageDepth;
typedef struct PuleGpuActionPushConstants {
  PuleGpuAction action;
  PuleGpuDescriptorStage stage;
  size_t byteLength;
  size_t byteOffset;
  void const * data;
} PuleGpuActionPushConstants;
typedef struct PuleGpuActionDispatchCommandList {
  PuleGpuAction action;
  /*  TODO remove this  */
  PuleGpuCommandListSubmitInfo submitInfo;
} PuleGpuActionDispatchCommandList;
typedef struct PuleGpuActionSetScissor {
  PuleGpuAction action;
  PuleI32v2 scissorMin;
  PuleI32v2 scissorMax;
} PuleGpuActionSetScissor;
typedef struct PuleGpuActionCopyImageToImage {
  PuleGpuAction action;
  PuleGpuImage srcImage;
  PuleGpuImage dstImage;
  /*  TODO = PuleU32v3{0, 0, 0}  */
  PuleU32v3 srcOffset;
  /*  TODO = PuleU32v3{0, 0, 0}  */
  PuleU32v3 dstOffset;
  /*  TODO = PuleU32v3{1, 1, 1}  */
  PuleU32v3 extent;
  bool isSrcDepthStencil;
  bool isDstDepthStencil;
} PuleGpuActionCopyImageToImage;
typedef union PuleGpuCommand {
  PuleGpuAction action;
  PuleGpuActionBindPipeline bindPipeline;
  PuleGpuActionBindFramebuffer bindFramebuffer;
  PuleGpuActionBindBuffer bindBuffer;
  PuleGpuActionBindTexture bindTexture;
  PuleGpuActionRenderPassBegin renderPassBegin;
  PuleGpuActionResourceBarrier resourceBarrier;
  PuleGpuActionRenderPassEnd renderPassEnd;
  PuleGpuActionBindElementBuffer bindElementBuffer;
  PuleGpuActionBindAttributeBuffer bindAttributeBuffer;
  PuleGpuActionClearImageColor clearImageColor;
  PuleGpuActionClearImageDepth clearImageDepth;
  PuleGpuActionDispatchRender dispatchRender;
  PuleGpuActionDispatchRenderIndirect dispatchRenderIndirect;
  PuleGpuActionDispatchRenderElements dispatchRenderElements;
  PuleGpuActionPushConstants pushConstants;
  PuleGpuActionDispatchCommandList dispatchCommandList;
  PuleGpuActionSetScissor setScissor;
  PuleGpuActionCopyImageToImage copyImageToImage;
} PuleGpuCommand;

// functions
PULE_exportFn PuleGpuCommandList puleGpuCommandListCreate(PuleAllocator allocator, PuleStringView label);
PULE_exportFn void puleGpuCommandListDestroy(PuleGpuCommandList commandList);
PULE_exportFn PuleStringView puleGpuCommandListName(PuleGpuCommandList commandList);
PULE_exportFn PuleGpuCommandListRecorder puleGpuCommandListRecorder(PuleGpuCommandList commandList);
PULE_exportFn void puleGpuCommandListRecorderFinish(PuleGpuCommandListRecorder commandListRecorder);
PULE_exportFn void puleGpuCommandListAppendAction(PuleGpuCommandListRecorder commandListRecorder, PuleGpuCommand action);
PULE_exportFn void puleGpuCommandListSubmit(PuleGpuCommandListSubmitInfo info, PuleError * error);
PULE_exportFn void puleGpuCommandListSubmitAndPresent(PuleGpuCommandListSubmitInfo info, PuleError * error);
PULE_exportFn void puleGpuCommandListDump(PuleGpuCommandList commandList);
PULE_exportFn PuleGpuCommandListChain puleGpuCommandListChainCreate(PuleAllocator allocator, PuleStringView label);
PULE_exportFn void puleGpuCommandListChainDestroy(PuleGpuCommandListChain commandListChain);
PULE_exportFn PuleGpuCommandList puleGpuCommandListChainCurrent(PuleGpuCommandListChain commandListChain);
/* 
  when submitting command list, be sure to set 'fenceTargetFinish' to this
 */
PULE_exportFn PuleGpuFence puleGpuCommandListChainCurrentFence(PuleGpuCommandListChain commandListChain);
PULE_exportFn PuleGpuBuffer puleGpuBufferCreate(PuleStringView name, void const * optionalInitialData, size_t byteLength, PuleGpuBufferUsage usage, PuleGpuBufferVisibilityFlag visibility);
PULE_exportFn void puleGpuBufferDestroy(PuleGpuBuffer buffer);
PULE_exportFn void * puleGpuBufferMap(PuleGpuBufferMapRange range);
PULE_exportFn void puleGpuBufferMappedFlush(PuleGpuBufferMappedFlushRange range);
PULE_exportFn void puleGpuBufferUnmap(PuleGpuBuffer buffer);
PULE_exportFn void puleGpuInitialize(PulePlatform platform, PuleError * error);
PULE_exportFn void puleGpuShutdown();
/* 
  prints all debug information
  TODO this should just stream out or something maybe serialize
 */
PULE_exportFn void puleGpuDebugPrint();
PULE_exportFn PuleStringView puleGpuBufferUsageLabel(PuleGpuBufferUsage usage);
PULE_exportFn PuleGpuSampler puleGpuSamplerCreate(PuleGpuSamplerCreateInfo createInfo);
PULE_exportFn void puleGpuSamplerDestroy(PuleGpuSampler sampler);
PULE_exportFn PuleStringView puleGpuImageLabel(PuleGpuImage image);
PULE_exportFn void puleGpuImageDestroy(PuleGpuImage image);
PULE_exportFn PuleGpuImage puleGpuImageCreate(PuleGpuImageCreateInfo imageCreateInfo);
PULE_exportFn PuleGpuFramebufferCreateInfo puleGpuFramebufferCreateInfo();
PULE_exportFn PuleGpuFramebuffer puleGpuFramebufferCreate(PuleGpuFramebufferCreateInfo framebufferCreateInfo, PuleError * error);
PULE_exportFn void puleGpuFramebufferDestroy(PuleGpuFramebuffer framebuffer);
PULE_exportFn PuleGpuFramebufferAttachments puleGpuFramebufferAttachments(PuleGpuFramebuffer framebuffer);
PULE_exportFn PuleStringView puleGpuImageLayoutLabel(PuleGpuImageLayout layout);
PULE_exportFn PuleGpuPipelineLayoutDescriptorSet puleGpuPipelineDescriptorSetLayout();
PULE_exportFn PuleGpuPipeline puleGpuPipelineCreate(PuleGpuPipelineCreateInfo createInfo, PuleError * error);
PULE_exportFn void puleGpuPipelineDestroy(PuleGpuPipeline pipeline);
PULE_exportFn PuleGpuImageChain puleGpuImageChain_create(PuleAllocator allocator, PuleStringView label, PuleGpuImageCreateInfo createInfo);
PULE_exportFn void puleGpuImageChain_destroy(PuleGpuImageChain imageChain);
PULE_exportFn PuleGpuImage puleGpuImageChain_current(PuleGpuImageChain imageChain);
PULE_exportFn PuleGpuImageReference puleGpuImageReference_createImageChain(PuleGpuImageChain imageChain);
PULE_exportFn PuleGpuImageReference puleGpuImageReference_createImage(PuleGpuImage image);
PULE_exportFn void puleGpuImageReference_updateImageChain(PuleGpuImageChain baseImageChain, PuleGpuImageChain newImageChain);
PULE_exportFn void puleGpuImageReference_updateImage(PuleGpuImage baseImage, PuleGpuImage newImage);
PULE_exportFn void puleGpuImageReference_destroy(PuleGpuImageReference reference);
PULE_exportFn PuleGpuImage puleGpuImageReference_image(PuleGpuImageReference reference);
PULE_exportFn PuleGpuImage puleGpuWindowSwapchainImage();
PULE_exportFn PuleGpuImageReference puleGpuWindowSwapchainImageReference();
PULE_exportFn PuleGpuShaderModule puleGpuShaderModuleCreate(PuleBufferView vertexShaderBytecode, PuleBufferView fragmentShaderBytecode, PuleError * error);
PULE_exportFn void puleGpuShaderModuleDestroy(PuleGpuShaderModule shaderModule);
PULE_exportFn PuleGpuSemaphore puleGpuSemaphoreCreate(PuleStringView label);
PULE_exportFn void puleGpuSemaphoreDestroy(PuleGpuSemaphore semaphore);
PULE_exportFn PuleGpuFence puleGpuFenceCreate(PuleStringView label);
PULE_exportFn void puleGpuFenceDestroy(PuleGpuFence fence);
PULE_exportFn bool puleGpuFenceWaitSignal(PuleGpuFence fence, PuleNanosecond timeout);
PULE_exportFn void puleGpuFenceReset(PuleGpuFence fence);
PULE_exportFn PuleGpuSemaphore puleGpuFrameStart();
/* 
 same value returned from puleGpuFrameStart
 */
PULE_exportFn PuleGpuSemaphore puleGpuSwapchainAvailableSemaphore();
/* call before pulePlatformSwapFramebuffer */
PULE_exportFn void puleGpuFrameEnd(size_t waitSemaphoreCount, PuleGpuSemaphore const * waitSemaphores);

#ifdef __cplusplus
} // extern C
#endif
