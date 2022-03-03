#include <pulchritude-gfx/gfx.h>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-window/window.h>
#include <util.hpp>

#include <cstring>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <glad/glad.h>
#pragma GCC diagnostic pop

extern "C" {

void puleGfxInitialize(PuleError * const error) {
  PULE_errorAssert(
    gladLoadGLLoader(
      reinterpret_cast<GLADloadproc>(puleWindowGetProcessAddress())
    ),
    PuleErrorGfx_creationFailed,
  );
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

  GLenum bufferVisibilityToGl(PuleGfxGpuBufferVisibility const usage) {
    switch (usage) {
      default:
        puleLogError("visibility is invalid with renderer: %d", usage);
        return 0;
      break;
      case PuleGfxGpuBufferVisibility_hostVisible:
      case PuleGfxGpuBufferVisibility_deviceOnly:
        return GL_STATIC_DRAW;
      break;
      case PuleGfxGpuBufferVisibility_hostWritable:
        return GL_DYNAMIC_DRAW;
      break;
    }
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
  PuleGfxGpuBufferUsage const usage,
  PuleGfxGpuBufferVisibility const visibility
) {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  puleLogDebug("buffer: %u", buffer);
  GLenum const bufferUsage = bufferUsageToGl(usage);
  glBindBuffer(bufferUsage, buffer);
  UTIL_processGlErrors();
  glBufferData(
    bufferUsage,
    byteLength,
    nullableInitialData,
    bufferVisibilityToGl(visibility)
  );
  UTIL_processGlErrors();

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
  glGenVertexArrays(1, &attributeDescriptorHandle);

  glBindVertexArray(attributeDescriptorHandle);

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
    glBindBuffer(GL_ARRAY_BUFFER, descriptorAttributeBinding.buffer.id);
    UTIL_processGlErrors();
    glEnableVertexAttribArray(it);
    UTIL_processGlErrors();
    glVertexAttribPointer(
      it,
      descriptorAttributeBinding.numComponents,
      attributeDataType,
      descriptorAttributeBinding.normalizeFixedDataTypeToNormalizedFloating,
      descriptorAttributeBinding.stridePerElement,
      reinterpret_cast<void *>(
        descriptorAttributeBinding.offsetIntoBuffer
      )
    );
    UTIL_processGlErrors();
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  UTIL_processGlErrors();

  // TODO element binding

  glBindVertexArray(0);

  for (size_t it = 0; it < 16; ++ it) {
    glDisableVertexAttribArray(it);
  }
  return { attributeDescriptorHandle };
}

void puleGfxPipelineLayoutDestroy(PuleGfxPipelineLayout const pipelineLayout) {
  GLuint handle = static_cast<GLuint>(pipelineLayout.id);
  glDeleteVertexArrays(1, &handle);
}


} // extern C
