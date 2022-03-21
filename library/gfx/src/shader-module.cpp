#include <pulchritude-gfx/shader-module.h>

#include <pulchritude-gfx/gfx.h>
#include <pulchritude-log/log.h>

#include <glad/glad.h>

#include <cstdlib>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glad/glad.h>
#pragma GCC diagnostic pop

namespace {
  struct ShaderModule {
    GLuint handle;
  };

  bool compileShader(
    GLuint const handle,
    char const * const label,
    char const * const source
  ) {
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);
    GLint compilationSuccess = false;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compilationSuccess);

    int32_t logLength;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
      char * message = reinterpret_cast<char *>(malloc(logLength+1));
      glGetShaderInfoLog(handle, logLength, nullptr, message);
      puleLogError("GL %s shader compilation failed: %s", label, message);
      free(message);
    }

    return compilationSuccess;
  }
}

PuleGfxShaderModule puleGfxShaderModuleCreate(
  char const * const vertexShaderSource,
  char const * const fragmentShaderSource,
  PuleError * const error
) {
  GLuint const vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
  GLuint const fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);

  if (!::compileShader(vertexShaderHandle, "vertex", vertexShaderSource)) {
    PULE_error(
      PuleErrorGfx_shaderModuleCompilationFailed,
      "failed to compile vertex shader",
    );
    return {0};
  }
  // defer glDeleteShader(vertexShaderHandle)

  if (!::compileShader(fragmentShaderHandle, "fragment", fragmentShaderSource)){
    PULE_error(
      PuleErrorGfx_shaderModuleCompilationFailed,
      "failed to compile fragment shader",
    );
    glDeleteShader(vertexShaderHandle);
    return {0};
  }
  // defer glDeleteShader(fragmentShaderHandle)

  GLuint const shaderModuleHandle = glCreateProgram();
  glAttachShader(shaderModuleHandle, vertexShaderHandle);
  glAttachShader(shaderModuleHandle, fragmentShaderHandle);
  glLinkProgram(shaderModuleHandle);
  // defer glDetachShader(shaderModuleHandle, vertexShaderHandle)
  // defer glDetachShader(shaderModuleHandle, fragmentShaderHandle)

  GLint shaderLinkSuccess = false;
  glGetProgramiv(shaderModuleHandle, GL_LINK_STATUS, &shaderLinkSuccess);

  int32_t logLength = 0;
  glGetProgramiv(shaderModuleHandle, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength > 0) {
    char * message = reinterpret_cast<char *>(malloc(logLength+1));
    glGetProgramInfoLog(shaderModuleHandle, logLength, nullptr, message);
    puleLogError("GL shader module linking failed: %s", message);
    free(message);
  }

  if (!shaderLinkSuccess) {
    PULE_error(
      PuleErrorGfx_shaderModuleCompilationFailed,
      "failed to link shader module",
    );
  }

  glDetachShader(shaderModuleHandle, vertexShaderHandle);
  glDetachShader(shaderModuleHandle, fragmentShaderHandle);

  return { static_cast<uint64_t>(shaderModuleHandle) };
}

void puleGfxShaderModuleDestroy(PuleGfxShaderModule const shaderModule) {
  if (shaderModule.id == 0) { return; }
  auto const handle = static_cast<GLuint>(shaderModule.id);
  if (handle != 0) {
    glDeleteProgram(static_cast<GLuint>(shaderModule.id));
  }
}
