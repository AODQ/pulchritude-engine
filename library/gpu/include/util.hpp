#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/pipeline.h>

#include <pulchritude-math/math.h>

#include <volk.h>

#include <string>
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
  struct CommandList {
    std::string label;
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
  PuleGpuPipeline currentBoundPipeline = { .id = 0, };
  PuleGpuCommandList commandList = { .id = 0, };
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
  // puleGpuImageView
  std::unordered_map<
    uint64_t, std::unordered_map<uint64_t, VkImageView>
  > imageViews;

  VkCommandBuffer utilCommandBuffer;
  VkBuffer utilStagingBuffer;
  VmaAllocation utilStagingBufferAllocation;
  VmaAllocationInfo utilStagingBufferAllocationInfo;

  std::unordered_map<uint64_t, util::CommandList> commandLists;
  std::unordered_map<uint64_t, util::Pipeline> pipelines;
  std::unordered_map<uint64_t, util::ShaderModule> shaderModules;
  std::unordered_map<
    uint64_t, util::CommandBufferRecorder
  > commandBufferRecorders;
  VkSwapchainKHR swapchain;
  uint32_t swapchainCurrentImageIdx;
  std::vector<VkImage> swapchainImages;
  std::vector<VkSemaphore> swapchainImageAvailableSemaphores;
  size_t frameIdx = 1;
};

Context & ctx();

VkDescriptorType toDescriptorType(
  PuleGpuBufferBindingDescriptor const bufferBindingDescriptor
);

VkBufferUsageFlags toBufferUsageFlags(PuleGpuBufferUsage const usage);

VkSwapchainKHR swapchainCreate();
uint32_t swapchainAcquireNextImage(VkFence const fence);
void swapchainImagesCreate();

VkPipelineStageFlagBits toVkPipelineStageFlagBits(
  PuleGpuPipelineStage const stage
);
VkShaderStageFlags toVkShaderStageFlags(PuleGpuDescriptorStage const stage);
VkIndexType toVkIndexType(PuleGpuElementType const elementType);
VkAccessFlags toVkAccessFlags(PuleGpuResourceAccess const access);
VkImageLayout toVkImageLayout(PuleGpuImageLayout const layout);
VkPipelineStageFlags toVkPipelineStageFlags(
  PuleGpuResourceBarrierStage const access
);
VkFormat toVkImageFormat(PuleGpuImageByteFormat const format);
VkFormat toVkBufferFormat(
  PuleGpuAttributeDataType const dataType,
  size_t const numComponents,
  bool const normalize
);
VkAttachmentLoadOp toVkAttachmentOpLoad(
  PuleGpuImageAttachmentOpLoad const op
);
VkAttachmentStoreOp toVkAttachmentOpStore(
  PuleGpuImageAttachmentOpStore const op
);


VkImageView fetchImageView(PuleGpuImageView const imageView);

} // ::util

namespace util::str {
  std::string vkImageLayout(VkImageLayout const layout);
  std::string vkAccessFlags(VkAccessFlags const flags);
}
