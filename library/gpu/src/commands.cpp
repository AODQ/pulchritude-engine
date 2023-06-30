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
  }
}

namespace {

void puClearImageColor(
  PuleGpuActionClearImageColor const action,
  VkCommandBuffer const commandBuffer,
  util::CommandBufferRecorder & commandBufferRecorder
) {
  /* prep */
  auto const clearSubresourceRange = VkImageSubresourceRange {
    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    .baseMipLevel = 0, .levelCount = 1,
    .baseArrayLayer = 0, .layerCount = 1,
  };
  auto color = VkClearColorValue {
    .float32 = {
      action.color.x, action.color.y, action.color.z, action.color.w,
    },
  };
  /* hazard */
  util::RecorderImage & recorderImage = (
    commandBufferRecorder.images.at(action.image.id)
  );
  auto const barrierImage = VkImageMemoryBarrier {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
    .pNext = nullptr,
    .srcAccessMask = recorderImage.access,
    .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
    .oldLayout = recorderImage.layout,
    .newLayout = VK_IMAGE_LAYOUT_GENERAL,
    .srcQueueFamilyIndex = recorderImage.deviceQueueIdx,
    .dstQueueFamilyIndex = util::ctx().device.queues.idxGTC,
    .image = reinterpret_cast<VkImage>(action.image.id),
    .subresourceRange = clearSubresourceRange,
  };
  recorderImage = util::RecorderImage {
    .access = barrierImage.dstAccessMask,
    .layout = barrierImage.newLayout,
    .deviceQueueIdx = util::ctx().device.queues.idxGTC,
  };
  vkCmdPipelineBarrier(
    commandBuffer,
    /*srcStageMask*/ VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
    /*dstStageMask*/ VK_PIPELINE_STAGE_TRANSFER_BIT,
    /*dependencyFlags*/ 0,
    /*memoryBarrier*/ 0, nullptr,
    /*bufferMemoryBarrier*/ 0, nullptr,
    /*imageMemoryBarrier*/ 1, &barrierImage
  );
  /* action */
  vkCmdClearColorImage(
    commandBuffer,
    reinterpret_cast<VkImage>(action.image.id),
    VK_IMAGE_LAYOUT_GENERAL,
    &color, 1, &clearSubresourceRange
  );
}

} // namespace

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
  PuleGpuCommandList const commandList,
  PuleGpuCommandPayload const beginCommandPayload
) {
  auto beginInfo = VkCommandBufferBeginInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = 0, // TODO::CRITCAL VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    .pInheritanceInfo = nullptr,
  };
  auto commandBuffer = reinterpret_cast<VkCommandBuffer>(commandList.id);
  PULE_assert(vkBeginCommandBuffer(commandBuffer, &beginInfo) == VK_SUCCESS);
  // TODO should I store the entire swapchain?
  util::CommandBufferRecorder cbRecorder;
  // move data from user command payload into recorder storage
  for (size_t it = 0; it < beginCommandPayload.payloadImagesLength; ++ it) {
    PuleGpuCommandPayloadImage const payloadImg = (
      beginCommandPayload.payloadImages[it]
    );
    cbRecorder.images.emplace(
      payloadImg.image.id,
      util::RecorderImage {
        .access = util::toVkAccessFlags(payloadImg.access),
        .layout = util::toVkImageLayout(payloadImg.layout),
        .deviceQueueIdx = util::ctx().device.queues.idxGTC,
      }
    );
  }
  util::ctx().commandBufferRecorders.emplace(
    commandList.id,
    util::CommandBufferRecorder { }
  );
  return { .id = reinterpret_cast<uint64_t>(commandBuffer), };
}

void puleGpuCommandListRecorderFinish(
  [[maybe_unused]] PuleGpuCommandListRecorder const commandListRecorder
) {
  vkEndCommandBuffer(reinterpret_cast<VkCommandBuffer>(commandListRecorder.id));
}
void puleGpuCommandListRecorderReset(
  [[maybe_unused]] PuleGpuCommandListRecorder const commandListRecorder
) {
  auto const commandBuffer = (
    reinterpret_cast<VkCommandBuffer>(commandListRecorder.id)
  );
  util::ctx().commandBufferRecorders.erase(commandListRecorder.id);
  vkResetCommandBuffer(commandBuffer, 0); // TODO what about reset flag?
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
        // TODO
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
      puClearImageColor(
        *reinterpret_cast<PuleGpuActionClearImageColor const *>(&command),
        commandBuffer,
        recorderInfo
      );
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
      // TODO::CRITICAL need the PipelineLayout to program the push apparently
      // TODO::CRITICAL need to prepare PuleGpuConstant array
      // vkCmdPushConstants(
      //   commandBuffer,
      //   layout,
      //   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      //   0,
      // );
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
        .dstSet = (
          util::ctx()
            .pipelines.at(recorderInfo.currentBoundPipeline.id)
            .descriptorSet
        ),
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
        1, 1, /* set, descriptorWriteCount */
        &writeDescriptorSet
      );
    }
    break;
    case PuleGpuAction_renderPassBegin: {
      auto const action = (
        *reinterpret_cast<PuleGpuActionRenderPassBegin const *>(&command)
      );
      VkRenderingAttachmentInfo colorAttachments[8];
      for (size_t it = 0; it <  8; ++ it) {
        if (action.colorAttachments[it].imageView.image.id == 0) {
          continue;
        }
        colorAttachments[it] = (
          imageAttachmentToVk(action.colorAttachments[it], false)
        );
      }
      VkRenderingAttachmentInfo depthAttachment = {};
      bool hasDepthAttachment = false;
      if (action.depthAttachment.imageView.image.id != 0) {
        depthAttachment = imageAttachmentToVk(action.depthAttachment, true);
        hasDepthAttachment = true;
      }
      auto const renderingInfo = VkRenderingInfo {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = VkRect2D {
          .offset = VkOffset2D {
            .x = action.viewportUpperLeft.x,
            .y = action.viewportUpperLeft.y,
          },
          .extent = VkExtent2D {
            .width = (uint32_t)action.viewportLowerRight.x,
            .height = (uint32_t)action.viewportLowerRight.y,
          },
        },
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = (uint32_t)action.colorAttachmentCount,
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
  }
}

void puleGpuCommandListSubmit(
  PuleGpuCommandListSubmitInfo const info,
  PuleError * const error
) {
  auto commandBuffer = reinterpret_cast<VkCommandBuffer>(info.commandList.id);
  auto submitInfo = VkSubmitInfo {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    .pNext = nullptr,
    // TODO::CRITICAL semaphores
    .waitSemaphoreCount = 0,
    .pWaitSemaphores = nullptr,
    .pWaitDstStageMask = nullptr,
    .commandBufferCount = 1,
    .pCommandBuffers = &commandBuffer,
    .signalSemaphoreCount = 0,
    .pSignalSemaphores = nullptr,
  };
  PULE_vkAssert(
    vkQueueSubmit(
      util::ctx().defaultQueue,
      1, &submitInfo,
      (
        info.fenceTargetFinish
          ? reinterpret_cast<VkFence>(info.fenceTargetFinish->id)
          : nullptr
      )
    ),
    PuleErrorGfx_invalidCommandList,
  );
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
