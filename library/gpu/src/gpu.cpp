#include <pulchritude/gpu.h>

#include <util.hpp>

#include <pulchritude/error.h>
#include <pulchritude/log.h>
#include <pulchritude/math.h>
#include <pulchritude/platform.h>

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#if defined(__APPLE__)
#define VK_USE_PLATFORM_MACOS_MVK
#endif

#include <volk.h>
#include <GLFW/glfw3.h>

#include "../include/vulkan-memory-allocator.hpp"

namespace {
  std::unordered_set<std::string> loggedErrorMessages;
}

#if VK_VALIDATION_ENABLED
extern "C" {
VkBool32 debugCallbackVk(
  [[maybe_unused]] VkDebugUtilsMessageSeverityFlagBitsEXT const severity,
  [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT const type,
  VkDebugUtilsMessengerCallbackDataEXT const * const callbackData,
  [[maybe_unused]] void * const userData
) {
  if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    puleLogWarn("%s", callbackData->pMessage);
  }
  else if (severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    puleLogError("%s", callbackData->pMessage);
  }
  return VK_FALSE;
}
} // extern C
#endif

namespace {

#if VK_VALIDATION_ENABLED
VkDebugUtilsMessengerCreateInfoEXT createDebugMessengerCi() {
  return VkDebugUtilsMessengerCreateInfoEXT {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = nullptr,
    .flags = 0,
    .messageSeverity = (
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
    ),
    .messageType = (
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
    ),
    .pfnUserCallback = &debugCallbackVk,
    .pUserData = nullptr,
  };
}
#endif

#if VK_VALIDATION_ENABLED
VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance const instance) {
  auto ci = createDebugMessengerCi();
  VkDebugUtilsMessengerEXT debugMessenger = nullptr;
  PULE_vkError(
    vkCreateDebugUtilsMessengerEXT(
      instance,
      &ci,
      nullptr,
      &debugMessenger
    )
  );
  return debugMessenger;
}
#endif

VkInstance createInstance(PuleError * const error) {
  puleLogDebug("[Pule|Vk] creating instance");
  auto applicationInfo = VkApplicationInfo {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = "N/A",
    .applicationVersion = 0x0001,
    .pEngineName = "Pulchritude",
    .engineVersion = 1,
    #if !defined(__APPLE__)
    .apiVersion = VK_API_VERSION_1_3,
    #else
    .apiVersion = VK_API_VERSION_1_2,
    #endif
  };

  // get GLFW extensions & add our own
  std::vector<char const *> instanceExtensions;
  {
    uint32_t glfwExtensionLength = 0;
    char const * * glfwExtensions = (
      pulePlatformRequiredExtensions(&glfwExtensionLength)
    );
    {
      for (size_t it = 0; it < glfwExtensionLength; ++ it) {
        instanceExtensions.push_back(glfwExtensions[it]);
      }
    }
    #if VK_VALIDATION_ENABLED
    instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif
    #if defined(__APPLE__)
    instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif
  }

  std::vector<char const *> instanceLayers = {
    #if VK_VALIDATION_ENABLED
    "VK_LAYER_KHRONOS_validation",
    #endif
  };

  std::vector<VkValidationFeatureEnableEXT> validationFeaturesEnabled = {
    #if VK_VALIDATION_ENABLED
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
    VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    //VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
    #endif
  };
  auto validationFeatures = VkValidationFeaturesEXT {
    .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    .pNext = nullptr,
    .enabledValidationFeatureCount = (uint32_t)validationFeaturesEnabled.size(),
    .pEnabledValidationFeatures = validationFeaturesEnabled.data(),
    .disabledValidationFeatureCount = 0,
    .pDisabledValidationFeatures = 0,
  };

  #if VK_VALIDATION_ENABLED
  auto debugMessengerCi = createDebugMessengerCi();
  validationFeatures.pNext = &debugMessengerCi;
  #endif

  auto instanceCi = VkInstanceCreateInfo {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = &validationFeatures,
    .flags = (
      0
      #if defined(__APPLE__)
      | VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
      #endif
    ),
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = (uint32_t)instanceLayers.size(),
    .ppEnabledLayerNames = instanceLayers.data(),
    .enabledExtensionCount = (uint32_t)instanceExtensions.size(),
    .ppEnabledExtensionNames = instanceExtensions.data(),
  };

  VkInstance instance;
  PULE_vkAssert(
    vkCreateInstance(&instanceCi, nullptr, &instance),
    PuleErrorGfx_creationFailed,
    { 0 }
  );
  return instance;
}

util::Device createDevice(VkInstance const instance, PuleError * const error) {
  puleLogDebug("[Gfx|Vk] creating device: %u", (uint64_t)instance);
  // TODO pick the best GPU or whatever, for now, who cares
  util::Device device {};
  puleLogDebug("[Gfx|Vk] creating physical device");
  { // physical device
    auto physicalDeviceCount = 0u;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(physicalDeviceCount);
    vkEnumeratePhysicalDevices(
      instance,
      &physicalDeviceCount, physicalDevices.data()
    );
    device.physical = physicalDevices[0];
  }

  puleLogDebug("[Gfx|Vk] creating queue family properties");
  { // queue family properties
    auto propertiesQueueFamilyLen = 0u;
    vkGetPhysicalDeviceQueueFamilyProperties(
      device.physical, &propertiesQueueFamilyLen, nullptr
    );
    device.propertiesQueueFamily.resize(propertiesQueueFamilyLen);

    vkGetPhysicalDeviceQueueFamilyProperties(
      device.physical,
      &propertiesQueueFamilyLen,
      device.propertiesQueueFamily.data()
    );
  }

  puleLogDebug("[Gfx|Vk] device queue indices");
  { // fetch device queue indices
    for (
      auto idxFamily = 0u;
      idxFamily < device.propertiesQueueFamily.size();
      ++ idxFamily
    ) {
      auto propertiesQueueFamily = VkQueueFamilyProperties(
        device.propertiesQueueFamily[idxFamily]
      );
      auto const hasGfx = bool(
        propertiesQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
      );
      auto const hasCompute = bool(
        propertiesQueueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT
      );
      auto const hasTransfer = bool(
        propertiesQueueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT
      );

      if (hasGfx) { device.queues.idxGraphics = idxFamily; }
      if (hasCompute) { device.queues.idxCompute = idxFamily; }
      if (hasTransfer) { device.queues.idxTransfer = idxFamily; }
      if (hasGfx && hasCompute && hasTransfer) {
        device.queues.idxGTC = idxFamily;
      }
    }
    // TODO get a better present I guess?
    device.queues.idxPresent = device.queues.idxGraphics;
  }

  puleLogDebug("[Gfx|Vk] logical device");
  puleLogDev("GTC: %d", device.queues.idxGTC);
  puleLogDev("Gfx: %d", device.queues.idxGraphics);
  puleLogDev("Trn: %d", device.queues.idxTransfer);
  puleLogDev("Cmp: %d", device.queues.idxCompute);
  puleLogDev("Prs: %d", device.queues.idxPresent);
  { // logical device
    auto queuePriorityDefault = 1.0f;
    auto deviceQueueCi = std::vector<VkDeviceQueueCreateInfo> {};
    auto & queues = device.queues;
    if (queues.idxGTC != -1u) {
      deviceQueueCi.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queues.idxGTC,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorityDefault,
      });
    }
    if (queues.idxGraphics != -1u && queues.idxGraphics != queues.idxGTC) {
      deviceQueueCi.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queues.idxGraphics,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorityDefault,
      });
    }
    if (queues.idxCompute != -1u && queues.idxCompute != queues.idxGTC) {
      deviceQueueCi.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queues.idxCompute,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorityDefault,
      });
    }
    if (queues.idxTransfer != -1u && queues.idxTransfer != queues.idxGTC) {
      deviceQueueCi.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queues.idxTransfer,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorityDefault,
      });
    }
    if (queues.idxPresent != -1u && queues.idxPresent != queues.idxGTC) {
      deviceQueueCi.push_back({
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queues.idxPresent,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorityDefault,
      });
    }

    VkPhysicalDeviceFeatures features;
    memset(&features, VK_FALSE, sizeof(VkPhysicalDeviceFeatures));
    #if !defined(__APPLE__)
    features.shaderFloat64 = VK_TRUE;
    #endif
    features.shaderInt64 = VK_TRUE;
    features.shaderInt16 = VK_TRUE;
    auto featuresRequest = VkPhysicalDeviceFeatures2 {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = nullptr,
      .features = features,
    };
    auto featuresSync2 = VkPhysicalDeviceSynchronization2FeaturesKHR {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES_KHR,
      .pNext = nullptr,
      .synchronization2 = true,
    };
    auto featuresDynamicRendering = VkPhysicalDeviceDynamicRenderingFeatures {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
      .pNext = nullptr,
      .dynamicRendering = VK_TRUE,
    };
    auto features8Bit = VkPhysicalDevice8BitStorageFeatures {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES,
      .pNext = nullptr,
      .storageBuffer8BitAccess = VK_TRUE,
      .uniformAndStorageBuffer8BitAccess = VK_TRUE,
      .storagePushConstant8 = VK_TRUE,
    };
#if !defined(__APPLE__)
    auto featuresImageAtomicInt64 = (
      VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT {
        .sType = (
          VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_IMAGE_ATOMIC_INT64_FEATURES_EXT
        ),
        .pNext = nullptr,
        .shaderImageInt64Atomics = VK_TRUE,
        .sparseImageInt64Atomics = VK_TRUE,
      }
    );
    auto featuresAtomicInt64 = (
      VkPhysicalDeviceShaderAtomicInt64Features {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_ATOMIC_INT64_FEATURES,
        .pNext = nullptr,
        .shaderBufferInt64Atomics = VK_TRUE,
        .shaderSharedInt64Atomics = VK_TRUE,
      }
    );
#endif
    puleLogSectionBegin({"[Gfx|Vk] device extensions", false, true});
    util::chainPNextList(
      {
        &featuresRequest, &features8Bit,
        &featuresSync2,
        &featuresDynamicRendering,
#if !defined(__APPLE__)
        &featuresImageAtomicInt64, &featuresAtomicInt64,
#endif
      }
    );
    puleLogSectionEnd();
    std::vector<char const *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
#if defined(__APPLE__)
      VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
      "VK_KHR_portability_subset",
#endif
#if !defined(__APPLE__)
      VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
#endif
      VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
      VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
    };
    auto deviceCi = VkDeviceCreateInfo {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = &featuresRequest,
      .flags = 0,
      .queueCreateInfoCount = (uint32_t)deviceQueueCi.size(),
      .pQueueCreateInfos = deviceQueueCi.data(),
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = (uint32_t)deviceExtensions.size(),
      .ppEnabledExtensionNames = deviceExtensions.data(),
      .pEnabledFeatures = nullptr,
    };
    PULE_vkAssert(
      vkCreateDevice(device.physical, &deviceCi, nullptr, &device.logical),
      PuleErrorGfx_creationFailed,
      {}
    );
  }

  puleLogDebug("[Gfx|Vk] fill out properties");
  { // fill out properties
    auto propertiesFetcher = VkPhysicalDeviceProperties2 {
      .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
      .pNext = nullptr,
      .properties = {},
    };
    util::chainPNextList({
      &propertiesFetcher, &device.propertiesVersion11,
      &device.propertiesVersion12, &device.propertiesVersion13,
      &device.propertiesMeshShader,
      &device.propertiesMemory,
      &device.propertiesMemoryVersion2,
      &device.propertiesMemoryBudget,
    });
    vkGetPhysicalDeviceProperties(
      device.physical,
      reinterpret_cast<VkPhysicalDeviceProperties *>(&propertiesFetcher)
    );
    device.properties = propertiesFetcher.properties;
  }
  return device;
}

} // namespace

extern "C" {

void puleGpuInitialize(PulePlatform const platform, PuleError * const error) {
  puleLog("Initializing vulkan loader");
  PULE_vkAssert(volkInitialize(), PuleErrorGfx_creationFailed,);
  puleLog("Initializing vulkan instance");
  VkInstance instance = createInstance(error);
  if (puleErrorExists(error)) { return; }
  puleLog("Instance created: %u", instance);
  puleLog("Initializing vulkan loader for instance");
  volkLoadInstance(instance);
  #if VK_VALIDATION_ENABLED
  puleLog("Creating debug messenger");
  auto debugMessenger = createDebugMessenger(instance);
  #endif
  puleLog("Creating device");
  util::Device device = createDevice(instance, error);
  if (puleErrorExists(error)) { return; }
  puleLog("Device created");
  // puleLog("Initializing vulkan loader for device");
  // volkLoadDevice(device.logical);
  // TODO this is a hack, but since i need the function, and the function
  // requires VkInstance, I'm not sure how to get around it without also
  // including Vulkan in Platform which I don't want to do
  VkSurfaceKHR surface;
  PULE_vkAssert(
    glfwCreateWindowSurface(
      instance,
      reinterpret_cast<GLFWwindow *>(platform.id),
      nullptr,
      &surface
    ),
    PuleErrorGfx_creationFailed,
  );
  puleLog("surface created: %u", (uint64_t)surface);

  auto vmaVulkanFunctions = VmaVulkanFunctions {
    .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
    .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
    .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
    .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
    .vkAllocateMemory = vkAllocateMemory,
    .vkFreeMemory = vkFreeMemory,
    .vkMapMemory = vkMapMemory,
    .vkUnmapMemory = vkUnmapMemory,
    .vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
    .vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
    .vkBindBufferMemory = vkBindBufferMemory,
    .vkBindImageMemory = vkBindImageMemory,
    .vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
    .vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
    .vkCreateBuffer = vkCreateBuffer,
    .vkDestroyBuffer = vkDestroyBuffer,
    .vkCreateImage = vkCreateImage,
    .vkDestroyImage = vkDestroyImage,
    .vkCmdCopyBuffer = vkCmdCopyBuffer,
    .vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2,
    .vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2,
    .vkBindBufferMemory2KHR = vkBindBufferMemory2,
    .vkBindImageMemory2KHR = vkBindImageMemory2,
    .vkGetPhysicalDeviceMemoryProperties2KHR = (
      vkGetPhysicalDeviceMemoryProperties2
    ),
    .vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
    .vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements,
  };
  VmaAllocatorCreateInfo allocatorCreateInfo = {
    .flags = 0,
      // TODO::BUFFER
      /*VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,*/
    .physicalDevice = device.physical,
    .device = device.logical,
    .preferredLargeHeapBlockSize = 0,
    .pAllocationCallbacks = nullptr,
    .pDeviceMemoryCallbacks = nullptr,
    .pHeapSizeLimit = nullptr,
    .pVulkanFunctions = &vmaVulkanFunctions,
    .instance = instance,
    .vulkanApiVersion = VK_API_VERSION_1_3,
    .pTypeExternalMemoryHandleTypes = nullptr,
  };
  VmaAllocator allocator;
  vmaCreateAllocator(&allocatorCreateInfo, &allocator);

  auto & ctx = util::ctx();
  ctx.instance = instance;
  #if VK_VALIDATION_ENABLED
  ctx.debugMessenger = debugMessenger;
  #endif
  ctx.device = device;
  ctx.surface = surface;
  ctx.vmaAllocator = allocator;

  if (ctx.device.queues.idxGTC == -1u) {
    PULE_assert(false && "No GTC queue found");
    return;
  }

  vkGetDeviceQueue(
    ctx.device.logical,
    ctx.device.queues.idxGTC,
    0,
    &ctx.defaultQueue
  );
  auto commandPoolCi = VkCommandPoolCreateInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = ctx.device.queues.idxGTC
  };
  PULE_vkAssert(
    vkCreateCommandPool(
      ctx.device.logical,
      &commandPoolCi, nullptr, &ctx.defaultCommandPool
    ),
    PuleErrorGfx_creationFailed,
  );
  auto descriptorPoolSizes = std::vector<VkDescriptorPoolSize> {
    { VK_DESCRIPTOR_TYPE_SAMPLER,                    1024 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,     1024 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,              1024 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,              1024 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,       1024 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,       1024 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,             1024 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,             1024 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,     1024 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,     1024 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,           1024 },
    // TODO::RAYTRACE
    // { VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1024 },
  };
  auto descriptorPoolCi = VkDescriptorPoolCreateInfo {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0, // TODO?
    .maxSets = 2048,
    .poolSizeCount = (uint32_t)descriptorPoolSizes.size(),
    .pPoolSizes = descriptorPoolSizes.data(),
  };
  PULE_vkAssert(
    vkCreateDescriptorPool(
      ctx.device.logical,
      &descriptorPoolCi, nullptr, &ctx.defaultDescriptorPool
    ),
    PuleErrorGfx_creationFailed,
  );
  {
    auto cmdBufferCi = VkCommandBufferAllocateInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
      .pNext = nullptr,
      .commandPool = util::ctx().defaultCommandPool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
      .commandBufferCount = 1,
    };
    PULE_vkAssert(
      vkAllocateCommandBuffers(
        util::ctx().device.logical,
        &cmdBufferCi, &ctx.utilCommandBuffer
      ),
      PuleErrorGfx_creationFailed,
    );
  }
  { // 64Kb staging memory buffer
    auto stagingBufferCi = VkBufferCreateInfo {
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = 4096*4096,
      .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 0,
      .pQueueFamilyIndices = nullptr,
    };
    auto stagingAllocationCi = VmaAllocationCreateInfo {
      .flags = (
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
        | VMA_ALLOCATION_CREATE_MAPPED_BIT
      ),
      .usage = VMA_MEMORY_USAGE_AUTO,
      .requiredFlags = 0,
      .preferredFlags = 0,
      .memoryTypeBits = 0,
      .pool = nullptr,
      .pUserData = nullptr,
      .priority = 1.0f,
    };
    PULE_vkError(
      vmaCreateBuffer(
        util::ctx().vmaAllocator,
        &stagingBufferCi, &stagingAllocationCi,
        &ctx.utilStagingBuffer, &ctx.utilStagingBufferAllocation,
        &ctx.utilStagingBufferAllocationInfo
      )
    );
  }
  util::ctx().swapchain = util::swapchainCreate();
  util::swapchainImagesCreate();

  { // create default sampler
    VkSamplerCreateInfo info = {
      .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
      .flags = 0,
      .magFilter = VK_FILTER_LINEAR,
      .minFilter = VK_FILTER_LINEAR,
      .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
      .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
      .maxAnisotropy = 1.0f,
      .minLod = -1000,
      .maxLod = +1000,
    };
    vkCreateSampler(
      util::ctx().device.logical, &info, nullptr, &util::ctx().samplerDefault
    );
  }
}

void puleGpuShutdown() {
  // TODO ...
  vkDestroyDevice(util::ctx().device.logical, nullptr);
}

}

//------------------------------------------------------------------------------
//-- GPU BUFFER ----------------------------------------------------------------
//------------------------------------------------------------------------------

extern "C" {

PuleGpuBuffer puleGpuBufferCreate(
  PuleStringView const name,
  size_t const byteLength,
  [[maybe_unused]] PuleGpuBufferUsage const usage,
  PuleGpuBufferVisibilityFlag const visibility
) {
  puleLogDebug(
    "[Gfx|Vk] creating buffer: %s initial data: %p size: %zu usage: ",
    name.contents
  );
  VkBufferUsageFlags usageVk = util::toBufferUsageFlags(usage);
  usageVk |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  usageVk |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  auto bufferCi = VkBufferCreateInfo {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = nullptr,
    .flags = (
      0
      // | VK_BUFFER_CREATE_SPARSE_BINDING_BIT
      // | VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT
    ),
    .size = byteLength,
    .usage = usageVk,
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 0,
    .pQueueFamilyIndices = nullptr, // TODO probably want to use concurrent l8r?
  };

  auto allocationFlags = VmaAllocationCreateFlags { 0 };
  VkMemoryPropertyFlags bufferMemoryProperties = 0;
  if (visibility & PuleGpuBufferVisibilityFlag_hostWritable) {
    allocationFlags |= (
      VMA_ALLOCATION_CREATE_MAPPED_BIT
      | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT
    );
    bufferMemoryProperties |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  }
  if (visibility & PuleGpuBufferVisibilityFlag_hostVisible) {
    allocationFlags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
    bufferMemoryProperties |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  }
  auto allocationCi = VmaAllocationCreateInfo {
    .flags = allocationFlags,
    .usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
    .requiredFlags = bufferMemoryProperties, // TODO do i need to set this?
    .preferredFlags = bufferMemoryProperties, // TODO do I need to set this?
    .memoryTypeBits = 0, // TODO I dont think i can even get this
    .pool = nullptr, // TODO Setup pools for buffers
    .pUserData = nullptr,
    .priority = 1.0f,
  };
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo allocationInfo;
  puleLog("Creating buffer with size: %zu", byteLength);
  PULE_vkError(
    vmaCreateBuffer(
      util::ctx().vmaAllocator,
      &bufferCi, &allocationCi,
      &buffer, &allocation, &allocationInfo
    )
  );
  util::ctx().buffers.emplace(
    reinterpret_cast<uint64_t>(buffer),
    util::Buffer {
      .vkHandle = buffer,
      .allocation = allocation, .allocationInfo = allocationInfo,
    }
  );

  #if VK_VALIDATION_ENABLED
  { // name the buffer object
    VkDebugUtilsObjectNameInfoEXT nameInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .pNext = nullptr,
      .objectType = VK_OBJECT_TYPE_BUFFER,
      .objectHandle = reinterpret_cast<uint64_t>(buffer),
      .pObjectName = name.contents,
    };
    vkSetDebugUtilsObjectNameEXT(util::ctx().device.logical, &nameInfo);
  }
  #endif

  return { .id = reinterpret_cast<uint64_t>(buffer), };
}

void puleGpuBufferMemcpy(
  PuleGpuBufferMappedFlushRange range,
  void const * data
) {
  auto buffer = util::ctx().buffers.at(range.buffer.id);
  VkMemoryPropertyFlags memoryPropertyFlags;
  vmaGetAllocationMemoryProperties(
    util::ctx().vmaAllocator,
    buffer.allocation,
    &memoryPropertyFlags
  );
  // check if we can upload directly to buffer, or if it needs to be staged
  if (
    buffer.allocationInfo.pMappedData
    && memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
  ) {
    memcpy(
      ((uint8_t *)buffer.allocationInfo.pMappedData) + range.byteOffset,
      data,
      range.byteLength
    );
  } else {
    size_t const utilSize = util::ctx().utilStagingBufferAllocationInfo.size;
    size_t const blockCopies = range.byteLength / utilSize + 1;
    puleLogDev("block copies: %zu", blockCopies);
    for (size_t blockIt = 0; blockIt < blockCopies; ++ blockIt) {
      if (blockIt*utilSize >= range.byteLength) { break; }
      size_t bytesToCopy = (
          (blockIt+1)*utilSize > range.byteLength
        ? range.byteLength - blockIt*utilSize
        : utilSize
      );
      puleLogDev(
        "copying block %zu/%zu, at %zu with %zu bytes",
        blockIt, blockCopies, blockIt*utilSize, bytesToCopy
      );
      memcpy(
        util::ctx().utilStagingBufferAllocationInfo.pMappedData,
        ((uint8_t *)data) + blockIt * utilSize,
        bytesToCopy
      );
      auto beginInfo = VkCommandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
      };
      PULE_vkError(
        vkBeginCommandBuffer(
          util::ctx().utilCommandBuffer,
          &beginInfo
        )
      );
      vmaFlushAllocation(
        util::ctx().vmaAllocator,
        util::ctx().utilStagingBufferAllocation,
        0, VK_WHOLE_SIZE
      );
      // might need vkCmdPipelineBarrier
      auto const stagingBufferCopy = VkBufferCopy {
        .srcOffset = 0,
        .dstOffset = range.byteOffset + blockIt*utilSize,
        .size = bytesToCopy,
      };
      vkCmdCopyBuffer(
        util::ctx().utilCommandBuffer,
        util::ctx().utilStagingBuffer,
        buffer.vkHandle,
        1, &stagingBufferCopy
      );
      vkEndCommandBuffer(util::ctx().utilCommandBuffer);
      auto stagingSubmitInfo = VkSubmitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0, .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &util::ctx().utilCommandBuffer,
        .signalSemaphoreCount = 0, .pSignalSemaphores = nullptr,
      };
      PULE_vkError(
        vkQueueSubmit(
          util::ctx().defaultQueue,
          1, &stagingSubmitInfo, nullptr
        )
      );
      // TODO replace with cmd pipeline barrier
      vkDeviceWaitIdle(util::ctx().device.logical);
    }
    puleLogDev("done copying");
  }
}

void puleGpuBufferDestroy(PuleGpuBuffer const pBuffer) {
  if (pBuffer.id == 0) { return; }
  util::Buffer & buffer = util::ctx().buffers.at(pBuffer.id);
  vmaDestroyBuffer(util::ctx().vmaAllocator, buffer.vkHandle, buffer.allocation);
}

void * puleGpuBufferMap(PuleGpuBufferMapRange const range) {
  void * mappedData = nullptr;
  util::Buffer & buffer = util::ctx().buffers.at(range.buffer.id);
  vmaMapMemory(util::ctx().vmaAllocator, buffer.allocation, &mappedData);
  PULE_assert(mappedData);
  return mappedData;
}

void puleGpuBufferUnmap(PuleGpuBuffer const pBuffer) {
  util::Buffer & buffer = util::ctx().buffers.at(pBuffer.id);
  vmaUnmapMemory(util::ctx().vmaAllocator, buffer.allocation);
}

PuleGpuSemaphore puleGpuFrameStart() {
  util::swapchainAcquireNextImage(nullptr);
  return puleGpuSwapchainAvailableSemaphore();
}

PuleGpuSemaphore puleGpuSwapchainAvailableSemaphore() {
  size_t const swapchainImageIndex = util::ctx().swapchainCurrentImageIdx;
  VkSemaphore const swapchainImageAvailableSemaphore = (
    util::ctx().swapchainImageAvailableSemaphores[swapchainImageIndex]
  );
  return { .id = (uint64_t)swapchainImageAvailableSemaphore, };
}

void puleGpuBufferMappedFlush(
  PuleGpuBufferMappedFlushRange const range
) {
  util::Buffer & buffer = util::ctx().buffers.at(range.buffer.id);
  vmaFlushAllocation(
    util::ctx().vmaAllocator,
    buffer.allocation,
    range.byteOffset,
    range.byteLength
  );
}

#include <pulchritude/imgui.h>

void puleGpuDebugPrint() {
  // util::printCommandsDebug();
}

PuleStringView puleGpuBufferUsageLabel(PuleGpuBufferUsage const usage) {
  switch (usage) {
    case PuleGpuBufferUsage_attribute: return puleCStr("attribute");
    case PuleGpuBufferUsage_element: return puleCStr("element");
    case PuleGpuBufferUsage_uniform: return puleCStr("uniform");
    case PuleGpuBufferUsage_storage: return puleCStr("storage");
    case PuleGpuBufferUsage_accelerationStructure:
      return puleCStr("accelerationStructure");
    case PuleGpuBufferUsage_indirect: return puleCStr("indirect");
    default: return puleCStr("unknown");
  }
}

} // extern C
