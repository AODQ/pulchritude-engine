//#include <pulchritude-renderer-3d/renderer-3d.h>
//
//#include <pulchritude-ecs/ecs.h>
//#include <pulchritude-gpu/commands.h>
//#include <pulchritude-gpu/gpu.h>
//#include <pulchritude-gpu/image.h>
//#include <pulchritude-gpu/pipeline.h>
//#include <pulchritude-gpu/shader-module.h>
//#include <pulchritude-gpu/synchronization.h>
//
//#include <cstring>
//#include <string>
//#include <unordered_map>
//#include <vector>
//
//// -- materials ----------------------------------------------------------------
//
//namespace pint {
//
//struct Material {
//  PuleGpuShaderModule shaderModule;
//};
//
//pule::ResourceContainer<pint::Material> materials;
//PuleRenderer3DMaterial defaultMaterial;
//
//} // namespace pint
//
//extern "C" {
//
//PuleRenderer3DMaterial puleRenderer3DMaterialCreateDefault() {
//  #include "autogen-material-default.vert.spv"
//  #include "autogen-material-default.frag.spv"
//
//  if (pint::defaultMaterial.id != 0) {
//    return pint::defaultMaterial;
//  }
//
//  PuleError error = puleError();
//
//  pint::Material material;
//  material.shaderModule = (
//    puleGpuShaderModuleCreate(
//      PuleBufferView {
//        .data = materialDefaultVert,
//        .byteLength = sizeof(materialDefaultVert),
//      },
//      PuleBufferView {
//        .data = materialDefaultFrag,
//        .byteLength = sizeof(materialDefaultFrag),
//      },
//      &error
//    )
//  );
//  if (puleErrorConsume(&error)) {
//    puleLogError("failed to create default shader module");
//    return PuleRenderer3DMaterial { .id = 0 };
//  }
//  pint::defaultMaterial = { .id = pint::materials.create(material), };
//  return pint::defaultMaterial;
//}
//
//} // extern "C"
//
//// -- model renderer -----------------------------------------------------------
//
//namespace pint {
//
//// TODO this should just be a single enum
//PuleGpuAttributeDataType componentDataType(
//  PuleAssetMeshAttributeComponentDataType const type
//) {
//  switch (type) {
//    case PuleAssetMeshAttributeComponentDataType_u8:
//      return PuleGpuAttributeDataType_unsignedByte;
//    case PuleAssetMeshAttributeComponentDataType_f32:
//      return PuleGpuAttributeDataType_f32;
//    case PuleAssetMeshAttributeComponentDataType_u16:
//    case PuleAssetMeshAttributeComponentDataType_u32:
//    case PuleAssetMeshAttributeComponentDataType_i8:
//    case PuleAssetMeshAttributeComponentDataType_i16:
//    case PuleAssetMeshAttributeComponentDataType_i32:
//    case PuleAssetMeshAttributeComponentDataType_f16:
//      PULE_assert(false && "unsupported");
//  }
//  return PuleGpuAttributeDataType_float;
//}
//
//struct ModelRendererAttribute {
//  size_t bufferIndex;
//  size_t byteOffset;
//  size_t byteStride;
//};
//
//struct ModelRendererMesh {
//  size_t verticesToDispatch;
//  size_t elementBufferIndex;
//  size_t elementBufferByteOffset;
//  std::vector<ModelRendererAttribute> attributes;
//};
//
//struct ModelRenderer {
//  PuleGpuPipeline pipeline;
//  std::vector<PuleGpuBuffer> buffers;
//  std::vector<pint::ModelRendererMesh> meshes;
//};
//
//pule::ResourceContainer<
//  pint::ModelRenderer, PuleRenderer3DModel
//> modelRenderers;
//
//
//} // namespace pint
//
//extern "C" {
//
//PuleRenderer3DModel puleRenderer3DModelCreate(
//  PuleRenderer3DModelCreateInfo const createInfo
//) {
//  PuleError err = puleError();
//
//  // -- create pipeline
//
//  auto layoutDescriptorSet = puleGpuPipelineDescriptorSetLayout();
//
//  for (size_t meshIt = 0; meshIt < createInfo.meshCount; ++ meshIt) {
//    auto & mesh = createInfo.meshes[meshIt];
//    for (size_t attr = 0; attr < PuleAssetMeshAttributeType_size; ++ attr) {
//      auto & attribute = mesh.attributes[attr];
//      layoutDescriptorSet.attributeBindings[attr] = {
//        .dataType = pint::componentDataType(attribute.componentDataType),
//        .bufferIndex = attr,
//        .numComponents = attribute.componentsPerVertex,
//        .convertFixedDataTypeToNormalizedFloating = false,
//        .offsetIntoBuffer = attribute.bufferByteOffset,
//      };
//      layoutDescriptorSet.attributeBufferBindings[attr] = {
//        .stridePerElement = attribute.bufferByteStride,
//      };
//    }
//  }
//
//  PuleGpuPipeline pipeline = (
//    puleGpuPipelineCreate({
//      .shaderModule = pint::materials.at(createInfo.material.id)->shaderModule,
//      .layoutDescriptorSet = layoutDescriptorSet,
//      .layoutPushConstants = nullptr,
//      .layoutPushConstantsCount = 0,
//      .config = {
//        .depthTestEnabled = false, // TODO true
//        .blendEnabled = false,
//        .scissorTestEnabled = false,
//        .viewportMin = {0, 0},
//        .viewportMax = {800, 600},
//        .scissorMin = {0, 0},
//        .scissorMax = {800, 600},
//        .drawPrimitive = PuleGpuDrawPrimitive_triangle,
//        .colorAttachmentCount = 1, // TODO
//        .colorAttachmentFormats = {
//          PuleGpuImageByteFormat_bgra8U,
//        },
//      },
//    }, &err)
//  );
//  if (puleErrorConsume(&err)) {
//    return { .id = 0, };
//  }
//
//  // -- upload buffers to gpu
//  std::vector<PuleGpuBuffer> buffers;
//  for (size_t bufferIt = 0; bufferIt < createInfo.bufferCount; ++ bufferIt) {
//    // determine its usage/visiblity flags
//    int32_t bufferUsage = 0;
//    for (size_t meshIt = 0; meshIt < createInfo.meshCount; ++ meshIt) {
//      auto & mesh = createInfo.meshes[meshIt];
//      if (mesh.element.bufferIndex == bufferIt) {
//        bufferUsage |= (int32_t)PuleGpuBufferUsage_element;
//      }
//      for (size_t attr = 0; attr < PuleAssetMeshAttributeType_size; ++ attr) {
//        auto & attribute = mesh.attributes[attr];
//        if (attribute.bufferIndex == bufferIt) {
//          bufferUsage |= (int32_t)PuleGpuBufferUsage_attribute;
//          break;
//        }
//      }
//    }
//    // TODO when API supports ability to have dynamic buffers
//    PuleGpuBufferVisibilityFlag visibility = (
//      PuleGpuBufferVisibilityFlag_deviceOnly
//    );
//    // create buffer
//    auto & buffer = createInfo.buffers[bufferIt];
//    PuleString label = (
//      puleStringFormatDefault(
//        "model-buffer-%s-%zu",
//        createInfo.label.contents, bufferIt
//      )
//    );
//    puleScopeExit {
//      puleStringDestroy(label);
//    };
//    buffers.emplace_back(
//      puleGpuBufferCreate(
//        puleStringView(label),
//        buffer.data,
//        buffer.byteLength,
//        (PuleGpuBufferUsage)bufferUsage,
//        visibility
//      )
//    );
//  }
//
//  // -- get additional draw data
//  std::vector<pint::ModelRendererMesh> meshes;
//  for (size_t it = 0; it < createInfo.meshCount; ++ it) {
//    auto & mesh = createInfo.meshes[it];
//    pint::ModelRendererMesh mrMesh;
//    mrMesh.verticesToDispatch = mesh.verticesToDispatch;
//    mrMesh.elementBufferIndex = mesh.element.bufferIndex;
//    mrMesh.elementBufferByteOffset = mesh.element.bufferByteOffset;
//    for (size_t attr = 0; attr < PuleAssetMeshAttributeType_size; ++ attr) {
//      auto & attribute = mesh.attributes[attr];
//      mrMesh.attributes.push_back({
//        .bufferIndex = attribute.bufferIndex,
//        .byteOffset = attribute.bufferByteOffset,
//        .byteStride = attribute.bufferByteStride,
//      });
//    }
//    meshes.push_back(std::move(mrMesh));
//  }
//
//  return pint::modelRenderers.create({
//    .pipeline = pipeline,
//    .buffers = std::move(buffers),
//    .meshes = std::move(meshes),
//  });
//}
//
//void puleRenderer3DDestroyModel(PuleRenderer3DModel const model) {
//  if (model.id == 0) { return; }
//  pint::modelRenderers.destroy(model);
//}
//
//void puleRenderer3DModelRender(
//  PuleGpuCommandListRecorder const commandListRecorder,
//  PuleRenderer3DModel const puModel,
//  PuleF32m44 const transform
//) {
//  auto & model = *pint::modelRenderers.at(puModel);
//
//  puleGpuCommandListAppendAction(commandListRecorder, {
//    .bindPipeline = {
//      .action = PuleGpuAction_bindPipeline,
//      .pipeline = model.pipeline,
//    },
//  });
//  puleGpuCommandListAppendAction(commandListRecorder, {
//    .setScissor = {
//      .scissorMin = { .x = 0, .y = 0, },
//      .scissorMax = { .x = 800, .y = 600, },
//    },
//  });
//  for (auto & mesh : model.meshes) {
//    for (size_t attr = 0; attr < mesh.attributes.size(); ++ attr) {
//      auto & attribute = mesh.attributes[attr];
//      puleGpuCommandListAppendAction(commandListRecorder, {
//        .bindAttributeBuffer = {
//          .bindingIndex = attr,
//          .buffer = model.buffers[attribute.bufferIndex],
//          .offset = 0,
//          .stride = attribute.byteStride,
//        },
//      });
//      puleGpuCommandListAppendAction(commandListRecorder, {
//        .bindElementBuffer = {
//          .buffer = model.buffers[mesh.elementBufferIndex],
//          .offset = mesh.elementBufferByteOffset,
//          .elementType = PuleGpuElementType_u32,
//        }
//      });
//      puleGpuCommandListAppendAction(commandListRecorder, {
//        .dispatchRenderElements = {
//          .numElements = mesh.verticesToDispatch,
//          .elementOffset = 0,
//          .baseVertexOffset = 0,
//        }
//      });
//    }
//  }
//}
//
//} // extern "C"
///*
//  puleGpuCommandListAppendAction(
//    command,
//    PuleGpuCommand {
//      .renderPassEnd = { .action = PuleGpuAction_renderPassEnd, },
//    }
//  );
//*/
