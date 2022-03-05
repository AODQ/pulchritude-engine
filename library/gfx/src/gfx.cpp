#include <pulchritude-gfx/gfx.h>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-window/window.h>

#include <cstring>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glad/glad.h>
#pragma GCC diagnostic pop

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

  GLenum attributeDataTypeToGl(
    PuleGfxAttributeDataType const dataType,
    PuleError * const error
  ) {
    switch (dataType) {
      default:
        PULE_error(
          PuleErrorGfx_invalidDescriptorSet,
          "invalid datatype %d", dataType
        );
        return 0;
      break;
      case PuleGfxAttributeDataType_float:
        return GL_FLOAT;
    }
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

//------------------------------------------------------------------------------
//-- DESCRIPTOR LAYOUT ---------------------------------------------------------
//------------------------------------------------------------------------------

PuleGfxPipelineDescriptorSetLayout puleGfxPipelineDescriptorSetLayout() {
  PuleGfxPipelineDescriptorSetLayout descriptorSetLayout;
  memset(
    descriptorSetLayout.bufferUniformBindings,
    0,
    sizeof(decltype(descriptorSetLayout.bufferUniformBindings[0])) * 16
  );
  memset(
    descriptorSetLayout.bufferAttributeBindings,
    0,
    sizeof(decltype(descriptorSetLayout.bufferAttributeBindings[0])) * 16
  );
  return descriptorSetLayout;
}

PuleGfxPipelineLayout puleGfxPipelineLayoutCreate(
  PuleGfxPipelineDescriptorSetLayout const * const descriptorSetLayout,
  PuleError * const error
) {
  GLuint attributeDescriptorHandle;
  glCreateVertexArrays(1, &attributeDescriptorHandle);

  // buffer attribute bindings
  for (size_t it = 0; it < 16; ++ it) {
    PuleGfxPipelineDescriptorAttributeBinding const &
      descriptorAttributeBinding = (
        descriptorSetLayout->bufferAttributeBindings[it]
      )
    ;
    if (descriptorAttributeBinding.buffer.id == 0) {
      continue;
    }
    PULE_errorAssert(
      descriptorAttributeBinding.numComponents != 0,
      PuleErrorGfx_invalidDescriptorSet,
      {}
    );
    GLenum const attributeDataType = (
      attributeDataTypeToGl(descriptorAttributeBinding.dataType, error)
    );
    if (error->id > 0) { return {}; }
    PULE_errorAssert(
      attributeDataType != 0,
      PuleErrorGfx_invalidDescriptorSet,
      {}
    );

    glVertexArrayVertexBuffer(
      attributeDescriptorHandle,
      it,
      descriptorAttributeBinding.buffer.id,
      descriptorAttributeBinding.offsetIntoBuffer,
      descriptorAttributeBinding.stridePerElement
    );
    glEnableVertexArrayAttrib(attributeDescriptorHandle, it);
    glVertexArrayAttribFormat(
      attributeDescriptorHandle,
      it,
      descriptorAttributeBinding.numComponents,
      attributeDataType,
      descriptorAttributeBinding.normalizeFixedDataTypeToNormalizedFloating,
      0
      //descriptorAttributeBinding.stridePerElement
    );
    glVertexArrayAttribBinding(attributeDescriptorHandle, it, it);
  }

  // TODO element binding

  return { attributeDescriptorHandle };
}

void puleGfxPipelineLayoutDestroy(PuleGfxPipelineLayout const pipelineLayout) {
  GLuint handle = static_cast<GLuint>(pipelineLayout.id);
  glDeleteVertexArrays(1, &handle);
}


} // extern C
