#include <pulchritude-gpu/module.h>

#include <util.hpp>

#include <pulchritude-log/log.h>

#include <volk.h>

#include <array>
#include <string>
#include <vector>

namespace {

void createPipelineVertexInputState(
  PuleGpuPipelineCreateInfo const * const info,
  std::vector<VkVertexInputBindingDescription> & vtxBuffers,
  std::vector<VkVertexInputAttributeDescription> & vtxAttributes
) {
  // attribute bindings
  for (size_t it = 0; it < puleGpuPipelineDescriptorMax_attribute; ++ it) {
    auto const binding = info->layoutDescriptorSet->attributeBindings[it];
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
  for (size_t it = 0; it < puleGpuPipelineDescriptorMax_attribute; ++ it) {
    auto const binding = info->layoutDescriptorSet->attributeBufferBindings[it];
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
  PuleGpuPipelineCreateInfo const * const info,
  VkDescriptorSetLayout & descriptorSetLayout
) {
  auto setLayouts = std::vector<VkDescriptorSetLayout> { };
  auto descriptorSetLayoutBindings = (
    std::vector<VkDescriptorSetLayoutBinding>{}
  );
  // compactStorage buffers
  for (size_t it = 0; it < puleGpuPipelineDescriptorMax_uniform; ++ it) {
    auto bufferStage = info->layoutDescriptorSet->bufferUniformBindings[it];
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
  for (size_t it = 0; it < puleGpuPipelineDescriptorMax_storage; ++ it) {
    auto bufferStage = info->layoutDescriptorSet->bufferStorageBindings[it];
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
    imageIt < puleGpuPipelineDescriptorMax_texture;
    ++ imageIt
  ) {
    auto imageStage = info->layoutDescriptorSet->textureBindings[imageIt];
    if (imageStage == 0) { continue; }
    descriptorSetLayoutBindings.emplace_back(VkDescriptorSetLayoutBinding {
      .binding = (uint32_t)imageIt,
      .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
      .descriptorCount = 1,
      .stageFlags = util::toVkShaderStageFlags(imageStage),
      .pImmutableSamplers = &util::ctx().samplerDefault,
    });
  }
  auto descriptorSetLayoutCi = VkDescriptorSetLayoutCreateInfo {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
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
  std::vector<VkPushConstantRange> pushConstantRanges;
  for (size_t pcIt = 0; pcIt < info->layoutPushConstantsCount; ++ pcIt) {
    auto const & pushConstant = info->layoutPushConstants[pcIt];
    pushConstantRanges.emplace_back( VkPushConstantRange {
      .stageFlags = util::toVkShaderStageFlags(pushConstant.stage),
      .offset = (uint32_t)pushConstant.byteOffset,
      .size = (uint32_t)pushConstant.byteLength,
    });
  }
  auto pipelineLayoutCi = VkPipelineLayoutCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .setLayoutCount = 1,
    .pSetLayouts = &descriptorSetLayout,
    .pushConstantRangeCount = (uint32_t)pushConstantRanges.size(),
    .pPushConstantRanges = pushConstantRanges.data(),
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

PuleGpuPipelineLayoutDescriptorSet puleGpuPipelineDescriptorSetLayout() {
  PuleGpuPipelineLayoutDescriptorSet descriptorSetLayout;
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

PuleGpuPipeline puleGpuPipelineCreate(
  PuleGpuPipelineCreateInfo const * const info,
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
  auto viewportMin = info->config.viewportMin;
  auto viewportMax = info->config.viewportMax;
  if (
    viewportMin.x == 0.0f && viewportMin.y == 0.0f
    && viewportMax.x == 0.0f && viewportMax.y == 0.0f
  ) {
    viewportMin.x = viewportMin.y = 0.0f;
    viewportMax.x = 800.0f;
    viewportMax.y = 600.0f;
  }
  auto viewport = VkViewport {
    .x = (float)viewportMin.x,
    .y = (float)viewportMin.y,
    .width = (float)(viewportMax.x - viewportMin.x),
    .height = (float)(viewportMax.y - viewportMin.y),
    .minDepth = 0.0f, .maxDepth = 1.0f,
  };
  auto scissor = VkRect2D {
    .offset = VkOffset2D {
      .x = info->config.scissorMin.x,
      .y = info->config.scissorMin.y,
    },
    .extent = VkExtent2D {
      .width = (uint32_t)info->config.scissorMin.x,
      .height = (uint32_t)info->config.scissorMin.y,
    },
  };
  auto viewportStateCi = VkPipelineViewportStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .viewportCount = 1,
    .pViewports = &viewport,
    .scissorCount = 1, // TODO reenable, but I should make it dynamic
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
      .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
      .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      .colorBlendOp = VK_BLEND_OP_ADD,
      .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
      .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
      .alphaBlendOp = VK_BLEND_OP_ADD,
      .colorWriteMask = (
        VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT
      ),
    },
  };
  auto colorBlendStateCi = VkPipelineColorBlendStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_SET,
    .attachmentCount = (uint32_t)attachments.size(),
    .pAttachments = attachments.data(),
    .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f, },
  };
  std::vector<VkDynamicState> dynamicStates = {
    VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE_EXT,
    VK_DYNAMIC_STATE_SCISSOR,
  };
  auto dynamicStateCi = VkPipelineDynamicStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .dynamicStateCount = (uint32_t)dynamicStates.size(),
    .pDynamicStates = dynamicStates.data(),
  };
  VkDescriptorSetLayout descriptorSetLayout = nullptr;
  VkPipelineLayout pipelineLayout = (
    createPipelineLayout(info, descriptorSetLayout)
  );
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
  util::ctx().pipelines.emplace(
    gfxPipelineId,
    util::Pipeline {
      .pipelineHandle = reinterpret_cast<uint64_t>(gfxPipeline),
      .shaderModuleHandle = info->shaderModule.id,
      .pipelineLayout = pipelineCi.layout,
      .descriptorSetLayout = descriptorSetLayout,
    }
  );
  return {.id = reinterpret_cast<uint64_t>(gfxPipeline),};
}

void puleGpuPipelineDestroy(PuleGpuPipeline const puPipeline) {
  auto pipeline = reinterpret_cast<VkPipeline>(puPipeline.id);
  vkDestroyPipeline(util::ctx().device.logical, pipeline, nullptr);
  // TODO destroy pipeline layout
}

} // extern C
