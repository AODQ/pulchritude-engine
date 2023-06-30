#include <pulchritude-gfx/commands.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/pipeline.h>

#include <pulchritude-math/math.h>

#include <volk.h>

#include <unordered_map>
#include <vector>

#include "vulkan-memory-allocator.hpp"

#define PULE_vkAssert(X, ErrorId, RetValue) \
  if (X != VK_SUCCESS) { \
    *error = { \
      .description = ( \
        puleStringFormatDefault( \
          "assertion failed; %s", #X \
        ) \
      ), \
      .id = ErrorId, \
      .sourceLocationNullable = __FILE__, \
      .lineNumber = __LINE__, \
    }; \
    return RetValue; \
  }

namespace util {

  struct DeviceQueues {
    uint32_t idxCompute = 0;
    uint32_t idxGraphics = 0;
    uint32_t idxPresent = 0;
    uint32_t idxTransfer = 0;
    uint32_t idxGTC = 0;
  };

  struct Device {
    VkPhysicalDevice physical;
    VkDevice logical;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceVulkan11Properties propertiesVersion11;
    VkPhysicalDeviceVulkan12Properties propertiesVersion12;
    VkPhysicalDeviceVulkan13Properties propertiesVersion13;
    VkPhysicalDeviceMeshShaderPropertiesNV propertiesMeshShader;
    VkPhysicalDeviceMemoryProperties propertiesMemory;
    VkPhysicalDeviceMemoryProperties2 propertiesMemoryVersion2;
    VkPhysicalDeviceMemoryBudgetPropertiesEXT propertiesMemoryBudget;
    std::vector<VkQueueFamilyProperties> propertiesQueueFamily;
    DeviceQueues queues;
  };

  struct DescriptorSetImageBinding {
    uint32_t imageHandle;
    uint32_t bindingSlot;
  };
  struct DescriptorSetStorageBinding {
    uint32_t storageHandle;
    uint32_t bindingSlot;
  };
  struct Pipeline {
    uint64_t pipelineHandle = 0;
    uint64_t shaderModuleHandle = 0;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSet descriptorSet;
    VkDescriptorSetLayout descriptorSetLayout;
  };

  // chains pNext together, pnextList[0].pNext = pnextList[1]; and so on
  void chainPNextList(std::vector<void *> const pnextList);
}

namespace util {

struct Buffer {
  VkBuffer vkHandle;
  VmaAllocation allocation;
  VmaAllocationInfo allocationInfo;
};

struct ShaderModule {
  VkShaderModule moduleVertex;
  VkShaderModule moduleFragment;
};

struct RecorderImage {
  VkAccessFlags access;
  VkImageLayout layout;
  uint32_t      deviceQueueIdx;
};

struct CommandBufferRecorder {
  std::unordered_map<uint64_t, util::RecorderImage> images;
  PuleGfxPipeline currentBoundPipeline = { .id = 0, };
};

struct Context {
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  util::Device device;
  VkSurfaceKHR surface;
  VmaAllocator allocator;
  std::unordered_map<uint64_t, util::Buffer> buffers;
  // TODO make unordered_map<uint32_t /*DeviceQueueIdx*/, vector<VkQueue>>
  VkQueue defaultQueue;
  VkCommandPool defaultCommandPool;
  VkDescriptorPool defaultDescriptorPool;

  // store all image views, indexed by the image handle and a hash of
  // puleGfxGpuImageView
  std::unordered_map<
    uint64_t, std::unordered_map<uint64_t, VkImageView>
  > imageViews;

  VkCommandBuffer utilCommandBuffer;
  VkBuffer utilStagingBuffer;
  VmaAllocation utilStagingBufferAllocation;
  VmaAllocationInfo utilStagingBufferAllocationInfo;

  std::unordered_map<uint64_t, util::Pipeline> pipelines;
  std::unordered_map<uint64_t, util::ShaderModule> shaderModules;
  std::unordered_map<
    uint64_t, util::CommandBufferRecorder
  > commandBufferRecorders;
  VkSwapchainKHR swapchain;
  uint32_t swapchainCurrentImageIdx;
  std::vector<VkImage> swapchainImages;
};

Context & ctx();

VkDescriptorType toDescriptorType(
  PuleGfxGpuBufferBindingDescriptor const bufferBindingDescriptor
);

VkBufferUsageFlags toBufferUsageFlags(PuleGfxGpuBufferUsage const usage);

VkSwapchainKHR swapchainCreate();
uint32_t swapchainAcquireNextImage(
  VkSemaphore const semaphore,
  VkFence const fence
);
void swapchainPresent(VkSemaphore const waitSemaphore);
void swapchainImages();

VkShaderStageFlags toVkShaderStageFlags(PuleGfxDescriptorStage const stage);
VkIndexType toVkIndexType(PuleGfxElementType const elementType);
VkAccessFlags toVkAccessFlags(PuleGfxCommandPayloadAccess const access);
VkImageLayout toVkImageLayout(PuleGfxImageLayout const layout);
VkFormat toVkImageFormat(PuleGfxImageByteFormat const format);
VkFormat toVkBufferFormat(
  PuleGfxAttributeDataType const dataType,
  size_t const numComponents,
  bool const normalize
);
VkAttachmentLoadOp toVkAttachmentOpLoad(
  PuleGfxImageAttachmentOpLoad const op
);
VkAttachmentStoreOp toVkAttachmentOpStore(
  PuleGfxImageAttachmentOpStore const op
);


VkImageView fetchImageView(PuleGfxGpuImageView const imageView);

} // ::util
