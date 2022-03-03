#pragma once

#include <pulchritude-gfx/shader-module.h>
#include <pulchritude-gfx/gfx.h>

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

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
  PuleGfxAction_dispatchRender,
  PuleGfxAction_dispatchRenderElements,
  PuleGfxAction_clearFramebufferColor,
  PuleGfxAction_clearFramebufferDepth,
  PuleGfxAction_pushConstants,
} PuleGfxAction;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_bindPipeline
  PuleGfxPipeline pipeline;
} PuleGfxActionBindPipeline;

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
  PuleGfxAction action; // PuleGfxAction_clearFramebufferColor
  float red; float green; float blue; float alpha; // TODO replace vec4
} PuleGfxActionClearFramebufferColor;

typedef struct {
  PuleGfxAction action; // PuleGfxAction_clearFramebufferDepth
  float depth;
} PuleGfxActionClearFramebufferDepth;

typedef union {
  PuleGfxActionBindPipeline bindPipeline;
  PuleGfxActionDispatchRender dispatchRender;
  PuleGfxActionClearFramebufferColor clearFramebufferColor;
  PuleGfxActionClearFramebufferDepth clearFramebufferDepth;
} PuleGfxCommand;

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

typedef struct { size_t id; } PuleGfxCommandList;
typedef struct { size_t id; } PuleGfxCommandListRecorder;

PULE_exportFn PuleGfxCommandList puleGfxCommandListCreate();
PULE_exportFn void puleGfxCommandListDestroy(
  PuleGfxCommandList const commandList
);

PULE_exportFn PuleGfxCommandListRecorder puleGfxCommandListRecorder(
  PuleGfxCommandList const commandList
);
PULE_exportFn void puleGfxCommandListRecorderFinish(
  PuleGfxCommandListRecorder const commandListRecorder
);

// TODO use record/finishrecord
PULE_exportFn void puleGfxCommandListAppendAction(
  PuleGfxCommandListRecorder const commandListRecorder,
  PuleGfxCommand const action
);

PULE_exportFn void puleGfxCommandListSubmit(
  PuleGfxCommandList const commandList
);

#ifdef __cplusplus
} // C
#endif
