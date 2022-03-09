#include <pulchritude-gfx/gfx.h>

namespace util {
  struct DescriptorSetImageBinding {
    uint32_t imageHandle;
    uint32_t bindingSlot;
  };
  struct DescriptorSetStorageBinding {
    uint32_t storageHandle;
    uint32_t bindingSlot;
  };
  struct PipelineLayout {
    DescriptorSetImageBinding textures[8];
    size_t texturesLength;

    DescriptorSetStorageBinding storages[16];
    size_t storagesLength;

    uint32_t descriptor;
  };

  PipelineLayout const * pipelineLayout(uint64_t const id);
  uint64_t createPipelineLayout(PipelineLayout const layout);
  void destroyPipelineLayout(uint64_t const id);
}
