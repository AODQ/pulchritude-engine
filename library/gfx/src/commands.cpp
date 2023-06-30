#include <pulchritude-gfx/commands.h>

#include <util.hpp>

#include <volk.h>

#include <string>
#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------
//-- ACTIONS -------------------------------------------------------------------
//------------------------------------------------------------------------------

VkRenderingAttachmentInfo imageAttachmentToVk(
  PuleGfxImageAttachment const attachment,
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

PuleStringView puleGfxActionToString(PuleGfxAction const action) {
  switch (action) {
    default: return puleCStr("unknown");
    case PuleGfxAction_bindPipeline:
      return puleCStr("bind-pipeline");
    case PuleGfxAction_bindBuffer:
      return puleCStr("bind-buffer");
    case PuleGfxAction_bindElementBuffer:
      return puleCStr("bind-element-buffer");
    case PuleGfxAction_bindAttributeBuffer:
      return puleCStr("bind-attribute-buffer");
    case PuleGfxAction_clearImageColor:
      return puleCStr("clear-image-color");
    case PuleGfxAction_clearImageDepth:
      return puleCStr("clear-image-depth");
    case PuleGfxAction_dispatchRender:
      return puleCStr("dispatch-render");
    case PuleGfxAction_dispatchRenderIndirect:
      return puleCStr("dispatch-render-indirect");
    case PuleGfxAction_dispatchRenderElements:
      return puleCStr("dispatch-render-elements");
    case PuleGfxAction_pushConstants:
      return puleCStr("push-constants");
    case PuleGfxAction_dispatchCommandList:
      return puleCStr("dispatch-command-list");
  }
}

namespace {

void puClearImageColor(
  PuleGfxActionClearImageColor const action,
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

PuleGfxCommandList puleGfxCommandListCreate(
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

void puleGfxCommandListDestroy(PuleGfxCommandList const commandList) {
  if (commandList.id == 0) { return; }
  auto cmdBuffer = reinterpret_cast<VkCommandBuffer>(commandList.id);
  vkFreeCommandBuffers(
    util::ctx().device.logical,
    util::ctx().defaultCommandPool,
    1, &cmdBuffer
  );
}

PuleStringView puleGfxCommandListName(
  PuleGfxCommandList const commandListId
) {
  (void)commandListId;
  //auto & commandList = ::commandLists.at(commandListId.id);
  // TODO::CRITICAL return named object label
  return puleCStr("UNKNOWN YET");
}

PuleGfxCommandListRecorder puleGfxCommandListRecorder(
  PuleGfxCommandList const commandList,
  PuleGfxCommandPayload const beginCommandPayload
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
    PuleGfxCommandPayloadImage const payloadImg = (
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

void puleGfxCommandListRecorderFinish(
  [[maybe_unused]] PuleGfxCommandListRecorder const commandListRecorder
) {
  vkEndCommandBuffer(reinterpret_cast<VkCommandBuffer>(commandListRecorder.id));
}
void puleGfxCommandListRecorderReset(
  [[maybe_unused]] PuleGfxCommandListRecorder const commandListRecorder
) {
  auto const commandBuffer = (
    reinterpret_cast<VkCommandBuffer>(commandListRecorder.id)
  );
  util::ctx().commandBufferRecorders.erase(commandListRecorder.id);
  vkResetCommandBuffer(commandBuffer, 0); // TODO what about reset flag?
}

void puleGfxCommandListAppendAction(
  PuleGfxCommandListRecorder const commandListRecorder,
  PuleGfxCommand const command
) {
  auto commandBuffer = (
    reinterpret_cast<VkCommandBuffer>(commandListRecorder.id)
  );

  auto & recorderInfo = (
    util::ctx().commandBufferRecorders.at(commandListRecorder.id)
  );

  switch (command.action) {
    default:
      puleLogError("Unknown command PuleGfxAction %d", command.action);
      PULE_assert(false);
    case PuleGfxAction_bindAttributeBuffer: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionBindAttributeBuffer const *>(&command)
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
    case PuleGfxAction_bindPipeline: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionBindPipeline const *>(&command)
      );
      auto pipeline = reinterpret_cast<VkPipeline>(action.pipeline.id);
      vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline
      );
      recorderInfo.currentBoundPipeline = action.pipeline;
    }
    break;
    case PuleGfxAction_bindFramebuffer: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionBindFramebuffer const *>(&command)
      );
      auto framebuffer = reinterpret_cast<VkFramebuffer>(action.framebuffer.id);
      (void)framebuffer;
      // TODO::CRITICAL
    }
    break;
    case PuleGfxAction_clearImageColor: {
      puClearImageColor(
        *reinterpret_cast<PuleGfxActionClearImageColor const *>(&command),
        commandBuffer,
        recorderInfo
      );
    } break;
    case PuleGfxAction_clearImageDepth: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionClearImageDepth const *>(&command)
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
    case PuleGfxAction_dispatchRender: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionDispatchRender const *>(&command)
      );
      vkCmdDraw(
        commandBuffer, action.numVertices, 1, action.vertexOffset, 0
      );
    }
    break;
    case PuleGfxAction_dispatchRenderElements: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionDispatchRenderElements const *>(&command)
      );
      vkCmdDrawIndexed(
        commandBuffer,
        action.numElements, 1,
        action.elementOffset, action.baseVertexOffset,
        0
      );
    } break;
    case PuleGfxAction_dispatchRenderIndirect: {
      PULE_assert(false && "TODO"); // TODO
    } break;
    case PuleGfxAction_pushConstants: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionPushConstants const *>(&command)
      );
      (void)action;
      // TODO::CRITICAL need the PipelineLayout to program the push apparently
      // TODO::CRITICAL need to prepare PuleGfxConstant array
      // vkCmdPushConstants(
      //   commandBuffer,
      //   layout,
      //   VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
      //   0,
      // );
    }
    break;
    case PuleGfxAction_dispatchCommandList: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionDispatchCommandList const *>(
          &command
        )
      );
      auto subCommandBuffer = (
        reinterpret_cast<VkCommandBuffer>(action.submitInfo.commandList.id)
      );
      vkCmdExecuteCommands(subCommandBuffer, 1, &subCommandBuffer);
    }
    break;
    case PuleGfxAction_bindBuffer: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionBindBuffer const *>(&command)
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
    case PuleGfxAction_renderPassBegin: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionRenderPassBegin const *>(&command)
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
    case PuleGfxAction_renderPassEnd: {
      vkCmdEndRendering(commandBuffer);
    }
    break;
    case PuleGfxAction_bindElementBuffer: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionBindElementBuffer const *>(&command)
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

void puleGfxCommandListSubmit(
  PuleGfxCommandListSubmitInfo const info,
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
// void commandListDump(PuleGfxCommandList const commandList, int32_t level=0) {
  // auto const commandListFind = ::commandLists.find(commandList.id);
  // std::string levelStr = "";
  // for (int32_t l = 0; l < level+1; ++ l) levelStr += "|  ";
  // char const * const levelCStr = levelStr.c_str();
  // puleLogDebug(
  //   "%sCommand list dump of '%s'",
  //   levelCStr, commandListFind->second.label.c_str()
  // );
  // for (auto const command : commandListFind->second.actions) {
  //   PuleGfxAction const actionType = (
  //     *reinterpret_cast<PuleGfxAction const *>(&command)
  //   );
  //   switch (actionType) {
  //     default:
  //       puleLogError("%sunknown action ID %d", levelCStr, actionType);
  //     break;
  //     case PuleGfxAction_clearImageColor: {
  //       puleLogDebug("%sPuleGfxAction_clearImageColor", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionClearImageColor const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_clearImageDepth: {
  //       puleLogDebug("%sPuleGfxAction_clearImageDepth", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionClearImageDepth const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_pushConstants: {
  //       puleLogDebug("%sPuleGfxAction_pushConstants", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionPushConstants const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_bindBuffer: {
  //       puleLogDebug("%sPuleGfxAction_bindBuffer", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionBindBuffer const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_bindAttributeBuffer: {
  //       puleLogDebug("%sPuleGfxAction_bindAttributeBuffer", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionBindAttribute const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_bindPipeline: {
  //       puleLogDebug("%sPuleGfxAction_bindPipeline", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionBindPipeline const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_bindFramebuffer: {
  //       puleLogDebug("%sPuleGfxAction_bindFramebuffer", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionBindFramebuffer const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_dispatchRender: {
  //       puleLogDebug("%sPuleGfxAction_dispatchRender", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionDispatchRender const *>(&command)
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_dispatchRenderIndirect: {
  //       puleLogDebug("%sPuleGfxAction_dispatchRenderIndirect", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionDispatchRenderIndirect const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_dispatchRenderElements: {
  //       puleLogDebug("%sPuleGfxAction_dispatchRenderElements", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionDispatchRenderElements const *>(
  //           &command
  //         )
  //       );
  //       (void)action;
  //     } break;
  //     case PuleGfxAction_dispatchCommandList: {
  //       puleLogDebug("%sPuleGfxAction_dispatchCommandList", levelCStr);
  //       auto const action = (
  //         *reinterpret_cast<PuleGfxActionDispatchCommandList const *>(
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

void puleGfxCommandListDump(PuleGfxCommandList const) {
  // TODO -- this can only be done by serializer module i suppose
  //puleLogDebug("-------------------------------------------------");
  //puleLogDebug("<NO>");
  // commandListDump(commandList);
  //puleLogDebug("-------------------------------------------------");
}

} // extern C
