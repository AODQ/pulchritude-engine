#include <pulchritude-gfx/pipeline.h>

#include <util.hpp>

#include <pulchritude-log/log.h>

#include <glad/glad.h>

#include <cstring>

namespace {
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

extern "C" {

PuleGfxPipelineDescriptorSetLayout puleGfxPipelineDescriptorSetLayout() {
  PuleGfxPipelineDescriptorSetLayout descriptorSetLayout;
  memset(
    &descriptorSetLayout.bufferUniformBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.bufferUniformBindings[0])) * 16
  );
  memset(
    &descriptorSetLayout.bufferStorageBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.bufferStorageBindings[0])) * 16
  );
  memset(
    &descriptorSetLayout.bufferAttributeBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.bufferAttributeBindings[0])) * 16
  );
  memset(
    &descriptorSetLayout.textureBindings[0],
    0,
    sizeof(decltype(descriptorSetLayout.textureBindings[0])) * 8
  );
  memset(
    &descriptorSetLayout.bufferElementBinding,
    0,
    sizeof(decltype(descriptorSetLayout.bufferElementBinding))
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
    PuleGfxPipelineAttributeDescriptorBinding const &
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
      ::attributeDataTypeToGl(descriptorAttributeBinding.dataType, error)
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

  // TODO right now I just return the attribute descriptor, but I need to also
  // store texture information, uniform bindings, etc

  // store into the descriptorsetlayout
  util::PipelineLayout pipelineLayout;
  pipelineLayout.descriptor = attributeDescriptorHandle;

  // collapse textures into a single array, such that if the pipeline layout
  // were [0, 0, 5, 0, 6], we want [{5, 2}, {6, 4}]
  pipelineLayout.texturesLength = 0;
  for (size_t it = 0; it < 8; ++ it) {
    PuleGfxGpuImage const texture = descriptorSetLayout->textureBindings[it];
    if (texture.id == 0) {
      continue;
    }
    pipelineLayout.textures[pipelineLayout.texturesLength] = (
      util::DescriptorSetImageBinding {
        .imageHandle = static_cast<uint32_t>(texture.id),
        .bindingSlot = static_cast<uint32_t>(it),
      }
    );
    ++ pipelineLayout.texturesLength;
  }

  // collapse storage buffers into array
  pipelineLayout.storagesLength = 0;
  for (size_t it = 0; it < 16; ++ it) {
    PuleGfxGpuBuffer const buffer = (
      descriptorSetLayout->bufferStorageBindings[it]
    );
    if (buffer.id == 0) {
      continue;
    }
    pipelineLayout.storages[pipelineLayout.storagesLength] = (
      util::DescriptorSetStorageBinding {
        .storageHandle = static_cast<uint32_t>(buffer.id),
        .bindingSlot = static_cast<uint32_t>(it),
      }
    );
    ++ pipelineLayout.storagesLength;
  }

  return { util::createPipelineLayout(pipelineLayout) };
}

void puleGfxPipelineLayoutDestroy(PuleGfxPipelineLayout const pipelineLayout) {
  util::destroyPipelineLayout(pipelineLayout.id);
}

} // extern C
