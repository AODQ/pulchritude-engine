#include <pulchritude/gpu.h>

#include "util.hpp"

#include <pulchritude/log.h>

#include <volk.h>

#include <string>
#include <unordered_map>

namespace {
std::unordered_map<uint64_t, std::string> fenceDebugNames;
}

// -- semaphore --
extern "C" {

PuleGpuSemaphore puleGpuSemaphoreCreate(PuleStringView const label) {
  VkSemaphore semaphore = VK_NULL_HANDLE;
  VkSemaphoreCreateInfo semaphoreCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_SEMAPHORE_TYPE_BINARY,
  };
  PULE_vkError(
    vkCreateSemaphore(
      util::ctx().device.logical, &semaphoreCreateInfo, nullptr, &semaphore
    )
  );
  { // name the semaphore
    VkDebugUtilsObjectNameInfoEXT nameInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .pNext = nullptr,
      .objectType = VK_OBJECT_TYPE_SEMAPHORE,
      .objectHandle = reinterpret_cast<uint64_t>(semaphore),
      .pObjectName = label.contents
    };
    vkSetDebugUtilsObjectNameEXT(util::ctx().device.logical, &nameInfo);
  }
  return { .id = reinterpret_cast<uint64_t>(semaphore) };
}

void puleGpuSemaphoreDestroy(PuleGpuSemaphore const semaphore) {
  if (semaphore.id == 0) { return; }
  auto const handle = reinterpret_cast<VkSemaphore>(semaphore.id);
  vkDestroySemaphore(util::ctx().device.logical, handle, nullptr);
}

} // extern c


extern "C" {

PuleGpuFence puleGpuFenceCreate(PuleStringView const label) {
  VkFence fence = VK_NULL_HANDLE;
  VkFenceCreateInfo fenceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
  };
  vkCreateFence(util::ctx().device.logical, &fenceCreateInfo, nullptr, &fence);
  { // name the semaphore
    VkDebugUtilsObjectNameInfoEXT nameInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
      .pNext = nullptr,
      .objectType = VK_OBJECT_TYPE_FENCE,
      .objectHandle = reinterpret_cast<uint64_t>(fence),
      .pObjectName = label.contents
    };
    vkSetDebugUtilsObjectNameEXT(util::ctx().device.logical, &nameInfo);
    fenceDebugNames.emplace(reinterpret_cast<uint64_t>(fence), label.contents);
  }
  return { .id = reinterpret_cast<uint64_t>(fence) };
}
void puleGpuFenceDestroy(PuleGpuFence const fence) {
  fenceDebugNames.erase(fence.id);
  vkDestroyFence(
    util::ctx().device.logical,
    reinterpret_cast<VkFence>(fence.id),
    nullptr
  );
}

bool puleGpuFenceWaitSignal(
  PuleGpuFence const fence,
  PuleNanosecond const timeout
) {
  auto result = (
    vkWaitForFences(
      util::ctx().device.logical,
      1, reinterpret_cast<VkFence const *>(&fence.id),
      VK_TRUE, timeout.valueNano
    )
  );
  switch (result) {
    case VK_SUCCESS: return true;
    case VK_TIMEOUT:
      if (timeout.valueNano == PuleGpuSignalTime_forever) {
        puleLogError(
          "waited forever on fence '%s'",
          fenceDebugNames.at(fence.id).c_str()
        );
        //PULE_assert(false);
      }
      return false;
    default: PULE_assert(false);
  }
}

void puleGpuFenceReset(PuleGpuFence const fence) {
  vkResetFences(
    util::ctx().device.logical,
    1, reinterpret_cast<VkFence const *>(&fence.id)
  );
}

}
