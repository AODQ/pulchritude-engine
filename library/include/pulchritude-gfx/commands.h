#pragma once

#include <pulchritude-gfx/barrier.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-gfx/pipeline.h>
#include <pulchritude-gfx/shader-module.h>

#include <pulchritude-allocator/allocator.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-math/math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint64_t id; } PuleGfxCommandList;
typedef struct { uint64_t id; } PuleGfxCommandListRecorder;
typedef struct {
  PuleGfxCommandList commandList;
  // before this command list starts, will wait on this fence to finish,
  //   will also destroy the fence
  // TODO::CRITICAL actually I guess I need semaphore start and semaphore end
  PuleGfxFence * fenceTargetStart;
  // at end of command list submission, if this is non-null, a fence will
  //   be created to this target
  PuleGfxFence * fenceTargetFinish;
} PuleGfxCommandListSubmitInfo;

typedef enum {
  PuleGfxElementType_u8,
  PuleGfxElementType_u16,
  PuleGfxElementType_u32,
} PuleGfxElementType;

//------------------------------------------------------------------------------
//-- ACTIONS -------------------------------------------------------------------
//------------------------------------------------------------------------------
// some amount of action independent of another, such that one complete amount
//   of work can be made between the GPU and CPU. Such as blits, allocations,
//   initiating a render, etc
// commands are not unique, and as such are treated as primitives and may not
//   refer to any specific unique index of a queue
typedef enum {
  PuleGfxAction_bindPipeline,
  PuleGfxAction_bindBuffer,
  PuleGfxAction_renderPassBegin,
  PuleGfxAction_renderPassEnd,
  PuleGfxAction_bindElementBuffer,
  PuleGfxAction_bindAttributeBuffer,
  PuleGfxAction_bindFramebuffer,
  PuleGfxAction_clearImageColor,
  PuleGfxAction_clearImageDepth,
  PuleGfxAction_dispatchRender,
  PuleGfxAction_dispatchRenderElements,
  PuleGfxAction_dispatchRenderIndirect,
  PuleGfxAction_pushConstants,
  PuleGfxAction_dispatchCommandList,
} PuleGfxAction;

PULE_exportFn PuleStringView puleGfxActionToString(PuleGfxAction const action);

typedef struct {
  PuleGfxAction action; // PuleGfxAction_bindPipeline
  PuleGfxPipeline pipeline;
} PuleGfxActionBindPipeline;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_bindBuffer
  PuleGfxGpuBufferBindingDescriptor bindingDescriptor;
  size_t bindingIndex;
  PuleGfxGpuBuffer buffer;
  size_t offset;
  size_t byteLen;
} PuleGfxActionBindBuffer;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_renderPassBegin
  PuleI32v2 viewportUpperLeft;
  PuleI32v2 viewportLowerRight;
  size_t colorAttachmentCount;
  PuleGfxImageAttachment colorAttachments[8];
  PuleGfxImageAttachment depthAttachment;
} PuleGfxActionRenderPassBegin;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_renderPassEnd
} PuleGfxActionRenderPassEnd;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_bindElementBuffer
  PuleGfxGpuBuffer buffer;
  size_t offset;
  PuleGfxElementType elementType;
} PuleGfxActionBindElementBuffer;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_bindAttributeBuffer
  PuleGfxPipeline pipeline; // TODO this should just be whatever is bound?
  size_t bindingIndex;
  PuleGfxGpuBuffer buffer;
  size_t offset;
  size_t stride; // this can be 0 to defer to pipeline's default stride at this index
} PuleGfxActionBindAttributeBuffer;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_bindFramebuffer
  PuleGfxFramebuffer framebuffer;
} PuleGfxActionBindFramebuffer;

typedef enum {
  PuleGfxDrawPrimitive_triangle,
  PuleGfxDrawPrimitive_triangleStrip,
  PuleGfxDrawPrimitive_point,
  PuleGfxDrawPrimitive_line,
} PuleGfxDrawPrimitive;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_dispatchRender
  PuleGfxDrawPrimitive drawPrimitive;
  size_t vertexOffset;
  size_t numVertices;
} PuleGfxActionDispatchRender;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_dispatchRenderIndirect
  PuleGfxDrawPrimitive drawPrimitive;
  PuleGfxGpuBuffer bufferIndirect;
  size_t byteOffset;
} PuleGfxActionDispatchRenderIndirect;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_dispatchRenderElements
  PuleGfxDrawPrimitive drawPrimitive;
  size_t numElements;
  size_t elementOffset; // can be 0
  size_t baseVertexOffset; // can be 0
} PuleGfxActionDispatchRenderElements;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_clearImageColor
  PuleGfxGpuImage image;
  PuleF32v4 color;
} PuleGfxActionClearImageColor;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_clearImageDepth
  PuleGfxGpuImage image;
  float depth;
} PuleGfxActionClearImageDepth;

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
} PuleGfxConstantValue;

typedef enum {
  PuleGfxConstantTypeTag_f32,
  PuleGfxConstantTypeTag_f32v2,
  PuleGfxConstantTypeTag_f32v3,
  PuleGfxConstantTypeTag_f32v4,
  PuleGfxConstantTypeTag_i32,
  PuleGfxConstantTypeTag_i32v2,
  PuleGfxConstantTypeTag_i32v3,
  PuleGfxConstantTypeTag_i32v4,
  PuleGfxConstantTypeTag_f32m44,
} PuleGfxConstantTypeTag;

typedef struct {
  PuleGfxConstantValue value;
  PuleGfxConstantTypeTag typeTag;
  uint32_t bindingSlot;
} PuleGfxConstant;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_pushConstants
  PuleGfxConstant const * constants;
  size_t constantsLength;
} PuleGfxActionPushConstants;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_dispatchCommandList
  PuleGfxCommandListSubmitInfo submitInfo; // TODO remove this
} PuleGfxActionDispatchCommandList;

typedef union {
  PuleGfxAction action;
  PuleGfxActionBindPipeline bindPipeline; // TODO need bindPipelineGfx/compute/rt
  PuleGfxActionBindFramebuffer bindFramebuffer;
  PuleGfxActionBindBuffer bindBuffer;
  PuleGfxActionRenderPassBegin renderPassBegin;
  PuleGfxActionRenderPassEnd renderPassEnd;
  PuleGfxActionBindElementBuffer bindElementBuffer;
  PuleGfxActionBindAttributeBuffer bindAttributeBuffer;
  PuleGfxActionClearImageColor clearImageColor;
  PuleGfxActionClearImageDepth clearImageDepth;
  PuleGfxActionDispatchRender dispatchRender;
  PuleGfxActionDispatchRenderIndirect dispatchRenderIndirect;
  PuleGfxActionDispatchRenderElements dispatchRenderElements;
  PuleGfxActionPushConstants pushConstants;
  PuleGfxActionDispatchCommandList dispatchCommandList;
} PuleGfxCommand;

typedef enum {
  PuleGfxCommandPayloadAccess_indirectCommandRead         = 0x000001,
  PuleGfxCommandPayloadAccess_indexRead                   = 0x000002,
  PuleGfxCommandPayloadAccess_vertexAttributeRead         = 0x000004,
  PuleGfxCommandPayloadAccess_uniformRead                 = 0x000008,
  PuleGfxCommandPayloadAccess_inputAttachmentRead         = 0x000010,
  PuleGfxCommandPayloadAccess_shaderRead                  = 0x000020,
  PuleGfxCommandPayloadAccess_shaderWrite                 = 0x000040,
  PuleGfxCommandPayloadAccess_colorAttachmentRead         = 0x000080,
  PuleGfxCommandPayloadAccess_colorAttachmentWrite        = 0x000100,
  PuleGfxCommandPayloadAccess_depthStencilAttachmentRead  = 0x000200,
  PuleGfxCommandPayloadAccess_depthStencilAttachmentWrite = 0x000400,
  PuleGfxCommandPayloadAccess_transferRead                = 0x000800,
  PuleGfxCommandPayloadAccess_transferWrite               = 0x001000,
  PuleGfxCommandPayloadAccess_hostRead                    = 0x002000,
  PuleGfxCommandPayloadAccess_hostWrite                   = 0x004000,
  PuleGfxCommandPayloadAccess_memoryRead                  = 0x008000,
  PuleGfxCommandPayloadAccess_memoryWrite                 = 0x010000,
} PuleGfxCommandPayloadAccess;

typedef struct {
  PuleGfxGpuImage image;
  PuleGfxCommandPayloadAccess access;
  PuleGfxImageLayout layout;
} PuleGfxCommandPayloadImage;

// these are the state of resources expected when starting to record. This can
//   be either filled out manually or automated using PuleRenderGraph
// Using resources without noting them in the payload can result in hazard
//   tracking errors.
// The resources can be in a different state up until they are expected to be
//   submitted/executed.
typedef struct {
  size_t payloadImagesLength;
  PuleGfxCommandPayloadImage * payloadImages;
} PuleGfxCommandPayload;

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

// TODO::CRITICAL THIS *needs* to specify primary or secondary...
PULE_exportFn PuleGfxCommandList puleGfxCommandListCreate(
  PuleAllocator const allocator,
  PuleStringView const label
);
PULE_exportFn void puleGfxCommandListDestroy(
  PuleGfxCommandList const commandList
);
PULE_exportFn PuleStringView puleGfxCommandListName(
  PuleGfxCommandList const commandList
);

PULE_exportFn PuleGfxCommandListRecorder puleGfxCommandListRecorder(
  PuleGfxCommandList const commandList,
  PuleGfxCommandPayload const beginCommandPayload
);
PULE_exportFn void puleGfxCommandListRecorderFinish(
  PuleGfxCommandListRecorder const commandListRecorder
);
PULE_exportFn void puleGfxCommandListRecorderReset(
  PuleGfxCommandListRecorder const commandListRecorder
);

// TODO use record/finishrecord
PULE_exportFn void puleGfxCommandListAppendAction(
  PuleGfxCommandListRecorder const commandListRecorder,
  PuleGfxCommand const action
);

PULE_exportFn void puleGfxCommandListSubmit(
  PuleGfxCommandListSubmitInfo const info,
  PuleError * const error
);

PULE_exportFn void puleGfxCommandListDump(PuleGfxCommandList const commandList);

#ifdef __cplusplus
} // C
#endif
