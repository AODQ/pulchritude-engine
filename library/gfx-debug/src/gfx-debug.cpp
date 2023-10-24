#include <pulchritude-gfx-debug/gfx-debug.h>

#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/shader-module.h>
#include <pulchritude-gpu/synchronization.h>

#include <string>
#include <vector>

namespace in {

struct DebugRecorder {
  PuleGpuCommandListRecorder commandListRecorder;
  PuleF32m44 transform;
  bool initialized;
  PuleGfxDebugRenderType previousRenderType;
};

pule::ResourceContainer<in::DebugRecorder> debugRecorder;

PuleGpuPipeline linePipeline;
PuleGpuShaderModule lineShaderModule;


} // namespace in

extern "C" {

void puleGfxDebugInitialize(PulePlatform const platform) {

  #include "autogen-debug-gfx-line.frag.spv"
  #include "autogen-debug-gfx-line.vert.spv"

  PuleError err = puleError();

  std::string vertexShaderSource = "";
  std::string fragmentShaderSource = "";

  { // line renderer
    in::lineShaderModule = (
      puleGpuShaderModuleCreate(
        PuleBufferView {
          .data = debugGfxLineVert,
          .byteLength = sizeof(debugGfxLineVert),
        },
        PuleBufferView {
          .data = debugGfxLineFrag,
          .byteLength = sizeof(debugGfxLineFrag),
        },
        &err
      )
    );
    if (puleErrorConsume(&err)) {
      return;
    }

    auto layoutDescriptorSet = puleGpuPipelineDescriptorSetLayout();

    auto pushConstant = (
      PuleGpuPipelineLayoutPushConstant {
        .stage = PuleGpuDescriptorStage_vertex,
        .byteLength = sizeof(float)*4,
        .byteOffset = 0,
      }
    );

    auto pipelineInfo = (
      PuleGpuPipelineCreateInfo {
        .shaderModule = in::lineShaderModule,
        .layoutDescriptorSet = &layoutDescriptorSet,
        .layoutPushConstants = &pushConstant,
        .layoutPushConstantsCount = 1,
        .config = {
          .depthTestEnabled = false,
          .blendEnabled = false,
          .scissorTestEnabled = false,
          .viewportMin = {0, 0},
          .viewportMax = {800, 600}, // TODO FIX
          .scissorMin = {0, 0},
          .scissorMax = {0, 0},
        },
      }
    );

    in::linePipeline = (
      puleGpuPipelineCreate(
        &pipelineInfo,
        &err
      )
    );

    if (puleErrorConsume(&err)) {
      return;
    }
  }
}

void puleGfxDebugShutdown() {
}

PuleGfxDebugRecorder puleGfxDebugStart(
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleGpuActionRenderPassBegin const renderPassBegin,
  PuleF32m44 const transform
) {
  puleGpuCommandListAppendAction(
    commandListRecorder,
    PuleGpuCommand {
      .renderPassBegin = renderPassBegin,
    }
  );
  return PuleGfxDebugRecorder {
    .id = in::debugRecorder.create(
      in::DebugRecorder {
        .commandListRecorder = commandListRecorder,
        .transform = transform,
        .initialized = false,
        .previousRenderType = PuleGfxDebugRenderType_line,
      }
    )
  };
}

void puleGfxDebugEnd(PuleGfxDebugRecorder const recorder) {
  puleGpuCommandListAppendAction(
    in::debugRecorder.at(recorder.id)->commandListRecorder,
    PuleGpuCommand {
      .renderPassEnd = {},
    }
  );
  in::debugRecorder.destroy(recorder.id);
}

void puleGfxDebugRender(
  PuleGfxDebugRecorder const puDebugRecorder,
  PuleGfxDebugRenderParam const param
) {
  in::DebugRecorder & debugRecorder = (
    *in::debugRecorder.at(puDebugRecorder.id)
  );
  // bind pipeline if needed
  if (
       !debugRecorder.initialized
    || debugRecorder.previousRenderType != param.type
  ) {
    switch (param.type) {
      case PuleGfxDebugRenderType_line:
      case PuleGfxDebugRenderType_quad: // use same pipeline for now
      {
        puleGpuCommandListAppendAction(
          debugRecorder.commandListRecorder,
          PuleGpuCommand {
            .bindPipeline = {
              .action = PuleGpuAction_bindPipeline,
              .pipeline = in::linePipeline,
            }
          }
        );
      } break;
    }
  }
  debugRecorder.previousRenderType = param.type;

  // scissor
  puleGpuCommandListAppendAction(
    debugRecorder.commandListRecorder,
    {
      .setScissor = {
        .action = PuleGpuAction_setScissor,
        .scissorMin = { .x = 0, .y = 0, },
        .scissorMax = { .x = 800, .y = 600, },
      },
    }
  );

  // bind push constants
  switch (param.type) {
    default:
      puleLogError("Unimplemented debug render type %d", param.type);
      PULE_assert(false && "unimplemented");
    case PuleGfxDebugRenderType_quad: {
      PuleF32v2 const ul = (
        PuleF32v2 {
          param.quad.originCenter.x - param.quad.dimensionsHalf.x,
          param.quad.originCenter.y - param.quad.dimensionsHalf.y
        }
      );
      PuleF32v2 const ur = (
        PuleF32v2 {
          param.quad.originCenter.x + param.quad.dimensionsHalf.x,
          param.quad.originCenter.y - param.quad.dimensionsHalf.y
        }
      );
      PuleF32v2 const ll = (
        PuleF32v2 {
          param.quad.originCenter.x - param.quad.dimensionsHalf.x,
          param.quad.originCenter.y + param.quad.dimensionsHalf.y
        }
      );
      PuleF32v2 const lr = (
        PuleF32v2 {
          param.quad.originCenter.x + param.quad.dimensionsHalf.x,
          param.quad.originCenter.y + param.quad.dimensionsHalf.y
        }
      );
      std::vector<PuleF32v2> vertices = {
        ul, ur, ur, lr, lr, ll, ll, ul,
      };
      for (size_t it = 0; it < 4; ++ it) {
        puleGpuCommandListAppendAction(
          debugRecorder.commandListRecorder,
          PuleGpuCommand {
            .pushConstants = {
              .stage = PuleGpuDescriptorStage_vertex,
              .byteLength = sizeof(float)*4,
              .byteOffset = 0,
              .data = &vertices[it*2],
            }
          }
        );
        puleGpuCommandListAppendAction(
          debugRecorder.commandListRecorder,
          PuleGpuCommand {
            .dispatchRender = {
              .drawPrimitive = PuleGpuDrawPrimitive_line,
              .vertexOffset = 0,
              .numVertices = 2,
            }
          }
        );
      }
    }
    case PuleGfxDebugRenderType_line: {
      float const vertices[4] = {
        param.line.a.x,
        param.line.a.y,
        param.line.b.x,
        param.line.b.y,
      };
      puleGpuCommandListAppendAction(
        debugRecorder.commandListRecorder,
        PuleGpuCommand {
          .pushConstants = {
            .stage = PuleGpuDescriptorStage_vertex,
            .byteLength = sizeof(float)*4,
            .byteOffset = 0,
            .data = vertices,
          }
        }
      );

      // draw
      puleGpuCommandListAppendAction(
        debugRecorder.commandListRecorder,
        PuleGpuCommand {
          .dispatchRender = {
            .drawPrimitive = PuleGpuDrawPrimitive_line,
            .vertexOffset = 0,
            .numVertices = 2,
          }
        }
      );
    } break;
  }

}


} // C
