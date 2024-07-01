/* auto generated file gpu */
#pragma once
#include "core.hpp"

#include "gpu.h"
#include "platform.hpp"
#include "error.hpp"
#include "core.hpp"
#include "math.hpp"
#include "array.hpp"
#include "time.hpp"

namespace pule {
struct GpuCommandList {
  PuleGpuCommandList _handle;
  inline operator PuleGpuCommandList() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuCommandListDestroy(this->_handle);
  }
  inline PuleStringView name() {
    return puleGpuCommandListName(this->_handle);
  }
  inline PuleGpuCommandListRecorder recorder() {
    return puleGpuCommandListRecorder(this->_handle);
  }
  inline void dump() {
    return puleGpuCommandListDump(this->_handle);
  }
  static inline GpuCommandList create(PuleAllocator allocator, PuleStringView label) {
    return { ._handle = puleGpuCommandListCreate(allocator, label),};
  }
  static inline GpuCommandList chainCurrent(PuleGpuCommandListChain commandListChain) {
    return { ._handle = puleGpuCommandListChainCurrent(commandListChain),};
  }
};
}
  inline void destroy(pule::GpuCommandList self) {
    return puleGpuCommandListDestroy(self._handle);
  }
  inline PuleStringView name(pule::GpuCommandList self) {
    return puleGpuCommandListName(self._handle);
  }
  inline PuleGpuCommandListRecorder recorder(pule::GpuCommandList self) {
    return puleGpuCommandListRecorder(self._handle);
  }
  inline void dump(pule::GpuCommandList self) {
    return puleGpuCommandListDump(self._handle);
  }
namespace pule {
struct GpuCommandListRecorder {
  PuleGpuCommandListRecorder _handle;
  inline operator PuleGpuCommandListRecorder() const {
    return _handle;
  }
  inline void finish() {
    return puleGpuCommandListRecorderFinish(this->_handle);
  }
};
}
  inline void finish(pule::GpuCommandListRecorder self) {
    return puleGpuCommandListRecorderFinish(self._handle);
  }
namespace pule {
struct GpuCommandListChain {
  PuleGpuCommandListChain _handle;
  inline operator PuleGpuCommandListChain() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuCommandListChainDestroy(this->_handle);
  }
  inline PuleGpuCommandList current() {
    return puleGpuCommandListChainCurrent(this->_handle);
  }
  inline PuleGpuFence currentFence() {
    return puleGpuCommandListChainCurrentFence(this->_handle);
  }
  static inline GpuCommandListChain create(PuleAllocator allocator, PuleStringView label) {
    return { ._handle = puleGpuCommandListChainCreate(allocator, label),};
  }
};
}
  inline void destroy(pule::GpuCommandListChain self) {
    return puleGpuCommandListChainDestroy(self._handle);
  }
  inline PuleGpuCommandList current(pule::GpuCommandListChain self) {
    return puleGpuCommandListChainCurrent(self._handle);
  }
  inline PuleGpuFence currentFence(pule::GpuCommandListChain self) {
    return puleGpuCommandListChainCurrentFence(self._handle);
  }
namespace pule {
struct GpuBuffer {
  PuleGpuBuffer _handle;
  inline operator PuleGpuBuffer() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuBufferDestroy(this->_handle);
  }
  inline void unmap() {
    return puleGpuBufferUnmap(this->_handle);
  }
  static inline GpuBuffer create(PuleStringView name, size_t byteLength, PuleGpuBufferUsage usage, PuleGpuBufferVisibilityFlag visibility) {
    return { ._handle = puleGpuBufferCreate(name, byteLength, usage, visibility),};
  }
};
}
  inline void destroy(pule::GpuBuffer self) {
    return puleGpuBufferDestroy(self._handle);
  }
  inline void unmap(pule::GpuBuffer self) {
    return puleGpuBufferUnmap(self._handle);
  }
namespace pule {
struct GpuSampler {
  PuleGpuSampler _handle;
  inline operator PuleGpuSampler() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuSamplerDestroy(this->_handle);
  }
  static inline GpuSampler create(PuleGpuSamplerCreateInfo createInfo) {
    return { ._handle = puleGpuSamplerCreate(createInfo),};
  }
};
}
  inline void destroy(pule::GpuSampler self) {
    return puleGpuSamplerDestroy(self._handle);
  }
namespace pule {
struct GpuImage {
  PuleGpuImage _handle;
  inline operator PuleGpuImage() const {
    return _handle;
  }
  inline PuleStringView label() {
    return puleGpuImageLabel(this->_handle);
  }
  inline void destroy() {
    return puleGpuImageDestroy(this->_handle);
  }
  inline PuleGpuImageReference reference_createImage() {
    return puleGpuImageReference_createImage(this->_handle);
  }
  inline void reference_updateImage(PuleGpuImage newImage) {
    return puleGpuImageReference_updateImage(this->_handle, newImage);
  }
  static inline GpuImage create(PuleGpuImageCreateInfo imageCreateInfo) {
    return { ._handle = puleGpuImageCreate(imageCreateInfo),};
  }
  static inline GpuImage chain_current(PuleGpuImageChain imageChain) {
    return { ._handle = puleGpuImageChain_current(imageChain),};
  }
  static inline GpuImage reference_image(PuleGpuImageReference reference) {
    return { ._handle = puleGpuImageReference_image(reference),};
  }
};
}
  inline PuleStringView label(pule::GpuImage self) {
    return puleGpuImageLabel(self._handle);
  }
  inline void destroy(pule::GpuImage self) {
    return puleGpuImageDestroy(self._handle);
  }
  inline PuleGpuImageReference reference_createImage(pule::GpuImage self) {
    return puleGpuImageReference_createImage(self._handle);
  }
  inline void reference_updateImage(pule::GpuImage self, PuleGpuImage newImage) {
    return puleGpuImageReference_updateImage(self._handle, newImage);
  }
namespace pule {
using GpuRenderStorage = PuleGpuRenderStorage;
}
namespace pule {
struct GpuPipeline {
  PuleGpuPipeline _handle;
  inline operator PuleGpuPipeline() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuPipelineDestroy(this->_handle);
  }
  static inline GpuPipeline create(PuleGpuPipelineCreateInfo createInfo, PuleError * error) {
    return { ._handle = puleGpuPipelineCreate(createInfo, error),};
  }
};
}
  inline void destroy(pule::GpuPipeline self) {
    return puleGpuPipelineDestroy(self._handle);
  }
namespace pule {
struct GpuImageChain {
  PuleGpuImageChain _handle;
  inline operator PuleGpuImageChain() const {
    return _handle;
  }
  inline void _destroy() {
    return puleGpuImageChain_destroy(this->_handle);
  }
  inline PuleGpuImage _current() {
    return puleGpuImageChain_current(this->_handle);
  }
  static inline GpuImageChain _create(PuleAllocator allocator, PuleStringView label, PuleGpuImageCreateInfo createInfo) {
    return { ._handle = puleGpuImageChain_create(allocator, label, createInfo),};
  }
};
}
  inline void _destroy(pule::GpuImageChain self) {
    return puleGpuImageChain_destroy(self._handle);
  }
  inline PuleGpuImage _current(pule::GpuImageChain self) {
    return puleGpuImageChain_current(self._handle);
  }
namespace pule {
struct GpuImageReference {
  PuleGpuImageReference _handle;
  inline operator PuleGpuImageReference() const {
    return _handle;
  }
  inline void _destroy() {
    return puleGpuImageReference_destroy(this->_handle);
  }
  inline PuleGpuImage _image() {
    return puleGpuImageReference_image(this->_handle);
  }
  static inline GpuImageReference _createImageChain(PuleGpuImageChain imageChain) {
    return { ._handle = puleGpuImageReference_createImageChain(imageChain),};
  }
  static inline GpuImageReference _createImage(PuleGpuImage image) {
    return { ._handle = puleGpuImageReference_createImage(image),};
  }
};
}
  inline void _destroy(pule::GpuImageReference self) {
    return puleGpuImageReference_destroy(self._handle);
  }
  inline PuleGpuImage _image(pule::GpuImageReference self) {
    return puleGpuImageReference_image(self._handle);
  }
namespace pule {
struct GpuShaderModule {
  PuleGpuShaderModule _handle;
  inline operator PuleGpuShaderModule() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuShaderModuleDestroy(this->_handle);
  }
  static inline GpuShaderModule create(PuleBufferView vertexShaderBytecode, PuleBufferView fragmentShaderBytecode, PuleError * error) {
    return { ._handle = puleGpuShaderModuleCreate(vertexShaderBytecode, fragmentShaderBytecode, error),};
  }
};
}
  inline void destroy(pule::GpuShaderModule self) {
    return puleGpuShaderModuleDestroy(self._handle);
  }
namespace pule {
struct GpuSemaphore {
  PuleGpuSemaphore _handle;
  inline operator PuleGpuSemaphore() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuSemaphoreDestroy(this->_handle);
  }
  static inline GpuSemaphore create(PuleStringView label) {
    return { ._handle = puleGpuSemaphoreCreate(label),};
  }
};
}
  inline void destroy(pule::GpuSemaphore self) {
    return puleGpuSemaphoreDestroy(self._handle);
  }
namespace pule {
struct GpuFence {
  PuleGpuFence _handle;
  inline operator PuleGpuFence() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuFenceDestroy(this->_handle);
  }
  inline bool waitSignal(PuleNanosecond timeout) {
    return puleGpuFenceWaitSignal(this->_handle, timeout);
  }
  inline void reset() {
    return puleGpuFenceReset(this->_handle);
  }
  static inline GpuFence create(PuleStringView label) {
    return { ._handle = puleGpuFenceCreate(label),};
  }
};
}
  inline void destroy(pule::GpuFence self) {
    return puleGpuFenceDestroy(self._handle);
  }
  inline bool waitSignal(pule::GpuFence self, PuleNanosecond timeout) {
    return puleGpuFenceWaitSignal(self._handle, timeout);
  }
  inline void reset(pule::GpuFence self) {
    return puleGpuFenceReset(self._handle);
  }
namespace pule {
using GpuCommandListSubmitInfo = PuleGpuCommandListSubmitInfo;
}
namespace pule {
using GpuImageView = PuleGpuImageView;
}
namespace pule {
using GpuResourceBarrierImage = PuleGpuResourceBarrierImage;
}
namespace pule {
using GpuResourceBarrierBuffer = PuleGpuResourceBarrierBuffer;
}
namespace pule {
using GpuConstantValue = PuleGpuConstantValue;
}
namespace pule {
using GpuConstant = PuleGpuConstant;
}
namespace pule {
using GpuDrawIndirectArrays = PuleGpuDrawIndirectArrays;
}
namespace pule {
using GpuBufferMapRange = PuleGpuBufferMapRange;
}
namespace pule {
using GpuBufferMappedFlushRange = PuleGpuBufferMappedFlushRange;
}
namespace pule {
using GpuSamplerCreateInfo = PuleGpuSamplerCreateInfo;
}
namespace pule {
using GpuImageCreateInfo = PuleGpuImageCreateInfo;
}
namespace pule {
struct GpuFramebuffer {
  PuleGpuFramebuffer _handle;
  inline operator PuleGpuFramebuffer() const {
    return _handle;
  }
  inline void destroy() {
    return puleGpuFramebufferDestroy(this->_handle);
  }
  inline PuleGpuFramebufferAttachments attachments() {
    return puleGpuFramebufferAttachments(this->_handle);
  }
  static inline GpuFramebuffer create(PuleGpuFramebufferCreateInfo framebufferCreateInfo, PuleError * error) {
    return { ._handle = puleGpuFramebufferCreate(framebufferCreateInfo, error),};
  }
};
}
  inline void destroy(pule::GpuFramebuffer self) {
    return puleGpuFramebufferDestroy(self._handle);
  }
  inline PuleGpuFramebufferAttachments attachments(pule::GpuFramebuffer self) {
    return puleGpuFramebufferAttachments(self._handle);
  }
namespace pule {
using GpuFramebufferImageAttachment = PuleGpuFramebufferImageAttachment;
}
namespace pule {
using GpuFramebufferAttachments = PuleGpuFramebufferAttachments;
}
namespace pule {
using GpuFramebufferCreateInfo = PuleGpuFramebufferCreateInfo;
}
namespace pule {
using GpuImageAttachmentClear = PuleGpuImageAttachmentClear;
}
namespace pule {
using GpuImageAttachment = PuleGpuImageAttachment;
}
namespace pule {
using GpuPipelineAttributeDescriptorBinding = PuleGpuPipelineAttributeDescriptorBinding;
}
namespace pule {
using GpuPipelineAttributeBufferDescriptorBinding = PuleGpuPipelineAttributeBufferDescriptorBinding;
}
namespace pule {
using GpuPipelineLayoutDescriptorSet = PuleGpuPipelineLayoutDescriptorSet;
}
namespace pule {
using GpuPipelineLayoutPushConstant = PuleGpuPipelineLayoutPushConstant;
}
namespace pule {
using GpuPipelineConfig = PuleGpuPipelineConfig;
}
namespace pule {
using GpuPipelineCreateInfo = PuleGpuPipelineCreateInfo;
}
namespace pule {
using GpuActionResourceBarrier = PuleGpuActionResourceBarrier;
}
namespace pule {
using GpuActionRenderPassBegin = PuleGpuActionRenderPassBegin;
}
namespace pule {
using GpuActionRenderPassEnd = PuleGpuActionRenderPassEnd;
}
namespace pule {
using GpuActionBindElementBuffer = PuleGpuActionBindElementBuffer;
}
namespace pule {
using GpuActionBindAttributeBuffer = PuleGpuActionBindAttributeBuffer;
}
namespace pule {
using GpuActionBindFramebuffer = PuleGpuActionBindFramebuffer;
}
namespace pule {
using GpuActionDispatchRender = PuleGpuActionDispatchRender;
}
namespace pule {
using GpuActionDispatchRenderIndirect = PuleGpuActionDispatchRenderIndirect;
}
namespace pule {
using GpuActionDispatchRenderElements = PuleGpuActionDispatchRenderElements;
}
namespace pule {
using GpuActionClearImageColor = PuleGpuActionClearImageColor;
}
namespace pule {
using GpuActionBindPipeline = PuleGpuActionBindPipeline;
}
namespace pule {
using GpuActionBindBuffer = PuleGpuActionBindBuffer;
}
namespace pule {
using GpuActionBindTexture = PuleGpuActionBindTexture;
}
namespace pule {
using GpuActionClearImageDepth = PuleGpuActionClearImageDepth;
}
namespace pule {
using GpuActionPushConstants = PuleGpuActionPushConstants;
}
namespace pule {
using GpuActionDispatchCommandList = PuleGpuActionDispatchCommandList;
}
namespace pule {
using GpuActionSetScissor = PuleGpuActionSetScissor;
}
namespace pule {
using GpuActionCopyImageToImage = PuleGpuActionCopyImageToImage;
}
namespace pule {
using GpuCommand = PuleGpuCommand;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleGpuElementType const e) {
  switch (e) {
    case PuleGpuElementType_u8: return puleString("u8");
    case PuleGpuElementType_u16: return puleString("u16");
    case PuleGpuElementType_u32: return puleString("u32");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuAction const e) {
  switch (e) {
    case PuleGpuAction_bindPipeline: return puleString("bindPipeline");
    case PuleGpuAction_bindBuffer: return puleString("bindBuffer");
    case PuleGpuAction_bindTexture: return puleString("bindTexture");
    case PuleGpuAction_resourceBarrier: return puleString("resourceBarrier");
    case PuleGpuAction_renderPassBegin: return puleString("renderPassBegin");
    case PuleGpuAction_renderPassEnd: return puleString("renderPassEnd");
    case PuleGpuAction_bindElementBuffer: return puleString("bindElementBuffer");
    case PuleGpuAction_bindAttributeBuffer: return puleString("bindAttributeBuffer");
    case PuleGpuAction_bindFramebuffer: return puleString("bindFramebuffer");
    case PuleGpuAction_clearImageColor: return puleString("clearImageColor");
    case PuleGpuAction_clearImageDepth: return puleString("clearImageDepth");
    case PuleGpuAction_dispatchRender: return puleString("dispatchRender");
    case PuleGpuAction_dispatchRenderElements: return puleString("dispatchRenderElements");
    case PuleGpuAction_dispatchRenderIndirect: return puleString("dispatchRenderIndirect");
    case PuleGpuAction_pushConstants: return puleString("pushConstants");
    case PuleGpuAction_dispatchCommandList: return puleString("dispatchCommandList");
    case PuleGpuAction_setScissor: return puleString("setScissor");
    case PuleGpuAction_copyImageToImage: return puleString("copyImageToImage");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuResourceBarrierStage const e) {
  std::string str = "( ";
  if (e & PuleGpuResourceBarrierStage_top) {
    str += "top | ";
  }
  if (e & PuleGpuResourceBarrierStage_drawIndirect) {
    str += "drawIndirect | ";
  }
  if (e & PuleGpuResourceBarrierStage_vertexInput) {
    str += "vertexInput | ";
  }
  if (e & PuleGpuResourceBarrierStage_shaderFragment) {
    str += "shaderFragment | ";
  }
  if (e & PuleGpuResourceBarrierStage_shaderVertex) {
    str += "shaderVertex | ";
  }
  if (e & PuleGpuResourceBarrierStage_shaderCompute) {
    str += "shaderCompute | ";
  }
  if (e & PuleGpuResourceBarrierStage_outputAttachmentColor) {
    str += "outputAttachmentColor | ";
  }
  if (e & PuleGpuResourceBarrierStage_outputAttachmentDepth) {
    str += "outputAttachmentDepth | ";
  }
  if (e & PuleGpuResourceBarrierStage_transfer) {
    str += "transfer | ";
  }
  if (e & PuleGpuResourceBarrierStage_bottom) {
    str += "bottom | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuResourceAccess const e) {
  std::string str = "( ";
  if (e & PuleGpuResourceAccess_none) {
    str += "none | ";
  }
  if (e & PuleGpuResourceAccess_indirectCommandRead) {
    str += "indirectCommandRead | ";
  }
  if (e & PuleGpuResourceAccess_indexRead) {
    str += "indexRead | ";
  }
  if (e & PuleGpuResourceAccess_vertexAttributeRead) {
    str += "vertexAttributeRead | ";
  }
  if (e & PuleGpuResourceAccess_uniformRead) {
    str += "uniformRead | ";
  }
  if (e & PuleGpuResourceAccess_inputAttachmentRead) {
    str += "inputAttachmentRead | ";
  }
  if (e & PuleGpuResourceAccess_shaderRead) {
    str += "shaderRead | ";
  }
  if (e & PuleGpuResourceAccess_shaderWrite) {
    str += "shaderWrite | ";
  }
  if (e & PuleGpuResourceAccess_attachmentColorRead) {
    str += "attachmentColorRead | ";
  }
  if (e & PuleGpuResourceAccess_attachmentColorWrite) {
    str += "attachmentColorWrite | ";
  }
  if (e & PuleGpuResourceAccess_attachmentDepthRead) {
    str += "attachmentDepthRead | ";
  }
  if (e & PuleGpuResourceAccess_attachmentDepthWrite) {
    str += "attachmentDepthWrite | ";
  }
  if (e & PuleGpuResourceAccess_transferRead) {
    str += "transferRead | ";
  }
  if (e & PuleGpuResourceAccess_transferWrite) {
    str += "transferWrite | ";
  }
  if (e & PuleGpuResourceAccess_hostRead) {
    str += "hostRead | ";
  }
  if (e & PuleGpuResourceAccess_hostWrite) {
    str += "hostWrite | ";
  }
  if (e & PuleGpuResourceAccess_memoryRead) {
    str += "memoryRead | ";
  }
  if (e & PuleGpuResourceAccess_memoryWrite) {
    str += "memoryWrite | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuConstantTypeTag const e) {
  switch (e) {
    case PuleGpuConstantTypeTag_f32: return puleString("f32");
    case PuleGpuConstantTypeTag_f32v2: return puleString("f32v2");
    case PuleGpuConstantTypeTag_f32v3: return puleString("f32v3");
    case PuleGpuConstantTypeTag_f32v4: return puleString("f32v4");
    case PuleGpuConstantTypeTag_i32: return puleString("i32");
    case PuleGpuConstantTypeTag_i32v2: return puleString("i32v2");
    case PuleGpuConstantTypeTag_i32v3: return puleString("i32v3");
    case PuleGpuConstantTypeTag_i32v4: return puleString("i32v4");
    case PuleGpuConstantTypeTag_f32m44: return puleString("f32m44");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleErrorGfx const e) {
  switch (e) {
    case PuleErrorGfx_none: return puleString("none");
    case PuleErrorGfx_creationFailed: return puleString("creationFailed");
    case PuleErrorGfx_shaderModuleCompilationFailed: return puleString("shaderModuleCompilationFailed");
    case PuleErrorGfx_invalidDescriptorSet: return puleString("invalidDescriptorSet");
    case PuleErrorGfx_invalidCommandList: return puleString("invalidCommandList");
    case PuleErrorGfx_invalidFramebuffer: return puleString("invalidFramebuffer");
    case PuleErrorGfx_submissionFenceWaitFailed: return puleString("submissionFenceWaitFailed");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuBufferUsage const e) {
  std::string str = "( ";
  if (e & PuleGpuBufferUsage_attribute) {
    str += "attribute | ";
  }
  if (e & PuleGpuBufferUsage_element) {
    str += "element | ";
  }
  if (e & PuleGpuBufferUsage_uniform) {
    str += "uniform | ";
  }
  if (e & PuleGpuBufferUsage_storage) {
    str += "storage | ";
  }
  if (e & PuleGpuBufferUsage_accelerationStructure) {
    str += "accelerationStructure | ";
  }
  if (e & PuleGpuBufferUsage_indirect) {
    str += "indirect | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuBufferBindingDescriptor const e) {
  switch (e) {
    case PuleGpuBufferBindingDescriptor_uniform: return puleString("uniform");
    case PuleGpuBufferBindingDescriptor_storage: return puleString("storage");
    case PuleGpuBufferBindingDescriptor_accelerationStructure: return puleString("accelerationStructure");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuBufferVisibilityFlag const e) {
  std::string str = "( ";
  if (e & PuleGpuBufferVisibilityFlag_deviceOnly) {
    str += "deviceOnly | ";
  }
  if (e & PuleGpuBufferVisibilityFlag_hostVisible) {
    str += "hostVisible | ";
  }
  if (e & PuleGpuBufferVisibilityFlag_hostWritable) {
    str += "hostWritable | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuBufferMapAccess const e) {
  std::string str = "( ";
  if (e & PuleGpuBufferMapAccess_hostVisible) {
    str += "hostVisible | ";
  }
  if (e & PuleGpuBufferMapAccess_hostWritable) {
    str += "hostWritable | ";
  }
  if (e & PuleGpuBufferMapAccess_invalidate) {
    str += "invalidate | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuImageMagnification const e) {
  switch (e) {
    case PuleGpuImageMagnification_nearest: return puleString("nearest");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuImageWrap const e) {
  switch (e) {
    case PuleGpuImageWrap_clampToEdge: return puleString("clampToEdge");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuImageByteFormat const e) {
  switch (e) {
    case PuleGpuImageByteFormat_undefined: return puleString("undefined");
    case PuleGpuImageByteFormat_bgra8U: return puleString("bgra8U");
    case PuleGpuImageByteFormat_rgba8U: return puleString("rgba8U");
    case PuleGpuImageByteFormat_rgb8U: return puleString("rgb8U");
    case PuleGpuImageByteFormat_r8U: return puleString("r8U");
    case PuleGpuImageByteFormat_depth16: return puleString("depth16");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuImageTarget const e) {
  switch (e) {
    case PuleGpuImageTarget_i2D: return puleString("i2D");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuFramebufferAttachment const e) {
  switch (e) {
    case PuleGpuFramebufferAttachment_color0: return puleString("color0");
    case PuleGpuFramebufferAttachment_color1: return puleString("color1");
    case PuleGpuFramebufferAttachment_color3: return puleString("color3");
    case PuleGpuFramebufferAttachment_color4: return puleString("color4");
    case PuleGpuFramebufferAttachment_depth: return puleString("depth");
    case PuleGpuFramebufferAttachment_stencil: return puleString("stencil");
    case PuleGpuFramebufferAttachment_depthStencil: return puleString("depthStencil");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuFramebufferType const e) {
  switch (e) {
    case PuleGpuFramebufferType_renderStorage: return puleString("renderStorage");
    case PuleGpuFramebufferType_imageStorage: return puleString("imageStorage");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuImageAttachmentOpLoad const e) {
  switch (e) {
    case PuleGpuImageAttachmentOpLoad_load: return puleString("load");
    case PuleGpuImageAttachmentOpLoad_clear: return puleString("clear");
    case PuleGpuImageAttachmentOpLoad_dontCare: return puleString("dontCare");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuImageAttachmentOpStore const e) {
  switch (e) {
    case PuleGpuImageAttachmentOpStore_store: return puleString("store");
    case PuleGpuImageAttachmentOpStore_dontCare: return puleString("dontCare");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuImageLayout const e) {
  switch (e) {
    case PuleGpuImageLayout_uninitialized: return puleString("uninitialized");
    case PuleGpuImageLayout_storage: return puleString("storage");
    case PuleGpuImageLayout_attachmentColor: return puleString("attachmentColor");
    case PuleGpuImageLayout_attachmentDepth: return puleString("attachmentDepth");
    case PuleGpuImageLayout_transferSrc: return puleString("transferSrc");
    case PuleGpuImageLayout_transferDst: return puleString("transferDst");
    case PuleGpuImageLayout_presentSrc: return puleString("presentSrc");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuPipelineStage const e) {
  std::string str = "( ";
  if (e & PuleGpuPipelineStage_top) {
    str += "top | ";
  }
  if (e & PuleGpuPipelineStage_drawIndirect) {
    str += "drawIndirect | ";
  }
  if (e & PuleGpuPipelineStage_vertexInput) {
    str += "vertexInput | ";
  }
  if (e & PuleGpuPipelineStage_vertexShader) {
    str += "vertexShader | ";
  }
  if (e & PuleGpuPipelineStage_fragmentShader) {
    str += "fragmentShader | ";
  }
  if (e & PuleGpuPipelineStage_colorAttachmentOutput) {
    str += "colorAttachmentOutput | ";
  }
  if (e & PuleGpuPipelineStage_computeShader) {
    str += "computeShader | ";
  }
  if (e & PuleGpuPipelineStage_transfer) {
    str += "transfer | ";
  }
  if (e & PuleGpuPipelineStage_bottom) {
    str += "bottom | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuDescriptorType const e) {
  switch (e) {
    case PuleGpuDescriptorType_sampler: return puleString("sampler");
    case PuleGpuDescriptorType_uniformBuffer: return puleString("uniformBuffer");
    case PuleGpuDescriptorType_storageBuffer: return puleString("storageBuffer");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuAttributeDataType const e) {
  switch (e) {
    case PuleGpuAttributeDataType_f32: return puleString("f32");
    case PuleGpuAttributeDataType_u8: return puleString("u8");
    case PuleGpuAttributeDataType_u16: return puleString("u16");
    case PuleGpuAttributeDataType_u32: return puleString("u32");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuPipelineDescriptorMax const e) {
  switch (e) {
    case PuleGpuPipelineDescriptorMax_uniform: return puleString("uniform");
    case PuleGpuPipelineDescriptorMax_texture: return puleString("texture");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuDescriptorStage const e) {
  std::string str = "( ";
  if (e & PuleGpuDescriptorStage_unused) {
    str += "unused | ";
  }
  if (e & PuleGpuDescriptorStage_vertex) {
    str += "vertex | ";
  }
  if (e & PuleGpuDescriptorStage_fragment) {
    str += "fragment | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuDrawPrimitive const e) {
  switch (e) {
    case PuleGpuDrawPrimitive_triangle: return puleString("triangle");
    case PuleGpuDrawPrimitive_triangleStrip: return puleString("triangleStrip");
    case PuleGpuDrawPrimitive_point: return puleString("point");
    case PuleGpuDrawPrimitive_line: return puleString("line");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuFenceConditionFlag const e) {
  switch (e) {
    case PuleGpuFenceConditionFlag_all: return puleString("all");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuSignalTime const e) {
  switch (e) {
    case PuleGpuSignalTime_forever: return puleString("forever");
    default: return puleString("N/A");
  }
}
}
