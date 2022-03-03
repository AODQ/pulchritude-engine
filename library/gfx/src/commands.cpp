#include <pulchritude-gfx/commands.h>

#include <util.hpp>

#include <glad/glad.h>

#include <vector>
#include <unordered_map>

//------------------------------------------------------------------------------
//-- COMMAND LIST --------------------------------------------------------------
//------------------------------------------------------------------------------

namespace {
  struct CommandList {
    std::vector<PuleGfxCommand> actions;
  };
  std::unordered_map<size_t, CommandList> commandLists;
  size_t commandListsIt = 0;

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
  PuleGfxCommandList puleGfxCommandListCreate() {
    PuleGfxCommandList commandList = {
      .id = commandListsIt,
    };
    ::commandLists.emplace(commandList.id, ::CommandList{});
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
    ::commandLists.at(commandListRecorder.id).actions.emplace_back(command);
  }

  void puleGfxCommandListSubmit(PuleGfxCommandList const commandList) {
    bool usedProgram = false;
    bool usedVertexArray = false;
    auto const commandListFind = ::commandLists.find(commandList.id);
    if (commandListFind == ::commandLists.end()) {
      puleLogError("invalid command list");
      return;
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
          glClearColor(action.red, action.green, action.blue, action.alpha);
          glClear(GL_COLOR_BUFFER_BIT);
          UTIL_processGlErrors();
        } break;
        case PuleGfxAction_bindPipeline: {
          auto const action = (
            *reinterpret_cast<PuleGfxActionBindPipeline const *>(&command)
          );
          glUseProgram(static_cast<GLuint>(action.pipeline.shaderModule.id));
          UTIL_processGlErrors();
          glBindVertexArray(static_cast<GLuint>(action.pipeline.layout.id));
          UTIL_processGlErrors();
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
          UTIL_processGlErrors();
        } break;
      }
    }

    if (usedProgram) { glUseProgram(0); }
    if (usedVertexArray) { glBindVertexArray(0); }
    UTIL_processGlErrors();
  }
}
