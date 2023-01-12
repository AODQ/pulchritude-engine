#include <pulchritude-renderer-3d/renderer-3d.h>

#include <pulchritude-ecs/ecs.h>
#include <pulchritude-gfx/barrier.h>
#include <pulchritude-gfx/commands.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-gfx/pipeline.h>

#include <cstring>
#include <unordered_map>
#include <vector>

namespace {

struct InternalModelInstance {
  PuleGfxPipeline pipeline;
  PuleGfxCommandList commandList;
};

struct InternalMesh {
  size_t elementVertexOffset;
  size_t elementOffset;
  size_t verticesToDispatch;
};

// provides primarily GPU buffer backing
//   At this point every 'useful modelling' abstraction such as origins and
//     normals don't explicitly exist. We have a known buffer of attribute
//     data, a known buffer of element data, but these are only hoisted to the
//     appropiate mesh shader as generic buffers.
struct InternalModel {
  PuleGfxGpuBuffer staticAttributeGpuBuffer;
  // TODO store dynamic attribute metadata (this will correlate to each
  //      instance of the model rather than being shared)
  /* PuleGfxGpuBuffer dynamicAttributeGpuBuffer; */
  PuleGfxGpuBuffer elementGpuBuffer;
  std::vector<InternalMesh> meshes;
  // TODO mesh + material shader
};

struct SystemInfo {
  PulePlatform platform;
  PuleEcsComponent componentRender;
  PuleGfxShaderModule shaderModule;

  std::unordered_map<uint64_t, InternalModel> internalModels = {};
  size_t internalModelCount = 0;
};
std::unordered_map<size_t, SystemInfo> systemToSystemInfo;

} // namespace

static void renderer3DSystemCallback(PuleEcsIterator iter) {
  SystemInfo const system = (
    systemToSystemInfo.find(puleEcsIteratorSystem(iter).id)->second
  );
  (void)system;

  InternalModelInstance * modelInstances = (
    reinterpret_cast<InternalModelInstance *>(
      puleEcsIteratorQueryComponents(iter, 0, sizeof(InternalModelInstance))
    )
  );

  size_t const entityCount = puleEcsIteratorEntityCount(iter);
  for (size_t it = 0; it < entityCount; ++ it) {
    PuleError err = puleError();
    puleGfxCommandListSubmit(
      {
        .commandList = modelInstances[it].commandList,
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

extern "C" {

PuleRenderer3D puleRenderer3DCreate(
  PuleEcsWorld const world,
  PulePlatform const platform
) {
  PuleEcsComponent const componentRender = (
    puleEcsComponentCreate(
      world, {
        .label = "Renderer3DModelInstance",
        .byteLength = sizeof(InternalModelInstance),
        .byteAlignment = alignof(InternalModelInstance),
        .imguiOverviewCallbackOptional = nullptr,
        .imguiEntityCallbackOptional = nullptr,
        .serializeComponentCallback = nullptr,
        .deserializeComponentCallback = nullptr,
      }
    )
  );

  PuleEcsSystem const system = (
    puleEcsSystemCreate(
      world,
      PuleEcsSystemCreateInfo {
        .label = "Renderer3D",
        .commaSeparatedComponentLabels = (
          "Renderer3DModelInstance"
        ),
        .callback = &renderer3DSystemCallback,
        .callbackFrequency = PuleEcsSystemCallbackFrequency_postUpdate,
      }
    )
  );

  PuleGfxShaderModule shaderModule = { 0 };
  {
    PuleError err = puleError();
    shaderModule = (
      puleGfxShaderModuleCreate(
        puleCStr("#version 450 core\n" // VERTEX
        PULE_multilineString(
          struct Attribute {
            vec4 origin;
          };
          layout(std430, binding = 0) buffer Attributes {
            Attribute attrs[];
          };
          out layout(location = 0) vec2 outUvcoord;
          void main() {
            const vec4 origin = attrs[gl_VertexID].origin;
            outUvcoord = origin.xy;
            gl_Position = origin;
          }
        )),
        puleCStr("#version 450 core\n" // FRAGMENT
        PULE_multilineString(
          in layout(location = 0) vec2 inUvcoord;
          out layout(location = 0) vec4 outColor;
          void main() {
            outColor = vec4(inUvcoord, 0.5f, 1.0f);
          }
        )),
        &err
      )
    );
    if (puleErrorConsume(&err)) {
      return { 0 };
    }
  }

  systemToSystemInfo.emplace(
    system.id,
    SystemInfo {
      .platform = platform,
      .componentRender = componentRender,
      .shaderModule = shaderModule,
    }
  );
  return PuleRenderer3D{.id = system.id,};
}

PuleEcsSystem puleRenderer3DEcsSystem(PuleRenderer3D const renderer3D) {
  return PuleEcsSystem{.id = renderer3D.id,};
}

PuleRenderer3DModel puleRenderer3DPrepareModel(
  PuleRenderer3D renderer3D,
  PuleAssetModel const assetModel
) {
  SystemInfo & system = systemToSystemInfo.find(renderer3D.id)->second;

  InternalModel model;

  // attribute format should look like
  //  <MESH0 <origin, normal, ..., origin, normal>, ..., MESHN <origin, ...>>
  // notice that not every mesh needs same attributes
  // and of course the element buffer needs to reflect this
  //  <MESH0 <0, 1, 2, ..>, MESHN<MESHN+0, MESHN+1, ...>>
  // elements are also resized to u32

  size_t globalMeshAttributeOffset = 0;
  size_t globalMeshElementOffset = 0;
  std::vector<uint8_t> attributesCollapsed;
  std::vector<uint32_t> elementsRevised;
  for (size_t meshIt = 0; meshIt < assetModel.meshCount; ++ meshIt) {
    PuleAssetMesh const & mesh = assetModel.meshes[meshIt];

    // get attribute element count in mesh
    size_t attributeElementCount = 0;
    for (
        size_t attributeIt = 0;
        attributeIt < PuleAssetMeshAttributeType_Size;
        ++ attributeIt
    ) {
      PuleAssetMeshAttribute const meshAttribute = (
        mesh.attributes[attributeIt]
      );
      if (meshAttribute.view.data == nullptr) { continue; }
      attributeElementCount = meshAttribute.view.elementCount;
      break;
    }
    PULE_assert(attributeElementCount != 0);

    // apply attribute collapsing
    for (
      size_t elementIt = 0;
      elementIt < attributeElementCount;
      ++ elementIt
    ) {
      for (
        size_t attributeIt = 0;
        attributeIt < PuleAssetMeshAttributeType_Size;
        ++ attributeIt
      ) {
        PuleAssetMeshAttribute const meshAttribute = (
          mesh.attributes[attributeIt]
        );
        if (meshAttribute.view.data == nullptr) { continue; }
        // copy just a single component (like a float4 or int3)
        size_t const componentByteLength = (
          meshAttribute.componentsPerVertex
          * (
            puleAssetMeshComponentDataTypeByteLength(
              meshAttribute.componentDataType
            )
          )
        );
        // use the attribute's stride to get the correct attribute
        uint8_t const * const componentDataPtr = (
            meshAttribute.view.data
          + (attributeIt * meshAttribute.view.elementStride)
        );
        // emplace the component
        size_t const attributeEmplacedBegin = attributesCollapsed.size();
        attributesCollapsed.resize(
          attributesCollapsed.size() + componentByteLength
        );
        memcpy(
          attributesCollapsed.data() + attributeEmplacedBegin,
          componentDataPtr,
          componentByteLength
        );
      }
    }

    puleLog(
      "vertices to dispatch %zu -- element count %zu",
      mesh.verticesToDispatch,
      mesh.element.view.elementCount
    );

    // apply element revision
    PULE_assert(mesh.verticesToDispatch == mesh.element.view.elementCount);
    for (size_t vertexIt = 0; vertexIt < mesh.verticesToDispatch; ++ vertexIt) {
      // apply element revision
      {
        uint8_t const * const elementDataPtr = (
          mesh.element.view.data + (vertexIt * mesh.element.view.elementStride)
        );
        size_t elementIdx = 0;
        switch (mesh.element.componentDataType) {
          default: PULE_assert(false);
          case PuleAssetMeshComponentDataType_u8:
            elementIdx = static_cast<uint32_t>(*elementDataPtr);
          break;
          case PuleAssetMeshComponentDataType_u16:
            elementIdx = (
              static_cast<uint32_t>(
                *reinterpret_cast<uint16_t const *>(elementDataPtr)
              )
            );
          break;
          case PuleAssetMeshComponentDataType_u32:
            elementIdx = *reinterpret_cast<uint32_t const *>(elementDataPtr);
          break;
        }
        elementsRevised.emplace_back(elementIdx);
      }
    }
    // apply the attribute offset for element
    model.meshes.emplace_back(
      InternalMesh {
        .elementVertexOffset = globalMeshAttributeOffset,
        .elementOffset = globalMeshElementOffset,
        .verticesToDispatch = mesh.verticesToDispatch,
      }
    );

    // apply global offsets
    globalMeshAttributeOffset += attributeElementCount;
    globalMeshElementOffset += mesh.verticesToDispatch;
  }

  model.staticAttributeGpuBuffer = (
    puleGfxGpuBufferCreate(
      attributesCollapsed.data(),
      attributesCollapsed.size() * sizeof(attributesCollapsed[0]),
      PuleGfxGpuBufferUsage_bufferStorage,
      PuleGfxGpuBufferVisibilityFlag_deviceOnly
    )
  );
  model.elementGpuBuffer = (
    puleGfxGpuBufferCreate(
      elementsRevised.data(),
      elementsRevised.size() * sizeof(elementsRevised[0]),
      PuleGfxGpuBufferUsage_bufferElement,
      PuleGfxGpuBufferVisibilityFlag_deviceOnly
    )
  );

  auto const retModel = PuleRenderer3DModel {.id = ++system.internalModelCount};
  system.internalModels.emplace(retModel.id, model);
  puleLog("internal model :: %zu", retModel.id);
  return retModel;
}

PuleEcsComponent puleRenderer3DAttachComponentRender(
  PuleEcsWorld const world,
  PuleRenderer3D const renderer3DSystem,
  PuleEcsEntity const entity,
  PuleRenderer3DModel const model
) {
  puleLog("looking for system %zu", renderer3DSystem.id);
  SystemInfo const system = (
    systemToSystemInfo.find(renderer3DSystem.id)->second
  );
  puleLog("model %zu", model.id);
  InternalModel const modelInternal = (
    system.internalModels.find(model.id)->second
  );

  auto modelInstance = InternalModelInstance {
    .pipeline = PuleGfxPipeline { .id = 0 },
    .commandList = (
      puleGfxCommandListCreate(
        puleAllocateDefault(),
        puleCStr("model-instance")
      )
    ),
  };

  {
    auto descriptorSetLayout = puleGfxPipelineDescriptorSetLayout();
    descriptorSetLayout.bufferElementBinding = modelInternal.elementGpuBuffer;
    descriptorSetLayout.bufferStorageBindings[0] = (
      modelInternal.staticAttributeGpuBuffer
    );
    puleLog("element gpu buffer: %zu\n", modelInternal.elementGpuBuffer.id);
    puleLog("storage gpu buffer: %zu\n", modelInternal.staticAttributeGpuBuffer.id);
    // TODO dynamic + custom bindings

    auto pipelineInfo = PuleGfxPipelineCreateInfo {
      .shaderModule = system.shaderModule,
      .framebuffer = puleGfxFramebufferWindow(),
      .layout = &descriptorSetLayout,
      .config = {
        .blendEnabled = false,
        .scissorTestEnabled = false,
        .viewportUl = PuleI32v2 { 0, 0, },
        .viewportLr = pulePlatformWindowSize(system.platform),
        .scissorUl = PuleI32v2 { 0, 0, },
        .scissorLr = pulePlatformWindowSize(system.platform),
      },
    };

    PuleError err = puleError();
    modelInstance.pipeline = (puleGfxPipelineCreate(&pipelineInfo, &err));
    if (puleErrorConsume(&err) > 0) {
      return { 0 };
    }
  }

  { // record command list
    auto commandListRecorder = (
      puleGfxCommandListRecorder(modelInstance.commandList)
    );

    puleGfxCommandListAppendAction(
      commandListRecorder,
      PuleGfxCommand {
        .bindPipeline = {
          .action = PuleGfxAction_bindPipeline,
          .pipeline = modelInstance.pipeline,
        },
      }
    );
    for (auto const & mesh : modelInternal.meshes) {
      puleGfxCommandListAppendAction(
        commandListRecorder,
        PuleGfxCommand {
          .dispatchRenderElements = {
            .action = PuleGfxAction_dispatchRenderElements,
            .drawPrimitive = PuleGfxDrawPrimitive_triangle,
            .numElements = mesh.verticesToDispatch,
            .elementType = PuleGfxElementType_u32,
            .elementOffset = mesh.elementOffset,
            .baseVertexOffset = mesh.elementVertexOffset,
          },
        }
      );
    }

    puleGfxCommandListRecorderFinish(commandListRecorder);
  }

  puleEcsEntityAttachComponent(
    world, entity, system.componentRender, &modelInstance
  );

  return system.componentRender;
}

} // C
