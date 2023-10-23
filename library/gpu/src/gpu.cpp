#include <pulchritude-gpu/gpu.h>

#include <util.hpp>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-platform/platform.h>

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include <volk.h>
#include <GLFW/glfw3.h>

#include "../include/vulkan-memory-allocator.hpp"
#include "pulchritude-math/math.h"

namespace {
  std::unordered_set<std::string> loggedErrorMessages;
}

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

namespace {

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

VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance const instance) {
  auto ci = createDebugMessengerCi();
  VkDebugUtilsMessengerEXT debugMessenger = nullptr;
  PULE_assert(
    vkCreateDebugUtilsMessengerEXT(
      instance,
      &ci,
      nullptr,
      &debugMessenger
    ) == VK_SUCCESS
  );
  return debugMessenger;
}

VkInstance createInstance(PuleError * const error) {
  puleLogDebug("[Pule|Vk] creating instance");
  auto applicationInfo = VkApplicationInfo {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = nullptr,
    .pApplicationName = "Made With <#",
    .applicationVersion = 0x0001,
    .pEngineName = "Pulchritude",
    .engineVersion = 0,
    .apiVersion = VK_API_VERSION_1_3,
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
        instanceExtensions.emplace_back(glfwExtensions[it]);
      }
    }
    instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  std::vector<char const *> instanceLayers = { "VK_LAYER_KHRONOS_validation", };

  std::vector<VkValidationFeatureEnableEXT> validationFeaturesEnabled = {
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
    VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
    VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
    //VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT,
    VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
  };
  auto validationFeatures = VkValidationFeaturesEXT {
    .sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT,
    .pNext = nullptr,
    .enabledValidationFeatureCount = (uint32_t)validationFeaturesEnabled.size(),
    .pEnabledValidationFeatures = validationFeaturesEnabled.data(),
    .disabledValidationFeatureCount = 0,
    .pDisabledValidationFeatures = 0,
  };

  auto debugMessengerCi = createDebugMessengerCi();
  validationFeatures.pNext = &debugMessengerCi;

  auto instanceCi = VkInstanceCreateInfo {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pNext = &validationFeatures,
    .flags = 0x0,
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
  { // logical device
    auto queuePriorityDefault = 1.0f;
    auto deviceQueueCi = std::vector<VkDeviceQueueCreateInfo> {
      {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = device.queues.idxGTC,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorityDefault,
      }, {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = device.queues.idxCompute,
        .queueCount = 1,
        .pQueuePriorities = &queuePriorityDefault,
      },
    };

    VkPhysicalDeviceFeatures features;
    memset(&features, VK_FALSE, sizeof(VkPhysicalDeviceFeatures));
    features.shaderFloat64 = VK_TRUE;
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
    util::chainPNextList(
      {
        &featuresRequest, &features8Bit,
        &featuresSync2,
        &featuresDynamicRendering,
        &featuresImageAtomicInt64, &featuresAtomicInt64,
      }
    );
    std::vector<char const *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
      VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
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

#if 0
static void GLAPIENTRY errorMessageCallback(
  [[maybe_unused]] GLenum source,
  GLenum type,
  [[maybe_unused]] GLenum id,
  GLenum severity,
  [[maybe_unused]] GLsizei length,
  GLchar const * message,
  [[maybe_unused]] void const * userdata
) {
  auto const messageStr = std::string(message);
  if (loggedErrorMessages.find(messageStr) != loggedErrorMessages.end()) {
    return;
  }
  switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      puleLogError("OpenGL error [0x%x]: %s", type, message);
    break;
    case GL_DEBUG_SEVERITY_MEDIUM:
    case GL_DEBUG_SEVERITY_LOW:
    case GL_DEBUG_SEVERITY_NOTIFICATION:
    default:
      puleLogDebug("OpenGL message [0x%x]: %s", type, message);
    break;
  }
  loggedErrorMessages.emplace(messageStr);
}
#endif

void puleGpuInitialize(PulePlatform const platform, PuleError * const error) {
  puleLog("Initializing vulkan loader");
  PULE_vkAssert(volkInitialize(), PuleErrorGfx_creationFailed,);
  puleLog("Initializing vulkan instance");
  VkInstance instance = createInstance(error);
  if (puleErrorExists(error)) { return; }
  puleLog("Instance created: %u", instance);
  puleLog("Initializing vulkan loader for instance");
  volkLoadInstance(instance);
  puleLog("Creating debug messenger");
  auto debugMessenger = createDebugMessenger(instance);
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

  #if 0
  // TODO check platform even exists;
  //  i should pass in platform here as insurance of this
  PULE_errorAssert(
    gladLoadGLLoader(
      reinterpret_cast<GLADloadproc>(pulePlatformGetProcessAddress())
    ),
    PuleErrorGfx_creationFailed,
  );

  glEnable(GL_SCISSOR_TEST);
  glDebugMessageCallback(errorMessageCallback, nullptr);
  glEnable(GL_DEBUG_OUTPUT); // fast asynchronous errors

  glDisable(GL_PRIMITIVE_RESTART);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE); // TODO configure
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_MULTISAMPLE);
  glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
  glDisable(GL_SAMPLE_ALPHA_TO_ONE);
  glDisable(GL_SAMPLE_COVERAGE);
  glDisable(GL_STENCIL_TEST);

  glFrontFace(GL_CW);
  #endif
  auto & ctx = util::ctx();
  ctx.instance = instance;
  ctx.debugMessenger = debugMessenger;
  ctx.device = device;
  ctx.surface = surface;
  ctx.vmaAllocator = allocator;

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
      .size = 65'536,
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
    puleLog("Creating staging buffer with size: %zu", 65'536);
    PULE_assert(
      vmaCreateBuffer(
        util::ctx().vmaAllocator,
        &stagingBufferCi, &stagingAllocationCi,
        &ctx.utilStagingBuffer, &ctx.utilStagingBufferAllocation,
        &ctx.utilStagingBufferAllocationInfo
      ) == VK_SUCCESS
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

#if 0
namespace {
  [[maybe_unused]]
  GLenum bufferUsageToGl(PuleGpuBufferUsage const usage) {
    switch (usage) {
      default:
        puleLogError("usage is invalid with renderer: %d", usage);
        return 0;
      case PuleGpuBufferUsage_bufferAttribute: return GL_ARRAY_BUFFER;
      case PuleGpuBufferUsage_bufferElement: return GL_ELEMENT_ARRAY_BUFFER;
      case PuleGpuBufferUsage_bufferUniform: return GL_UNIFORM_BUFFER;
      case PuleGpuBufferUsage_bufferIndirect: return GL_DRAW_INDIRECT_BUFFER;
    }
  }

  GLbitfield bufferVisibilityToGl(PuleGpuBufferVisibilityFlag const usage) {
    if (usage & PuleGpuBufferVisibilityFlag_deviceOnly) {
      if (usage != PuleGpuBufferVisibilityFlag_deviceOnly) {
        puleLogError("incompatible buffer visibility: %d", usage);
      }
      return 0;
    }
    GLbitfield field = 0;
    if (usage & PuleGpuBufferVisibilityFlag_hostVisible) {
      field |= GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT;
    }
    if (usage & PuleGpuBufferVisibilityFlag_hostWritable) {
      field |= GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
    }
    return field;
  }

  GLbitfield bufferMapAccessToGl(PuleGpuBufferMapAccess const access) {
    GLbitfield field = 0;
    if (access & PuleGpuBufferMapAccess_hostVisible) {
      field |= (
        GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT
      );
    }
    if (access & PuleGpuBufferMapAccess_hostWritable) {
      field |= (
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
      );
    }
    if (access & PuleGpuBufferMapAccess_invalidate) {
      field |= GL_MAP_INVALIDATE_RANGE_BIT;
    }
    return field;
  }
}
#endif

extern "C" {

PuleGpuBuffer puleGpuBufferCreate(
  PuleStringView const name,
  void const * const optionalInitialData,
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

  // upload initial memory into buffer if requested
  if (optionalInitialData) {
    VkMemoryPropertyFlags memoryPropertyFlags;
    vmaGetAllocationMemoryProperties(
      util::ctx().vmaAllocator,
      allocation,
      &memoryPropertyFlags
    );
    // check if we can upload directly to buffer, or if it needs to be staged
    if (
      allocationInfo.pMappedData
      && memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    ) {
      memcpy(allocationInfo.pMappedData, optionalInitialData, byteLength);
    } else {
      // TODO::CRIT stage up the 64Kb blocks until complete
      memcpy(
        util::ctx().utilStagingBufferAllocationInfo.pMappedData,
        optionalInitialData,
        byteLength
      );
      auto beginInfo = VkCommandBufferBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
      };
      PULE_assert(
        vkBeginCommandBuffer(
          util::ctx().utilCommandBuffer,
          &beginInfo
        ) == VK_SUCCESS
      );
      vmaFlushAllocation(
        util::ctx().vmaAllocator,
        util::ctx().utilStagingBufferAllocation,
        0, VK_WHOLE_SIZE
      );
      // might need vkCmdPipelineBarrier
      auto const stagingBufferCopy = VkBufferCopy { 0, 0, byteLength, };
      vkCmdCopyBuffer(
        util::ctx().utilCommandBuffer,
        util::ctx().utilStagingBuffer,
        buffer,
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
      PULE_assert(
        vkQueueSubmit(
          util::ctx().defaultQueue,
          1, &stagingSubmitInfo, nullptr
        ) == VK_SUCCESS
      );
      // TODO replace with cmd pipeline barrier
      vkDeviceWaitIdle(util::ctx().device.logical);
    }
  }

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

  return { .id = reinterpret_cast<uint64_t>(buffer), };
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

#include <pulchritude-imgui/imgui.h>

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
