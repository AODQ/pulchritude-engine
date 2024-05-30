#include <pulchritude/gpu.h>

#include <util.hpp>

#include <pulchritude/log.h>

#include <volk.h>

#include <cstdlib>

PuleGpuShaderModule puleGpuShaderModuleCreate(
  PuleBufferView const vertexShaderBytecode,
  PuleBufferView const fragmentShaderBytecode,
  PuleError * const error
) {
  auto vertexShaderModuleCi = VkShaderModuleCreateInfo {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .codeSize = vertexShaderBytecode.byteLength,
    .pCode = reinterpret_cast<uint32_t const *>(vertexShaderBytecode.data),
  };
  auto vertexShaderModule = VkShaderModule {};
  PULE_vkAssert(
    vkCreateShaderModule(
      util::ctx().device.logical,
      &vertexShaderModuleCi, nullptr, &vertexShaderModule
    ),
    PuleErrorGfx_shaderModuleCompilationFailed,
    { .id = 0 }
  );
  auto fragmentShaderModuleCi = VkShaderModuleCreateInfo {
    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .codeSize = fragmentShaderBytecode.byteLength,
    .pCode = reinterpret_cast<uint32_t const *>(fragmentShaderBytecode.data),
  };
  auto fragmentShaderModule = VkShaderModule {};
  PULE_vkAssert(
    vkCreateShaderModule(
      util::ctx().device.logical,
      &fragmentShaderModuleCi, nullptr, &fragmentShaderModule
    ),
    PuleErrorGfx_shaderModuleCompilationFailed,
    { .id = 0 }
  );
  auto const shaderModuleId = reinterpret_cast<uint64_t>(vertexShaderModule);
  util::ctx().shaderModules.emplace(
    shaderModuleId,
    util::ShaderModule {
      .moduleVertex = vertexShaderModule,
      .moduleFragment = fragmentShaderModule,
    }
  );
  return { .id = shaderModuleId, };
}

void puleGpuShaderModuleDestroy(PuleGpuShaderModule const shaderModule) {
  if (shaderModule.id == 0) { return; }
  vkDestroyShaderModule(
    util::ctx().device.logical,
    reinterpret_cast<VkShaderModule>(shaderModule.id),
    nullptr
  );
}
