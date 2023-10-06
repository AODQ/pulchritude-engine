#include <pulchritude-gpu/commands.h>

#include <util.hpp>

#include <volk.h>

#include <string>
#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------
//-- ACTIONS -------------------------------------------------------------------
//------------------------------------------------------------------------------

VkRenderingAttachmentInfo imageAttachmentToVk(
  PuleGpuImageAttachment const attachment,
  bool const isDepth
) {
  return VkRenderingAttachmentInfo {
    .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
    .pNext = nullptr,
    .imageView = util::fetchImageView(attachment.imageView),
    .imageLayout = (
      isDepth
      ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
      : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    ),
    .resolveMode = VK_RESOLVE_MODE_NONE, // TODO support multisampling
    .resolveImageView = VK_NULL_HANDLE,
    .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    .loadOp = util::toVkAttachmentOpLoad(attachment.opLoad),
    .storeOp = util::toVkAttachmentOpStore(attachment.opStore),
    .clearValue = VkClearValue {
      .color = VkClearColorValue {
        .float32 = {
          attachment.clearColor.x, attachment.clearColor.y,
          attachment.clearColor.z, attachment.clearColor.w,
        },
      },
    },
  };
}

PuleStringView puleGpuActionToString(PuleGpuAction const action) {
  switch (action) {
    default: return puleCStr("unknown");
    case PuleGpuAction_bindPipeline:
      return puleCStr("bind-pipeline");
    case PuleGpuAction_bindBuffer:
      return puleCStr("bind-buffer");
    case PuleGpuAction_bindTexture:
      return puleCStr("bind-texture");
    case PuleGpuAction_bindElementBuffer:
      return puleCStr("bind-element-buffer");
    case PuleGpuAction_bindAttributeBuffer:
      return puleCStr("bind-attribute-buffer");
    case PuleGpuAction_clearImageColor:
      return puleCStr("clear-image-color");
    case PuleGpuAction_clearImageDepth:
      return puleCStr("clear-image-depth");
    case PuleGpuAction_dispatchRender:
      return puleCStr("dispatch-render");
    case PuleGpuAction_dispatchRenderIndirect:
      return puleCStr("dispatch-render-indirect");
    case PuleGpuAction_dispatchRenderElements:
      return puleCStr("dispatch-render-elements");
    case PuleGpuAction_pushConstants:
      return puleCStr("push-constants");
    case PuleGpuAction_dispatchCommandList:
      return puleCStr("dispatch-command-list");
    case PuleGpuAction_setScissor:
      return puleCStr("set-scissor");
    case PuleGpuAction_copyImageToImage:
      return puleCStr("copy-image-to-image");
  }
}

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

extern "C" {

PuleGpuCommandList puleGpuCommandListCreate(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleStringView const label
) {
  auto cmdBufferCi = VkCommandBufferAllocateInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = nullptr,
    .commandPool = util::ctx().defaultCommandPool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1,
  };
  VkCommandBuffer cmdBuffer;
  PULE_assert(
    vkAllocateCommandBuffers(
      util::ctx().device.logical,
      &cmdBufferCi,
      &cmdBuffer
    ) == VK_SUCCESS
  );
  // TODO name object
  util::ctx().commandLists.emplace(
    reinterpret_cast<uint64_t>(cmdBuffer),
    util::CommandList { .label = std::string(label.contents), }
  );
  return { .id = reinterpret_cast<uint64_t>(cmdBuffer), };
}

void puleGpuCommandListDestroy(PuleGpuCommandList const commandList) {
  if (commandList.id == 0) { return; }
  auto cmdBuffer = reinterpret_cast<VkCommandBuffer>(commandList.id);
  vkFreeCommandBuffers(
    util::ctx().device.logical,
    util::ctx().defaultCommandPool,
    1, &cmdBuffer
  );
}

PuleStringView puleGpuCommandListName(
  PuleGpuCommandList const commandListId
) {
  (void)commandListId;
  //auto & commandList = ::commandLists.at(commandListId.id);
  // TODO::CRITICAL return named object label
  return puleCStr("UNKNOWN YET");
}

PuleGpuCommandListRecorder puleGpuCommandListRecorder(
  PuleGpuCommandList const commandList
) {
  auto beginInfo = VkCommandBufferBeginInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = 0, // TODO::CRITCAL VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    .pInheritanceInfo = nullptr,
  };
  auto commandBuffer = reinterpret_cast<VkCommandBuffer>(commandList.id);
  PULE_assert(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS);
  util::CommandBufferRecorder cbRecorder;
  cbRecorder.commandList = commandList;
  // TODO should I store the entire swapchain?
  cbRecorder.images.emplace(
    reinterpret_cast<uint64_t>(
      util::ctx().swapchainImages[util::ctx().swapchainCurrentImageIdx]
    ),
    util::RecorderImage {
      .access = 0, // TODO change this i guess?
      .layout = VK_IMAGE_LAYOUT_UNDEFINED, // TODO change this i guess?
      .deviceQueueIdx = util::ctx().device.queues.idxGTC,
    }
  );
  util::ctx().commandBufferRecorders.emplace(commandList.id, cbRecorder);
  return { .id = reinterpret_cast<uint64_t>(commandBuffer), };
}

void puleGpuCommandListRecorderFinish(
  [[maybe_unused]] PuleGpuCommandListRecorder const commandListRecorder
) {
  vkEndCommandBuffer(reinterpret_cast<VkCommandBuffer>(commandListRecorder.id));
  util::ctx().commandBufferRecorders.erase(commandListRecorder.id);
}

void puleGpuCommandListAppendAction(
  PuleGpuCommandListRecorder const commandListRecorder,
  PuleGpuCommand const command
) {
  auto commandBuffer = (
    reinterpret_cast<VkCommandBuffer>(commandListRecorder.id)
  );

  auto & recorderInfo = (
    util::ctx().commandBufferRecorders.at(commandListRecorder.id)
  );

  switch (command.action) {
    default:
      puleLogError("Unknown command PuleGpuAction %d", command.action);
      PULE_assert(false);
    case PuleGpuAction_bindAttributeBuffer: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionBindAttributeBuffer const *>(&command)
      );
      auto buffer = reinterpret_cast<VkBuffer>(action.buffer.id);
      auto offset = VkDeviceSize { action.offset };
      auto stride = VkDeviceSize { action.stride };
      if (stride == 0) {
        auto const & pipelineInfo = util::ctx().pipelines.at(
          recorderInfo.currentBoundPipeline.id
        );
        (void)pipelineInfo;
        // TODO why are these not supported again? Is this Vulkan's API fault?
        PULE_assert(false && "zero strides not supported yet");
        //pipelineInfo.attributes.at(action.attributeIndex).stride;
      }
      vkCmdBindVertexBuffers2(
        commandBuffer,
        action.bindingIndex, 1,
        &buffer, &offset, /*size*/ nullptr, &stride
      );
    }
    break;
    case PuleGpuAction_bindPipeline: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionBindPipeline const *>(&command)
      );
      auto pipeline = reinterpret_cast<VkPipeline>(action.pipeline.id);
      vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline
      );
      recorderInfo.currentBoundPipeline = action.pipeline;
    }
    break;
    case PuleGpuAction_bindFramebuffer: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionBindFramebuffer const *>(&command)
      );
      auto framebuffer = reinterpret_cast<VkFramebuffer>(action.framebuffer.id);
      (void)framebuffer;
      // TODO::CRITICAL
    }
    break;
    case PuleGpuAction_clearImageColor: {
      //puClearImageColor(
      //  *reinterpret_cast<PuleGpuActionClearImageColor const *>(&command),
      //  commandBuffer,
      //  recorderInfo
      //);
    } break;
    case PuleGpuAction_clearImageDepth: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionClearImageDepth const *>(&command)
      );
      auto const depthStencil = VkClearDepthStencilValue {
        .depth = action.depth,
        .stencil = 0,
      };
      auto const clearSubresourceRange = VkImageSubresourceRange {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0, .levelCount = 1,
        .baseArrayLayer = 0, .layerCount = 1,
      };
      vkCmdClearDepthStencilImage(
        commandBuffer,
        reinterpret_cast<VkImage>(action.image.id),
        VK_IMAGE_LAYOUT_GENERAL,
        &depthStencil,
        1, &clearSubresourceRange
      );
    } break;
    case PuleGpuAction_dispatchRender: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionDispatchRender const *>(&command)
      );
      vkCmdDraw(
        commandBuffer, action.numVertices, 1, action.vertexOffset, 0
      );
    }
    break;
    case PuleGpuAction_dispatchRenderElements: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionDispatchRenderElements const *>(&command)
      );
      vkCmdDrawIndexed(
        commandBuffer,
        action.numElements, 1,
        action.elementOffset, action.baseVertexOffset,
        0
      );
    } break;
    case PuleGpuAction_dispatchRenderIndirect: {
      PULE_assert(false && "TODO"); // TODO
    } break;
    case PuleGpuAction_pushConstants: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionPushConstants const *>(&command)
      );
      (void)action;
      auto const & pipelineInfo = util::ctx().pipelines.at(
        recorderInfo.currentBoundPipeline.id
      );
      vkCmdPushConstants(
        commandBuffer,
        pipelineInfo.pipelineLayout,
        util::toVkShaderStageFlags(action.stage),
        action.byteOffset,
        action.byteLength,
        action.data
      );
    }
    break;
    case PuleGpuAction_dispatchCommandList: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionDispatchCommandList const *>(
          &command
        )
      );
      auto subCommandBuffer = (
        reinterpret_cast<VkCommandBuffer>(action.submitInfo.commandList.id)
      );
      vkCmdExecuteCommands(subCommandBuffer, 1, &subCommandBuffer);
    }
    break;
    case PuleGpuAction_bindTexture: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionBindTexture const *>(&command)
      );
      // write descriptor set to command 'cache'
      auto const imageInfo = VkDescriptorImageInfo {
        .sampler = VK_NULL_HANDLE,
        .imageView = util::fetchImageView(action.imageView),
        .imageLayout = util::toVkImageLayout(action.imageLayout),
      };
      auto const writeDescriptorSet = VkWriteDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = 0, // I believe this is ignored using pushDescriptorSet
        .dstBinding = (uint32_t)action.bindingIndex,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
      };
      vkCmdPushDescriptorSetKHR(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS, // TODO support compute bindpoint
        (
          util::ctx()
            .pipelines.at(recorderInfo.currentBoundPipeline.id)
            .pipelineLayout
        ),
        0, 1, /* set, descriptorwrite count */
        &writeDescriptorSet
      );
    }
    break;
    case PuleGpuAction_bindBuffer: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionBindBuffer const *>(&command)
      );
      // write the descriptor set to our command 'cache'
      auto const bufferInfo = VkDescriptorBufferInfo {
        .buffer = reinterpret_cast<VkBuffer>(action.buffer.id),
        .offset = (VkDeviceSize)action.offset,
        .range = (VkDeviceSize)action.byteLen,
      };
      auto const writeDescriptorSet = VkWriteDescriptorSet {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = 0,
        .dstBinding = (uint32_t)action.bindingIndex,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = util::toDescriptorType(action.bindingDescriptor),
        .pImageInfo = nullptr,
        .pBufferInfo = &bufferInfo,
        .pTexelBufferView = nullptr,
      };
      vkCmdPushDescriptorSetKHR(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS, // TODO support compute bindpoint
        (
          util::ctx()
            .pipelines.at(recorderInfo.currentBoundPipeline.id)
            .pipelineLayout
        ),
        0, 1, /* set, descriptorWriteCount */
        &writeDescriptorSet
      );
    }
    break;
    case PuleGpuAction_resourceBarrier: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionResourceBarrier const *>(&command)
      );
      std::vector<VkImageMemoryBarrier> imageBarriers;
      // iterate through image barriers
      for (size_t it = 0; it < action.resourceImageCount; ++ it) {
        auto const & imageBarrier = action.resourceImages[it];
        auto const imageSubresourceRange = VkImageSubresourceRange {
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .baseMipLevel = 0, .levelCount = 1,
          .baseArrayLayer = 0, .layerCount = 1,
        };
        imageBarriers.emplace_back(
          VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = util::toVkAccessFlags(imageBarrier.accessSrc),
            .dstAccessMask = util::toVkAccessFlags(imageBarrier.accessDst),
            .oldLayout = util::toVkImageLayout(imageBarrier.layoutSrc),
            .newLayout = util::toVkImageLayout(imageBarrier.layoutDst),
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = reinterpret_cast<VkImage>(imageBarrier.image.id),
            .subresourceRange = imageSubresourceRange,
          }
        );
      }
      puleLogDebug(
        "resource barrier src %s dst %s,\n -> %zu image barriers [[",
        puleGpuResourceBarrierStageLabel(action.stageSrc).contents,
        puleGpuResourceBarrierStageLabel(action.stageDst).contents,
        action.resourceImageCount
      );
      for (size_t it = 0; it < action.resourceImageCount; ++ it) {
        puleLogDebug(
          "  access %s -> %s, layout %s -> %s, label %s",
          puleGpuResourceAccessLabel(
            action.resourceImages[it].accessSrc
          ).contents,
          puleGpuResourceAccessLabel(
            action.resourceImages[it].accessDst
          ).contents,
          puleGpuImageLayoutLabel(
            action.resourceImages[it].layoutSrc
          ).contents,
          puleGpuImageLayoutLabel(
            action.resourceImages[it].layoutDst
          ).contents,
          puleGpuImageLabel(action.resourceImages[it].image).contents
        );
      }
      puleLogDebug("]]");
      vkCmdPipelineBarrier(
        commandBuffer,
        util::toVkPipelineStageFlags(action.stageSrc),
        util::toVkPipelineStageFlags(action.stageDst),
        0, // dependency flag
        0, nullptr, // memory barriers
        0, nullptr, // buffer memory barriers
        (uint32_t)imageBarriers.size(), imageBarriers.data()
      );
    } break;
    case PuleGpuAction_renderPassBegin: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionRenderPassBegin const *>(&command)
      );
      VkRenderingAttachmentInfo colorAttachments[8];
      for (size_t it = 0; it <  8; ++ it) {
        if (action.attachmentColor[it].imageView.image.id == 0) {
          continue;
        }
        colorAttachments[it] = (
          imageAttachmentToVk(action.attachmentColor[it], false)
        );
      }
      VkRenderingAttachmentInfo depthAttachment = {};
      bool hasDepthAttachment = false;
      if (action.attachmentDepth.imageView.image.id != 0) {
        depthAttachment = imageAttachmentToVk(action.attachmentDepth, true);
        hasDepthAttachment = true;
      }
      auto const renderingInfo = VkRenderingInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = VkRect2D {
          .offset = VkOffset2D {
            .x = action.viewportMin.x,
            .y = action.viewportMin.y,
          },
          .extent = VkExtent2D {
            .width = (uint32_t)action.viewportMax.x,
            .height = (uint32_t)action.viewportMax.y,
          },
        },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = (uint32_t)action.attachmentColorCount,
        .pColorAttachments = colorAttachments,
        .pDepthAttachment = hasDepthAttachment ? &depthAttachment : nullptr,
        .pStencilAttachment = nullptr,
      };
      vkCmdBeginRendering(commandBuffer, &renderingInfo);
    }
    break;
    case PuleGpuAction_renderPassEnd: {
      vkCmdEndRendering(commandBuffer);
    }
    break;
    case PuleGpuAction_bindElementBuffer: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionBindElementBuffer const *>(&command)
      );
      auto buffer = reinterpret_cast<VkBuffer>(action.buffer.id);
      vkCmdBindIndexBuffer(
        commandBuffer,
        buffer, action.offset, util::toVkIndexType(action.elementType)
      );
    }
    break;
    case PuleGpuAction_setScissor: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionSetScissor const *>(&command)
      );
      VkRect2D scissor;
      scissor.offset.x = action.scissorMin.x;
      scissor.offset.x = action.scissorMin.y;
      scissor.extent.width = (
        action.scissorMax.x - action.scissorMin.x
      );
      scissor.extent.height = (
        action.scissorMax.y - action.scissorMin.y
      );
      vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }
    break;
    case PuleGpuAction_copyImageToImage: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionCopyImageToImage const *>(&command)
      );
      auto const imageCopyRegion = VkImageCopy2KHR {
        .sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2,
        .pNext = nullptr,
        .srcSubresource = { // TODO::CRIT
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .mipLevel = 0,
          .baseArrayLayer = 0,
          .layerCount = 1,
        },
        .srcOffset = VkOffset3D {
          .x = (int32_t)action.srcOffset.x,
          .y = (int32_t)action.srcOffset.y,
          .z = (int32_t)action.srcOffset.z,
        },
        .dstSubresource = { // TODO::CRIT
          .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
          .mipLevel = 0,
          .baseArrayLayer = 0,
          .layerCount = 1,
        },
        .dstOffset = VkOffset3D {
          .x = (int32_t)action.dstOffset.x,
          .y = (int32_t)action.dstOffset.y,
          .z = (int32_t)action.dstOffset.z,
        },
        .extent = VkExtent3D {
          .width = action.extent.x,
          .height = action.extent.y,
          .depth = action.extent.z,
        },
      };
      auto const imageCopy = VkCopyImageInfo2KHR {
        .sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2_KHR,
        .pNext = nullptr,
        .srcImage = reinterpret_cast<VkImage>(action.srcImage.id),
        .dstImage = reinterpret_cast<VkImage>(action.dstImage.id),
        .regionCount = 1,
        .pRegions = &imageCopyRegion,
      };
      vkCmdCopyImage2KHR(commandBuffer, &imageCopy);
    }
    break;
  }
}

void puleGpuCommandListSubmit(
  PuleGpuCommandListSubmitInfo const info,
  PuleError * const error
) {
  auto commandBuffer = reinterpret_cast<VkCommandBuffer>(info.commandList.id);
  for (size_t it = 0; it < info.signalSemaphoreCount; ++ it) {
    if (info.signalSemaphores[it].id > 0) { continue; }
    // create semaphore if user's semaphore is null
    auto commandList = util::ctx().commandLists.at(info.commandList.id);
    info.signalSemaphores[it] = (
      puleGpuSemaphoreCreate(
        puleCStr((
          std::string("signal for command list '") + commandList.label + "'"
        ).c_str())
      )
    );
  }
  std::vector<VkPipelineStageFlags> waitStages(info.waitSemaphoreCount);
  for (size_t it = 0; it < info.waitSemaphoreCount; ++ it) {
    waitStages[it] = (
      util::toVkPipelineStageFlagBits(info.waitSemaphoreStages[it])
    );
  }
  auto const submitInfo = VkSubmitInfo {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    .waitSemaphoreCount = (uint32_t)info.waitSemaphoreCount,
    .pWaitSemaphores = (
      reinterpret_cast<VkSemaphore const *>(info.waitSemaphores)
    ),
    .pWaitDstStageMask = waitStages.data(),
    .commandBufferCount = 1,
    .pCommandBuffers = &commandBuffer,
    .signalSemaphoreCount = (uint32_t)info.signalSemaphoreCount,
    .pSignalSemaphores = reinterpret_cast<VkSemaphore *>(info.signalSemaphores),
  };
  PULE_vkAssert(
    vkQueueSubmit(
      util::ctx().defaultQueue,
      1, &submitInfo,
      reinterpret_cast<VkFence>(info.fenceTargetFinish.id)
    ),
    PuleErrorGfx_invalidCommandList,
  );
}

void puleGpuFrameEnd(
  size_t const waitSemaphoreCount,
  PuleGpuSemaphore const * const waitSemaphores
) {
  auto const presentInfo = VkPresentInfoKHR {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = nullptr,
    .waitSemaphoreCount = (uint32_t)waitSemaphoreCount,
    .pWaitSemaphores = reinterpret_cast<VkSemaphore const *>(waitSemaphores),
    .swapchainCount = 1,
    .pSwapchains = &util::ctx().swapchain,
    .pImageIndices = &util::ctx().swapchainCurrentImageIdx,
    .pResults = nullptr,
  };
  PULE_assert(
    vkQueuePresentKHR(util::ctx().defaultQueue, &presentInfo) == VK_SUCCESS
  );
  util::ctx().frameIdx += 1;

  // TODO remove this idle, something isn't being synchronized properly :/
  vkQueueWaitIdle(util::ctx().defaultQueue);
}

} // extern C

namespace {

// TODO this should probably go to some serializer module
// void commandListDump(PuleGpuCommandList const commandList, int32_t level=0) {
  // auto const commandListFind = ::commandLists.find(commandList.id);
  // std::string levelStr = "";
  // for (int32_t l = 0; l < level+1; ++ l) levelStr += "|  ";
  // char const * const levelCStr = levelStr.c_str();
  // puleLogDebug(
  //   "%sCommand list dump of '%s'",
  //   levelCStr, commandListFind->second.label.c_str()
  // );
  // for (auto const command : commandListFind->second.actions) {
  //   PuleGpuAction const actionType = (
  //     *reinterpret_cast<PuleGpuAction const *>(&command)
  //   );
  //   switch (actionType) {
  //     default:
  //       puleLogError("%sunknown action ID %d", levelCStr, actionType);
  //     break;
  //     case PuleGpuAction_clearImageColor: {
  //       puleLogDebug("%sPuleGpuAction_clearImageColor", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionClearImageColor const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_clearImageDepth: {
  //       puleLogDebug("%sPuleGpuAction_clearImageDepth", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionClearImageDepth const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_pushConstants: {
  //       puleLogDebug("%sPuleGpuAction_pushConstants", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionPushConstants const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_bindBuffer: {
  //       puleLogDebug("%sPuleGpuAction_bindBuffer", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionBindBuffer const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_bindAttributeBuffer: {
  //       puleLogDebug("%sPuleGpuAction_bindAttributeBuffer", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionBindAttribute const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_bindPipeline: {
  //       puleLogDebug("%sPuleGpuAction_bindPipeline", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionBindPipeline const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_bindFramebuffer: {
  //       puleLogDebug("%sPuleGpuAction_bindFramebuffer", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionBindFramebuffer const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_dispatchRender: {
  //       puleLogDebug("%sPuleGpuAction_dispatchRender", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionDispatchRender const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_dispatchRenderIndirect: {
  //       puleLogDebug("%sPuleGpuAction_dispatchRenderIndirect", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionDispatchRenderIndirect const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_dispatchRenderElements: {
  //       puleLogDebug("%sPuleGpuAction_dispatchRenderElements", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionDispatchRenderElements const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGpuAction_dispatchCommandList: {
  //       puleLogDebug("%sPuleGpuAction_dispatchCommandList", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGpuActionDispatchCommandList const *>(
  //           &command
  //         )
  //       );
  //       commandListDump(action.submitInfo.commandList, level+1);
  //     } break;
  //   }
  // }
// }

} // namespace

extern "C" {

void puleGpuCommandListDump(PuleGpuCommandList const) {
  // TODO -- this can only be done by serializer module i suppose
  //puleLogDebug("-------------------------------------------------");
  //puleLogDebug("<NO>");
  // commandListDump(commandList);
  //puleLogDebug("-------------------------------------------------");
}

} // extern C

//------------------------------------------------------------------------------
//-- COMMAND LIST CHAIN --------------------------------------------------------
//------------------------------------------------------------------------------

namespace {
struct ImplCommandListChainCommand {
  PuleGpuCommandList commandList;
  PuleGpuFence fence;
  std::string label;
  uint64_t frameIdx;
};
struct ImplCommandListChain {
  std::vector<ImplCommandListChainCommand> commandListChain;
  PuleAllocator allocator;
};
std::unordered_map<uint64_t, ImplCommandListChain> commandListChains;
uint64_t commandListChainId = 0;
} // namespace

extern "C" {

PuleGpuCommandListChain puleGpuCommandListChainCreate(
  PuleAllocator const allocator,
  PuleStringView const label
) {
  auto const id = commandListChainId ++;
  std::vector<ImplCommandListChainCommand> commandListChain;
  for (size_t it = 0; it < util::ctx().swapchainImages.size(); ++ it) {
    std::string const fenceLabel = (
      std::string(label.contents, label.len) + "-frame-" + std::to_string(it)
    );
    commandListChain.emplace_back(ImplCommandListChainCommand{
      .commandList = PuleGpuCommandList{.id = 0,},
      .fence = puleGpuFenceCreate(puleCStr(fenceLabel.c_str())),
      .label = std::string(label.contents, label.len),
      .frameIdx = 0,
    });
  }
  commandListChains.emplace(
    id,
    ImplCommandListChain{
      .commandListChain = std::move(commandListChain),
      .allocator = allocator,
    }
  );
  return {.id = id,};
}

void puleGpuCommandListChainDestroy(PuleGpuCommandListChain const chain) {
  if (chain.id == 0) { return; }
  commandListChains.erase(chain.id);
}

PuleGpuCommandList puleGpuCommandListChainCurrent(
  PuleGpuCommandListChain const chain
) {
  auto & commandListChain = commandListChains.at(chain.id);
  auto & currentCommandList = (
    commandListChain.commandListChain.at(util::ctx().swapchainCurrentImageIdx)
  );
  // check if this has already been handled this frame
  if (currentCommandList.frameIdx == util::ctx().frameIdx) {
    return currentCommandList.commandList;
  }
  // create command list, destroy previous one if needed
  if (currentCommandList.commandList.id != 0) {
    puleGpuFenceWaitSignal(
      currentCommandList.fence,
      PuleNanosecond { PuleGpuSignalTime_forever, }
    );
    puleGpuFenceReset(currentCommandList.fence);
    puleGpuCommandListDestroy(currentCommandList.commandList);
  }
  std::string const label = (
    currentCommandList.label + "-[" + std::to_string(util::ctx().frameIdx) + "]"
  );
  currentCommandList.frameIdx = util::ctx().frameIdx;
  return currentCommandList.commandList = (
    puleGpuCommandListCreate(
      commandListChain.allocator, puleCStr(label.c_str())
    )
  );
}

PuleGpuFence puleGpuCommandListChainCurrentFence(
  PuleGpuCommandListChain const commandListChain
) {
  auto & chain = commandListChains.at(commandListChain.id);
  auto & currentCommandList = (
    chain.commandListChain.at(util::ctx().swapchainCurrentImageIdx)
  );
  // TODO move the asserts to debug layer
  PULE_assert(currentCommandList.commandList.id != 0);
  PULE_assert(currentCommandList.fence.id != 0);
  PULE_assert(currentCommandList.frameIdx == util::ctx().frameIdx);
  return currentCommandList.fence;
}

PuleStringView puleGpuResourceBarrierStageLabel(
  PuleGpuResourceBarrierStage const stage
) {
  switch (stage) {
    case PuleGpuResourceBarrierStage_top:
      return puleCStr("top");
    case PuleGpuResourceBarrierStage_drawIndirect:
      return puleCStr("drawIndirect");
    case PuleGpuResourceBarrierStage_vertexInput:
      return puleCStr("vertexInput");
    case PuleGpuResourceBarrierStage_shaderFragment:
      return puleCStr("shaderFragment");
    case PuleGpuResourceBarrierStage_shaderVertex:
      return puleCStr("shaderVertex");
    case PuleGpuResourceBarrierStage_shaderCompute:
      return puleCStr("shaderCompute");
    case PuleGpuResourceBarrierStage_outputAttachmentColor:
      return puleCStr("outputAttachmentColor");
    case PuleGpuResourceBarrierStage_transfer:
      return puleCStr("transfer");
    case PuleGpuResourceBarrierStage_bottom:
      return puleCStr("bottom");
  }
}

PuleStringView puleGpuResourceAccessLabel(PuleGpuResourceAccess const access) {
  switch (access) {
    case PuleGpuResourceAccess_none:
      return puleCStr("PuleGpuResourceAccess_none");
    case PuleGpuResourceAccess_indirectCommandRead:
      return puleCStr("PuleGpuResourceAccess_indirectCommandRead");
    case PuleGpuResourceAccess_indexRead:
      return puleCStr("PuleGpuResourceAccess_indexRead");
    case PuleGpuResourceAccess_vertexAttributeRead:
      return puleCStr("PuleGpuResourceAccess_vertexAttributeRead");
    case PuleGpuResourceAccess_uniformRead:
      return puleCStr("PuleGpuResourceAccess_uniformRead");
    case PuleGpuResourceAccess_inputAttachmentRead:
      return puleCStr("PuleGpuResourceAccess_inputAttachmentRead");
    case PuleGpuResourceAccess_shaderRead:
      return puleCStr("PuleGpuResourceAccess_shaderRead");
    case PuleGpuResourceAccess_shaderWrite:
      return puleCStr("PuleGpuResourceAccess_shaderWrite");
    case PuleGpuResourceAccess_attachmentColorRead:
      return puleCStr("PuleGpuResourceAccess_attachmentColorRead");
    case PuleGpuResourceAccess_attachmentColorWrite:
      return puleCStr("PuleGpuResourceAccess_attachmentColorWrite");
    case PuleGpuResourceAccess_attachmentDepthRead:
      return puleCStr("PuleGpuResourceAccess_attachmentDepthRead");
    case PuleGpuResourceAccess_attachmentDepthWrite:
      return puleCStr("PuleGpuResourceAccess_attachmentDepthWrite");
    case PuleGpuResourceAccess_transferRead:
      return puleCStr("PuleGpuResourceAccess_transferRead");
    case PuleGpuResourceAccess_transferWrite:
      return puleCStr("PuleGpuResourceAccess_transferWrite");
    case PuleGpuResourceAccess_hostRead:
      return puleCStr("PuleGpuResourceAccess_hostRead");
    case PuleGpuResourceAccess_hostWrite:
      return puleCStr("PuleGpuResourceAccess_hostWrite");
    case PuleGpuResourceAccess_memoryRead:
      return puleCStr("PuleGpuResourceAccess_memoryRead");
    case PuleGpuResourceAccess_memoryWrite:
      return puleCStr("PuleGpuResourceAccess_memoryWrite");
  }
}

} // extern C
