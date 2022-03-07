#include <pulchritude-gfx/gfx.h>

namespace util {
  struct DescriptorSetImageBinding {
    uint32_t imageHandle;
    uint32_t bindingSlot;
  };
  struct PipelineLayout {
    DescriptorSetImageBinding textures[8];
    size_t texturesLength;
    uint32_t descriptor;
  };

  PipelineLayout const * pipelineLayout(uint64_t const id);
  uint64_t createPipelineLayout(PipelineLayout const layout);
  void destroyPipelineLayout(uint64_t const id);
}
