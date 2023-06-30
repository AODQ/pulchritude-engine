#pragma once

#include <pulchritude-gpu/barrier.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-gpu/pipeline.h>
#include <pulchritude-gpu/shader-module.h>

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
  // before this command list starts, will wait on this fence to finish,
  //   will also destroy the fence
  // TODO::CRITICAL actually I guess I need semaphore start and semaphore end
  PuleGpuFence * fenceTargetStart;
  // at end of command list submission, if this is non-null, a fence will
  //   be created to this target
  PuleGpuFence * fenceTargetFinish;
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
typedef enum {
  PuleGpuAction_bindPipeline,
  PuleGpuAction_bindBuffer,
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
} PuleGpuAction;

PULE_exportFn PuleStringView puleGpuActionToString(PuleGpuAction const action);

typedef struct {
  PuleGpuAction action; // PuleGpuAction_bindPipeline
  PuleGpuPipeline pipeline;
} PuleGpuActionBindPipeline;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_bindBuffer
  PuleGpuBufferBindingDescriptor bindingDescriptor;
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  size_t byteLen;
} PuleGpuActionBindBuffer;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_renderPassBegin
  PuleI32v2 viewportUpperLeft;
  PuleI32v2 viewportLowerRight;
  size_t colorAttachmentCount;
  PuleGpuImageAttachment colorAttachments[8];
  PuleGpuImageAttachment depthAttachment;
} PuleGpuActionRenderPassBegin;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_renderPassEnd
} PuleGpuActionRenderPassEnd;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_bindElementBuffer
  PuleGpuBuffer buffer;
  size_t offset;
  PuleGpuElementType elementType;
} PuleGpuActionBindElementBuffer;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_bindAttributeBuffer
  PuleGpuPipeline pipeline; // TODO this should just be whatever is bound?
  size_t bindingIndex;
  PuleGpuBuffer buffer;
  size_t offset;
  size_t stride; // this can be 0 to defer to pipeline's default stride at this index
} PuleGpuActionBindAttributeBuffer;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_bindFramebuffer
  PuleGpuFramebuffer framebuffer;
} PuleGpuActionBindFramebuffer;

typedef enum {
  PuleGpuDrawPrimitive_triangle,
  PuleGpuDrawPrimitive_triangleStrip,
  PuleGpuDrawPrimitive_point,
  PuleGpuDrawPrimitive_line,
} PuleGpuDrawPrimitive;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_dispatchRender
  PuleGpuDrawPrimitive drawPrimitive;
  size_t vertexOffset;
  size_t numVertices;
} PuleGpuActionDispatchRender;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_dispatchRenderIndirect
  PuleGpuDrawPrimitive drawPrimitive;
  PuleGpuBuffer bufferIndirect;
  size_t byteOffset;
} PuleGpuActionDispatchRenderIndirect;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_dispatchRenderElements
  PuleGpuDrawPrimitive drawPrimitive;
  size_t numElements;
  size_t elementOffset; // can be 0
  size_t baseVertexOffset; // can be 0
} PuleGpuActionDispatchRenderElements;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_clearImageColor
  PuleGpuImage image;
  PuleF32v4 color;
} PuleGpuActionClearImageColor;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_clearImageDepth
  PuleGpuImage image;
  float depth;
} PuleGpuActionClearImageDepth;

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

typedef struct {
  PuleGpuAction action; // PuleGpuAction_pushConstants
  PuleGpuConstant const * constants;
  size_t constantsLength;
} PuleGpuActionPushConstants;

typedef struct {
  PuleGpuAction action; // PuleGpuAction_dispatchCommandList
  PuleGpuCommandListSubmitInfo submitInfo; // TODO remove this
} PuleGpuActionDispatchCommandList;

typedef union {
  PuleGpuAction action;
  PuleGpuActionBindPipeline bindPipeline; // TODO need bindPipelineGfx/compute/rt
  PuleGpuActionBindFramebuffer bindFramebuffer;
  PuleGpuActionBindBuffer bindBuffer;
  PuleGpuActionRenderPassBegin renderPassBegin;
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
} PuleGpuCommand;

typedef enum {
  PuleGpuCommandPayloadAccess_indirectCommandRead         = 0x000001,
  PuleGpuCommandPayloadAccess_indexRead                   = 0x000002,
  PuleGpuCommandPayloadAccess_vertexAttributeRead         = 0x000004,
  PuleGpuCommandPayloadAccess_uniformRead                 = 0x000008,
  PuleGpuCommandPayloadAccess_inputAttachmentRead         = 0x000010,
  PuleGpuCommandPayloadAccess_shaderRead                  = 0x000020,
  PuleGpuCommandPayloadAccess_shaderWrite                 = 0x000040,
  PuleGpuCommandPayloadAccess_colorAttachmentRead         = 0x000080,
  PuleGpuCommandPayloadAccess_colorAttachmentWrite        = 0x000100,
  PuleGpuCommandPayloadAccess_depthStencilAttachmentRead  = 0x000200,
  PuleGpuCommandPayloadAccess_depthStencilAttachmentWrite = 0x000400,
  PuleGpuCommandPayloadAccess_transferRead                = 0x000800,
  PuleGpuCommandPayloadAccess_transferWrite               = 0x001000,
  PuleGpuCommandPayloadAccess_hostRead                    = 0x002000,
  PuleGpuCommandPayloadAccess_hostWrite                   = 0x004000,
  PuleGpuCommandPayloadAccess_memoryRead                  = 0x008000,
  PuleGpuCommandPayloadAccess_memoryWrite                 = 0x010000,
} PuleGpuCommandPayloadAccess;

typedef struct {
  PuleGpuImage image;
  PuleGpuCommandPayloadAccess access;
  PuleGpuImageLayout layout;
} PuleGpuCommandPayloadImage;

// these are the state of resources expected when starting to record. This can
//   be either filled out manually or automated using PuleRenderGraph
// Using resources without noting them in the payload can result in hazard
//   tracking errors.
// The resources can be in a different state up until they are expected to be
//   submitted/executed.
typedef struct {
  size_t payloadImagesLength;
  PuleGpuCommandPayloadImage * payloadImages;
} PuleGpuCommandPayload;

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
  PuleGpuCommandList const commandList,
  PuleGpuCommandPayload const beginCommandPayload
);
PULE_exportFn void puleGpuCommandListRecorderFinish(
  PuleGpuCommandListRecorder const commandListRecorder
);
PULE_exportFn void puleGpuCommandListRecorderReset(
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

PULE_exportFn void puleGpuCommandListDump(PuleGpuCommandList const commandList);

#ifdef __cplusplus
} // C
#endif
