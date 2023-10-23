#include <pulchritude-gpu/image.h>

// module include
#include <util.hpp>

// pulchritude engine include
#include <pulchritude-log/log.h>

// third party include
#include <volk.h>

// stdlib include
#include <unordered_map>

namespace { // sampler
  std::unordered_map<uint64_t, PuleGpuSamplerCreateInfo> samplers;
  uint64_t samplersIt = 1;

#if 0
  GLenum imageMagnificationToGl(PuleGpuImageMagnification const magnify) {
    switch (magnify) {
      default:
        puleLogError("unknown image magnification: %d", magnify);
        return GL_NEAREST;
      case PuleGpuImageMagnification_nearest:
        return GL_NEAREST;
    }
  }

  GLenum imageWrapToGl(PuleGpuImageWrap const wrap) {
    switch (wrap) {
      default:
        puleLogError("unknown image wrap: %d", wrap);
        return GL_CLAMP_TO_EDGE;
      case PuleGpuImageWrap_clampToEdge:
        return GL_CLAMP_TO_EDGE;
    }
  }

  void applySampler(GLuint const textureHandle, PuleGpuSampler const sampler) {
    PuleGpuSamplerCreateInfo const & samplerInfo = samplers.at(sampler.id);
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_MAG_FILTER,
      imageMagnificationToGl(samplerInfo.magnify)
    );
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_MIN_FILTER,
      imageMagnificationToGl(samplerInfo.minify)
    );
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_WRAP_S,
      imageWrapToGl(samplerInfo.wrapU)
    );
    glTextureParameteri(
      textureHandle,
      GL_TEXTURE_WRAP_T,
      imageWrapToGl(samplerInfo.wrapV)
    );
  }
#endif
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

#if 0
namespace { // image
  GLenum byteFormatToGlInternalFormat(PuleGpuImageByteFormat const byteFormat) {
    switch (byteFormat) {
      default:
        puleLogError("unknown byte format: %d", byteFormat);
        return GL_RGBA8;
      case PuleGpuImageByteFormat_rgba8U: return GL_RGBA8;
      case PuleGpuImageByteFormat_rgb8U: return GL_RGB8;
      case PuleGpuImageByteFormat_r8U: return GL_R8;
      case PuleGpuImageByteFormat_depth16: return GL_DEPTH_COMPONENT16;
    }
  }

  GLenum byteFormatToGlFormat(PuleGpuImageByteFormat const byteFormat) {
    switch (byteFormat) {
      default:
        puleLogError("unknown byte format: %d", byteFormat);
        return GL_RGBA;
      case PuleGpuImageByteFormat_rgba8U:
        return GL_RGBA;
      case PuleGpuImageByteFormat_rgb8U:
        return GL_RGB;
      case PuleGpuImageByteFormat_r8U:
        return GL_RED;
      case PuleGpuImageByteFormat_depth16: return GL_DEPTH_COMPONENT;
    }
  }

  GLenum byteFormatToGlType(PuleGpuImageByteFormat const byteFormat) {
    switch (byteFormat) {
      default:
        puleLogError("unknown byte format: %d", byteFormat);
        return GL_UNSIGNED_BYTE;
      case PuleGpuImageByteFormat_rgb8U:
      case PuleGpuImageByteFormat_rgba8U:
      case PuleGpuImageByteFormat_r8U:
        return GL_UNSIGNED_BYTE;
      case PuleGpuImageByteFormat_depth16:
        return GL_UNSIGNED_SHORT;
    }
  }

  GLenum imageTargetToGl(PuleGpuImageTarget const imageTarget) {
    switch (imageTarget) {
      default:
        puleLogError("unknown image target: %d", imageTarget);
        return GL_TEXTURE_2D;
      case PuleGpuImageTarget_i2D:
        return GL_TEXTURE_2D;
    }
  }
}
#endif

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
    .usage = (
        VK_IMAGE_USAGE_SAMPLED_BIT
      | VK_IMAGE_USAGE_TRANSFER_DST_BIT
      | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
      | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT

     ),
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
      createInfo.optionalInitialData,
      imageAllocationInfo.size,
      PuleGpuBufferUsage_storage,
      PuleGpuBufferVisibilityFlag_hostWritable
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
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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
    PULE_assert(
      vkBeginCommandBuffer(
        util::ctx().utilCommandBuffer,
        &beginInfo
      ) == VK_SUCCESS
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
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
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
    PULE_assert(
      vkQueueSubmit(
        util::ctx().defaultQueue,
        1, &stagingSubmitInfo, nullptr
      ) == VK_SUCCESS
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

#if 0
namespace {
  GLuint framebufferAttachmenToGl(PuleGpuFramebufferAttachment attachment) {
    switch (attachment) {
      default:
        puleLogError("unknown attachment: %d", attachment);
      [[fallthrough]];
      case PuleGpuFramebufferAttachment_color0:
        return GL_COLOR_ATTACHMENT0;
      case PuleGpuFramebufferAttachment_color1:
        return GL_COLOR_ATTACHMENT1;
      case PuleGpuFramebufferAttachment_color3:
        return GL_COLOR_ATTACHMENT2;
      case PuleGpuFramebufferAttachment_color4:
        return GL_COLOR_ATTACHMENT3;
      case PuleGpuFramebufferAttachment_depth:
        return GL_DEPTH_ATTACHMENT;
      case PuleGpuFramebufferAttachment_stencil:
        return GL_STENCIL_ATTACHMENT;
      case PuleGpuFramebufferAttachment_depthStencil:
        return GL_DEPTH_STENCIL_ATTACHMENT;
    }
  }
} // namespace
#endif

extern "C" {
PuleGpuFramebufferCreateInfo puleGpuFramebufferCreateInfo() {
  PuleGpuFramebufferCreateInfo info;
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
  #if 0
  GLuint framebuffer;
  glCreateFramebuffers(1, &framebuffer);

  for (size_t it = 0; it < PuleGpuFramebufferAttachment_End; ++ it) {
    if (info.attachment.images[it].image.id == 0) {
      continue;
    }

    puleLog("BINDING FRAMEBUFFER %u", it);
    glNamedFramebufferTexture(
      framebuffer,
      framebufferAttachmenToGl(static_cast<PuleGpuFramebufferAttachment>(it)),
      info.attachment.images[it].image.id,
      info.attachment.images[it].mipmapLevel
    );
  }

  PULE_errorAssert(
    (
      glCheckNamedFramebufferStatus(
        framebuffer, GL_FRAMEBUFFER
      ) == GL_FRAMEBUFFER_COMPLETE
    ),
    PuleErrorGfx_invalidFramebuffer,
    {0}
  )

  return { framebuffer };
  #endif
  return { 0 };
}

void puleGpuFramebufferDestroy(PuleGpuFramebuffer const framebuffer) {
  #if 0
  if (framebuffer.id == 0) { return; }
  auto const glFramebuffer = static_cast<GLuint>(framebuffer.id);
  if (glFramebuffer != 0) {
    glDeleteFramebuffers(1, &glFramebuffer);
  }
  #endif
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
