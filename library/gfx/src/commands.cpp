#include <pulchritude-gfx/commands.h>

#include <util.hpp>

#include <glad/glad.h>

#include <string>
#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------
//-- ACTIONS -------------------------------------------------------------------
//------------------------------------------------------------------------------

PuleStringView puleGfxActionToString(PuleGfxAction const action) {
  switch (action) {
    default: return puleCStr("unknown");
    case PuleGfxAction_bindPipeline:
      return puleCStr("bind-pipeline");
    case PuleGfxAction_bindAttribute:
      return puleCStr("bind-attribute");
    case PuleGfxAction_clearFramebufferColor:
      return puleCStr("clear-framebuffer-color");
    case PuleGfxAction_clearFramebufferDepth:
      return puleCStr("clear-framebuffer-depth");
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

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

namespace {
  struct CommandList {
    PuleAllocator allocator;
    // TODO use allocator instead of vector
    std::vector<PuleGfxCommand> actions;
    std::vector<uint8_t> constantData;
    std::string label;
  };
  std::unordered_map<uint64_t, CommandList> commandLists;
  uint64_t commandListsIt = 1;

  GLenum drawPrimitiveToGl(PuleGfxDrawPrimitive drawPrimitive) {
    switch (drawPrimitive) {
      default:
        puleLogError("unknown draw primitive: %d", drawPrimitive);
      return 0;
      case PuleGfxDrawPrimitive_triangle: return GL_TRIANGLES;
      case PuleGfxDrawPrimitive_triangleStrip: return GL_TRIANGLE_STRIP;
      case PuleGfxDrawPrimitive_line: return GL_LINES;
      case PuleGfxDrawPrimitive_point: return GL_POINTS;
    }
  }

  GLenum elementTypeToGl(PuleGfxElementType elementType) {
    switch (elementType) {
      default:
        puleLogError("unknown element type: %d", elementType);
      return 0;
      case PuleGfxElementType_u8: return GL_UNSIGNED_BYTE;
      case PuleGfxElementType_u16: return GL_UNSIGNED_SHORT;
      case PuleGfxElementType_u32: return GL_UNSIGNED_INT;
    }
  }
}

extern "C" {

PuleGfxCommandList puleGfxCommandListCreate(
  PuleAllocator const allocator,
  PuleStringView const label
) {
  PuleGfxCommandList commandList = {
    .id = commandListsIt,
  };
  ::commandLists.emplace(
    commandList.id,
    ::CommandList{
      .allocator = allocator,
      .actions = {},
      .constantData = {},
      .label = label.contents,
    }
  );
  commandListsIt += 1;
  return commandList;
}

void puleGfxCommandListDestroy(PuleGfxCommandList const commandList) {
  if (commandList.id == 0) { return; }
  ::commandLists.erase(commandList.id);
}

PuleStringView puleGfxCommandListName(
  PuleGfxCommandList const commandListId
) {
  auto & commandList = ::commandLists.at(commandListId.id);
  return puleCStr(commandList.label.c_str());
}

PuleGfxCommandListRecorder puleGfxCommandListRecorder(
  PuleGfxCommandList const commandList
) {
  // TODO just need to check that it's not already being recorded to
  return { commandList.id };
}

void puleGfxCommandListRecorderFinish(
  [[maybe_unused]] PuleGfxCommandListRecorder const commandListRecorder
) {
}
void puleGfxCommandListRecorderReset(
  [[maybe_unused]] PuleGfxCommandListRecorder const commandListRecorder
) {
  auto & commandList = ::commandLists.at(commandListRecorder.id);
  commandList.actions.resize(0);
  commandList.constantData.resize(0);
}

void puleGfxCommandListAppendAction(
  PuleGfxCommandListRecorder const commandListRecorder,
  PuleGfxCommand const command
) {
  auto & commandList = ::commandLists.at(commandListRecorder.id);

  // validate
  switch (command.action) {
    default: break;
    case PuleGfxAction_bindPipeline: {
      auto const action = (
        *reinterpret_cast<PuleGfxActionBindPipeline const *>(
          &command
        )
      );
      if (action.pipeline.id == 0) {
        puleLogError("attempting to bind null pipeline to command list");
      }
    } break;
  }

  switch (command.action) {
    default:
      commandList.actions.emplace_back(command);
    break;
    case PuleGfxAction_pushConstants: {
      // allocate push constants into command list's constant data
      size_t const prevConstantDataLength = commandList.constantData.size();
      commandList.constantData.resize(
        commandList.constantData.size()
        + sizeof(PuleGfxConstant) * command.pushConstants.constantsLength
      );
      memcpy(
        commandList.constantData.data() + prevConstantDataLength,
        command.pushConstants.constants,
        sizeof(PuleGfxConstant) * command.pushConstants.constantsLength
      );

      // assign push constants to command
      PuleGfxCommand copyCmd = command;
      copyCmd.pushConstants.constants = (
        reinterpret_cast<PuleGfxConstant *>(
          commandList.constantData.data() + prevConstantDataLength
        )
      );
      commandList.actions.emplace_back(copyCmd);
    } break;
  }
}

void puleGfxCommandListSubmit(
  PuleGfxCommandListSubmitInfo const info,
  PuleError * const error
) {
  bool usedProgram = false;
  bool usedVertexArray = false;
  auto const commandListFind = ::commandLists.find(info.commandList.id);
  PULE_errorAssert(
    commandListFind != ::commandLists.end(),
    PuleErrorGfx_invalidCommandList,
  );

  if (info.fenceTargetStart != nullptr) {
    if (
      !puleGfxFenceCheckSignal(
        *info.fenceTargetStart,
        PuleNanosecond{100'000'000}
      )
    ) {
      PULE_error(
        PuleErrorGfx_submissionFenceWaitFailed,
        "failed to wait for fence '%d' on submission",
        info.fenceTargetStart->id
      );
      puleGfxFenceDestroy(*info.fenceTargetStart);
      info.fenceTargetStart->id = 0;
      return;
    }
  }

  for (auto const command : commandListFind->second.actions) {
    PuleGfxAction const actionType = (
      *reinterpret_cast<PuleGfxAction const *>(&command)
    );
    switch (actionType) {
      default:
        puleLogError("unknown action ID %d", actionType);
      break;
      case PuleGfxAction_clearFramebufferColor: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionClearFramebufferColor const *>(
            &command
          )
        );
        glClearNamedFramebufferfv(
          static_cast<GLuint>(action.framebuffer.id),
          GL_COLOR, 0,
          &action.color.x
        );
      } break;
      case PuleGfxAction_clearFramebufferDepth: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionClearFramebufferDepth const *>(
            &command
          )
        );
        glClearNamedFramebufferfv(
          static_cast<GLuint>(action.framebuffer.id),
          GL_DEPTH, 0,
          &action.depth
        );
      } break;
      case PuleGfxAction_pushConstants: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionPushConstants const *>(
            &command
          )
        );

        for (size_t it = 0; it < action.constantsLength; ++ it) {
          PuleGfxConstant const & constant = action.constants[it];
          auto dataAsF32 = reinterpret_cast<float const *>(&constant.value);
          auto dataAsI32 = reinterpret_cast<int32_t const *>(&constant.value);
          switch (constant.typeTag) {
            case PuleGfxConstantTypeTag_f32:
              glUniform1fv(constant.bindingSlot, 1, dataAsF32);
            break;
            case PuleGfxConstantTypeTag_f32v2:
              glUniform2fv(constant.bindingSlot, 1, dataAsF32);
            break;
            case PuleGfxConstantTypeTag_f32v3:
              glUniform3fv(constant.bindingSlot, 1, dataAsF32);
            break;
            case PuleGfxConstantTypeTag_f32v4:
              glUniform4fv(constant.bindingSlot, 1, dataAsF32);
            break;
            case PuleGfxConstantTypeTag_i32:
              glUniform1iv(constant.bindingSlot, 1, dataAsI32);
            break;
            case PuleGfxConstantTypeTag_i32v2:
              glUniform2iv(constant.bindingSlot, 1, dataAsI32);
            break;
            case PuleGfxConstantTypeTag_i32v3:
              glUniform3iv(constant.bindingSlot, 1, dataAsI32);
            break;
            case PuleGfxConstantTypeTag_i32v4:
              glUniform4iv(constant.bindingSlot, 1, dataAsI32);
            break;
            case PuleGfxConstantTypeTag_f32m44:
              glUniformMatrix4fv(constant.bindingSlot, 1, GL_FALSE, dataAsF32);
            break;
          }
        }
      } break;
      case PuleGfxAction_bindAttribute: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionBindAttribute const *>(&command)
        );
        util::Pipeline const & pipeline = *util::pipeline(action.pipeline.id);
        GLuint const bufferId = static_cast<GLuint>(action.buffer.id);
        GLintptr const offset = static_cast<GLintptr>(action.offset);
        GLsizei const stride = static_cast<GLsizei>(action.stride);

        util::verifyIsBuffer(bufferId);
        glVertexArrayVertexBuffer(
          static_cast<GLuint>(pipeline.attributeDescriptorHandle),
          action.bindingIndex,
          bufferId, offset, stride
        );
      } break;
      case PuleGfxAction_bindPipeline: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionBindPipeline const *>(&command)
        );
        util::Pipeline const & pipeline = *util::pipeline(action.pipeline.id);
        glBindFramebuffer(
          GL_FRAMEBUFFER,
          static_cast<GLuint>(pipeline.framebufferHandle)
        );
        glUseProgram(static_cast<GLuint>(pipeline.shaderModuleHandle));
        glBindVertexArray(
          static_cast<GLuint>(pipeline.attributeDescriptorHandle)
        );
        for (size_t it = 0; it < pipeline.texturesLength; ++ it) {
          util::DescriptorSetImageBinding const & binding = (
            pipeline.textures[it]
          );
          glBindTextureUnit(binding.bindingSlot, binding.imageHandle);
        }
        for (size_t it = 0; it < pipeline.storagesLength; ++ it) {
          util::DescriptorSetStorageBinding const & binding = (
            pipeline.storages[it]
          );
          glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER,
            binding.bindingSlot,
            binding.storageHandle
          );
        }
        usedProgram = true;
        usedVertexArray = true;

        if (pipeline.blendEnabled) {
          glEnable(GL_BLEND);
          glBlendEquation(GL_FUNC_ADD);
          glBlendFuncSeparate(
            GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
            GL_ONE, GL_ONE_MINUS_SRC_ALPHA
          );
        } else {
          glDisable(GL_BLEND);
        }

        if (pipeline.depthTestEnabled) {
          glEnable(GL_DEPTH_TEST);
          glDepthFunc(GL_LESS);
        } else {
          glDisable(GL_DEPTH_TEST);
        }

        (pipeline.scissorTestEnabled ? glEnable : glDisable)(GL_STENCIL_TEST);

        glViewport(
          static_cast<GLsizei>(pipeline.viewportUl.x),
          static_cast<GLsizei>(pipeline.viewportUl.y),
          static_cast<GLsizei>(pipeline.viewportLr.x),
          static_cast<GLsizei>(pipeline.viewportLr.y)
        );

        glScissor(
          static_cast<int32_t>(pipeline.scissorUl.x),
          static_cast<int32_t>(pipeline.scissorUl.y),
          static_cast<int32_t>(pipeline.scissorLr.x),
          static_cast<int32_t>(pipeline.scissorLr.y)
        );
      } break;
      case PuleGfxAction_bindFramebuffer: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionBindFramebuffer const *>(&command)
        );
        glBindFramebuffer(
          GL_FRAMEBUFFER,
          static_cast<GLuint>(action.framebuffer.id)
        );
      } break;
      case PuleGfxAction_dispatchRender: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchRender const *>(&command)
        );
        glDrawArrays(
          drawPrimitiveToGl(action.drawPrimitive),
          action.vertexOffset,
          action.numVertices
        );
      } break;
      case PuleGfxAction_dispatchRenderIndirect: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchRenderIndirect const *>(
            &command
          )
        );
        glBindBuffer(
          GL_DRAW_INDIRECT_BUFFER,
          action.bufferIndirect.id
        );
        glDrawArraysIndirect(
          drawPrimitiveToGl(action.drawPrimitive),
          reinterpret_cast<void *>(action.byteOffset)
        );
      } break;
      case PuleGfxAction_dispatchRenderElements: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchRenderElements const *>(
            &command
          )
        );
        glDrawElementsBaseVertex(
          drawPrimitiveToGl(action.drawPrimitive),
          action.numElements,
          elementTypeToGl(action.elementType),
          reinterpret_cast<void const *>(action.elementOffset),
          action.baseVertexOffset
        );
      } break;
      case PuleGfxAction_dispatchCommandList: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchCommandList const *>(
            &command
          )
        );
        puleGfxCommandListSubmit(action.submitInfo, error);
        if (puleErrorExists(error)) {
          return;
        }
      } break;
    }
  }

  if (usedProgram) { glUseProgram(0); }
  if (usedVertexArray) { glBindVertexArray(0); }

  if (info.fenceTargetFinish != nullptr) {
    *info.fenceTargetFinish = puleGfxFenceCreate(PuleGfxFenceConditionFlag_all);
  }
}

} // extern C

namespace {

void commandListDump(PuleGfxCommandList const commandList, int32_t level=0) {
  auto const commandListFind = ::commandLists.find(commandList.id);
  std::string levelStr = "";
  for (int32_t l = 0; l < level+1; ++ l) levelStr += "|  ";
  char const * const levelCStr = levelStr.c_str();
  puleLogDebug(
    "%sCommand list dump of '%s'",
    levelCStr, commandListFind->second.label.c_str()
  );
  for (auto const command : commandListFind->second.actions) {
    PuleGfxAction const actionType = (
      *reinterpret_cast<PuleGfxAction const *>(&command)
    );
    switch (actionType) {
      default:
        puleLogError("%sunknown action ID %d", levelCStr, actionType);
      break;
      case PuleGfxAction_clearFramebufferColor: {
        puleLogDebug("%sPuleGfxAction_clearFramebufferColor", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionClearFramebufferColor const *>(
            &command
          )
        );
        (void)action;
      } break;
      case PuleGfxAction_clearFramebufferDepth: {
        puleLogDebug("%sPuleGfxAction_clearFramebufferDepth", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionClearFramebufferDepth const *>(
            &command
          )
        );
        (void)action;
      } break;
      case PuleGfxAction_pushConstants: {
        puleLogDebug("%sPuleGfxAction_pushConstants", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionPushConstants const *>(
            &command
          )
        );
        (void)action;
      } break;
      case PuleGfxAction_bindAttribute: {
        puleLogDebug("%sPuleGfxAction_bindAttribute", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionBindAttribute const *>(&command)
        );
        (void)action;
      } break;
      case PuleGfxAction_bindPipeline: {
        puleLogDebug("%sPuleGfxAction_bindPipeline", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionBindPipeline const *>(&command)
        );
        (void)action;
      } break;
      case PuleGfxAction_bindFramebuffer: {
        puleLogDebug("%sPuleGfxAction_bindFramebuffer", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionBindFramebuffer const *>(&command)
        );
        (void)action;
      } break;
      case PuleGfxAction_dispatchRender: {
        puleLogDebug("%sPuleGfxAction_dispatchRender", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchRender const *>(&command)
        );
        (void)action;
      } break;
      case PuleGfxAction_dispatchRenderIndirect: {
        puleLogDebug("%sPuleGfxAction_dispatchRenderIndirect", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchRenderIndirect const *>(
            &command
          )
        );
        (void)action;
      } break;
      case PuleGfxAction_dispatchRenderElements: {
        puleLogDebug("%sPuleGfxAction_dispatchRenderElements", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchRenderElements const *>(
            &command
          )
        );
        (void)action;
      } break;
      case PuleGfxAction_dispatchCommandList: {
        puleLogDebug("%sPuleGfxAction_dispatchCommandList", levelCStr);
        auto const action = (
          *reinterpret_cast<PuleGfxActionDispatchCommandList const *>(
            &command
          )
        );
        commandListDump(action.submitInfo.commandList, level+1);
      } break;
    }
  }
}

} // namespace

extern "C" {

void puleGfxCommandListDump(PuleGfxCommandList const commandList) {
  puleLogDebug("-------------------------------------------------");
  commandListDump(commandList);
  puleLogDebug("-------------------------------------------------");
}

} // extern C

//------------------------------------------------------------------------------
//-- DEBUG ---------------------------------------------------------------------
//------------------------------------------------------------------------------

void util::printCommandsDebug() {
  puleLog(
    "-------------------- command lists "
    "--------------------"
  );
  for (auto const & commandListIter : ::commandLists) {
    ::CommandList const & commandList = commandListIter.second;
    puleLog(">>> %s", commandList.label.c_str());
    puleLog("\t> actions [%zu total]", commandList.actions.size());
    for (PuleGfxCommand const & command : commandList.actions) {
      puleLog("\t\t> %s", puleGfxActionToString(command.action));
      switch (command.action) {
        default: break;
        case PuleGfxAction_bindPipeline:
          puleLog("\t\t\tpipeline: %u", command.bindPipeline.pipeline);
        break;
        case PuleGfxAction_dispatchRender:
          puleLog(
            "\t\t\tdispatch render: vertex offset %zu num vertices %zu",
            command.dispatchRender.vertexOffset,
            command.dispatchRender.numVertices
          );
        break;
      }
    }
    puleLogLn(
      "\t> constant data [%zu total]\n\t\t",
      commandList.constantData.size()
    );
    size_t constantIter = 0;
    for (uint8_t const & constant : commandList.constantData) {
      if (constantIter ++ > 40) {
        constantIter += 1;
        puleLogLn("\n");
      }
      puleLogLn("%x", constant);
    }
    puleLogLn("\n");
  }
  puleLog(
    "-----------------------------------"
    "--------------------"
  );
}
