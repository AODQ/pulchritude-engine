#include <pulchritude-gfx-debug/gfx-debug.h>

namespace {

struct ShapeRenderer {
  PuleGfxShaderModule shaderModule { 0 };
  PuleGfxPipeline pipeline { 0 };
  size_t requestedDraws = 0;
};

struct Context {
  PuleGfxGpuBuffer mappedBuffer { 0 };
  uint8_t * mappedBufferContents = nullptr;
  size_t bufferByteLength { 0 };
  size_t lineCountCurrent { 0 };
  size_t lineCountRequested { 0 };
  size_t lineCountMax { 0 };
  ShapeRenderer lineRenderer { 0 };
  PuleGfxCommandList commandList { 0 };
};
Context ctx { };

void refreshContext(
  PuleGfxFramebuffer const framebuffer,
  PuleF32m44 const transform,
  PuleError * const err
) {
  { // line pipeline layout
    auto descriptorSetLayout = puleGfxPipelineDescriptorSetLayout();
    descriptorSetLayout.bufferAttributeBindings[0] = {
      .buffer = ctx.mappedBuffer,
      .numComponents = 2,
      .dataType = PuleGfxAttributeDataType_float,
      .normalizeFixedDataTypeToNormalizedFloating = false,
      .stridePerElement = sizeof(float)*2,
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
        .viewportLr = puleGfxFramebufferDimensions(1, 1),
        .scissorUL = PuleI32v2 { 0, 0, },
        .scissorLr = puleGfxFramebufferDimensions(1, 1),
      },
    };
    puleGfxPipelineDestroy(ctx.lineRenderer.pipeline);
    ctx.lineRenderer.pipeline = puleGfxPipelineCreate(&pipelineInfo, err);
    if (puleErrorExists(err)) {
      return;
    }
  }

  puleGfxCommandListDestroy(ctx.commandList);
  ctx.commandList = puleGfxCommandListCreate(puleAllocateDefault());
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
          .pipeline = context.pipeline,
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
  ctx.mappedBufferContents = (
    puleGfxGpuBufferMap( PuleGfxGpuBufferMapRange {
      .buffer = ctx.mappedBuffer,
      .access = PuleGfxGpuBufferMapAccess_hostWritable,
      .byteOffset = 0,
      .byteLength = ctx.bufferByteLength,
    })
  );

  char const * const vertexShaderSource = (
    PULE_multilineString(
      uniform layout(binding = 0) mat4 transform;
      in layout(location = 0) vec2 attributeOrigin;
      out layout(location = 0) flat int outVertexId;
      void main() {
        gl_Position = transform * vec4(attributeOrigin.xy, 0.0f, 1.0f);
        outVertexId = gl_VertexID;
      }
    ),
  );

  PuleError err = puleError();

  { // line renderer
    ctx.lineRenderer.shaderModule = (
      puleGfxShaderModuleCreate(
        vertexShaderSource,
        PULE_multilineString(
          layout(std430, binding = 0) buffer Attributes {
            vec4 colors[];
          };
          in layout(location = 0) flat int inVertexId;
          out layout(location = 0) vec4 outColor;
          void main() {
            outColor = colors[inVertexId/2];
          }
        )
      )
    );
    if (puleErrorConsume(&err) > 0) {
      return;
    }
  }
}

void puleGfxShutdown() {
}

PULE_exportFn void puleGfxDebugRender(
  PuleGfxFramebuffer const framebuffer,
  PuleF32m44 const transform
) {
  puleGfxGpuBufferMappedFlush({
    .buffer = ctx.mappedBuffer,
    .byteOffset = 0,
    .byteLength = ctx.lineCountCurrent*sizeof(float)*2,
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

} // C
