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
        .byteLength = sizeof(float)*8,
        .byteOffset = 0,
      }
    );

    auto pipelineInfo = (
      PuleGpuPipelineCreateInfo {
        .shaderModule = in::lineShaderModule,
        .layoutDescriptorSet = layoutDescriptorSet,
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
          .drawPrimitive = PuleGpuDrawPrimitive_line,
          .colorAttachmentCount = 1,
          .colorAttachmentFormats = {
            PuleGpuImageByteFormat_rgba8U,
          },
          .depthAttachmentFormat = PuleGpuImageByteFormat_undefined,
        },
      }
    );

    in::linePipeline = puleGpuPipelineCreate(pipelineInfo, &err);

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
      PuleF32v2 const ul = (PuleF32v2 { -1.0f, -1.0f, });
      PuleF32v2 const ur = (PuleF32v2 { +1.0f, -1.0f, });
      PuleF32v2 const ll = (PuleF32v2 { -1.0f, +1.0f, });
      PuleF32v2 const lr = (PuleF32v2 { +1.0f, +1.0f, });
      std::vector<PuleF32v2> vertices = { ul, ur, ur, lr, lr, ll, ll, ul, };
      // create rotation matrix
      PuleF32m44 const rotation = (
        puleF32m44Rotation(param.quad.angle, PuleF32v3 { 0.0f, 0.0f, 1.0f, })
      );
      // apply rotation
      for (auto & vertex : vertices) {
        // rotate point
        PuleF32v4 vtx = PuleF32v4{vertex.x, vertex.y, 0.0f, 1.0f};
        vtx = puleF32m44MulV4(rotation, vtx);
        vertex.x = vtx.x;
        vertex.y = vtx.y;
        // scale
        vertex.x *= param.quad.dimensionsHalf.x;
        vertex.y *= param.quad.dimensionsHalf.y;
        // add origin
        vertex = puleF32v2Add(vertex, param.quad.originCenter);
      }
      for (size_t it = 0; it < 4; ++ it) {
        std::vector<PuleF32v4> data = {
          PuleF32v4 {
            vertices[it*2].x, vertices[it*2].y,
            vertices[it*2+1].x, vertices[it*2+1].y
          },
          PuleF32v4 {
            param.line.color.x, param.line.color.y, param.line.color.z, 1.0f,
          },
        };
        puleGpuCommandListAppendAction(
          debugRecorder.commandListRecorder,
          PuleGpuCommand {
            .pushConstants = {
              .stage = PuleGpuDescriptorStage_vertex,
              .byteLength = sizeof(float)*8,
              .byteOffset = 0,
              .data = data.data(),
            }
          }
        );
        puleGpuCommandListAppendAction(
          debugRecorder.commandListRecorder,
          PuleGpuCommand {
            .dispatchRender = {
              .vertexOffset = 0,
              .numVertices = 2,
            }
          }
        );
      }
    }
    break;
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
            .vertexOffset = 0,
            .numVertices = 2,
          }
        }
      );
    } break;
  }

}


} // C
