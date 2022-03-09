#include <pulchritude-gfx/commands.h>

#include <util.hpp>

#include <glad/glad.h>

#include <cstring>
#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

namespace {
  struct CommandList {
    PuleAllocator allocator;
    // TODO use allocator instead of vector lol
    std::vector<PuleGfxCommand> actions;
    std::vector<uint8_t> constantData;
  };
  std::unordered_map<uint64_t, CommandList> commandLists;
  uint64_t commandListsIt = 0;

  GLenum drawPrimitiveToGl(PuleGfxDrawPrimitive drawPrimitive) {
    switch (drawPrimitive) {
      default:
        puleLogError("unknown draw primitive: %d", drawPrimitive);
        return 0;
      break;
      case PuleGfxDrawPrimitive_triangle:
        return GL_TRIANGLES;
      case PuleGfxDrawPrimitive_triangleStrip:
        return GL_TRIANGLE_STRIP;
      case PuleGfxDrawPrimitive_line:
        return GL_LINES;
      case PuleGfxDrawPrimitive_point:
        return GL_POINTS;
    }
  }
}

extern "C" {

PuleGfxCommandList puleGfxCommandListCreate(PuleAllocator const allocator) {
  PuleGfxCommandList commandList = {
    .id = commandListsIt,
  };
  ::commandLists.emplace(
    commandList.id,
    ::CommandList{
      .allocator = allocator,
      .actions = {},
      .constantData = {},
    }
  );
  commandListsIt += 1;
  return commandList;
}

void puleGfxCommandListDestroy(PuleGfxCommandList const commandList) {
  ::commandLists.erase(commandList.id);
}

PuleGfxCommandListRecorder puleGfxCommandListRecorder(
  PuleGfxCommandList const commandList
) {
  return { commandList.id };
}

void puleGfxCommandListRecorderFinish(
  [[maybe_unused]] PuleGfxCommandListRecorder const commandListRecorder
) {
}

void puleGfxCommandListAppendAction(
  PuleGfxCommandListRecorder const commandListRecorder,
  PuleGfxCommand const command
) {
  auto & commandList = ::commandLists.at(commandListRecorder.id);
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
  PuleGfxCommandList const commandList,
  PuleError * const error
) {
  bool usedProgram = false;
  bool usedVertexArray = false;
  auto const commandListFind = ::commandLists.find(commandList.id);
  PULE_errorAssert(
    commandListFind != ::commandLists.end(),
    PuleErrorGfx_invalidCommandList,
  );
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
      case PuleGfxAction_bindPipeline: {
        auto const action = (
          *reinterpret_cast<PuleGfxActionBindPipeline const *>(&command)
        );
        util::PipelineLayout const & pipelineLayout = (
          *util::pipelineLayout(action.pipeline.layout.id)
        );
        puleLogDebug("binding framebuffer: %d", 
          static_cast<GLuint>(action.pipeline.framebuffer.id)
        );
        glBindFramebuffer(
          GL_FRAMEBUFFER,
          static_cast<GLuint>(action.pipeline.framebuffer.id)
        );
        glUseProgram(static_cast<GLuint>(action.pipeline.shaderModule.id));
        glBindVertexArray(static_cast<GLuint>(pipelineLayout.descriptor));
        for (size_t it = 0; it < pipelineLayout.texturesLength; ++ it) {
          util::DescriptorSetImageBinding const & binding = (
            pipelineLayout.textures[it]
          );
          glBindTextureUnit(binding.bindingSlot, binding.imageHandle);
        }
        for (size_t it = 0; it < pipelineLayout.storagesLength; ++ it) {
          util::DescriptorSetStorageBinding const & binding = (
            pipelineLayout.storages[it]
          );
          glBindBufferBase(
            GL_SHADER_STORAGE_BUFFER,
            binding.bindingSlot,
            binding.storageHandle
          );
        }
        usedProgram = true;
        usedVertexArray = true;
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
    }
  }

  if (usedProgram) { glUseProgram(0); }
  if (usedVertexArray) { glBindVertexArray(0); }
}

}
