#include <pulchritude-gfx/module.h>

#include <util.hpp>

#include <pulchritude-log/log.h>

#include <volk.h>

#include <array>
#include <string>
#include <vector>

namespace {

void createPipelineVertexInputState(
  PuleGfxPipelineCreateInfo const * const info,
  std::vector<VkVertexInputBindingDescription> & vtxBuffers,
  std::vector<VkVertexInputAttributeDescription> & vtxAttributes
) {
  // attribute bindings
  for (size_t it = 0; it < puleGfxPipelineDescriptorMax_attribute; ++ it) {
    auto const binding = info->layout->attributeBindings[it];
    if (binding.numComponents == 0) {
      continue;
    }
    puleLog("Attribute binding %zu location %zu binding %zu", it, it, binding.bufferIndex);
    vtxAttributes.emplace_back( VkVertexInputAttributeDescription {
      .location = (uint32_t)it,
      .binding = (uint32_t)binding.bufferIndex,
      .format = (
        util::toVkBufferFormat(
          binding.dataType,
          binding.numComponents,
          binding.convertFixedDataTypeToNormalizedFloating
        )
      ),
      .offset = (uint32_t)binding.offsetIntoBuffer,
    });
  }
  // attribute buffer bindings
  for (size_t it = 0; it < puleGfxPipelineDescriptorMax_attribute; ++ it) {
    auto const binding = info->layout->attributeBufferBindings[it];
    if (binding.stridePerElement == 0) {
      continue;
    }
    puleLog("Attribute buffer binding %zu stride %zu", it, binding.stridePerElement);
    vtxBuffers.emplace_back( VkVertexInputBindingDescription {
      .binding = (uint32_t)it,
      .stride = (
        (uint32_t)binding.stridePerElement
      ),
      .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    });
  }
}

VkPipelineLayout createPipelineLayout(
  PuleGfxPipelineCreateInfo const * const info,
  VkDescriptorSetLayout & descriptorSetLayout
) {
  auto setLayouts = std::vector<VkDescriptorSetLayout> { };
  auto descriptorSetLayoutBindings = (
    std::vector<VkDescriptorSetLayoutBinding>{}
  );
  // compactStorage buffers
  for (size_t it = 0; it < puleGfxPipelineDescriptorMax_uniform; ++ it) {
    auto bufferStage = info->layout->bufferUniformBindings[it];
    if (bufferStage == 0) { continue; }
    descriptorSetLayoutBindings.emplace_back(VkDescriptorSetLayoutBinding {
      .binding = (uint32_t)it,
      .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      .descriptorCount = 1,
      .stageFlags = util::toVkShaderStageFlags(bufferStage),
      .pImmutableSamplers = nullptr,
    });
  }
  // storage buffers
  for (size_t it = 0; it < puleGfxPipelineDescriptorMax_storage; ++ it) {
    auto bufferStage = info->layout->bufferStorageBindings[it];
    if (bufferStage == 0) { continue; }
    descriptorSetLayoutBindings.emplace_back(VkDescriptorSetLayoutBinding {
      .binding = (uint32_t)it,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .stageFlags = util::toVkShaderStageFlags(bufferStage),
      .pImmutableSamplers = nullptr,
    });
  }
  // TODO::CRIT how to bind images?
  for (
    size_t imageIt = 0;
    imageIt < puleGfxPipelineDescriptorMax_texture;
    ++ imageIt
  ) {
    auto imageStage = info->layout->textureBindings[imageIt];
    if (imageStage == 0) { continue; }
    descriptorSetLayoutBindings.emplace_back(VkDescriptorSetLayoutBinding {
      .binding = (uint32_t)imageIt,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = util::toVkShaderStageFlags(imageStage),
      .pImmutableSamplers = nullptr,
    });
  }
  auto descriptorSetLayoutCi = VkDescriptorSetLayoutCreateInfo {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0, // TODO probably need to change this?
    .bindingCount = (uint32_t)descriptorSetLayoutBindings.size(),
    .pBindings = descriptorSetLayoutBindings.data(),
  };
  PULE_assert(
    vkCreateDescriptorSetLayout(
      util::ctx().device.logical,
      &descriptorSetLayoutCi, nullptr,
      &descriptorSetLayout
    ) == VK_SUCCESS
  );
  auto const pushConstantRange = VkPushConstantRange {
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .offset = 0,
    .size = sizeof(float)*4*4,
  };
  auto pipelineLayoutCi = VkPipelineLayoutCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .setLayoutCount = 1,
    .pSetLayouts = &descriptorSetLayout,
// TODO::CRIT push constants
    .pushConstantRangeCount = 1,
    .pPushConstantRanges = &pushConstantRange,
  };
  auto pipelineLayout = VkPipelineLayout{};
  PULE_assert(
    vkCreatePipelineLayout(
      util::ctx().device.logical,
      &pipelineLayoutCi, nullptr, &pipelineLayout
    ) == VK_SUCCESS
  );
  return pipelineLayout;
}

} // namespace

extern "C" {

PuleGfxPipelineDescriptorSetLayout puleGfxPipelineDescriptorSetLayout() {
  PuleGfxPipelineDescriptorSetLayout descriptorSetLayout;
  memset(
    &descriptorSetLayout.bufferUniformBindings[0],
    0,

    sizeof(decltype(descriptorSetLayout.bufferUniformBindings[0])) * 16
  );
  memset(
    &descriptorSetLayout.bufferStorageBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.bufferStorageBindings[0])) * 16
  );
  memset(
    &descriptorSetLayout.attributeBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.attributeBindings[0])) * 16
  );
  memset(
    &descriptorSetLayout.attributeBufferBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.attributeBufferBindings[0])) * 16
  );
  memset(
    &descriptorSetLayout.textureBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.textureBindings[0])) * 8
  );
  return descriptorSetLayout;
}

void puleGfxPipelineUpdate(
  PuleGfxPipeline const pipeline,
  PuleGfxPipelineCreateInfo const * const info,
  PuleError * const error
) {
  (void)pipeline;(void)info;(void)error;
  // TODO::CRITICAL
  #if 0
  util::Pipeline & utilPipeline = *util::pipeline(pipeline.id);

  // buffer attribute bindings
  for (size_t it = 0; it < 16; ++ it) {
    PuleGfxPipelineAttributeDescriptorBinding const &
      descriptorAttributeBinding = (
        info->layout->bufferAttributeBindings[it]
      )
    ;
    bool const usesBuffer = descriptorAttributeBinding.buffer.id > 0;
    // if there are no components, this binding is disabled
    if (descriptorAttributeBinding.numComponents == 0) {
      glDisableVertexArrayAttrib(utilPipeline.attributeDescriptorHandle, it);
      continue;
    }
    PULE_errorAssert(
      descriptorAttributeBinding.numComponents != 0,
      PuleErrorGfx_invalidDescriptorSet,
    );
    GLenum const attributeDataType = (
      ::attributeDataTypeToGl(descriptorAttributeBinding.dataType, error)
    );
    if (error->id > 0) { return; }
    PULE_errorAssert(
      attributeDataType != 0,
      PuleErrorGfx_invalidDescriptorSet,
    );

    if (usesBuffer) {
      glVertexArrayVertexBuffer(
        utilPipeline.attributeDescriptorHandle,
        it,
        descriptorAttributeBinding.buffer.id,
        descriptorAttributeBinding.offsetIntoBuffer,
        descriptorAttributeBinding.stridePerElement
      );
    }
    glEnableVertexArrayAttrib(utilPipeline.attributeDescriptorHandle, it);
    glVertexArrayAttribFormat(
      utilPipeline.attributeDescriptorHandle,
      it,
      descriptorAttributeBinding.numComponents,
      attributeDataType,
      descriptorAttributeBinding.convertFixedDataTypeToNormalizedFloating,
      0
      //descriptorAttributeBinding.stridePerElement
    );
    glVertexArrayAttribBinding(utilPipeline.attributeDescriptorHandle, it, it);
  }

  // element binding
  if (info->layout->bufferElementBinding.id != 0) {
    glVertexArrayElementBuffer(
      utilPipeline.attributeDescriptorHandle,
      info->layout->bufferElementBinding.id
    );
  }

  // collapse textures into a single array, such that if the pipeline layout
  // were [0, 0, 5, 0, 6], we want [{5, 2}, {6, 4}]
  utilPipeline.texturesLength = 0;
  for (size_t it = 0; it < 8; ++ it) {
    PuleGfxGpuImage const texture = info->layout->textureBindings[it];
    if (texture.id == 0) {
      continue;
    }
    utilPipeline.textures[utilPipeline.texturesLength] = (
      util::DescriptorSetImageBinding {
        .imageHandle = static_cast<uint32_t>(texture.id),
        .bindingSlot = static_cast<uint32_t>(it),
      }
    );
    ++ utilPipeline.texturesLength;
  }

  // collapse storage buffers into array
  utilPipeline.storagesLength = 0;
  for (size_t it = 0; it < puleGfxPipelineDescriptorMax_storage; ++ it) {
    PuleGfxGpuBuffer const buffer = info->layout->bufferStorageBindings[it];
    if (buffer.id == 0) {
      continue;
    }
    utilPipeline.storages[utilPipeline.storagesLength] = (
      util::DescriptorSetStorageBinding {
        .storageHandle = static_cast<uint32_t>(buffer.id),
        .bindingSlot = static_cast<uint32_t>(it),
      }
    );
    ++ utilPipeline.storagesLength;
  }

  utilPipeline.pipelineHandle     = pipeline.id;
  utilPipeline.shaderModuleHandle = info->shaderModule.id;
  utilPipeline.blendEnabled       = info->config.blendEnabled;
  utilPipeline.depthTestEnabled   = info->config.depthTestEnabled;
  utilPipeline.scissorTestEnabled = info->config.scissorTestEnabled;
  utilPipeline.viewportUl         = info->config.viewportUl;
  utilPipeline.viewportLr         = info->config.viewportLr;
  utilPipeline.scissorUl          = info->config.scissorUl;
  utilPipeline.scissorLr          = info->config.scissorLr;
  #endif
}

PuleGfxPipeline puleGfxPipelineCreate(
  PuleGfxPipelineCreateInfo const * const info,
  PuleError * const error
) {
  auto const shaderModule = (
    util::ctx().shaderModules.at(info->shaderModule.id)
  );
  std::vector<VkPipelineShaderStageCreateInfo> stageInfo = {
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = shaderModule.moduleVertex,
      .pName = "main",
      .pSpecializationInfo = nullptr,
    },
    {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = shaderModule.moduleFragment,
      .pName = "main",
      .pSpecializationInfo = nullptr,
    },
  };
  std::vector<VkVertexInputBindingDescription> vtxBuffers;
  std::vector<VkVertexInputAttributeDescription> vtxAttributes;
  createPipelineVertexInputState(info, vtxBuffers, vtxAttributes);
  auto inputStateCi = VkPipelineVertexInputStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .vertexBindingDescriptionCount = (uint32_t)vtxBuffers.size(),
    .pVertexBindingDescriptions = vtxBuffers.data(),
    .vertexAttributeDescriptionCount = (uint32_t)vtxAttributes.size(),
    .pVertexAttributeDescriptions = vtxAttributes.data(),
  };
  auto inputAssemblyStateCi = VkPipelineInputAssemblyStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };
  auto inputTessellationStateCi = VkPipelineTessellationStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .patchControlPoints = 0,
  };
  auto viewportUl = info->config.viewportUl;
  auto viewportLr = info->config.viewportLr;
  if (
    viewportUl.x == 0.0f && viewportUl.y == 0.0f
    && viewportLr.x == 0.0f && viewportLr.y == 0.0f
  ) {
    viewportUl.x = viewportUl.y = 0.0f;
    viewportLr.x = viewportLr.y = 800.0f;
  }
  auto viewport = VkViewport {
    .x = (float)viewportUl.x,
    .y = (float)viewportUl.y,
    .width = (float)(viewportLr.x - viewportUl.x),
    .height = (float)(viewportLr.y - viewportUl.y),
    .minDepth = 0.0f, .maxDepth = 1.0f,
  };
  auto scissor = VkRect2D {
    .offset = VkOffset2D {
      .x = info->config.scissorUl.x,
      .y = info->config.scissorUl.y,
    },
    .extent = VkExtent2D {
      .width = (uint32_t)info->config.scissorUl.x,
      .height = (uint32_t)info->config.scissorUl.y,
    },
  };
  auto viewportStateCi = VkPipelineViewportStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1,
    .pScissors = &scissor,
  };
  auto rasterizerStateCi = VkPipelineRasterizationStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthClampEnable = VK_FALSE, // TODO do I want this?
    //.rasterizerDiscardEnable = VK_TRUE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_NONE, // TODO cull proper
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0.0f,
    .depthBiasClamp = 0.0f,
    .depthBiasSlopeFactor = 1.0f,
    .lineWidth = 1.0f,
  };
  auto multisampleStateCi = VkPipelineMultisampleStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0.0f,
    .pSampleMask = nullptr,
    .alphaToCoverageEnable = VK_FALSE,
    .alphaToOneEnable = VK_FALSE,
  };
  auto depthStencilStateCi = VkPipelineDepthStencilStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .depthTestEnable = info->config.depthTestEnabled,
    .depthWriteEnable = info->config.depthTestEnabled,
    .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
    .depthBoundsTestEnable = VK_TRUE,
    .stencilTestEnable = VK_FALSE,
    .front = VkStencilOpState { },
    .back = VkStencilOpState { },
    .minDepthBounds = 0.0f,
    .maxDepthBounds = 1.0f,
  };
  auto attachments = std::vector<VkPipelineColorBlendAttachmentState> {
    VkPipelineColorBlendAttachmentState {
      .blendEnable = info->config.blendEnabled,
      // TODO expose this
      .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ONE,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = (
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
      ),
    },
  };
  auto colorBlendStateCi = VkPipelineColorBlendStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .logicOpEnable = VK_TRUE,
    .logicOp = VK_LOGIC_OP_SET,
    .attachmentCount = (uint32_t)attachments.size(),
    .pAttachments = attachments.data(),
    .blendConstants = { 0.0f, 0.0f, 0.0f, 1.0f, },
  };
  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT,
  };
  auto dynamicStateCi = VkPipelineDynamicStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .dynamicStateCount = (uint32_t)dynamicStates.size(),
    .pDynamicStates = dynamicStates.data(),
  };
  VkDescriptorSetLayout descriptorSetLayout = nullptr;
  VkPipelineLayout pipelineLayout = createPipelineLayout(info, descriptorSetLayout);
  puleLog("pipelineLayout: %p", pipelineLayout);
  auto pipelineCi = VkGraphicsPipelineCreateInfo {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .stageCount = (uint32_t)stageInfo.size(),
    .pStages = stageInfo.data(),
    .pVertexInputState = &inputStateCi,
    .pInputAssemblyState = &inputAssemblyStateCi,
    .pTessellationState = &inputTessellationStateCi,
    .pViewportState = &viewportStateCi,
    .pRasterizationState = &rasterizerStateCi,
    .pMultisampleState = &multisampleStateCi, // TODO MSAA?
    .pDepthStencilState = &depthStencilStateCi,
    .pColorBlendState = &colorBlendStateCi,
    .pDynamicState = &dynamicStateCi,
    .layout = pipelineLayout,
    .renderPass = nullptr,
    .subpass = 0,
    .basePipelineHandle = 0,
    .basePipelineIndex = 0,
  };

  auto gfxPipeline = VkPipeline {};
  PULE_vkAssert(
    vkCreateGraphicsPipelines(
      util::ctx().device.logical,
      VK_NULL_HANDLE,
      1,
      &pipelineCi,
      nullptr,
      &gfxPipeline
    ),
    PuleErrorGfx_invalidDescriptorSet,
    {.id = 0}
  );
  auto gfxPipelineId = reinterpret_cast<uint64_t>(gfxPipeline);
  VkDescriptorSet descriptorSet;
  { // create descriptor set
    auto const descriptorSetAllocateInfo = VkDescriptorSetAllocateInfo {
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = nullptr,
      .descriptorPool = util::ctx().defaultDescriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &descriptorSetLayout,
    };
    PULE_assert(
      vkAllocateDescriptorSets(
        util::ctx().device.logical,
        &descriptorSetAllocateInfo,
        &descriptorSet
      ) == VK_SUCCESS
    );
  }
  util::ctx().pipelines.emplace(
    gfxPipelineId,
    util::Pipeline {
      .pipelineHandle = reinterpret_cast<uint64_t>(gfxPipeline),
      .shaderModuleHandle = info->shaderModule.id,
      .pipelineLayout = pipelineCi.layout,
      .descriptorSet = descriptorSet,
      .descriptorSetLayout = descriptorSetLayout,
    }
  );
  return {.id = reinterpret_cast<uint64_t>(gfxPipeline),};
}

void puleGfxPipelineDestroy(PuleGfxPipeline const puPipeline) {
  auto pipeline = reinterpret_cast<VkPipeline>(puPipeline.id);
  vkDestroyPipeline(util::ctx().device.logical, pipeline, nullptr);
  // TODO destroy pipeline layout
}

} // extern C
