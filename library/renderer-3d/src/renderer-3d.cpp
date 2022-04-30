#include <pulchritude-renderer-3d/renderer-3d.h>

#include <pulchritude-gfx/gfx.h>

#include <unordered_map>
#include <vector>


namespace {

using Position = PuleF32v2;
struct RenderInfo {
  PuleGfxPipeline pipeline;
  PuleGfxGpuBuffer modelBuffer;
  PuleGfxCommandList commandList;
};

struct SystemInfo {
  PuleEcsComponent componentRender;
  PuleGfxShaderModule shaderModule;
};
std::unordered_map<size_t, SystemInfo> systemToSystemInfo;

} // namespace

static void renderer3DSystemCallback(PuleEcsIterator iter) {
  SystemInfo const system = (
    *systemToSystemInfo.find(puleEcsIteratorSystem(iter))
  );

  RenderInfo * renderInfos = (
    reinterpret_cast<RenderInfo *>(
      puleEcsIteratorQueryComponents(iter, 0, sizeof(RenderInfo))
    )
  );

  Position const * positions = (
    reinterpret_cast<Position *>(
      puleEcsIteratorQueryComponents(iter, 1, sizeof(Position))
    )
  );

  size_t const entityCount = puleEcsIteratorEntityCount(iter);
  for (size_t it = 0; it < entityCount; ++ it) {
    PuleError err = puleError();
    puleGfxCommandListSubmit(
      {
        .commandList = renderInfos[it].commandList,
        .fenceTargetStart = nullptr,
        .fenceTargetFinish = nullptr,
      },
      &err
    );

    if (puleErrorConsume(&err) > 0) {
      continue;
    }
  }
}

PuleEcsSystem puleRender3DSystem(
  PuleEcsWorld const world,
  PuleGfxContext const gfx
) {
  PuleEcsSystem const system = (
    puleEcsSystemCreate(
      world,
      PuleEcsSystemCreateInfo {
        .label = "Renderer3D",
        .commaSeparatedComponentLabels = "Render3D, Position",
        .callback = &renderer3DSystemCallback,
        .callbackFrequency = PuleEcsSystemCallbackFrequency_postUpdate,
      }
    )
  );
  PuleEcsComponent const componentRender = (
    puleEcsComponentCreate(
      world, {
        .label = "Render3D",
        .byteLength = sizeof(RenderInfo),
        .byteAlignment = alignof(RenderInfo),
      }
    )
  );

  PuleGfxShaderModule shaderModule = { 0 };
  {
    PuleError err = puleError();
    shaderModule = (
      puleGfxShaderModuleCreate(
        SHADER( // VERTEX
          layout(std430, binding = 0) buffer AttributeOrigin {
            vec4 origins[];
          };
          layout(std430, binding = 0) buffer AttributeUvcoord {
            vec2 uvcoords[];
          };
          out layout(location = 0) vec2 outUvcoord;
          void main() {
            const vec4 origin = origins[gl_VertexID%6];
            outUvcoord = uvcoords[gl_vertexID%6];
            gl_Position = origin;
          }
        ),
        SHADER( // FRAGMENT
          in layout(location = 0) vec2 inUvcoord;
          out layout(location = 0) vec4 outColor;
          void main() {
            outColor = vec4(inUvcoord, 0.5f, 1.0f);
          }
        ),
        &err
      )
    );
    if (puleErrorConsume(&err)) {
      return { 0 };
    }
  }

  systemToSystemInfo.emplace(
    system.id,
    SystemInfo { .componentRender = commandList, }
  );
  return system;
}

PuleEcsComponent puleRenderer3DAttachComponentRender(
  PuleEcsWorld const world,
  PuleEcsSystem const renderer3DSystem,
  PuleEcsEntity const entity,
  PuleAssetModel const model
) {
  SystemInfo const system = (
    *systemToSystemInfo.find(puleEcsIteratorSystem(iter))
  );

  float testBuffer[] = {
    0.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
  };
  auto renderInfo = RenderInfo {
    .modelBuffer = (
      puleGfxGpuBufferCreate(
        &testBuffer,
        PULE_arraySize(testBuffer)*sizeof(float)
        PuleGfxGpuBufferUsage_bufferStorage,
        PuleGfxGpuBufferVisibilityFlag_deviceOnly
      )
    ),
    .commandList = puleGfxCommandListCreate(puleAllocateDefault()),
    .pipeline = 0,
  };

  PuleGfxPipeline pipeline = { 0 };
  {
    auto descriptorSetLayout = puleGfxPipelineDescriptorSetLayout();
    descriptorSetLayout.bufferStorageBindings[0] = renderInfo.modelBuffer;

    auto pipelineInfo = PuleGfxPipelineCreateInfo {
      .shaderModule = system.shaderModule,
      .framebuffer = puleGfxFramebufferWindow(),
      .layout = &descriptorSetLayout,
      .config = {
        .blendEnabled = false,
        .scissorTestEnabled = false,
        .viewportUl = PuleI32v2 { 0, 0, },
        .viewportLr = pulePlatformWindowSize(context.platform),
        .scissorUl = PuleI32v2 { 0, 0, },
        .scissorLr = pulePlatformWindowSize(context.platform),
      },
    };

    PuleError err = puleError();
    context.pipeline = (puleGfxPipelineCreate(&pipelineInfo, &err));
    if (puleErrorConsume(&err) > 0) {
      return { 0 };
    }
  }

  { // record command list
    auto commandListRecorder = (
      puleGfxCommandListRecorder(renderInfo.commandList)
    );

    puleGfxCommandListAppendAction(
      commandListRecorder,
      PuleGfxCommand {
        .bindPipeline = {
          .action = PuleGfxAction_bindPipeline,
          .pipeline = renderInfo.pipeline,
        },
      }
    );
    PuleGfxCommandListAppendAction(
      commandListRecorder,
      PuleGfxCommand {
        .dispatchRender = {
          .action = PuleGfxAction_dispatchRender,
          .drawPrimitive = PuleGfxDrawPrimitive_triangle,
          .vertexOffset = 0,
          .numVertices = PULE_arraySize(testBuffer),
        },
      }
    );

    puleGfxCommandListRecorderFinish(commandListRecorder);
  }

  puleEcsEntityAttachComponent(world, entity, renderer3DSystem, &renderInfo);
}
