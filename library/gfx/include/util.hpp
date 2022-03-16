#include <pulchritude-gfx/gfx.h>

#include <pulchritude-math/math.h>

namespace util {
  struct DescriptorSetImageBinding {
    uint32_t imageHandle;
    uint32_t bindingSlot;
  };
  struct DescriptorSetStorageBinding {
    uint32_t storageHandle;
    uint32_t bindingSlot;
  };
  struct Pipeline {
    DescriptorSetImageBinding textures[8];
    size_t texturesLength = 0;

    DescriptorSetStorageBinding storages[16];
    size_t storagesLength = 0;

    uint32_t attributeDescriptorHandle = 0;

    uint64_t pipelineHandle = 0;
    uint64_t framebufferHandle = 0;
    uint64_t shaderModuleHandle = 0;

    bool blendEnabled = false;
    bool scissorTestEnabled = false;
    PuleI32v2 viewportUl = PuleI32v2 { 0, 0 }; // default 0, 0
    PuleI32v2 viewportLr = PuleI32v2 { 1, 1 }; // default 1, 1
    PuleI32v2 scissorUl = PuleI32v2 { 0, 0 }; // default 0, 0
    PuleI32v2 scissorLr = PuleI32v2 { 1, 1 }; // default 1, 1
  };

  Pipeline * pipeline(uint64_t const id);
  uint64_t createPipeline();
  void destroyPipeline(uint64_t const id);
}
