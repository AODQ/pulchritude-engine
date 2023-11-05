#pragma once

#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-gpu/pipeline.h>
#include <pulchritude-gpu/shader-module.h>
#include <pulchritude-gpu/synchronization.h>

#include <pulchritude-allocator/allocator.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleGpuCommandList;
typedef struct { uint64_t id; } PuleGpuCommandListRecorder;
typedef struct {
  PuleGpuCommandList commandList;
  size_t waitSemaphoreCount;
  PuleGpuSemaphore const * waitSemaphores;
  PuleGpuPipelineStage const * waitSemaphoreStages;
  // will create semaphore if id is 0, otherwise will use existing semaphore
  size_t signalSemaphoreCount;
  PuleGpuSemaphore * signalSemaphores;
  // at end of command list submission, if this is non-zero, fence will
  // be triggered
  PuleGpuFence fenceTargetFinish;
} PuleGpuCommandListSubmitInfo;

typedef enum {
  PuleGpuElementType_u8,
  PuleGpuElementType_u16,
  PuleGpuElementType_u32,
} PuleGpuElementType;

//------------------------------------------------------------------------------
//-- ACTIONS -------------------------------------------------------------------
//------------------------------------------------------------------------------
// some amount of action independent of another, such that one complete amount
//   of work can be made between the GPU and CPU. Such as blits, allocations,
//   initiating a render, etc
// commands are not unique, and as such are treated as primitives and may not
//   refer to any specific unique index of a queue
// TODO just rename to GpuCommand_
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

PULE_exportFn PuleStringView puleGpuActionToString(PuleGpuAction const action);

typedef struct PuleGpuActionBindPipeline {
  PuleGpuAction action PULE_param(PuleGpuAction_bindPipeline);
  PuleGpuPipeline pipeline;
} PuleGpuActionBindPipeline;

typedef struct PuleGpuActionBindBuffer {
  PuleGpuAction action PULE_param(PuleGpuAction_bindBuffer);
  PuleGpuBufferBindingDescriptor bindingDescriptor;
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  size_t byteLen;
} PuleGpuActionBindBuffer;

typedef struct PuleGpuActionBindTexture {
  PuleGpuAction action PULE_param(PuleGpuAction_bindBuffer);
  size_t bindingIndex;
  PuleGpuImageView imageView;
  PuleGpuImageLayout imageLayout;
  // TODO sampler or allow constant
} PuleGpuActionBindTexture;

typedef enum {
  PuleGpuResourceBarrierStage_top                   = 0x0001,
  PuleGpuResourceBarrierStage_drawIndirect          = 0x0002,
  PuleGpuResourceBarrierStage_vertexInput           = 0x0004,
  PuleGpuResourceBarrierStage_shaderFragment        = 0x0008,
  PuleGpuResourceBarrierStage_shaderVertex          = 0x0010,
  PuleGpuResourceBarrierStage_shaderCompute         = 0x0020,
  PuleGpuResourceBarrierStage_outputAttachmentColor = 0x0040,
  PuleGpuResourceBarrierStage_transfer              = 0x0080,
  PuleGpuResourceBarrierStage_bottom                = 0x0100,
} PuleGpuResourceBarrierStage;

typedef enum {
  PuleGpuResourceAccess_none                 = 0x00000000,
  PuleGpuResourceAccess_indirectCommandRead  = 0x00000001,
  PuleGpuResourceAccess_indexRead            = 0x00000002,
  PuleGpuResourceAccess_vertexAttributeRead  = 0x00000004,
  PuleGpuResourceAccess_uniformRead          = 0x00000008,
  PuleGpuResourceAccess_inputAttachmentRead  = 0x00000010,
  PuleGpuResourceAccess_shaderRead           = 0x00000020,
  PuleGpuResourceAccess_shaderWrite          = 0x00000040,
  PuleGpuResourceAccess_attachmentColorRead  = 0x00000080,
  PuleGpuResourceAccess_attachmentColorWrite = 0x00000100,
  PuleGpuResourceAccess_attachmentDepthRead  = 0x00000200,
  PuleGpuResourceAccess_attachmentDepthWrite = 0x00000400,
  PuleGpuResourceAccess_transferRead         = 0x00000800,
  PuleGpuResourceAccess_transferWrite        = 0x00001000,
  PuleGpuResourceAccess_hostRead             = 0x00002000,
  PuleGpuResourceAccess_hostWrite            = 0x00004000,
  PuleGpuResourceAccess_memoryRead           = 0x00008000,
  PuleGpuResourceAccess_memoryWrite          = 0x00010000,
} PuleGpuResourceAccess;


typedef struct {
  PuleGpuImage image;
  PuleGpuResourceAccess accessSrc;
  PuleGpuResourceAccess accessDst;
  PuleGpuImageLayout layoutSrc;
  PuleGpuImageLayout layoutDst;
  // TODO maybe subresource range
} PuleGpuResourceBarrierImage;

typedef struct PuleGpuActionResourceBarrier {
  PuleGpuAction action PULE_param(PuleGpuAction_resourceBarrier);
  PuleGpuResourceBarrierStage stageSrc;
  PuleGpuResourceBarrierStage stageDst;
  size_t resourceImageCount;
  PuleGpuResourceBarrierImage const * resourceImages;
} PuleGpuActionResourceBarrier;

typedef struct PuleGpuActionRenderPassBegin {
  PuleGpuAction action PULE_param(PuleGpuAction_renderPassBegin);
  PuleI32v2 viewportMin;
  PuleI32v2 viewportMax;
  PuleGpuImageAttachment attachmentColor[8];
  size_t attachmentColorCount;
  PuleGpuImageAttachment attachmentDepth;
} PuleGpuActionRenderPassBegin;

typedef struct PuleGpuActionRenderPassEnd {
  PuleGpuAction action PULE_param(PuleGpuAction_renderPassEnd);
} PuleGpuActionRenderPassEnd;

typedef struct PuleGpuActionBindElementBuffer {
  PuleGpuAction action PULE_param(PuleGpuAction_bindElementBuffer);
  PuleGpuBuffer buffer;
  size_t offset;
  PuleGpuElementType elementType;
} PuleGpuActionBindElementBuffer;

typedef struct PuleGpuActionBindAttributeBuffer {
  PuleGpuAction action PULE_param(PuleGpuAction_bindAttributeBuffer);
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  size_t stride; // this can be 0 to defer to pipeline's default stride at this index
} PuleGpuActionBindAttributeBuffer;

typedef struct PuleGpuActionBindFramebuffer {
  PuleGpuAction action PULE_param(PuleGpuAction_bindFramebuffer);
  PuleGpuFramebuffer framebuffer;
} PuleGpuActionBindFramebuffer;

typedef struct PuleGpuActionDispatchRender {
  PuleGpuAction action PULE_param(PuleGpuAction_dispatchRender);
  size_t vertexOffset;
  size_t numVertices;
} PuleGpuActionDispatchRender;

typedef struct PuleGpuActionDispatchRenderIndirect {
  PuleGpuAction action PULE_param(PuleGpuAction_dispatchRenderIndirect);
  PuleGpuBuffer bufferIndirect;
  size_t byteOffset;
} PuleGpuActionDispatchRenderIndirect;

typedef struct PuleGpuActionDispatchRenderElements {
  PuleGpuAction action PULE_param(PuleGpuAction_dispatchRenderElements);
  size_t numElements;
  size_t elementOffset; // can be 0
  size_t baseVertexOffset; // can be 0
} PuleGpuActionDispatchRenderElements;

typedef struct PuleGpuActionClearImageColor {
  PuleGpuAction action PULE_param(PuleGpuAction_clearImageColor);
  PuleGpuImage image;
  PuleF32v4 color;
} PuleGpuActionClearImageColor;

typedef struct PuleGpuActionClearImageDepth {
  PuleGpuAction action PULE_param(PuleGpuAction_clearImageDepth);
  PuleGpuImage image;
  float depth;
} PuleGpuActionClearImageDepth;

// TODO probably not necessary anymore,
//      I believe Vulkan can predict this from the shader
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

// TODO probably not necessary anymore
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

typedef struct {
  PuleGpuConstantValue value;
  PuleGpuConstantTypeTag typeTag;
  uint32_t bindingSlot;
} PuleGpuConstant;

typedef struct PuleGpuActionPushConstants {
  PuleGpuAction action PULE_param(PuleGpuAction_pushConstants);
  PuleGpuDescriptorStage stage;
  size_t byteLength;
  size_t byteOffset;
  void const * data;
} PuleGpuActionPushConstants;

typedef struct PuleGpuActionDispatchCommandList {
  PuleGpuAction action PULE_param(PuleGpuAction_dispatchCommandList);
  PuleGpuCommandListSubmitInfo submitInfo; // TODO remove this
} PuleGpuActionDispatchCommandList;

typedef struct PuleGpuActionSetScissor {
  PuleGpuAction action PULE_param(PuleGpuAction_setScissor);
  PuleI32v2 scissorMin;
  PuleI32v2 scissorMax;
} PuleGpuActionSetScissor;

typedef struct PuleGpuActionCopyImageToImage {
  PuleGpuAction action PULE_param(PuleGpuAction_copyImageToImage);
  PuleGpuImage srcImage;
  PuleGpuImage dstImage;
  // TODO subresource layers
  PuleU32v3 srcOffset PULE_param(PuleU32v3{0, 0, 0});
  PuleU32v3 dstOffset PULE_param(PuleU32v3{0, 0, 0});
  PuleU32v3 extent PULE_param(PuleU32v3{1, 1, 1});
} PuleGpuActionCopyImageToImage;

typedef union {
  PuleGpuAction action;
  PuleGpuActionBindPipeline bindPipeline; // TODO need bindPipelineGfx/compute/rt
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

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

// TODO::CRITICAL THIS *needs* to specify primary or secondary...
PULE_exportFn PuleGpuCommandList puleGpuCommandListCreate(
  PuleAllocator const allocator,
  PuleStringView const label
);
PULE_exportFn void puleGpuCommandListDestroy(
  PuleGpuCommandList const commandList
);
PULE_exportFn PuleStringView puleGpuCommandListName(
  PuleGpuCommandList const commandList
);

PULE_exportFn PuleGpuCommandListRecorder puleGpuCommandListRecorder(
  PuleGpuCommandList const commandList
);
PULE_exportFn void puleGpuCommandListRecorderFinish(
  PuleGpuCommandListRecorder const commandListRecorder
);

// TODO use record/finishrecord
PULE_exportFn void puleGpuCommandListAppendAction(
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleGpuCommand const action
);

PULE_exportFn void puleGpuCommandListSubmit(
  PuleGpuCommandListSubmitInfo const info,
  PuleError * const error
);

PULE_exportFn void puleGpuCommandListSubmitAndPresent(
  PuleGpuCommandListSubmitInfo const info,
  PuleError * const error
);

PULE_exportFn void puleGpuCommandListDump(PuleGpuCommandList const commandList);

//------------------------------------------------------------------------------
//-- COMMAND LIST CHAIN --------------------------------------------------------
//------------------------------------------------------------------------------

// utility to allow recording to a command list while it is being submitted,
// which occurs if double or triple buffering for present is enabled
// It's an implementation detail if command lists are reused or destroyed
typedef struct { uint64_t id; } PuleGpuCommandListChain;

PULE_exportFn PuleGpuCommandListChain puleGpuCommandListChainCreate(
  PuleAllocator const allocator,
  PuleStringView const label
);
PULE_exportFn void puleGpuCommandListChainDestroy(
  PuleGpuCommandListChain const commandListChain
);

PULE_exportFn PuleGpuCommandList puleGpuCommandListChainCurrent(
  PuleGpuCommandListChain const commandListChain
);

// when submitting command list, be sure to set `fenceTargetFinish` to this
PULE_exportFn PuleGpuFence puleGpuCommandListChainCurrentFence(
  PuleGpuCommandListChain const commandListChain
);

PULE_exportFn PuleString puleGpuResourceBarrierStageLabel(
  PuleGpuResourceBarrierStage const stage
);
PULE_exportFn PuleString puleGpuResourceAccessLabel(
  PuleGpuResourceAccess const access
);

#ifdef __cplusplus
} // C
#endif
