#include <pulchritude-gfx/pipeline.h>

#include <util.hpp>

#include <pulchritude-log/log.h>

#include <glad/glad.h>

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
      case PuleGfxAttributeDataType_unsignedByte:
        return GL_UNSIGNED_BYTE;
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

void puleGfxPipelineUpdate(
  PuleGfxPipeline const pipeline,
  PuleGfxPipelineCreateInfo const * const info,
  PuleError * const error
) {
  util::Pipeline & utilPipeline = *util::pipeline(pipeline.id);

  // buffer attribute bindings
  for (size_t it = 0; it < 16; ++ it) {
    PuleGfxPipelineAttributeDescriptorBinding const &
      descriptorAttributeBinding = (
        info->layout->bufferAttributeBindings[it]
      )
    ;
    bool const usesBuffer = descriptorAttributeBinding.buffer.id > 0;
    // if there are no components, this binding is disabled
    if (descriptorAttributeBinding.numComponents == 0) {
      glDisableVertexArrayAttrib(utilPipeline.attributeDescriptorHandle, it);
      continue;
    }
    PULE_errorAssert(
      descriptorAttributeBinding.numComponents != 0,
      PuleErrorGfx_invalidDescriptorSet,
    );
    GLenum const attributeDataType = (
      ::attributeDataTypeToGl(descriptorAttributeBinding.dataType, error)
    );
    if (error->id > 0) { return; }
    PULE_errorAssert(
      attributeDataType != 0,
      PuleErrorGfx_invalidDescriptorSet,
    );

    if (usesBuffer) {
      glVertexArrayVertexBuffer(
        utilPipeline.attributeDescriptorHandle,
        it,
        descriptorAttributeBinding.buffer.id,
        descriptorAttributeBinding.offsetIntoBuffer,
        descriptorAttributeBinding.stridePerElement
      );
    }
    glEnableVertexArrayAttrib(utilPipeline.attributeDescriptorHandle, it);
    glVertexArrayAttribFormat(
      utilPipeline.attributeDescriptorHandle,
      it,
      descriptorAttributeBinding.numComponents,
      attributeDataType,
      descriptorAttributeBinding.convertFixedDataTypeToNormalizedFloating,
      0
      //descriptorAttributeBinding.stridePerElement
    );
    glVertexArrayAttribBinding(utilPipeline.attributeDescriptorHandle, it, it);
  }

  // element binding
  if (info->layout->bufferElementBinding.id != 0) {
    glVertexArrayElementBuffer(
      utilPipeline.attributeDescriptorHandle,
      info->layout->bufferElementBinding.id
    );
  }

  // collapse textures into a single array, such that if the pipeline layout
  // were [0, 0, 5, 0, 6], we want [{5, 2}, {6, 4}]
  utilPipeline.texturesLength = 0;
  for (size_t it = 0; it < 8; ++ it) {
    PuleGfxGpuImage const texture = info->layout->textureBindings[it];
    if (texture.id == 0) {
      continue;
    }
    utilPipeline.textures[utilPipeline.texturesLength] = (
      util::DescriptorSetImageBinding {
        .imageHandle = static_cast<uint32_t>(texture.id),
        .bindingSlot = static_cast<uint32_t>(it),
      }
    );
    ++ utilPipeline.texturesLength;
  }

  // collapse storage buffers into array
  utilPipeline.storagesLength = 0;
  for (size_t it = 0; it < 16; ++ it) {
    PuleGfxGpuBuffer const buffer = info->layout->bufferStorageBindings[it];
    if (buffer.id == 0) {
      continue;
    }
    utilPipeline.storages[utilPipeline.storagesLength] = (
      util::DescriptorSetStorageBinding {
        .storageHandle = static_cast<uint32_t>(buffer.id),
        .bindingSlot = static_cast<uint32_t>(it),
      }
    );
    ++ utilPipeline.storagesLength;
  }

  utilPipeline.pipelineHandle     = pipeline.id;
  utilPipeline.shaderModuleHandle = info->shaderModule.id;
  utilPipeline.blendEnabled       = info->config.blendEnabled;
  utilPipeline.depthTestEnabled   = info->config.depthTestEnabled;
  utilPipeline.scissorTestEnabled = info->config.scissorTestEnabled;
  utilPipeline.viewportUl         = info->config.viewportUl;
  utilPipeline.viewportLr         = info->config.viewportLr;
  utilPipeline.scissorUl          = info->config.scissorUl;
  utilPipeline.scissorLr          = info->config.scissorLr;
}

PuleGfxPipeline puleGfxPipelineCreate(
  PuleGfxPipelineCreateInfo const * const info,
  PuleError * const error
) {
  GLuint attributeDescriptorHandle;
  glCreateVertexArrays(1, &attributeDescriptorHandle);

  // create the pipeline handle, shove the attribute descriptor into the 
  //   pipeline and send it up to be 'updated', which will store the rest
  //   of the pipeline information

  uint64_t handle = util::createPipeline();
  util::pipeline(handle)->attributeDescriptorHandle = attributeDescriptorHandle;
  puleGfxPipelineUpdate({handle}, info, error);
  if (error->id > 0) {
    return { static_cast<uint64_t>(-1) };
  }

  return { handle };
}

void puleGfxPipelineDestroy(PuleGfxPipeline const pipeline) {
  if (pipeline.id == 0) { return; }
  util::destroyPipeline(pipeline.id);
}

} // extern C
