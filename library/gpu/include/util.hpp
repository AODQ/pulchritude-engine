#include <pulchritude/gpu.h>

#include <pulchritude/core.hpp>
#include <pulchritude/math.h>

#include <volk.h>

#include <array>
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

// macro to convert vulkan error to string print
#define PULE_vkError(X) \
  if (X != VK_SUCCESS) { \
    puleLogError( \
      "vulkan error with %s: %s", \
      #X, \
      util::vkErrorToString(X) \
    ); \
    PULE_assert(false); \
  }

namespace util {

  char const * vkErrorToString(VkResult const result);

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

struct ImageChainImage {
  PuleGpuImage image;
  std::string label;
  uint64_t frameIdx;
};

struct ImageChain {
  std::vector<ImageChainImage> chain = {};
  PuleAllocator allocator = {
    .implementation = nullptr,
    .allocate = nullptr,
    .reallocate = nullptr,
    .deallocate = nullptr,
  };
};

enum struct ImageReferenceType {
  imageChain,
  image,
  size,
};

struct ImageReference {
  union {
    PuleGpuImageChain imageChain;
    PuleGpuImage image;
  };
  ImageReferenceType type;
  int32_t ownerCount; // simple lifetime tracking
};

struct ImageInfo {
  std::string label;
};

struct Context {
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  util::Device device;
  VkSurfaceKHR surface;
  VmaAllocator vmaAllocator;
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
  std::unordered_map<uint64_t, util::ImageInfo> images;
  std::unordered_map<
    uint64_t, util::CommandBufferRecorder
  > commandBufferRecorders;
  VkSwapchainKHR swapchain;
  uint32_t swapchainCurrentImageIdx;
  std::vector<VkImage> swapchainImages;
  std::vector<VkSemaphore> swapchainImageAvailableSemaphores;
  VkSampler samplerDefault;
  size_t frameIdx = 1;

  pule::ResourceContainer<util::ImageChain> imageChains;
  pule::ResourceContainer<util::ImageReference> imageReferences;

  // this tracks underlying image references (images, image chains, etc)
  // and maps them to the reference. This way, if any image/image chain
  // reference is requested which is already referenced, then that reference
  // can be returned. Thusly, if any image is 'updated', the sole underlying
  // reference can have its handle remapped to the new image
  std::array<
    std::unordered_map<uint64_t, uint64_t>,
    (size_t)util::ImageReferenceType::size
  > imageHandleToReference = {};
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

VkPrimitiveTopology toVkPrimitiveTopology(
  PuleGpuDrawPrimitive const primitiveTopology
);

} // ::util

namespace util::str {
  std::string vkImageLayout(VkImageLayout const layout);
  std::string vkAccessFlags(VkAccessFlags const flags);
}
