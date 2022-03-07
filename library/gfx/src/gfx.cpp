#include <pulchritude-gfx/gfx.h>

#include <util.hpp>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-window/window.h>

#include <cstring>
#include <functional>
#include <string>
#include <unordered_set>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glad/glad.h>
#pragma GCC diagnostic pop

namespace {
  std::unordered_set<std::string> loggedErrorMessages;
}

extern "C" {

static void GLAPIENTRY errorMessageCallback(
  [[maybe_unused]] GLenum source,
  GLenum type,
  [[maybe_unused]] GLenum id,
  GLenum severity,
  [[maybe_unused]] GLsizei length,
  GLchar const * message,
  [[maybe_unused]] void const * userdata
) {
  auto const messageStr = std::string(message);
  if (loggedErrorMessages.find(messageStr) == loggedErrorMessages.end()) {
    return;
  }
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      puleLogError("OpenGL error [0x%x]: %s", type, message);
    break;
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_NOTIFICATION:
    default:
      puleLogDebug("OpenGL message [0x%x]: %s", type, message);
    break;
  }
  loggedErrorMessages.emplace(messageStr);
}

void puleGfxInitialize(PuleError * const error) {
  PULE_errorAssert(
    gladLoadGLLoader(
      reinterpret_cast<GLADloadproc>(puleWindowGetProcessAddress())
    ),
    PuleErrorGfx_creationFailed,
  );

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(errorMessageCallback, nullptr);
}

void puleGfxShutdown() {
}

//------------------------------------------------------------------------------
//-- GPU BUFFER ----------------------------------------------------------------
//------------------------------------------------------------------------------

namespace {
  GLenum bufferUsageToGl(PuleGfxGpuBufferUsage const usage) {
    switch (usage) {
      default:
        puleLogError("usage is invalid with renderer: %d", usage);
        return 0;
      break;
      case PuleGfxGpuBufferUsage_bufferAttribute:
        puleLogDebug("GL_ARRAY_BUFFER");
        return GL_ARRAY_BUFFER;
      break;
      case PuleGfxGpuBufferUsage_bufferElement:
        return GL_ELEMENT_ARRAY_BUFFER;
      break;
      case PuleGfxGpuBufferUsage_bufferUniform:
        return GL_UNIFORM_BUFFER;
      break;
    }
  }

  GLbitfield bufferVisibilityToGl(PuleGfxGpuBufferVisibilityFlag const usage) {
    if (usage & PuleGfxGpuBufferVisibilityFlag_deviceOnly) {
      if (usage != PuleGfxGpuBufferVisibilityFlag_deviceOnly) {
        puleLogError("incompatible buffer visibility: %d", usage);
      }
      return 0;
    }
    GLbitfield field = 0;
    if (usage & PuleGfxGpuBufferVisibilityFlag_hostVisible) {
      field |= GL_MAP_READ_BIT;
    }
    if (usage & PuleGfxGpuBufferVisibilityFlag_hostWritable) {
      field |= GL_MAP_WRITE_BIT;
    }
    return field;
  }
}

PuleGfxGpuBuffer puleGfxGpuBufferCreate(
  void * const nullableInitialData,
  size_t const byteLength,
  [[maybe_unused]] PuleGfxGpuBufferUsage const usage,
  PuleGfxGpuBufferVisibilityFlag const visibility
) {
  GLuint buffer;
  glCreateBuffers(1, &buffer);
  puleLogDebug("buffer: %u", buffer);
  glNamedBufferStorage(
    buffer, byteLength, nullableInitialData, bufferVisibilityToGl(visibility)
  );

  return { static_cast<uint64_t>(buffer) };
}

void puleGfxGpuBufferDestroy(PuleGfxGpuBuffer const buffer) {
  GLuint const bufferHandle = static_cast<GLuint>(buffer.id);
  glDeleteBuffers(1, &bufferHandle);
}

} // extern C
