#include <pulchritude-gfx-debug/gfx-debug.h>

#include <pulchritude-gfx/barrier.h>
#include <pulchritude-gfx/commands.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/shader-module.h>

namespace {

struct ShapeRenderer {
  PuleGfxShaderModule shaderModule { 0 };
  PuleGfxPipeline pipeline { 0 };
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
  PuleGfxGpuBuffer mappedBuffer { 0 };
  uint8_t * mappedBufferContents = nullptr;
  size_t bufferByteLength { 0 };
  ShapeRenderer lineRenderer { 0 };
  PuleGfxCommandList commandList { 0 };
};
Context ctx { };

void refreshContext(
  [[maybe_unused]] PuleGfxFramebuffer const framebuffer,
  [[maybe_unused]] PuleF32m44 const transform,
  PuleError * const err
) {
  { // line pipeline layout
    auto descriptorSetLayout = puleGfxPipelineDescriptorSetLayout();
    descriptorSetLayout.bufferAttributeBindings[0] = {
      .buffer = ctx.mappedBuffer,
      .numComponents = 3,
      .dataType = PuleGfxAttributeDataType_float,
      .normalizeFixedDataTypeToNormalizedFloating = false,
      .stridePerElement = sizeof(PuleF32v3)*2,
      .offsetIntoBuffer = 0,
    };

    auto pipelineInfo = PuleGfxPipelineCreateInfo {
      .shaderModule = ctx.lineRenderer.shaderModule,
      .framebuffer = puleGfxFramebufferWindow(),
      .layout = &descriptorSetLayout,
      .config = {
        .blendEnabled = false,
        .scissorTestEnabled = false,
        .viewportUl = PuleI32v2 { 0, 0, },
        .viewportLr = pulePlatformWindowSize(ctx.platform),
        .scissorUl = PuleI32v2 { 0, 0, },
        .scissorLr = pulePlatformWindowSize(ctx.platform),
      },
    };
    puleGfxPipelineDestroy(ctx.lineRenderer.pipeline);
    ctx.lineRenderer.pipeline = puleGfxPipelineCreate(&pipelineInfo, err);
    if (puleErrorExists(err)) {
      return;
    }
  }


  puleGfxCommandListDestroy(ctx.commandList);
  ctx.commandList = (
    puleGfxCommandListCreate(
      puleAllocateDefault(),
      puleStringViewCStr("pule-gfx-debug")
    )
  );
  { // record command list
    auto commandListRecorder = puleGfxCommandListRecorder(ctx.commandList);

    if (ctx.lineRenderer.requestedDraws == 0) {
      goto finishLineRendering;
    }
    puleGfxCommandListAppendAction(
      commandListRecorder,
      PuleGfxCommand {
        .bindPipeline = {
          .action = PuleGfxAction_bindPipeline,
          .pipeline = ctx.lineRenderer.pipeline,
        },
      }
    );
    puleGfxCommandListAppendAction(
      commandListRecorder,
      PuleGfxCommand {
        .dispatchRender = {
          .action = PuleGfxAction_dispatchRender,
          .drawPrimitive = PuleGfxDrawPrimitive_line,
          .vertexOffset = 0,
          .numVertices = ctx.lineRenderer.requestedDraws*2,
        },
      }
    );
    finishLineRendering:

    puleGfxCommandListRecorderFinish(commandListRecorder);
  }
}

} // namespace

extern "C" {

void puleGfxDebugInitialize(PulePlatform const platform) {
  ctx.platform = platform;
  ctx.bufferByteLength = 8192;
  ctx.mappedBuffer = (
    puleGfxGpuBufferCreate(
      nullptr,
      ctx.bufferByteLength,
      PuleGfxGpuBufferUsage_bufferStorage,
      PuleGfxGpuBufferVisibilityFlag_hostWritable
    )
  );
  ctx.mappedBufferContents = nullptr;
  ctx.mappedBufferContents = (
    reinterpret_cast<uint8_t *>(
      puleGfxGpuBufferMap( PuleGfxGpuBufferMapRange {
        .buffer = ctx.mappedBuffer,
        .access = PuleGfxGpuBufferMapAccess_hostWritable,
        .byteOffset = 0,
        .byteLength = ctx.bufferByteLength,
      })
    )
  );
  puleLog("mapped buffer %p", ctx.mappedBufferContents);

  char const * const vertexShaderSource = (
    "#version 460 core\n" \
    PULE_multilineString(
      uniform layout(location = 0) mat4 transform;
      in layout(location = 0) vec2 attributeOrigin;
      out layout(location = 0) flat int outVertexId;
      void main() {
        gl_Position = vec4(attributeOrigin.xy, 0.0f, 1.0f);
        outVertexId = gl_VertexID;
      }
    )
  );

  char const * const fragmentShaderSource = (
    "#version 460 core\n" \
    PULE_multilineString(
      in layout(location = 0) flat int inVertexId;
      out layout(location = 0) vec4 outColor;
      void main() {
        outColor = vec4(1.0f);
      }
    )
  );

  PuleError err = puleError();

  { // line renderer
    ctx.lineRenderer.shaderModule = (
      puleGfxShaderModuleCreate(vertexShaderSource, fragmentShaderSource, &err)
    );
    if (puleErrorConsume(&err) > 0) {
      return;
    }
  }
}

void puleGfxDebugShutdown() {
}

void puleGfxDebugFrameStart() {
  ctx.lineRenderer.requestedDraws = 0;
}

void puleGfxDebugRender(
  PuleGfxFramebuffer const framebuffer,
  PuleF32m44 const transform
) {
  // update GPU info
  puleGfxGpuBufferMappedFlush({
    .buffer = ctx.mappedBuffer,
    .byteOffset = 0,
    .byteLength = ctx.lineRenderer.requestedDraws*sizeof(MappedLine),
  });
  puleGfxMemoryBarrier(PuleGfxMemoryBarrierFlag_bufferUpdate);

  PuleError err = puleError();
  refreshContext(framebuffer, transform, &err);
  if (puleErrorConsume(&err) > 0) {
    return;
  }
  puleGfxCommandListSubmit(
    {
      .commandList = ctx.commandList,
      .fenceTargetStart = nullptr,
      .fenceTargetFinish = nullptr,
    },
    &err
  );
  if (puleErrorConsume(&err) > 0) {
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
