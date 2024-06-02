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

char const * util::vkErrorToString(VkResult const result) {
  switch (result) {
    case VK_SUCCESS:
      return "VK_SUCCESS";
    case VK_NOT_READY:
      return "VK_NOT_READY";
    case VK_TIMEOUT:
      return "VK_TIMEOUT";
    case VK_EVENT_SET:
      return "VK_EVENT_SET";
    case VK_EVENT_RESET:
      return "VK_EVENT_RESET";
    case VK_INCOMPLETE:
      return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
      return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
      return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
      return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
      return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
      return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
      return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL:
      return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN:
      return "VK_ERROR_UNKNOWN";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_FRAGMENTATION:
      return "VK_ERROR_FRAGMENTATION";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
      return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    case VK_PIPELINE_COMPILE_REQUIRED:
      return "VK_PIPELINE_COMPILE_REQUIRED";
    case VK_ERROR_SURFACE_LOST_KHR:
      return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR:
      return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
      return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT:
      return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV:
      return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
      return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
      return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
      return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_NOT_PERMITTED_KHR:
      return "VK_ERROR_NOT_PERMITTED_KHR";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
      return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    case VK_THREAD_IDLE_KHR:
      return "VK_THREAD_IDLE_KHR";
    case VK_THREAD_DONE_KHR:
      return "VK_THREAD_DONE_KHR";
    case VK_OPERATION_DEFERRED_KHR:
      return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR:
      return "VK_OPERATION_NOT_DEFERRED_KHR";
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
      return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
    case VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT:
      return "VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT";
    case VK_RESULT_MAX_ENUM:
      return "VK_RESULT_MAX_ENUM";
    default: return "UNKNOWN";
  }
}

void util::chainPNextList(std::vector<void *> const pnextList) {
  for (size_t it = 1; it < pnextList.size(); ++ it) {
    static_cast<chainPNext *>(pnextList[it-1])->pNext = pnextList[it];
  }
  static_cast<chainPNext *>(pnextList[pnextList.size()-1])->pNext = nullptr;
}

util::Context & util::ctx() {
  static util::Context ctx = {};
  return ctx;
}

VkDescriptorType util::toDescriptorType(
  PuleGpuBufferBindingDescriptor const descriptor
) {
  switch (descriptor) {
    default: PULE_assert(false);
    case PuleGpuBufferBindingDescriptor_uniform:
      return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    case PuleGpuBufferBindingDescriptor_storage:
      return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case PuleGpuBufferBindingDescriptor_accelerationStructure:
      return VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
  }
}

VkBufferUsageFlags util::toBufferUsageFlags(
  PuleGpuBufferUsage const usage
) {
  VkBufferUsageFlags usageVk = 0;
  if (usage & PuleGpuBufferUsage_attribute) {
    usageVk |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  }
  if (usage & PuleGpuBufferUsage_element) {
    usageVk |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  }
  if (usage & PuleGpuBufferUsage_uniform) {
    usageVk |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  }
  if (usage & PuleGpuBufferUsage_storage) {
    usageVk |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  }
  if (usage & PuleGpuBufferUsage_accelerationStructure) {
    usageVk |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
  }
  if (usage & PuleGpuBufferUsage_indirect) {
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
  PULE_vkError(
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      util::ctx().device.physical,
      util::ctx().surface,
      &surfaceCapabilities
    )
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
  PULE_vkError(
    vkCreateSwapchainKHR(
      util::ctx().device.logical, &swapchainCi, nullptr, &swapchain
    )
  );
  return swapchain;
}

uint32_t util::swapchainAcquireNextImage(VkFence const fence) {
  VkSemaphore imageAvailableSemaphore;
  { // create image available semaphore
    auto semaphoreCi = VkSemaphoreCreateInfo {
      .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
      .pNext = nullptr,
      .flags = VK_SEMAPHORE_TYPE_BINARY,
    };
    PULE_vkError(
      vkCreateSemaphore(
        util::ctx().device.logical,
        &semaphoreCi, nullptr,
        &imageAvailableSemaphore
      )
    );

    std::string label = (
      std::string("image-available-semaphore-")
      + std::to_string(util::ctx().frameIdx)
    );

    VkDebugUtilsObjectNameInfoEXT nameInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .pNext = nullptr,
      .objectType = VK_OBJECT_TYPE_SEMAPHORE,
      .objectHandle = reinterpret_cast<uint64_t>(imageAvailableSemaphore),
      .pObjectName = label.c_str(),
    };
    vkSetDebugUtilsObjectNameEXT(util::ctx().device.logical, &nameInfo);
  }
  uint32_t imageIdx;
  // TODO I need to handle cases where no image is available (this is
  //        going to happen when the window-manager believes the window
  //        is not visible)
  PULE_vkError(
    vkAcquireNextImageKHR(
      util::ctx().device.logical,
      util::ctx().swapchain,
      1'00'000'000'000'000, // nanosecond wait, so 100ms
      imageAvailableSemaphore, fence, &imageIdx
    )
  );
  util::ctx().swapchainCurrentImageIdx = imageIdx;

  // replace existing semaphore with new one
  VkSemaphore & previousAvailableSemaphore = (
    util::ctx().swapchainImageAvailableSemaphores[imageIdx]
  );
  if (previousAvailableSemaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(
      util::ctx().device.logical,
      previousAvailableSemaphore,
      nullptr
    );
  }
  util::ctx().swapchainImageAvailableSemaphores[imageIdx] = (
    imageAvailableSemaphore
  );

  return imageIdx;
}

void util::swapchainImagesCreate() {
  uint32_t swapchainImageCount = 0;
  PULE_vkError(
    vkGetSwapchainImagesKHR(
      util::ctx().device.logical,
      util::ctx().swapchain,
      &swapchainImageCount, nullptr
    )
  );
  util::ctx().swapchainImages.resize(swapchainImageCount);
  util::ctx().swapchainImageAvailableSemaphores.resize(swapchainImageCount);
  vkGetSwapchainImagesKHR(
    util::ctx().device.logical,
    util::ctx().swapchain,
    &swapchainImageCount, util::ctx().swapchainImages.data()
  );
  for (size_t it = 0; it < swapchainImageCount; ++ it) {
    puleLogDebug(
      "labelling swapchain image %zu ",
      reinterpret_cast<uint64_t>(util::ctx().swapchainImages[it])
    );
    util::ctx().images.emplace(
      reinterpret_cast<uint64_t>(util::ctx().swapchainImages[it]),
      util::ImageInfo {
        .label = "window-swapchain-image-" + std::to_string(it),
      }
    );
  }
}

VkPipelineStageFlagBits util::toVkPipelineStageFlagBits(
  PuleGpuPipelineStage const stage
) {
  switch (stage) {
    default: PULE_assert(false);
    case PuleGpuPipelineStage_vertexInput:
      return VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    case PuleGpuPipelineStage_vertexShader:
      return VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    case PuleGpuPipelineStage_fragmentShader:
      return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    case PuleGpuPipelineStage_colorAttachmentOutput:
      return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    case PuleGpuPipelineStage_computeShader:
      return VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    case PuleGpuPipelineStage_transfer:
      return VK_PIPELINE_STAGE_TRANSFER_BIT;
    case PuleGpuPipelineStage_bottom:
      return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  }
}

VkShaderStageFlags util::toVkShaderStageFlags(
  PuleGpuDescriptorStage const stage
) {
  auto flags = VkShaderStageFlags {};
  if (stage & PuleGpuDescriptorStage_vertex) {
    flags |= VK_SHADER_STAGE_VERTEX_BIT;
  }
  if (stage & PuleGpuDescriptorStage_fragment) {
    flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
  }
  return flags;
}

VkIndexType util::toVkIndexType(PuleGpuElementType const elementType) {
  switch (elementType) {
    default: PULE_assert(false);
    case PuleGpuElementType_u8: return VK_INDEX_TYPE_UINT8_EXT;
    case PuleGpuElementType_u16: return VK_INDEX_TYPE_UINT16;
    case PuleGpuElementType_u32: return VK_INDEX_TYPE_UINT32;
  }
}

VkAccessFlags util::toVkAccessFlags(PuleGpuResourceAccess const access) {
  VkAccessFlags flag = 0;
  if (access & PuleGpuResourceAccess_indirectCommandRead)
    flag |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
  if (access & PuleGpuResourceAccess_indexRead)
    flag |= VK_ACCESS_INDEX_READ_BIT;
  if (access & PuleGpuResourceAccess_vertexAttributeRead)
    flag |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
  if (access & PuleGpuResourceAccess_uniformRead)
    flag |= VK_ACCESS_UNIFORM_READ_BIT;
  if (access & PuleGpuResourceAccess_inputAttachmentRead)
    flag |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
  if (access & PuleGpuResourceAccess_shaderRead)
    flag |= VK_ACCESS_SHADER_READ_BIT;
  if (access & PuleGpuResourceAccess_shaderWrite)
    flag |= VK_ACCESS_SHADER_WRITE_BIT;
  if (access & PuleGpuResourceAccess_attachmentColorRead)
    flag |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
  if (access & PuleGpuResourceAccess_attachmentColorWrite)
    flag |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  if (access & PuleGpuResourceAccess_attachmentDepthRead)
    flag |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
  if (access & PuleGpuResourceAccess_attachmentDepthWrite)
    flag |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  if (access & PuleGpuResourceAccess_transferRead)
    flag |= VK_ACCESS_TRANSFER_READ_BIT;
  if (access & PuleGpuResourceAccess_transferWrite)
    flag |= VK_ACCESS_TRANSFER_WRITE_BIT;
  if (access & PuleGpuResourceAccess_hostRead)
    flag |= VK_ACCESS_HOST_READ_BIT;
  if (access & PuleGpuResourceAccess_hostWrite)
    flag |= VK_ACCESS_HOST_WRITE_BIT;
  if (access & PuleGpuResourceAccess_memoryRead)
    flag |= VK_ACCESS_MEMORY_READ_BIT;
  if (access & PuleGpuResourceAccess_memoryWrite)
    flag |= VK_ACCESS_MEMORY_WRITE_BIT;
  return flag;
}

VkImageLayout util::toVkImageLayout(
  PuleGpuImageLayout const layout
) {
  switch (layout) {
    default: PULE_assert(false);
    case PuleGpuImageLayout_uninitialized:
      return VK_IMAGE_LAYOUT_UNDEFINED;
    case PuleGpuImageLayout_storage:
      return VK_IMAGE_LAYOUT_GENERAL;
    case PuleGpuImageLayout_attachmentColor:
      return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    case PuleGpuImageLayout_attachmentDepth:
      return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    case PuleGpuImageLayout_transferSrc:
      return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    case PuleGpuImageLayout_presentSrc:
      return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    case PuleGpuImageLayout_transferDst:
      return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  }
}

VkPipelineStageFlags util::toVkPipelineStageFlags(
  PuleGpuResourceBarrierStage const stage
) {
  VkPipelineStageFlags flag = 0;
  if (stage & PuleGpuResourceBarrierStage_top)
    flag |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  if (stage & PuleGpuResourceBarrierStage_drawIndirect)
    flag |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
  if (stage & PuleGpuResourceBarrierStage_vertexInput)
    flag |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
  if (stage & PuleGpuResourceBarrierStage_shaderVertex)
    flag |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
  if (stage & PuleGpuResourceBarrierStage_shaderFragment)
    flag |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  if (stage & PuleGpuResourceBarrierStage_shaderCompute)
    flag |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
  if (stage & PuleGpuResourceBarrierStage_outputAttachmentColor)
    flag |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  if (stage & PuleGpuResourceBarrierStage_outputAttachmentDepth)
    flag |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  if (stage & PuleGpuResourceBarrierStage_transfer)
    flag |= VK_PIPELINE_STAGE_TRANSFER_BIT;
  if (stage & PuleGpuResourceBarrierStage_bottom)
    flag |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  return flag;
}

VkFormat util::toVkImageFormat(PuleGpuImageByteFormat const format) {
  switch (format) {
    default: PULE_assert(false);
    case PuleGpuImageByteFormat_undefined:
      return VK_FORMAT_UNDEFINED;
    case PuleGpuImageByteFormat_bgra8U:
      return VK_FORMAT_B8G8R8A8_UNORM;
    case PuleGpuImageByteFormat_rgba8U:
      return VK_FORMAT_R8G8B8A8_UNORM;
    case PuleGpuImageByteFormat_rgb8U:
      return VK_FORMAT_R8G8B8_UNORM;
    case PuleGpuImageByteFormat_r8U:
      return VK_FORMAT_R8_UNORM;
    case PuleGpuImageByteFormat_depth16:
      return VK_FORMAT_D16_UNORM;
  }
}

VkFormat util::toVkBufferFormat(
  PuleGpuAttributeDataType const dataType,
  size_t const numComponents,
  bool const normalize
) {
  switch (dataType) {
    case PuleGpuAttributeDataType_f32:
      switch (numComponents) {
        case 1: return VK_FORMAT_R32_SFLOAT;
        case 2: return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
      }
      PULE_assert(false);
    case PuleGpuAttributeDataType_u8:
      switch (numComponents) {
        case 1:
          return (normalize ? VK_FORMAT_R8_UNORM : VK_FORMAT_R8_UINT);
        case 2:
          return (normalize ? VK_FORMAT_R8G8_UNORM : VK_FORMAT_R8G8_UINT);
        case 3:
          return (normalize ? VK_FORMAT_R8G8B8_UNORM : VK_FORMAT_R8G8B8_UINT);
        case 4:
          return (
            normalize ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_UINT
          );
      }
      PULE_assert(false);
  }
  PULE_assert(false);
}

VkAttachmentLoadOp util::toVkAttachmentOpLoad(
  PuleGpuImageAttachmentOpLoad const op
) {
  switch (op) {
    default: PULE_assert(false);
    case PuleGpuImageAttachmentOpLoad_load:
      return VK_ATTACHMENT_LOAD_OP_LOAD;
    case PuleGpuImageAttachmentOpLoad_clear:
      return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case PuleGpuImageAttachmentOpLoad_dontCare:
      return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  }
}

VkAttachmentStoreOp util::toVkAttachmentOpStore(
  PuleGpuImageAttachmentOpStore const op
  ) {
  switch (op) {
    default: PULE_assert(false);
    case PuleGpuImageAttachmentOpStore_store:
      return VK_ATTACHMENT_STORE_OP_STORE;
    case PuleGpuImageAttachmentOpStore_dontCare:
      return VK_ATTACHMENT_STORE_OP_DONT_CARE;
  }
}

VkImageView createImageView(PuleGpuImageView const puImageView) {
  // create vulkan image view
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
        puImageView.byteFormat == PuleGpuImageByteFormat_depth16
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

VkImageView util::fetchImageView(PuleGpuImageView const imageView) {
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

VkPrimitiveTopology util::toVkPrimitiveTopology(
  PuleGpuDrawPrimitive const primitiveTopology
) {
  switch (primitiveTopology) {
    case PuleGpuDrawPrimitive_triangle:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PuleGpuDrawPrimitive_triangleStrip:
      return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case PuleGpuDrawPrimitive_line:
      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PuleGpuDrawPrimitive_point:
      return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
  }
}
