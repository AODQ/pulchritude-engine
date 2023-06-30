#include <util.hpp>

#include <volk.h>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
typedef struct {
  VkStructureType sType;
  void * pNext;
} chainPNext;
} // namespace

void util::chainPNextList(std::vector<void *> const pnextList) {
  for (size_t it = 1; it < pnextList.size(); ++ it) {
    static_cast<chainPNext *>(pnextList[it-1])->pNext = pnextList[it];
  }
  static_cast<chainPNext *>(pnextList[pnextList.size()-1])->pNext = nullptr;
}

util::Context & util::ctx() {
  static util::Context ctx;
  return ctx;
}

VkDescriptorType util::toDescriptorType(
  PuleGfxGpuBufferBindingDescriptor const descriptor
) {
  switch (descriptor) {
    default: PULE_assert(false);
    case PuleGfxGpuBufferBindingDescriptor_uniform:
      return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case PuleGfxGpuBufferBindingDescriptor_storage:
      return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case PuleGfxGpuBufferBindingDescriptor_accelerationStructure:
      return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
  }
}

VkBufferUsageFlags util::toBufferUsageFlags(
  PuleGfxGpuBufferUsage const usage
) {
  VkBufferUsageFlags usageVk = 0;
  if (usage & PuleGfxGpuBufferUsage_attribute) {
    usageVk |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  }
  if (usage & PuleGfxGpuBufferUsage_element) {
    usageVk |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  }
  if (usage & PuleGfxGpuBufferUsage_uniform) {
    usageVk |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  }
  if (usage & PuleGfxGpuBufferUsage_storage) {
    usageVk |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  }
  if (usage & PuleGfxGpuBufferUsage_accelerationStructure) {
    usageVk |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
  }
  if (usage & PuleGfxGpuBufferUsage_indirect) {
    usageVk |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
  }
  return usageVk;
}

VkSwapchainKHR util::swapchainCreate() {
  auto surfaceFormat = VkSurfaceFormatKHR {
    .format = VK_FORMAT_B8G8R8A8_UNORM,
    .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
  };

  // TODO assert getPhysicalDeviceSupportKHR, and cycle thru possible formats

  // TODO getPhysicalDeviceSurfaceCapabilitiesKHR
  auto surfaceCapabilities = VkSurfaceCapabilitiesKHR{};
  PULE_assert(
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      util::ctx().device.physical,
      util::ctx().surface,
      &surfaceCapabilities
    ) == VK_SUCCESS
  );

  auto const imageExtent = VkExtent2D {
    .width = 800, // TODO::CRIT
    .height = 600, // TODO::CRIT
  };

  auto const swapchainCi = VkSwapchainCreateInfoKHR {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = nullptr,
    .flags = 0, // TODO::FUTURE should i use mutable format?
    .surface = util::ctx().surface,
    .minImageCount = surfaceCapabilities.minImageCount,
    .imageFormat = surfaceFormat.format,
    .imageColorSpace = surfaceFormat.colorSpace,
    .imageExtent = imageExtent,
    .imageArrayLayers = 1,
    .imageUsage = (
        VK_IMAGE_USAGE_TRANSFER_DST_BIT
        | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    ),
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices = &util::ctx().device.queues.idxGTC,
    .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR, // TODO::FUTURE
    .clipped = VK_TRUE,
    .oldSwapchain = nullptr, // TODO::CRIT
  };

  VkSwapchainKHR swapchain;
  PULE_assert(
    vkCreateSwapchainKHR(
      util::ctx().device.logical, &swapchainCi, nullptr, &swapchain
    ) == VK_SUCCESS
  );
  return swapchain;
}

uint32_t util::swapchainAcquireNextImage(
  VkSemaphore const signalSemaphore,
  VkFence const fence
) {
  static auto clearSemaphoreTmp = VkSemaphore { nullptr };
  if (clearSemaphoreTmp != nullptr) {
    vkDestroySemaphore(util::ctx().device.logical, clearSemaphoreTmp, nullptr);
  }

  { // create clear semaphore
    auto semaphoreCi = VkSemaphoreCreateInfo {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_SEMAPHORE_TYPE_BINARY,
    };
    PULE_assert(
      vkCreateSemaphore(
        util::ctx().device.logical,
        &semaphoreCi, nullptr, &clearSemaphoreTmp
      ) == VK_SUCCESS
    );
  }
  uint32_t imageIndex;
  PULE_assert(
    vkAcquireNextImageKHR(
      util::ctx().device.logical,
      util::ctx().swapchain,
      50,
      clearSemaphoreTmp, fence, &imageIndex
    ) == VK_SUCCESS
  );
  util::ctx().swapchainCurrentImageIdx = imageIndex;


  // -- set image to general layout
  auto commandBuffer = util::ctx().utilCommandBuffer;
  auto swapchainImage = (
    util::ctx().swapchainImages[util::ctx().swapchainCurrentImageIdx]
  );
  { // buffer begin
    auto const bufferBeginInfo = VkCommandBufferBeginInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
    };
    vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);
  }

  auto const subresourceRange = VkImageSubresourceRange {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0, .levelCount = 1,
    .baseArrayLayer = 0, .layerCount = 1,
  };

  auto const barrierSwapchainImagePresentToClear  = VkImageMemoryBarrier {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcAccessMask = 0,
    .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
    .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = swapchainImage,
    .subresourceRange = subresourceRange,
  };

  auto const clearColorValue = VkClearColorValue {
    .float32 = { 0.5f, 0.1f, 0.1f, 1.0f, },
  };
  vkCmdPipelineBarrier(
    commandBuffer,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrierSwapchainImagePresentToClear
  );
  vkCmdClearColorImage(
    commandBuffer,
    swapchainImage,
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    &clearColorValue,
    1, &subresourceRange
  );
  PULE_assert(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);
  auto waitSemaphoreSubmitInfo = VkSemaphoreSubmitInfoKHR {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
    .pNext = nullptr,
    .semaphore = clearSemaphoreTmp,
    .value = 1,
    .stageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
    .deviceIndex = 0,
  };
  auto signalSemaphoreSubmitInfo = VkSemaphoreSubmitInfoKHR {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
    .pNext = nullptr,
    .semaphore = signalSemaphore,
    .value = 1,
    .stageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
    .deviceIndex = 0,
  };
  auto commandBufferSubmitInfo = VkCommandBufferSubmitInfoKHR {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR,
    .pNext = nullptr,
    .commandBuffer = commandBuffer,
    .deviceMask = 0,
  };
  auto submitInfo = VkSubmitInfo2KHR {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR,
    .pNext = nullptr,
    .flags = 0,
    .waitSemaphoreInfoCount = 1,
    .pWaitSemaphoreInfos = &waitSemaphoreSubmitInfo,
    .commandBufferInfoCount = 1,
    .pCommandBufferInfos = &commandBufferSubmitInfo,
    .signalSemaphoreInfoCount = 1u,
    .pSignalSemaphoreInfos = &signalSemaphoreSubmitInfo,
  };
  vkQueueSubmit2KHR(
    util::ctx().defaultQueue,
    1, &submitInfo,
    fence
  );

  return imageIndex;
}

void util::swapchainPresent(VkSemaphore const waitSemaphore) {
  auto commandBufferCi = (
    VkCommandBufferAllocateInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = util::ctx().defaultCommandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
    }
  );
  auto commandBuffer = VkCommandBuffer { };
  vkAllocateCommandBuffers(
    util::ctx().device.logical,
    &commandBufferCi,
    &commandBuffer
  );
  // -- set image to present layout
  vkDeviceWaitIdle(util::ctx().device.logical);
  auto swapchainImage = (
    util::ctx().swapchainImages[util::ctx().swapchainCurrentImageIdx]
  );
  { // buffer begin
    auto const bufferBeginInfo = VkCommandBufferBeginInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
    };
    PULE_assert(
      vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo) == VK_SUCCESS
    );
  }

  auto const subresourceRange = VkImageSubresourceRange {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0, .levelCount = 1,
    .baseArrayLayer = 0, .layerCount = 1,
  };

  auto const barrierSwapchainImageTransferToPresent = VkImageMemoryBarrier {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcAccessMask = 0,
    .dstAccessMask = 0,
    .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
    .image = swapchainImage,
    .subresourceRange = subresourceRange,
  };

  vkCmdPipelineBarrier(
    commandBuffer,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrierSwapchainImageTransferToPresent
  );
  PULE_assert(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS);
  auto waitSemaphoreSubmitInfo = VkSemaphoreSubmitInfoKHR {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
    .pNext = nullptr,
    .semaphore = waitSemaphore,
    .value = 1,
    .stageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
    .deviceIndex = 0,
  };
  auto commandBufferSubmitInfo = VkCommandBufferSubmitInfoKHR {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO_KHR,
    .pNext = nullptr,
    .commandBuffer = commandBuffer,
    .deviceMask = 0,
  };
  VkSemaphore presentSemaphore = VK_NULL_HANDLE;
  auto presentSemaphoreCi = VkSemaphoreCreateInfo {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
  };
  vkCreateSemaphore(
    util::ctx().device.logical,
    &presentSemaphoreCi,
    nullptr,
    &presentSemaphore
  );
  auto presentSemaphoreSubmitInfo = VkSemaphoreSubmitInfo {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO_KHR,
    .pNext = nullptr,
    .semaphore = presentSemaphore,
    .value = 0,
    .stageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
    .deviceIndex = 0,
  };
  auto submitInfo = VkSubmitInfo2KHR {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2_KHR,
    .pNext = nullptr,
    .flags = 0,
    .waitSemaphoreInfoCount = 1,
    .pWaitSemaphoreInfos = &waitSemaphoreSubmitInfo,
    .commandBufferInfoCount = 1,
    .pCommandBufferInfos = &commandBufferSubmitInfo,
    .signalSemaphoreInfoCount = 1,
    .pSignalSemaphoreInfos = &presentSemaphoreSubmitInfo,
  };
  vkQueueSubmit2KHR(util::ctx().defaultQueue, 1, &submitInfo, nullptr);
  // -- present
  auto const presentInfo = VkPresentInfoKHR {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = nullptr,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &presentSemaphore,
    .swapchainCount = 1,
    .pSwapchains = &util::ctx().swapchain,
    .pImageIndices = &util::ctx().swapchainCurrentImageIdx,
    .pResults = nullptr,
  };
  PULE_assert(
    vkQueuePresentKHR(util::ctx().defaultQueue, &presentInfo) == VK_SUCCESS
  );

  vkQueueWaitIdle(util::ctx().defaultQueue);

  vkFreeCommandBuffers(
    util::ctx().device.logical,
    util::ctx().defaultCommandPool,
    1, &commandBuffer
  );
}

void util::swapchainImages() {
  uint32_t swapchainImageCount = 0;
  PULE_assert(
    vkGetSwapchainImagesKHR(
      util::ctx().device.logical,
      util::ctx().swapchain,
      &swapchainImageCount, nullptr
    ) == VK_SUCCESS
  );
  std::vector<VkImage> swapchainImages;
  util::ctx().swapchainImages.resize(swapchainImageCount);
  vkGetSwapchainImagesKHR(
    util::ctx().device.logical,
    util::ctx().swapchain,
    &swapchainImageCount, util::ctx().swapchainImages.data()
  );
}

VkShaderStageFlags util::toVkShaderStageFlags(
  PuleGfxDescriptorStage const stage
) {
  auto flags = VkShaderStageFlags {};
  if (stage & PuleGfxDescriptorStage_vertex) {
    flags |= VK_SHADER_STAGE_VERTEX_BIT;
  }
  if (stage & PuleGfxDescriptorStage_fragment) {
    flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
  }
  return flags;
}

VkIndexType util::toVkIndexType(PuleGfxElementType const elementType) {
  switch (elementType) {
    default: PULE_assert(false);
    case PuleGfxElementType_u8: return VK_INDEX_TYPE_UINT8_EXT;
    case PuleGfxElementType_u16: return VK_INDEX_TYPE_UINT16;
    case PuleGfxElementType_u32: return VK_INDEX_TYPE_UINT32;
  }
}

VkAccessFlags util::toVkAccessFlags(PuleGfxCommandPayloadAccess const access) {
  VkAccessFlags flag = 0;
  if (access & PuleGfxCommandPayloadAccess_indirectCommandRead)
    flag |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_indexRead)
    flag |= VK_ACCESS_INDEX_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_vertexAttributeRead)
    flag |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_uniformRead)
    flag |= VK_ACCESS_UNIFORM_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_inputAttachmentRead)
    flag |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_shaderRead)
    flag |= VK_ACCESS_SHADER_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_shaderWrite)
    flag |= VK_ACCESS_SHADER_WRITE_BIT;
  if (access & PuleGfxCommandPayloadAccess_colorAttachmentRead)
    flag |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_colorAttachmentWrite)
    flag |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  if (access & PuleGfxCommandPayloadAccess_depthStencilAttachmentRead)
    flag |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_depthStencilAttachmentRead)
    flag |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  if (access & PuleGfxCommandPayloadAccess_transferRead)
    flag |= VK_ACCESS_TRANSFER_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_transferWrite)
    flag |= VK_ACCESS_TRANSFER_WRITE_BIT;
  if (access & PuleGfxCommandPayloadAccess_hostRead)
    flag |= VK_ACCESS_HOST_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_hostWrite)
    flag |= VK_ACCESS_HOST_WRITE_BIT;
  if (access & PuleGfxCommandPayloadAccess_memoryRead)
    flag |= VK_ACCESS_MEMORY_READ_BIT;
  if (access & PuleGfxCommandPayloadAccess_memoryWrite)
    flag |= VK_ACCESS_MEMORY_WRITE_BIT;
  return flag;
}

VkImageLayout util::toVkImageLayout(
  PuleGfxImageLayout const layout
) {
  switch (layout) {
    default: PULE_assert(false);
    case PuleGfxImageLayout_uninitialized:
      return VK_IMAGE_LAYOUT_UNDEFINED;
    case PuleGfxImageLayout_storage:
      return VK_IMAGE_LAYOUT_GENERAL;
    case PuleGfxImageLayout_attachmentColor:
      return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case PuleGfxImageLayout_attachmentDepth:
      return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    case PuleGfxImageLayout_transferSrc:
      return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case PuleGfxImageLayout_transferDst:
      return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  }
}

VkFormat util::toVkImageFormat(PuleGfxImageByteFormat const format) {
  switch (format) {
    default: PULE_assert(false);
    case PuleGfxImageByteFormat_bgra8U:
      return VK_FORMAT_B8G8R8A8_UNORM;
    case PuleGfxImageByteFormat_rgba8U:
      return VK_FORMAT_R8G8B8A8_UNORM;
    case PuleGfxImageByteFormat_rgb8U:
      return VK_FORMAT_R8G8B8_UNORM;
    case PuleGfxImageByteFormat_r8U:
      return VK_FORMAT_R8_UNORM;
    case PuleGfxImageByteFormat_depth16:
      return VK_FORMAT_D16_UNORM;
  }
}

VkFormat util::toVkBufferFormat(
  PuleGfxAttributeDataType const dataType,
  size_t const numComponents,
  bool const normalize
) {
  switch (dataType) {
    case PuleGfxAttributeDataType_float:
      switch (numComponents) {
        case 1: return VK_FORMAT_R32_SFLOAT;
        case 2: return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
      }
      PULE_assert(false);
    case PuleGfxAttributeDataType_unsignedByte:
      switch (numComponents) {
        case 1:
          return (normalize ? VK_FORMAT_R8_USCALED : VK_FORMAT_R8_UINT);
        case 2:
          return (normalize ? VK_FORMAT_R8G8_USCALED : VK_FORMAT_R8G8_UINT);
        case 3:
          return (normalize ? VK_FORMAT_R8G8B8_USCALED : VK_FORMAT_R8G8B8_UINT);
        case 4:
          return (
            normalize ? VK_FORMAT_R8G8B8A8_USCALED : VK_FORMAT_R8G8B8A8_UINT
          );
      }
      PULE_assert(false);
  }
  PULE_assert(false);
}

VkAttachmentLoadOp util::toVkAttachmentOpLoad(
  PuleGfxImageAttachmentOpLoad const op
) {
  switch (op) {
    default: PULE_assert(false);
    case PuleGfxImageAttachmentOpLoad_load:
      return VK_ATTACHMENT_LOAD_OP_LOAD;
    case PuleGfxImageAttachmentOpLoad_clear:
      return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case PuleGfxImageAttachmentOpLoad_dontCare:
      return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  }
}

VkAttachmentStoreOp util::toVkAttachmentOpStore(
  PuleGfxImageAttachmentOpStore const op
  ) {
  switch (op) {
    default: PULE_assert(false);
    case PuleGfxImageAttachmentOpStore_store:
      return VK_ATTACHMENT_STORE_OP_STORE;
    case PuleGfxImageAttachmentOpStore_dontCare:
      return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  }
}

VkImageView createImageView(PuleGfxGpuImageView const puImageView) {
  // create vulkan image view
  puleLogDev("Creating image view for image %d", puImageView.image.id);
  auto const imageViewCreateInfo = VkImageViewCreateInfo {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .image = reinterpret_cast<VkImage>(puImageView.image.id),
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = util::toVkImageFormat(puImageView.byteFormat),
    .components = VkComponentMapping {
      .r = VK_COMPONENT_SWIZZLE_R,
      .g = VK_COMPONENT_SWIZZLE_G,
      .b = VK_COMPONENT_SWIZZLE_B,
      .a = VK_COMPONENT_SWIZZLE_A,
    },
    .subresourceRange = VkImageSubresourceRange {
      .aspectMask = (
        puImageView.byteFormat == PuleGfxImageByteFormat_depth16
        ? VK_IMAGE_ASPECT_DEPTH_BIT
        : VK_IMAGE_ASPECT_COLOR_BIT
      ),
      .baseMipLevel = (uint32_t)puImageView.mipmapLevelStart,
      .levelCount = (uint32_t)puImageView.mipmapLevelCount,
      .baseArrayLayer = (uint32_t)puImageView.arrayLayerStart,
      .layerCount = (uint32_t)puImageView.arrayLayerCount,
    },
  };
  VkImageView imageView;
  vkCreateImageView(
    util::ctx().device.logical,
    &imageViewCreateInfo,
    nullptr,
    &imageView
  );
  return imageView;
}

VkImageView util::fetchImageView(PuleGfxGpuImageView const imageView) {
  // find image view list from image
  auto imageViewListIt = util::ctx().imageViews.find(imageView.image.id);
  // if image view list not found, create it
  if (imageViewListIt == util::ctx().imageViews.end()) {
    util::ctx().imageViews.emplace(
      imageView.image.id,
      std::unordered_map<uint64_t, VkImageView> {}
    );
    imageViewListIt = util::ctx().imageViews.find(imageView.image.id);
  }
  PULE_assert(imageViewListIt != util::ctx().imageViews.end());
  // hash image view
  size_t const imageViewHash = (
      imageView.mipmapLevelStart
    + imageView.mipmapLevelCount*16
    + imageView.arrayLayerStart*16*64
    + imageView.arrayLayerCount*16*64*64
    + imageView.byteFormat*16*64*64*64
  );
  // find image view in list
  auto imageViewIt = imageViewListIt->second.find(imageViewHash);
  // if image view not found, create it
  if (imageViewIt == imageViewListIt->second.end()) {
    imageViewListIt->second.emplace(imageViewHash, createImageView(imageView));
    imageViewIt = imageViewListIt->second.find(imageViewHash);
  }
  PULE_assert(imageViewIt != imageViewListIt->second.end());

  return imageViewIt->second;
}
