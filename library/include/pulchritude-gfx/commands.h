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
  PuleGfxFence * fenceTargetStart;
  // at end of command list submission, if this is non-null, a fence will
  //   be created to this target
  PuleGfxFence * fenceTargetFinish;
} PuleGfxCommandListSubmitInfo;


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
  PuleGfxAction_bindAttribute, // TODO bindBuffer
  PuleGfxAction_bindFramebuffer,
  PuleGfxAction_clearFramebufferColor,
  PuleGfxAction_clearFramebufferDepth,
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
  PuleGfxAction action; // PuleGfxAction_bindAttribute
  PuleGfxPipeline pipeline;
  size_t bindingIndex;
  PuleGfxGpuBuffer buffer;
  size_t offset;
  size_t stride;
} PuleGfxActionBindAttribute;

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

typedef enum {
  PuleGfxElementType_u8,
  PuleGfxElementType_u16,
  PuleGfxElementType_u32,
} PuleGfxElementType;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_dispatchRenderElements
  PuleGfxDrawPrimitive drawPrimitive;
  size_t numElements;
  PuleGfxElementType elementType;
  size_t elementOffset; // can be 0
  size_t baseVertexOffset; // can be 0
} PuleGfxActionDispatchRenderElements;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_clearFramebufferColor
  PuleGfxFramebuffer framebuffer;
  PuleF32v4 color;
} PuleGfxActionClearFramebufferColor;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_clearFramebufferDepth
  PuleGfxFramebuffer framebuffer;
  float depth;
} PuleGfxActionClearFramebufferDepth;

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
  PuleGfxCommandListSubmitInfo submitInfo;
} PuleGfxActionDispatchCommandList;

typedef union {
  PuleGfxAction action;
  PuleGfxActionBindPipeline bindPipeline;
  PuleGfxActionBindFramebuffer bindFramebuffer;
  PuleGfxActionBindAttribute bindAttribute;
  PuleGfxActionClearFramebufferColor clearFramebufferColor;
  PuleGfxActionClearFramebufferDepth clearFramebufferDepth;
  PuleGfxActionDispatchRender dispatchRender;
  PuleGfxActionDispatchRenderIndirect dispatchRenderIndirect;
  PuleGfxActionDispatchRenderElements dispatchRenderElements;
  PuleGfxActionPushConstants pushConstants;
  PuleGfxActionDispatchCommandList dispatchCommandList;
} PuleGfxCommand;

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

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
  PuleGfxCommandList const commandList
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
