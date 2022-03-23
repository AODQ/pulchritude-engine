#include <pulchritude-gfx/gfx.h>

#include <util.hpp>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-platform/platform.h>

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
  if (loggedErrorMessages.find(messageStr) != loggedErrorMessages.end()) {
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
      reinterpret_cast<GLADloadproc>(pulePlatformGetProcessAddress())
    ),
    PuleErrorGfx_creationFailed,
  );

  glEnable(GL_SCISSOR_TEST);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(errorMessageCallback, nullptr);

  glDisable(GL_PRIMITIVE_RESTART);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_MULTISAMPLE);
  glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
  glDisable(GL_SAMPLE_ALPHA_TO_ONE);
  glDisable(GL_SAMPLE_COVERAGE);
  glDisable(GL_STENCIL_TEST);
}

void puleGfxShutdown() {
}

}

//------------------------------------------------------------------------------
//-- GPU BUFFER ----------------------------------------------------------------
//------------------------------------------------------------------------------

namespace {
  [[maybe_unused]]
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
      field |= GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT;
    }
    if (usage & PuleGfxGpuBufferVisibilityFlag_hostWritable) {
      field |= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
    }
    return field;
  }

  GLbitfield bufferMapAccessToGl(PuleGfxGpuBufferMapAccess const access) {
    GLbitfield field = 0;
    if (access & PuleGfxGpuBufferMapAccess_hostVisible) {
      field |= (
        GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
      );
    }
    if (access & PuleGfxGpuBufferMapAccess_hostWritable) {
      field |= (
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
      );
    }
    if (access & PuleGfxGpuBufferMapAccess_invalidate) {
      field |= GL_MAP_INVALIDATE_RANGE_BIT;
    }
    return field;
  }
}

extern "C" {

PuleGfxGpuBuffer puleGfxGpuBufferCreate(
  void const * const optionalInitialData,
  size_t const byteLength,
  [[maybe_unused]] PuleGfxGpuBufferUsage const usage,
  PuleGfxGpuBufferVisibilityFlag const visibility
) {
  GLuint buffer;
  glCreateBuffers(1, &buffer);
  puleLogDebug("buffer: %u", buffer);
  glNamedBufferStorage(
    buffer, byteLength, optionalInitialData, bufferVisibilityToGl(visibility)
  );

  return { static_cast<uint64_t>(buffer) };
}

void puleGfxGpuBufferDestroy(PuleGfxGpuBuffer const buffer) {
  if (buffer.id == 0) { return; }
  GLuint const bufferHandle = static_cast<GLuint>(buffer.id);
  if (bufferHandle != 0) {
    glDeleteBuffers(1, &bufferHandle);
  }
}

void * puleGfxGpuBufferMap(PuleGfxGpuBufferMapRange const range) {
  return (
    glMapNamedBufferRange(
      static_cast<GLuint>(range.buffer.id),
      range.byteOffset,
      range.byteLength,
      bufferMapAccessToGl(range.access)
    )
  );
}

void puleGfxGpuBufferUnmap(PuleGfxGpuBuffer const buffer) {
  if (buffer.id == 0) {
    return;
  }
  glUnmapNamedBuffer(static_cast<GLuint>(buffer.id));
}

void puleGfxFrameStart() {
  glViewport(
    static_cast<GLsizei>(0),
    static_cast<GLsizei>(0),
    static_cast<GLsizei>(1),
    static_cast<GLsizei>(1)
  );

  glScissor(
    static_cast<int32_t>(0),
    static_cast<int32_t>(0),
    static_cast<int32_t>(1),
    static_cast<int32_t>(1)
  );
}

void puleGfxGpuBufferMappedFlush(
  PuleGfxGpuBufferMappedFlushRange const range
) {
  glFlushMappedNamedBufferRange(
    static_cast<GLuint>(range.buffer.id),
    range.byteOffset,
    range.byteLength
  );
}

} // extern C
