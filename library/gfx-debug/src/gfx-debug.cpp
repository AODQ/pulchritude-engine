#include <pulchritude-gfx-debug/gfx-debug.h>

#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/shader-module.h>
#include <pulchritude-gpu/synchronization.h>

namespace {

struct ShapeRenderer {
  PuleGpuShaderModule shaderModule { 0 };
  PuleGpuPipeline pipeline { 0 };
  size_t requestedDraws = 0;
};

struct MappedLine {
  PuleF32v3 start;
  PuleF32v3 color;
  PuleF32v3 end;
  PuleF32v3 reserved;
};

struct Context {
  PulePlatform platform;
  PuleGpuBuffer mappedBuffer { 0 };
  uint8_t * mappedBufferContents = nullptr;
  size_t bufferByteLength { 0 };
  ShapeRenderer lineRenderer { 0 };
  PuleGpuCommandList commandList { 0 };
};
Context ctx { };

void refreshContext(
  [[maybe_unused]] PuleGpuFramebuffer const framebuffer,
  [[maybe_unused]] PuleF32m44 const transform,
  PuleError * const err
) {
  // // have to do this because pipelines depend on viewports, but they really
  // // shouldn't
  // static int32_t prevWidth = 0, prevHeight = 0;
  // auto const windowSize = pulePlatformWindowSize(ctx.platform);
  // if (prevWidth == windowSize.x && prevHeight == windowSize.y) {
  //   return;
  // }
  // prevWidth = windowSize.x;
  // prevHeight = windowSize.y;
  //
  // { // line pipeline layout
  //   auto descriptorSetLayout = puleGpuPipelineDescriptorSetLayout();
  //   descriptorSetLayout.bufferAttributeBindings[0] = {
  //     .dataType = PuleGpuAttributeDataType_float,
  //     .numComponents = 3,
  //     .convertFixedDataTypeToNormalizedFloating = false,
  //     .stridePerElement = sizeof(PuleF32v3)*2,
  //     .offsetIntoBuffer = 0,
  //   };
  //
  //   auto pipelineInfo = PuleGpuPipelineCreateInfo {
  //     .shaderModule = ctx.lineRenderer.shaderModule,
  //     .layout = &descriptorSetLayout,
  //     .config = {
  //       .depthTestEnabled = true,
  //       .blendEnabled = false,
  //       .scissorTestEnabled = false,
  //       .viewportUl = PuleI32v2 { 0, 0, },
  //       .viewportLr = pulePlatformWindowSize(ctx.platform),
  //       .scissorUl = PuleI32v2 { 0, 0, },
  //       .scissorLr = pulePlatformWindowSize(ctx.platform),
  //     },
  //   };
  //   puleGpuPipelineDestroy(ctx.lineRenderer.pipeline);
  //   ctx.lineRenderer.pipeline = puleGpuPipelineCreate(&pipelineInfo, err);
  //   if (puleErrorExists(err)) {
  //     return;
  //   }
  // }
  //
  // puleGpuCommandListDestroy(ctx.commandList);
  // ctx.commandList = (
  //   puleGpuCommandListCreate(
  //     puleAllocateDefault(),
  //     puleCStr("pule-gfx-debug")
  //   )
  // );
  // { // record command list
  //   auto commandListRecorder = puleGpuCommandListRecorder(ctx.commandList);
  //
  //   if (ctx.lineRenderer.requestedDraws == 0) {
  //     goto finishLineRendering;
  //   }
  //   puleGpuCommandListAppendAction(
  //     commandListRecorder,
  //     PuleGpuCommand {
  //       .bindPipeline = {
  //         .action = PuleGpuAction_bindPipeline,
  //         .pipeline = ctx.lineRenderer.pipeline,
  //       },
  //     }
  //   );
  //   puleGpuCommandListAppendAction(
  //     commandListRecorder,
  //     PuleGpuCommand {
  //       .dispatchRender = {
  //         .action = PuleGpuAction_dispatchRender,
  //         .drawPrimitive = PuleGpuDrawPrimitive_line,
  //         .vertexOffset = 0,
  //         .numVertices = ctx.lineRenderer.requestedDraws*2,
  //       },
  //     }
  //   );
  //   finishLineRendering:
  //
  //   puleGpuCommandListRecorderFinish(commandListRecorder);
  // }
}

} // namespace

extern "C" {

void puleGfxDebugInitialize(PulePlatform const platform) {
  ctx.platform = platform;
  ctx.bufferByteLength = 8192;
  ctx.mappedBuffer = (
    puleGpuBufferCreate(
      puleCStr("pule-gfx-mapped-debug-buffer"),
      nullptr,
      ctx.bufferByteLength,
      PuleGpuBufferUsage_storage,
      PuleGpuBufferVisibilityFlag_hostWritable
    )
  );
  ctx.mappedBufferContents = nullptr;
  ctx.mappedBufferContents = (
    reinterpret_cast<uint8_t *>(
      puleGpuBufferMap( PuleGpuBufferMapRange {
        .buffer = ctx.mappedBuffer,
        .access = PuleGpuBufferMapAccess_hostWritable,
        .byteOffset = 0,
        .byteLength = ctx.bufferByteLength,
      })
    )
  );
  puleLog("mapped buffer %p", ctx.mappedBufferContents);

  char const * const moduleShaderSource = "TODO";
  /*   "#version 460 core\n" \
  //   PULE_multilineString(
  //     uniform layout(location = 0) mat4 transform;
  //     in layout(location = 0) vec2 attributeOrigin;
  //     out layout(location = 0) flat int outVertexId;
  //     void main() {
  //       gl_Position = vec4(attributeOrigin.xy, 0.0f, 1.0f);
  //       outVertexId = gl_VertexID;
  //     }
  //   )
  // );
  //
  // char const * const fragmentShaderSource = (
  //   "#version 460 core\n" \
  //   PULE_multilineString(
  //     in layout(location = 0) flat int inVertexId;
  //     out layout(location = 0) vec4 outColor;
  //     void main() {
  //       outColor = vec4(1.0f);
  //     }
  //   )
  // );

  // PuleError err = puleError();

  // { // line renderer
  //   ctx.lineRenderer.shaderModule = (
  //     puleGpuShaderModuleCreate(
  //       puleCStr(moduleShaderSource),
  //       &err
  //     )
  //   );
  //   if (puleErrorConsume(&err)) {
  //     return;
  //   }
  // }
  */
}

void puleGfxDebugShutdown() {
}

void puleGfxDebugFrameStart() {
  ctx.lineRenderer.requestedDraws = 0;
}

void puleGfxDebugRender(
  PuleGpuFramebuffer const framebuffer,
  PuleF32m44 const transform
) {
  // update GPU info
  puleGpuBufferMappedFlush({
    .buffer = ctx.mappedBuffer,
    .byteOffset = 0,
    .byteLength = ctx.lineRenderer.requestedDraws*sizeof(MappedLine),
  });
  puleGpuMemoryBarrier(PuleGpuMemoryBarrierFlag_bufferUpdate);

  PuleError err = puleError();
  refreshContext(framebuffer, transform, &err);
  if (puleErrorConsume(&err)) {
    return;
  }
  puleGpuCommandListSubmit(
    {
      .commandList = ctx.commandList,
      .fenceTargetFinish = { 0 },
    },
    &err
  );
  if (puleErrorConsume(&err)) {
    return;
  }
}

void puleGfxDebugRenderLine(
  PuleF32v3 const originStart,
  PuleF32v3 const originEnd,
  [[maybe_unused]] PuleF32v3 const color
) {
  MappedLine & mappedLine = *(
    reinterpret_cast<MappedLine *>(
      ctx.mappedBufferContents
      + (
        sizeof(MappedLine)*ctx.lineRenderer.requestedDraws
      )
    )
  );
  mappedLine.start = originStart;
  mappedLine.end = originEnd;
  ctx.lineRenderer.requestedDraws += 1;
}

void puleGfxDebugRenderRectOutline(
  PuleF32v3 const originCenter,
  PuleF32v2 const dimensions,
  PuleF32v3 const color
) {
  PuleF32v3 const bounds[4] = {
    PuleF32v3{-dimensions.x, -dimensions.y, 0.0f},
    PuleF32v3{ dimensions.x, -dimensions.y, 0.0f},
    PuleF32v3{ dimensions.x,  dimensions.y, 0.0f},
    PuleF32v3{-dimensions.x,  dimensions.y, 0.0f},
  };
  for (size_t it = 0; it < 4; ++ it) {
    puleGfxDebugRenderLine(
      puleF32v3Add(originCenter, bounds[it]),
      puleF32v3Add(originCenter, bounds[(it+1)%4]),
      color
    );
  }
}

} // C
