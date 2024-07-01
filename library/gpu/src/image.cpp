#include <pulchritude/gpu.h>

// module include
#include <util.hpp>

// pulchritude engine include
#include <pulchritude/log.h>

// third party include
#include <volk.h>

// stdlib include
#include <unordered_map>

namespace { // sampler
  std::unordered_map<uint64_t, PuleGpuSamplerCreateInfo> samplers;
  uint64_t samplersIt = 1;
}

extern "C" { // sampler

PuleGpuSampler puleGpuSamplerCreate(
  PuleGpuSamplerCreateInfo const createInfo
) {
  auto const prevSamplerIt = samplersIt;
  samplersIt += 1;
  puleLogDebug("creating sampler %u", prevSamplerIt);

  ::samplers.emplace(prevSamplerIt, createInfo);
  return { prevSamplerIt };
}

void puleGpuSamplerDestroy(
  PuleGpuSampler const sampler
) {
  if (sampler.id == 0) { return; }
  ::samplers.erase(sampler.id);
}

} // C

namespace {

VkImageType toVkImageType(PuleGpuImageTarget const target) {
  switch (target) {
    default:
      puleLogError("unknown image target: %d", target);
      PULE_assert(false);
    case PuleGpuImageTarget_i2D:
      return VK_IMAGE_TYPE_2D;
  }
}

uint32_t vkCalculateMemoryType(
  VkMemoryPropertyFlags const properties, uint32_t const typeBits
) {
  auto const & propMem = util::ctx().device.propertiesMemory;
  for (uint32_t it = 0; it < propMem.memoryTypeCount; ++ it) {
    if (
      (propMem.memoryTypes[it].propertyFlags & properties) == properties
      && (typeBits & (1 << it))
    ) {
      return it;
    }
  }
  PULE_assert(false && "could not find memory type");
}

struct InternalImage {
  VkImage image;
  VmaAllocation allocation;
};

std::unordered_map<uint64_t, InternalImage> internalImages;

} // image

extern "C" { // image

PuleGpuImage puleGpuImageCreate(PuleGpuImageCreateInfo const createInfo) {
  if (createInfo.width == 0 || createInfo.height == 0) {
    PULE_assert(false && "image width or height is 0");
  }
  VkImageUsageFlags usage = 0;
  if (createInfo.byteFormat == PuleGpuImageByteFormat_depth16) {
    usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  } else {
    usage = (
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
      | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
      | VK_IMAGE_USAGE_TRANSFER_DST_BIT
      | VK_IMAGE_USAGE_SAMPLED_BIT
    );
  }
  auto const imageInfo = VkImageCreateInfo {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .imageType = toVkImageType(createInfo.target),
    .format = util::toVkImageFormat(createInfo.byteFormat),
    .extent = {
      .width = createInfo.width,
      .height = createInfo.height,
      .depth = 1,
    },
    .mipLevels = 1, // TODO
    .arrayLayers = 1, // TODO
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = usage,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,

  };
  auto const allocCreateInfo = VmaAllocationCreateInfo {
      .flags = 0,
      .usage = VMA_MEMORY_USAGE_AUTO,
      .requiredFlags = 0,
      .preferredFlags = 0,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 1.0f,
  };
  VkImage image;
  VmaAllocation imageAllocation;
  VmaAllocationInfo imageAllocationInfo;
  vmaCreateImage(
    util::ctx().vmaAllocator,
    &imageInfo,
    &allocCreateInfo,
    &image,
    &imageAllocation,
    &imageAllocationInfo
  );
  puleLog(
    "extent { %u, %u, %u }, size %u",
    imageInfo.extent.width,
    imageInfo.extent.height,
    imageInfo.extent.depth,
    imageAllocationInfo.size
  );
  ::internalImages.emplace(
    reinterpret_cast<uint64_t>(image),
    InternalImage {
      .image = image,
      .allocation = imageAllocation,
    }
  );

  // if the image starts with any memory, put it into a buffer,
  // then copy buffer to image
  if (createInfo.optionalInitialData) {
    puleLog("Creating optional initial data");
    // create temporary staging buffer
    auto const uploadBuffer = puleGpuBufferCreate(
      puleCStr("staging-image-buffer"),
      imageAllocationInfo.size,
      PuleGpuBufferUsage_storage,
      PuleGpuBufferVisibilityFlag_hostWritable
    );
    puleGpuBufferMemcpy(
      PuleGpuBufferMappedFlushRange {
        .buffer = uploadBuffer,
        .byteOffset = 0,
        .byteLength = imageAllocationInfo.size,
      },
      createInfo.optionalInitialData
    );

    auto const copyBarrier = VkImageMemoryBarrier {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image,
      .subresourceRange = {
        .aspectMask = (
            createInfo.byteFormat == PuleGpuImageByteFormat_depth16
          ? VK_IMAGE_ASPECT_DEPTH_BIT
          : VK_IMAGE_ASPECT_COLOR_BIT
        ),
        .levelCount = 1,
        .layerCount = 1,
      },
    };
    auto const beginInfo = VkCommandBufferBeginInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
      .pInheritanceInfo = nullptr,
    };
    PULE_vkError(
      vkBeginCommandBuffer(
        util::ctx().utilCommandBuffer,
        &beginInfo
      )
    );
    vkCmdPipelineBarrier(
      util::ctx().utilCommandBuffer,
      VK_PIPELINE_STAGE_HOST_BIT,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      0, // dependency
      0, nullptr, // memory barrier
      0, nullptr, // buffer barrier
      1, &copyBarrier // image barrier
    );

    auto const region = VkBufferImageCopy {
      .imageSubresource {
        .aspectMask = (
            createInfo.byteFormat == PuleGpuImageByteFormat_depth16
          ? VK_IMAGE_ASPECT_DEPTH_BIT
          : VK_IMAGE_ASPECT_COLOR_BIT
        ),
        .layerCount = 1,
      },
      .imageExtent = {
        .width = createInfo.width,
        .height = createInfo.height,
        .depth = 1,
      }
    };
    vkCmdCopyBufferToImage(
      util::ctx().utilCommandBuffer,
      reinterpret_cast<VkBuffer>(uploadBuffer.id),
      image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      1, &region
    );

    auto const usageBarrier = VkImageMemoryBarrier {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_GENERAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .image = image,
      .subresourceRange = {
        .aspectMask = (
            createInfo.byteFormat == PuleGpuImageByteFormat_depth16
          ? VK_IMAGE_ASPECT_DEPTH_BIT
          : VK_IMAGE_ASPECT_COLOR_BIT
        ),
        .levelCount = 1,
        .layerCount = 1,
      },
    };
    vkCmdPipelineBarrier(
      util::ctx().utilCommandBuffer,
      VK_PIPELINE_STAGE_TRANSFER_BIT,
      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
      0, // dependency
      0, nullptr, // memory barrier
      0, nullptr, // buffer barrier
      1, &usageBarrier // image barrier
    );
    vkEndCommandBuffer(util::ctx().utilCommandBuffer);
    auto stagingSubmitInfo = VkSubmitInfo {
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = nullptr,
      .waitSemaphoreCount = 0, .pWaitSemaphores = nullptr,
      .pWaitDstStageMask = nullptr,
      .commandBufferCount = 1,
      .pCommandBuffers = &util::ctx().utilCommandBuffer,
      .signalSemaphoreCount = 0, .pSignalSemaphores = nullptr,
    };
    PULE_vkError(
      vkQueueSubmit(
        util::ctx().defaultQueue,
        1, &stagingSubmitInfo, nullptr
      )
    );
    // TODO replace with cmd pipeline barrier, or fence or something
    vkDeviceWaitIdle(util::ctx().device.logical);
  }

  util::ctx().images.emplace(
    reinterpret_cast<uint64_t>(image),
    util::ImageInfo {
      .label = std::string(createInfo.label.contents, createInfo.label.len),
    }
  );

  #if VK_VALIDATION_ENABLED
  { // name image
    VkDebugUtilsObjectNameInfoEXT nameInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .pNext = nullptr,
      .objectType = VK_OBJECT_TYPE_IMAGE,
      .objectHandle = reinterpret_cast<uint64_t>(image),
      .pObjectName = createInfo.label.contents,
    };
    vkSetDebugUtilsObjectNameEXT(util::ctx().device.logical, &nameInfo);
  }
  #endif

  puleLog("created image %u, label %s", reinterpret_cast<uint64_t>(image), createInfo.label.contents);

  return { .id = reinterpret_cast<uint64_t>(image), };
}

void puleGpuImageDestroy(PuleGpuImage const image) {
  if (image.id == 0) { return; }
  auto const & info = ::internalImages.at(image.id);
  vmaDestroyImage(util::ctx().vmaAllocator, info.image, info.allocation);
  // TODO... destroy image views in util
}

PuleStringView puleGpuImageLabel(PuleGpuImage const image) {
  if (image.id == 0) {
    PULE_assert(false && "image id is 0");
  }
  if (!util::ctx().images.contains(image.id)) {
    puleLog("image id %zu not found", image.id);
    return puleCStr("[unknown image]");
  }
  auto const & info = util::ctx().images.at(image.id);
  return puleCStr(info.label.c_str());
}

} // C

// ------------------------------------------------------------------------------
// -- FRAMEBUFFER ---------------------------------------------------------------
// ------------------------------------------------------------------------------

extern "C" {
PuleGpuFramebufferCreateInfo puleGpuFramebufferCreateInfo() {
  PuleGpuFramebufferCreateInfo info = {};
  memset(
    &info.attachment,
    0,
    sizeof(PuleGpuFramebufferAttachments)
  );
  return info;
}

PuleGpuFramebuffer puleGpuFramebufferCreate(
  PuleGpuFramebufferCreateInfo const info,
  PuleError * const error
) {
  return { 0 };
}

void puleGpuFramebufferDestroy(PuleGpuFramebuffer const framebuffer) {
}

PuleGpuImage puleGpuWindowImage() {
  VkImage const image = (
    util::ctx().swapchainImages[util::ctx().swapchainCurrentImageIdx]
  );
  return PuleGpuImage { .id = reinterpret_cast<uint64_t>(image), };
}

PuleGpuFramebufferAttachments puleGpuFramebufferAttachments(
  PuleGpuFramebuffer const framebuffer
) {
  PULE_assert(0);
}

PuleStringView puleGpuImageLayoutLabel(PuleGpuImageLayout const layout) {
  switch (layout) {
    case PuleGpuImageLayout_uninitialized:
      return puleCStr("uninitialized");
    case PuleGpuImageLayout_storage:
      return puleCStr("storage");
    case PuleGpuImageLayout_attachmentColor:
      return puleCStr("attachmentColor");
    case PuleGpuImageLayout_attachmentDepth:
      return puleCStr("attachmentDepth");
    case PuleGpuImageLayout_transferSrc:
      return puleCStr("transferSrc");
    case PuleGpuImageLayout_transferDst:
      return puleCStr("transferDst");
    case PuleGpuImageLayout_presentSrc:
      return puleCStr("presentSrc");
  }
}

} // C
