/* auto generated file gpu */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/platform.h>
#include <pulchritude/error.h>
#include <pulchritude/core.h>
#include <pulchritude/math.h>
#include <pulchritude/array.h>
#include <pulchritude/time.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
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
typedef struct {
  PuleGpuAction action;
  PuleGpuPipeline pipeline;
} PuleGpuActionBindPipeline;
typedef struct {
  PuleGpuAction action;
  PuleGpuBufferBindingDescriptor bindingDescriptor;
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  size_t byteLen;
} PuleGpuActionBindBuffer;
typedef struct {
  PuleGpuAction action;
  size_t bindingIndex;
  PuleGpuImageView imageView;
  PuleGpuImageLayout imageLayout;
} PuleGpuActionBindTexture;
typedef struct {
  PuleGpuImage image;
  PuleGpuResourceAccess accessSrc;
  PuleGpuResourceAccess accessDst;
  PuleGpuImageLayout layoutSrc;
  PuleGpuImageLayout layoutDst;
  bool isDepthStencil;
} PuleGpuResourceBarrierImage;
typedef struct {
  PuleGpuBuffer buffer;
  PuleGpuResourceAccess accessSrc;
  PuleGpuResourceAccess accessDst;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuResourceBarrierBuffer;
typedef struct {
  PuleGpuAction action;
  PuleGpuResourceBarrierStage stageSrc;
  PuleGpuResourceBarrierStage stageDst;
  size_t resourceImageCount;
  PuleGpuResourceBarrierImage const * resourceImages;
  size_t resourceBufferCount;
  PuleGpuResourceBarrierBuffer const * resourceBuffers;
} PuleGpuActionResourceBarrier;
typedef struct {
  PuleGpuAction action;
  PuleI32v2 viewportMin;
  PuleI32v2 viewportMax;
  PuleGpuImageAttachment [8] attachmentColor;
  size_t attachmentColorCount;
  PuleGpuImageAttachment attachmentDepth;
} PuleGpuActionRenderPassBegin;
typedef struct {
  PuleGpuAction action;
} PuleGpuActionRenderPassEnd;
typedef struct {
  PuleGpuAction action;
  PuleGpuBuffer buffer;
  size_t offset;
  PuleGpuElementType elementType;
} PuleGpuActionBindElementBuffer;
typedef struct {
  PuleGpuAction action;
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  /* 
    this can be 0 to defer to pipeline's default stride at this indx
   */
  size_t stride;
} PuleGpuActionBindAttributeBuffer;
typedef struct {
  PuleGpuAction action;
  PuleGpuFramebuffer framebuffer;
} PuleGpuActionBindFramebuffer;
typedef struct {
  PuleGpuAction action;
  size_t vertexOffset;
  size_t numVertices;
} PuleGpuActionDispatchRender;
typedef struct {
  PuleGpuAction action;
  PuleGpuBuffer bufferIndirect;
  size_t byteOffset;
} PuleGpuActionDispatchRenderIndirect;
typedef struct {
  PuleGpuAction action;
  size_t numElements;
  /*  can be 0  */
  size_t elementOffset;
  /*  can be 0  */
  size_t baseVertexOffset;
} PuleGpuActionDispatchRenderElements;
typedef struct {
  PuleGpuAction action;
  PuleGpuImage image;
  PuleF32v4 color;
} PuleGpuActionClearImageColor;
typedef struct {
  PuleGpuAction action;
  PuleGpuImage image;
  float depth;
} PuleGpuActionClearImageDepth;
/*  TODO deprecate this in favor of shader introspection  */
typedef union {
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
typedef struct {
  PuleGpuConstantValue value;
  PuleGpuConstantTypeTag typeTag;
  uint32_t bindingSlot;
} PuleGpuConstant;
typedef struct {
  PuleGpuAction action;
  PuleGpuDescriptorStage stage;
  size_t byteLength;
  size_t byteOffset;
  void const * data;
} PuleGpuActionPushConstants;
typedef struct {
  PuleGpuAction action;
  /*  TODO remove this  */
  PuleGpuCommandListSubmitInfo submitInfo;
} PuleGpuActionDispatchCommandList;
typedef struct {
  PuleGpuAction action;
  PuleI32v2 scissorMin;
  PuleI32v2 scissorMax;
} PuleGpuActionSetScissor;
typedef struct {
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
typedef union {
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
typedef struct {
  uint32_t vertexCount;
  uint32_t instanceCount;
  uint32_t vertexOffset;
  uint32_t instanceOffset;
} PuleGpuDrawIndirectArrays;
typedef struct {
  PuleGpuBuffer buffer;
  PuleGpuBufferMapAccess access;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuBufferMapRange;
typedef struct {
  PuleGpuBuffer buffer;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuBufferMappedFlushRange;
typedef struct {
  PuleGpuImageMagnification minify;
  PuleGpuImageMagnification magnify;
  PuleGpuImageWrap wrapU;
  PuleGpuImageWrap wrapV;
  /*  only valid on 3D targets  */
  PuleGpuImageWrap wrapW;
} PuleGpuSamplerCreateInfo;
typedef struct {
  uint32_t width;
  uint32_t height;
  PuleGpuImageTarget target;
  PuleGpuImageByteFormat byteFormat;
  /*  this should technically be changable  */
  PuleGpuSampler sampler;
  PuleStringView label;
  void const * optionalInitialData;
} PuleGpuImageCreateInfo;
typedef struct {
  uint64_t id;
} PuleGpuFramebuffer;
typedef struct {
  PuleGpuImage image;
  uint32_t mipmapLevel;
} PuleGpuFramebufferImageAttachment;
typedef union {
  PuleGpuFramebufferImageAttachment [8] images;
  PuleGpuRenderStorage [8] renderStorages;
} PuleGpuFramebufferAttachments;
typedef struct {
  PuleGpuFramebufferAttachments attachment;
  PuleGpuFramebufferType attachmentType;
} PuleGpuFramebufferCreateInfo;
typedef struct {
  PuleGpuImage image;
  size_t mipmapLevelStart;
  size_t mipmapLevelCount;
  size_t arrayLayerStart;
  size_t arrayLayerCount;
  PuleGpuImageByteFormat byteFormat;
} PuleGpuImageView;
typedef union {
  PuleF32v4 color;
  float clearDepth;
} PuleGpuImageAttachmentClear;
typedef struct {
  PuleGpuImageAttachmentOpLoad opLoad;
  PuleGpuImageAttachmentOpStore opStore;
  PuleGpuImageLayout layout;
  PuleGpuImageAttachmentClear clear;
  PuleGpuImageView imageView;
} PuleGpuImageAttachment;
typedef struct {
  PuleGpuAttributeDataType dataType;
  size_t bufferIndex;
  size_t numComponents;
  bool convertFixedDataTypeToNormalizedFloating;
  size_t offsetIntoBuffer;
} PuleGpuPipelineAttributeDescriptorBinding;
/*  TODO input rate  */
typedef struct {
  /*  must be non-zeo  */
  size_t stridePerElement;
} PuleGpuPipelineAttributeBufferDescriptorBinding;
/* 
  here are some known & fixable limitations with the current model:
    - can't reference the same buffer in separate elements
    - can't reference relative offset strides (related to above)
    - of course, maximum 16 attributes (I think this is fine though)
 */
typedef struct {
  /* 
     TODO change 'uniform' to just 'smallStorage' in this context
     !!!!!!!!!!!!!!
     TODO.... DONT BIND BUFFERS HERE!!!!!!!
     Need to just bind what stages they will be used!!!
     !!!!!!!!!!!!!!
   */
  PuleGpuDescriptorStage [puleGpuPipelineDescriptorMax_uniform] bufferUniformBindings;
  /* 
    TODO use range
   */
  PuleGpuDescriptorStage [16] bufferStorageBindings;
  PuleGpuPipelineAttributeDescriptorBinding [16] attributeBindings;
  PuleGpuPipelineAttributeBufferDescriptorBinding [16] attributeBufferBindings;
  PuleGpuDescriptorStage [8] textureBindings;
} PuleGpuPipelineLayoutDescriptorSet;
typedef struct {
  PuleGpuDescriptorStage stage;
  size_t byteLength;
  size_t byteOffset;
} PuleGpuPipelineLayoutPushConstant;
/* 
  TODO viewport/scissor like this shouldn't be in a viewport
   */
typedef struct {
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
  PuleGpuImageByteFormat [8] colorAttachmentFormats;
  PuleGpuImageByteFormat depthAttachmentFormat;
} PuleGpuPipelineConfig;
typedef struct {
  PuleGpuShaderModule shaderModule;
  PuleGpuPipelineLayoutDescriptorSet layoutDescriptorSet;
  PuleGpuPipelineLayoutPushConstant const * layoutPushConstants;
  size_t layoutPushConstantsCount;
  PuleGpuPipelineConfig config;
} PuleGpuPipelineCreateInfo;

// enum
typedef enum {
  PuleGpuElementType_u8,
  PuleGpuElementType_u16,
  PuleGpuElementType_u32,
} PuleGpuElementType;
/* 
   some amount of action independent of another, such that one complete amount
     of work can be made between the GPU and CPU. Such as blits, allocations,
     initiating a render, etc
   commands are not unique, and as such are treated as primitives and may not
     refer to any specific unique index of a queue
   TODO just rename to GpuCommand_
 */
typedef enum {
  PuleGpuAction_bindPipeline,
  PuleGpuAction_bindBuffer,
  PuleGpuAction_bindTexture,
  PuleGpuAction_resourceBarrier,
  PuleGpuAction_renderPassBegin,
  PuleGpuAction_renderPassEnd,
  PuleGpuAction_bindElementBuffer,
  PuleGpuAction_bindAttributeBuffer,
  PuleGpuAction_bindFramebuffer,
  PuleGpuAction_clearImageColor,
  PuleGpuAction_clearImageDepth,
  PuleGpuAction_dispatchRender,
  PuleGpuAction_dispatchRenderElements,
  PuleGpuAction_dispatchRenderIndirect,
  PuleGpuAction_pushConstants,
  PuleGpuAction_dispatchCommandList,
  PuleGpuAction_setScissor,
  PuleGpuAction_copyImageToImage,
} PuleGpuAction;
typedef enum {
  PuleGpuResourceBarrierStage_top,
  PuleGpuResourceBarrierStage_drawIndirect,
  PuleGpuResourceBarrierStage_vertexInput,
  PuleGpuResourceBarrierStage_shaderFragment,
  PuleGpuResourceBarrierStage_shaderVertex,
  PuleGpuResourceBarrierStage_shaderCompute,
  PuleGpuResourceBarrierStage_outputAttachmentColor,
  PuleGpuResourceBarrierStage_outputAttachmentDepth,
  PuleGpuResourceBarrierStage_transfer,
  PuleGpuResourceBarrierStage_bottom,
} PuleGpuResourceBarrierStage;
typedef enum {
  PuleGpuResourceAccess_none,
  PuleGpuResourceAccess_indirectCommandRead,
  PuleGpuResourceAccess_indexRead,
  PuleGpuResourceAccess_vertexAttributeRead,
  PuleGpuResourceAccess_uniformRead,
  PuleGpuResourceAccess_inputAttachmentRead,
  PuleGpuResourceAccess_shaderRead,
  PuleGpuResourceAccess_shaderWrite,
  PuleGpuResourceAccess_attachmentColorRead,
  PuleGpuResourceAccess_attachmentColorWrite,
  PuleGpuResourceAccess_attachmentDepthRead,
  PuleGpuResourceAccess_attachmentDepthWrite,
  PuleGpuResourceAccess_transferRead,
  PuleGpuResourceAccess_transferWrite,
  PuleGpuResourceAccess_hostRead,
  PuleGpuResourceAccess_hostWrite,
  PuleGpuResourceAccess_memoryRead,
  PuleGpuResourceAccess_memoryWrite,
} PuleGpuResourceAccess;
/*  TODO deprecate this in favor of shader introspection  */
typedef enum {
  PuleGpuConstantTypeTag_f32,
  PuleGpuConstantTypeTag_f32v2,
  PuleGpuConstantTypeTag_f32v3,
  PuleGpuConstantTypeTag_f32v4,
  PuleGpuConstantTypeTag_i32,
  PuleGpuConstantTypeTag_i32v2,
  PuleGpuConstantTypeTag_i32v3,
  PuleGpuConstantTypeTag_i32v4,
  PuleGpuConstantTypeTag_f32m44,
} PuleGpuConstantTypeTag;
typedef enum {
  PuleErrorGfx_none,
  PuleErrorGfx_creationFailed,
  PuleErrorGfx_shaderModuleCompilationFailed,
  PuleErrorGfx_invalidDescriptorSet,
  PuleErrorGfx_invalidCommandList,
  PuleErrorGfx_invalidFramebuffer,
  PuleErrorGfx_submissionFenceWaitFailed,
} PuleErrorGfx;
typedef enum {
  PuleGpuBufferUsage_attribute,
  PuleGpuBufferUsage_element,
  PuleGpuBufferUsage_uniform,
  PuleGpuBufferUsage_storage,
  PuleGpuBufferUsage_accelerationStructure,
  PuleGpuBufferUsage_indirect,
} PuleGpuBufferUsage;
typedef enum {
  PuleGpuBufferBindingDescriptor_uniform,
  PuleGpuBufferBindingDescriptor_storage,
  PuleGpuBufferBindingDescriptor_accelerationStructure,
} PuleGpuBufferBindingDescriptor;
typedef enum {
  /*  incompatible with rest  */
  PuleGpuBufferVisibilityFlag_deviceOnly,
  PuleGpuBufferVisibilityFlag_hostVisible,
  PuleGpuBufferVisibilityFlag_hostWritable,
} PuleGpuBufferVisibilityFlag;
typedef enum {
  PuleGpuBufferMapAccess_hostVisible,
  PuleGpuBufferMapAccess_hostWritable,
  PuleGpuBufferMapAccess_invalidate,
} PuleGpuBufferMapAccess;
typedef enum {
  PuleGpuImageMagnification_nearest,
} PuleGpuImageMagnification;
typedef enum {
  PuleGpuImageWrap_clampToEdge,
} PuleGpuImageWrap;
typedef enum {
  PuleGpuImageByteFormat_undefined,
  PuleGpuImageByteFormat_bgra8U,
  PuleGpuImageByteFormat_rgba8U,
  PuleGpuImageByteFormat_rgb8U,
  PuleGpuImageByteFormat_r8U,
  PuleGpuImageByteFormat_depth16,
} PuleGpuImageByteFormat;
typedef enum {
  PuleGpuImageTarget_i2D,
} PuleGpuImageTarget;
typedef enum {
  PuleGpuFramebufferAttachment_color0,
  PuleGpuFramebufferAttachment_color1,
  PuleGpuFramebufferAttachment_color3,
  PuleGpuFramebufferAttachment_color4,
  PuleGpuFramebufferAttachment_depth,
  PuleGpuFramebufferAttachment_stencil,
  PuleGpuFramebufferAttachment_depthStencil,
} PuleGpuFramebufferAttachment;
typedef enum {
  PuleGpuFramebufferType_renderStorage,
  PuleGpuFramebufferType_imageStorage,
} PuleGpuFramebufferType;
typedef enum {
  PuleGpuImageAttachmentOpLoad_load,
  PuleGpuImageAttachmentOpLoad_clear,
  PuleGpuImageAttachmentOpLoad_dontCare,
} PuleGpuImageAttachmentOpLoad;
typedef enum {
  PuleGpuImageAttachmentOpStore_store,
  PuleGpuImageAttachmentOpStore_dontCare,
} PuleGpuImageAttachmentOpStore;
typedef enum {
  PuleGpuImageLayout_uninitialized,
  PuleGpuImageLayout_storage,
  PuleGpuImageLayout_attachmentColor,
  PuleGpuImageLayout_attachmentDepth,
  PuleGpuImageLayout_transferSrc,
  PuleGpuImageLayout_transferDst,
  PuleGpuImageLayout_presentSrc,
} PuleGpuImageLayout;
typedef enum {
  PuleGpuPipelineStage_top,
  PuleGpuPipelineStage_drawIndirect,
  PuleGpuPipelineStage_vertexInput,
  PuleGpuPipelineStage_vertexShader,
  PuleGpuPipelineStage_fragmentShader,
  PuleGpuPipelineStage_colorAttachmentOutput,
  PuleGpuPipelineStage_computeShader,
  PuleGpuPipelineStage_transfer,
  PuleGpuPipelineStage_bottom,
} PuleGpuPipelineStage;
typedef enum {
  PuleGpuDescriptorType_sampler,
  PuleGpuDescriptorType_uniformBuffer,
  /*  not supported  */
  PuleGpuDescriptorType_storageBuffer,
} PuleGpuDescriptorType;
typedef enum {
  PuleGpuAttributeDataType_float,
  PuleGpuAttributeDataType_unsignedByte,
} PuleGpuAttributeDataType;
typedef enum {
  puleGpuPipelineDescriptorMax_uniform,
  puleGpuPipelineDescriptorMax_storage,
  puleGpuPipelineDescriptorMax_attribute,
  puleGpuPipelineDescriptorMax_texture,
} PuleGpuPipelineDescriptorMax;
typedef enum {
  PuleGpuDescriptorStage_unused,
  PuleGpuDescriptorStage_vertex,
  PuleGpuDescriptorStage_fragment,
} PuleGpuDescriptorStage;
typedef enum {
  PuleGpuDrawPrimitive_triangle,
  PuleGpuDrawPrimitive_triangleStrip,
  PuleGpuDrawPrimitive_point,
  PuleGpuDrawPrimitive_line,
} PuleGpuDrawPrimitive;
typedef enum {
  PuleGpuFenceConditionFlag_all,
} PuleGpuFenceConditionFlag;
typedef enum {
  PuleGpuSignalTime_forever,
} PuleGpuSignalTime;

// entities
typedef struct { uint64_t id; } PuleGpuCommandList;
typedef struct { uint64_t id; } PuleGpuCommandListRecorder;
/* 
  utility to allow recording to a command list while it is being submitted,
  which occurs if double or triple buffering for present is enabled It's an
  implementation detail if command lists are reused or destroyed
 */
typedef struct { uint64_t id; } PuleGpuCommandListChain;
typedef struct { uint64_t id; } PuleGpuBuffer;
typedef struct { uint64_t id; } PuleGpuSampler;
typedef struct { uint64_t id; } PuleGpuImage;
typedef struct { uint64_t id; } PuleGpuRenderStorage;
typedef struct { uint64_t id; } PuleGpuPipeline;
typedef struct { uint64_t id; } PuleGpuImageChain;
/* 
   allows you to reference images, even if they're behind a chain, and track
   their lifetimes (e.g. if they get reconstructed)
 */
typedef struct { uint64_t id; } PuleGpuImageReference;
typedef struct { uint64_t id; } PuleGpuShaderModule;
typedef struct { uint64_t id; } PuleGpuSemaphore;
typedef struct { uint64_t id; } PuleGpuFence;

// functions
PULE_exportFn PuleStringView puleGpuActionToString(PuleGpuAction action);
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
PULE_exportFn PuleString puleGpuResourceBarrierStageLabel(PuleGpuResourceBarrierStage stage);
PULE_exportFn PuleString puleGpuResourceAccessLabel(PuleGpuResourceAccess access);
PULE_exportFn PuleGpuBuffer puleGpuBufferCreate(PuleStringView name, void const * optionalInitialData, size_t byteLength, PuleGpuBufferUsage usage, PuleGpuBufferVisibilityFlag visibility);
PULE_exportFn void puleGpuBufferDestroy(PuleGpuBuffer buffer);
PULE_exportFn void puleGpuBufferMap(PuleGpuBufferMapRange * range);
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
PULE_exportFn void puleGpuFrameEnd(size_t waitSemaphoreCount, PuleGpuSemaphore * const waitSemaphores);

#ifdef __cplusplus
} // extern C
#endif
