/* auto generated file gpu */
#pragma once
#include "core.h"

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
  static inline GpuBuffer create(PuleStringView name, void const * optionalInitialData, size_t byteLength, PuleGpuBufferUsage usage, PuleGpuBufferVisibilityFlag visibility) {
    return { ._handle = puleGpuBufferCreate(name, optionalInitialData, byteLength, usage, visibility),};
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
  static inline GpuPipeline create(PuleGpuPipelineCreateInfo createInfo, struct PuleError * error) {
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
  static inline GpuShaderModule create(PuleBufferView vertexShaderBytecode, PuleBufferView fragmentShaderBytecode, struct PuleError * error) {
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
  static inline GpuFramebuffer create(PuleGpuFramebufferCreateInfo framebufferCreateInfo, struct PuleError * error) {
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
namespace pule {
inline char const * toStr(PuleGpuElementType const e) {
  switch (e) {
    case PuleGpuElementType_u8: return "u8";
    case PuleGpuElementType_u16: return "u16";
    case PuleGpuElementType_u32: return "u32";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuAction const e) {
  switch (e) {
    case PuleGpuAction_bindPipeline: return "bindPipeline";
    case PuleGpuAction_bindBuffer: return "bindBuffer";
    case PuleGpuAction_bindTexture: return "bindTexture";
    case PuleGpuAction_resourceBarrier: return "resourceBarrier";
    case PuleGpuAction_renderPassBegin: return "renderPassBegin";
    case PuleGpuAction_renderPassEnd: return "renderPassEnd";
    case PuleGpuAction_bindElementBuffer: return "bindElementBuffer";
    case PuleGpuAction_bindAttributeBuffer: return "bindAttributeBuffer";
    case PuleGpuAction_bindFramebuffer: return "bindFramebuffer";
    case PuleGpuAction_clearImageColor: return "clearImageColor";
    case PuleGpuAction_clearImageDepth: return "clearImageDepth";
    case PuleGpuAction_dispatchRender: return "dispatchRender";
    case PuleGpuAction_dispatchRenderElements: return "dispatchRenderElements";
    case PuleGpuAction_dispatchRenderIndirect: return "dispatchRenderIndirect";
    case PuleGpuAction_pushConstants: return "pushConstants";
    case PuleGpuAction_dispatchCommandList: return "dispatchCommandList";
    case PuleGpuAction_setScissor: return "setScissor";
    case PuleGpuAction_copyImageToImage: return "copyImageToImage";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuResourceBarrierStage const e) {
  switch (e) {
    case PuleGpuResourceBarrierStage_top: return "top";
    case PuleGpuResourceBarrierStage_drawIndirect: return "drawIndirect";
    case PuleGpuResourceBarrierStage_vertexInput: return "vertexInput";
    case PuleGpuResourceBarrierStage_shaderFragment: return "shaderFragment";
    case PuleGpuResourceBarrierStage_shaderVertex: return "shaderVertex";
    case PuleGpuResourceBarrierStage_shaderCompute: return "shaderCompute";
    case PuleGpuResourceBarrierStage_outputAttachmentColor: return "outputAttachmentColor";
    case PuleGpuResourceBarrierStage_outputAttachmentDepth: return "outputAttachmentDepth";
    case PuleGpuResourceBarrierStage_transfer: return "transfer";
    case PuleGpuResourceBarrierStage_bottom: return "bottom";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuResourceAccess const e) {
  switch (e) {
    case PuleGpuResourceAccess_none: return "none";
    case PuleGpuResourceAccess_indirectCommandRead: return "indirectCommandRead";
    case PuleGpuResourceAccess_indexRead: return "indexRead";
    case PuleGpuResourceAccess_vertexAttributeRead: return "vertexAttributeRead";
    case PuleGpuResourceAccess_uniformRead: return "uniformRead";
    case PuleGpuResourceAccess_inputAttachmentRead: return "inputAttachmentRead";
    case PuleGpuResourceAccess_shaderRead: return "shaderRead";
    case PuleGpuResourceAccess_shaderWrite: return "shaderWrite";
    case PuleGpuResourceAccess_attachmentColorRead: return "attachmentColorRead";
    case PuleGpuResourceAccess_attachmentColorWrite: return "attachmentColorWrite";
    case PuleGpuResourceAccess_attachmentDepthRead: return "attachmentDepthRead";
    case PuleGpuResourceAccess_attachmentDepthWrite: return "attachmentDepthWrite";
    case PuleGpuResourceAccess_transferRead: return "transferRead";
    case PuleGpuResourceAccess_transferWrite: return "transferWrite";
    case PuleGpuResourceAccess_hostRead: return "hostRead";
    case PuleGpuResourceAccess_hostWrite: return "hostWrite";
    case PuleGpuResourceAccess_memoryRead: return "memoryRead";
    case PuleGpuResourceAccess_memoryWrite: return "memoryWrite";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuConstantTypeTag const e) {
  switch (e) {
    case PuleGpuConstantTypeTag_f32: return "f32";
    case PuleGpuConstantTypeTag_f32v2: return "f32v2";
    case PuleGpuConstantTypeTag_f32v3: return "f32v3";
    case PuleGpuConstantTypeTag_f32v4: return "f32v4";
    case PuleGpuConstantTypeTag_i32: return "i32";
    case PuleGpuConstantTypeTag_i32v2: return "i32v2";
    case PuleGpuConstantTypeTag_i32v3: return "i32v3";
    case PuleGpuConstantTypeTag_i32v4: return "i32v4";
    case PuleGpuConstantTypeTag_f32m44: return "f32m44";
    default: return "N/A";
  }
}
inline char const * toStr(PuleErrorGfx const e) {
  switch (e) {
    case PuleErrorGfx_none: return "none";
    case PuleErrorGfx_creationFailed: return "creationFailed";
    case PuleErrorGfx_shaderModuleCompilationFailed: return "shaderModuleCompilationFailed";
    case PuleErrorGfx_invalidDescriptorSet: return "invalidDescriptorSet";
    case PuleErrorGfx_invalidCommandList: return "invalidCommandList";
    case PuleErrorGfx_invalidFramebuffer: return "invalidFramebuffer";
    case PuleErrorGfx_submissionFenceWaitFailed: return "submissionFenceWaitFailed";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuBufferUsage const e) {
  switch (e) {
    case PuleGpuBufferUsage_attribute: return "attribute";
    case PuleGpuBufferUsage_element: return "element";
    case PuleGpuBufferUsage_uniform: return "uniform";
    case PuleGpuBufferUsage_storage: return "storage";
    case PuleGpuBufferUsage_accelerationStructure: return "accelerationStructure";
    case PuleGpuBufferUsage_indirect: return "indirect";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuBufferBindingDescriptor const e) {
  switch (e) {
    case PuleGpuBufferBindingDescriptor_uniform: return "uniform";
    case PuleGpuBufferBindingDescriptor_storage: return "storage";
    case PuleGpuBufferBindingDescriptor_accelerationStructure: return "accelerationStructure";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuBufferVisibilityFlag const e) {
  switch (e) {
    case PuleGpuBufferVisibilityFlag_deviceOnly: return "deviceOnly";
    case PuleGpuBufferVisibilityFlag_hostVisible: return "hostVisible";
    case PuleGpuBufferVisibilityFlag_hostWritable: return "hostWritable";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuBufferMapAccess const e) {
  switch (e) {
    case PuleGpuBufferMapAccess_hostVisible: return "hostVisible";
    case PuleGpuBufferMapAccess_hostWritable: return "hostWritable";
    case PuleGpuBufferMapAccess_invalidate: return "invalidate";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuImageMagnification const e) {
  switch (e) {
    case PuleGpuImageMagnification_nearest: return "nearest";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuImageWrap const e) {
  switch (e) {
    case PuleGpuImageWrap_clampToEdge: return "clampToEdge";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuImageByteFormat const e) {
  switch (e) {
    case PuleGpuImageByteFormat_undefined: return "undefined";
    case PuleGpuImageByteFormat_bgra8U: return "bgra8U";
    case PuleGpuImageByteFormat_rgba8U: return "rgba8U";
    case PuleGpuImageByteFormat_rgb8U: return "rgb8U";
    case PuleGpuImageByteFormat_r8U: return "r8U";
    case PuleGpuImageByteFormat_depth16: return "depth16";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuImageTarget const e) {
  switch (e) {
    case PuleGpuImageTarget_i2D: return "i2D";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuFramebufferAttachment const e) {
  switch (e) {
    case PuleGpuFramebufferAttachment_color0: return "color0";
    case PuleGpuFramebufferAttachment_color1: return "color1";
    case PuleGpuFramebufferAttachment_color3: return "color3";
    case PuleGpuFramebufferAttachment_color4: return "color4";
    case PuleGpuFramebufferAttachment_depth: return "depth";
    case PuleGpuFramebufferAttachment_stencil: return "stencil";
    case PuleGpuFramebufferAttachment_depthStencil: return "depthStencil";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuFramebufferType const e) {
  switch (e) {
    case PuleGpuFramebufferType_renderStorage: return "renderStorage";
    case PuleGpuFramebufferType_imageStorage: return "imageStorage";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuImageAttachmentOpLoad const e) {
  switch (e) {
    case PuleGpuImageAttachmentOpLoad_load: return "load";
    case PuleGpuImageAttachmentOpLoad_clear: return "clear";
    case PuleGpuImageAttachmentOpLoad_dontCare: return "dontCare";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuImageAttachmentOpStore const e) {
  switch (e) {
    case PuleGpuImageAttachmentOpStore_store: return "store";
    case PuleGpuImageAttachmentOpStore_dontCare: return "dontCare";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuImageLayout const e) {
  switch (e) {
    case PuleGpuImageLayout_uninitialized: return "uninitialized";
    case PuleGpuImageLayout_storage: return "storage";
    case PuleGpuImageLayout_attachmentColor: return "attachmentColor";
    case PuleGpuImageLayout_attachmentDepth: return "attachmentDepth";
    case PuleGpuImageLayout_transferSrc: return "transferSrc";
    case PuleGpuImageLayout_transferDst: return "transferDst";
    case PuleGpuImageLayout_presentSrc: return "presentSrc";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuPipelineStage const e) {
  switch (e) {
    case PuleGpuPipelineStage_top: return "top";
    case PuleGpuPipelineStage_drawIndirect: return "drawIndirect";
    case PuleGpuPipelineStage_vertexInput: return "vertexInput";
    case PuleGpuPipelineStage_vertexShader: return "vertexShader";
    case PuleGpuPipelineStage_fragmentShader: return "fragmentShader";
    case PuleGpuPipelineStage_colorAttachmentOutput: return "colorAttachmentOutput";
    case PuleGpuPipelineStage_computeShader: return "computeShader";
    case PuleGpuPipelineStage_transfer: return "transfer";
    case PuleGpuPipelineStage_bottom: return "bottom";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuDescriptorType const e) {
  switch (e) {
    case PuleGpuDescriptorType_sampler: return "sampler";
    case PuleGpuDescriptorType_uniformBuffer: return "uniformBuffer";
    case PuleGpuDescriptorType_storageBuffer: return "storageBuffer";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuAttributeDataType const e) {
  switch (e) {
    case PuleGpuAttributeDataType_float: return "float";
    case PuleGpuAttributeDataType_unsignedByte: return "unsignedByte";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuPipelineDescriptorMax const e) {
  switch (e) {
    case PuleGpuPipelineDescriptorMax_uniform: return "uniform";
    case PuleGpuPipelineDescriptorMax_storage: return "storage";
    case PuleGpuPipelineDescriptorMax_attribute: return "attribute";
    case PuleGpuPipelineDescriptorMax_texture: return "texture";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuDescriptorStage const e) {
  switch (e) {
    case PuleGpuDescriptorStage_unused: return "unused";
    case PuleGpuDescriptorStage_vertex: return "vertex";
    case PuleGpuDescriptorStage_fragment: return "fragment";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuDrawPrimitive const e) {
  switch (e) {
    case PuleGpuDrawPrimitive_triangle: return "triangle";
    case PuleGpuDrawPrimitive_triangleStrip: return "triangleStrip";
    case PuleGpuDrawPrimitive_point: return "point";
    case PuleGpuDrawPrimitive_line: return "line";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuFenceConditionFlag const e) {
  switch (e) {
    case PuleGpuFenceConditionFlag_all: return "all";
    default: return "N/A";
  }
}
inline char const * toStr(PuleGpuSignalTime const e) {
  switch (e) {
    case PuleGpuSignalTime_forever: return "forever";
    default: return "N/A";
  }
}
}
