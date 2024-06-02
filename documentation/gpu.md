# gpu

## structs
### PuleGpuCommandListSubmitInfo
```c
struct {
  commandList : PuleGpuCommandList;
  waitSemaphoreCount : size_t;
  waitSemaphores : PuleGpuSemaphore const ptr;
  waitSemaphoreStages : PuleGpuPipelineStage const ptr;
  /* 
    will create semaphore if id is 0, otherwise will use existing semaphore
   */
  signalSemaphoreCount : size_t;
  signalSemaphores : PuleGpuSemaphore ptr;
  /* 
    at end of command list submission, if this is non-zero, fence will be
    triggered
   */
  fenceTargetFinish : PuleGpuFence;
};
```
### PuleGpuImageView
```c
struct {
  image : PuleGpuImage;
  mipmapLevelStart : size_t;
  mipmapLevelCount : size_t;
  arrayLayerStart : size_t;
  arrayLayerCount : size_t;
  byteFormat : PuleGpuImageByteFormat;
};
```
### PuleGpuResourceBarrierImage
```c
struct {
  image : PuleGpuImage;
  accessSrc : PuleGpuResourceAccess;
  accessDst : PuleGpuResourceAccess;
  layoutSrc : PuleGpuImageLayout;
  layoutDst : PuleGpuImageLayout;
  isDepthStencil : bool;
};
```
### PuleGpuResourceBarrierBuffer
```c
struct {
  buffer : PuleGpuBuffer;
  accessSrc : PuleGpuResourceAccess;
  accessDst : PuleGpuResourceAccess;
  byteOffset : size_t;
  byteLength : size_t;
};
```
### PuleGpuConstantValue
 TODO deprecate this in favor of shader introspection 
```c
union {
  constantF32 : float;
  constantF32v2 : PuleF32v2;
  constantF32v3 : PuleF32v3;
  constantF32v4 : PuleF32v4;
  constantI32 : int32_t;
  constantI32v2 : PuleI32v2;
  constantI32v3 : PuleI32v3;
  constantI32v4 : PuleI32v4;
  constantF32m44 : PuleF32m44;
};
```
### PuleGpuConstant
```c
struct {
  value : PuleGpuConstantValue;
  typeTag : PuleGpuConstantTypeTag;
  bindingSlot : uint32_t;
};
```
### PuleGpuDrawIndirectArrays
```c
struct {
  vertexCount : uint32_t;
  instanceCount : uint32_t;
  vertexOffset : uint32_t;
  instanceOffset : uint32_t;
};
```
### PuleGpuBufferMapRange
```c
struct {
  buffer : PuleGpuBuffer;
  access : PuleGpuBufferMapAccess;
  byteOffset : size_t;
  byteLength : size_t;
};
```
### PuleGpuBufferMappedFlushRange
```c
struct {
  buffer : PuleGpuBuffer;
  byteOffset : size_t;
  byteLength : size_t;
};
```
### PuleGpuSamplerCreateInfo
```c
struct {
  minify : PuleGpuImageMagnification;
  magnify : PuleGpuImageMagnification;
  wrapU : PuleGpuImageWrap;
  wrapV : PuleGpuImageWrap;
  /*  only valid on 3D targets  */
  wrapW : PuleGpuImageWrap;
};
```
### PuleGpuImageCreateInfo
```c
struct {
  width : uint32_t;
  height : uint32_t;
  target : PuleGpuImageTarget;
  byteFormat : PuleGpuImageByteFormat;
  /*  this should technically be changable  */
  sampler : PuleGpuSampler;
  label : PuleStringView;
  optionalInitialData : void const ptr;
};
```
### PuleGpuFramebuffer
```c
struct {
  id : uint64_t;
};
```
### PuleGpuFramebufferImageAttachment
```c
struct {
  image : PuleGpuImage;
  mipmapLevel : uint32_t = 0;
};
```
### PuleGpuFramebufferAttachments
```c
union {
  images : PuleGpuFramebufferImageAttachment [8];
  renderStorages : PuleGpuRenderStorage [8];
};
```
### PuleGpuFramebufferCreateInfo
```c
struct {
  attachment : PuleGpuFramebufferAttachments;
  attachmentType : PuleGpuFramebufferType;
};
```
### PuleGpuImageAttachmentClear
```c
union {
  color : PuleF32v4;
  clearDepth : float;
};
```
### PuleGpuImageAttachment
```c
struct {
  opLoad : PuleGpuImageAttachmentOpLoad;
  opStore : PuleGpuImageAttachmentOpStore;
  layout : PuleGpuImageLayout;
  clear : PuleGpuImageAttachmentClear;
  imageView : PuleGpuImageView;
};
```
### PuleGpuPipelineAttributeDescriptorBinding
```c
struct {
  dataType : PuleGpuAttributeDataType;
  bufferIndex : size_t;
  numComponents : size_t;
  convertFixedDataTypeToNormalizedFloating : bool;
  offsetIntoBuffer : size_t;
};
```
### PuleGpuPipelineAttributeBufferDescriptorBinding
 TODO input rate 
```c
struct {
  /*  must be non-zeo  */
  stridePerElement : size_t;
};
```
### PuleGpuPipelineLayoutDescriptorSet

  here are some known & fixable limitations with the current model:
    - can't reference the same buffer in separate elements
    - can't reference relative offset strides (related to above)
    - of course, maximum 16 attributes (I think this is fine though)

```c
struct {
  /* 
     TODO change 'uniform' to just 'smallStorage' in this context
     !!!!!!!!!!!!!!
     TODO.... DONT BIND BUFFERS HERE!!!!!!!
     Need to just bind what stages they will be used!!!
     !!!!!!!!!!!!!!
   */
  bufferUniformBindings : PuleGpuDescriptorStage [PuleGpuPipelineDescriptorMax_uniform];
  /*  TODO use range  */
  bufferStorageBindings : PuleGpuDescriptorStage [PuleGpuPipelineDescriptorMax_storage];
  attributeBindings : PuleGpuPipelineAttributeDescriptorBinding [PuleGpuPipelineDescriptorMax_attribute];
  attributeBufferBindings : PuleGpuPipelineAttributeBufferDescriptorBinding [PuleGpuPipelineDescriptorMax_attribute];
  textureBindings : PuleGpuDescriptorStage [PuleGpuPipelineDescriptorMax_texture];
};
```
### PuleGpuPipelineLayoutPushConstant
```c
struct {
  stage : PuleGpuDescriptorStage;
  byteLength : size_t;
  byteOffset : size_t;
};
```
### PuleGpuPipelineConfig

  TODO viewport/scissor like this shouldn't be in a viewport
  
```c
struct {
  depthTestEnabled : bool;
  blendEnabled : bool;
  scissorTestEnabled : bool;
  /*  = 0,0  */
  viewportMin : PuleI32v2;
  /*  = 1, 1  */
  viewportMax : PuleI32v2;
  /*  = 0, 0  */
  scissorMin : PuleI32v2;
  /*  = 1, 1  */
  scissorMax : PuleI32v2;
  drawPrimitive : PuleGpuDrawPrimitive;
  colorAttachmentCount : size_t;
  colorAttachmentFormats : PuleGpuImageByteFormat [8];
  depthAttachmentFormat : PuleGpuImageByteFormat;
};
```
### PuleGpuPipelineCreateInfo
```c
struct {
  shaderModule : PuleGpuShaderModule;
  layoutDescriptorSet : PuleGpuPipelineLayoutDescriptorSet;
  layoutPushConstants : PuleGpuPipelineLayoutPushConstant const ptr;
  layoutPushConstantsCount : size_t;
  config : PuleGpuPipelineConfig;
};
```
### PuleGpuActionResourceBarrier
```c
struct {
  action : PuleGpuAction = PuleGpuAction_resourceBarrier;
  stageSrc : PuleGpuResourceBarrierStage;
  stageDst : PuleGpuResourceBarrierStage;
  resourceImageCount : size_t;
  resourceImages : PuleGpuResourceBarrierImage const ptr;
  resourceBufferCount : size_t;
  resourceBuffers : PuleGpuResourceBarrierBuffer const ptr;
};
```
### PuleGpuActionRenderPassBegin
```c
struct {
  action : PuleGpuAction = PuleGpuAction_renderPassBegin;
  viewportMin : PuleI32v2;
  viewportMax : PuleI32v2;
  attachmentColor : PuleGpuImageAttachment [8];
  attachmentColorCount : size_t;
  attachmentDepth : PuleGpuImageAttachment;
};
```
### PuleGpuActionRenderPassEnd
```c
struct {
  action : PuleGpuAction = PuleGpuAction_renderPassEnd;
};
```
### PuleGpuActionBindElementBuffer
```c
struct {
  action : PuleGpuAction = PuleGpuAction_bindElementBuffer;
  buffer : PuleGpuBuffer;
  offset : size_t;
  elementType : PuleGpuElementType;
};
```
### PuleGpuActionBindAttributeBuffer
```c
struct {
  action : PuleGpuAction = PuleGpuAction_bindAttributeBuffer;
  bindingIndex : size_t;
  buffer : PuleGpuBuffer;
  offset : size_t;
  /* 
    this can be 0 to defer to pipeline's default stride at this indx
   */
  stride : size_t;
};
```
### PuleGpuActionBindFramebuffer
```c
struct {
  action : PuleGpuAction = PuleGpuAction_bindFramebuffer;
  framebuffer : PuleGpuFramebuffer;
};
```
### PuleGpuActionDispatchRender
```c
struct {
  action : PuleGpuAction = PuleGpuAction_dispatchRender;
  vertexOffset : size_t;
  numVertices : size_t;
};
```
### PuleGpuActionDispatchRenderIndirect
```c
struct {
  action : PuleGpuAction = PuleGpuAction_dispatchRenderIndirect;
  bufferIndirect : PuleGpuBuffer;
  byteOffset : size_t;
};
```
### PuleGpuActionDispatchRenderElements
```c
struct {
  action : PuleGpuAction = PuleGpuAction_dispatchRenderElements;
  numElements : size_t;
  /*  can be 0  */
  elementOffset : size_t;
  /*  can be 0  */
  baseVertexOffset : size_t;
};
```
### PuleGpuActionClearImageColor
```c
struct {
  action : PuleGpuAction = PuleGpuAction_clearImageColor;
  image : PuleGpuImage;
  color : PuleF32v4;
};
```
### PuleGpuActionBindPipeline
```c
struct {
  action : PuleGpuAction = PuleGpuAction_bindPipeline;
  pipeline : PuleGpuPipeline;
};
```
### PuleGpuActionBindBuffer
```c
struct {
  action : PuleGpuAction = PuleGpuAction_bindBuffer;
  bindingDescriptor : PuleGpuBufferBindingDescriptor;
  bindingIndex : size_t;
  buffer : PuleGpuBuffer;
  offset : size_t;
  byteLen : size_t;
};
```
### PuleGpuActionBindTexture
```c
struct {
  action : PuleGpuAction = PuleGpuAction_bindTexture;
  bindingIndex : size_t;
  imageView : PuleGpuImageView;
  imageLayout : PuleGpuImageLayout;
};
```
### PuleGpuActionClearImageDepth
```c
struct {
  action : PuleGpuAction = PuleGpuAction_clearImageDepth;
  image : PuleGpuImage;
  depth : float;
};
```
### PuleGpuActionPushConstants
```c
struct {
  action : PuleGpuAction = PuleGpuAction_pushConstants;
  stage : PuleGpuDescriptorStage;
  byteLength : size_t;
  byteOffset : size_t;
  data : void const ptr;
};
```
### PuleGpuActionDispatchCommandList
```c
struct {
  action : PuleGpuAction = PuleGpuAction_dispatchCommandList;
  /*  TODO remove this  */
  submitInfo : PuleGpuCommandListSubmitInfo;
};
```
### PuleGpuActionSetScissor
```c
struct {
  action : PuleGpuAction = PuleGpuAction_setScissor;
  scissorMin : PuleI32v2;
  scissorMax : PuleI32v2;
};
```
### PuleGpuActionCopyImageToImage
```c
struct {
  action : PuleGpuAction = PuleGpuAction_copyImageToImage;
  srcImage : PuleGpuImage;
  dstImage : PuleGpuImage;
  /*  TODO = PuleU32v3{0, 0, 0}  */
  srcOffset : PuleU32v3;
  /*  TODO = PuleU32v3{0, 0, 0}  */
  dstOffset : PuleU32v3;
  /*  TODO = PuleU32v3{1, 1, 1}  */
  extent : PuleU32v3;
  isSrcDepthStencil : bool;
  isDstDepthStencil : bool;
};
```
### PuleGpuCommand
```c
union {
  action : PuleGpuAction;
  bindPipeline : PuleGpuActionBindPipeline;
  bindFramebuffer : PuleGpuActionBindFramebuffer;
  bindBuffer : PuleGpuActionBindBuffer;
  bindTexture : PuleGpuActionBindTexture;
  renderPassBegin : PuleGpuActionRenderPassBegin;
  resourceBarrier : PuleGpuActionResourceBarrier;
  renderPassEnd : PuleGpuActionRenderPassEnd;
  bindElementBuffer : PuleGpuActionBindElementBuffer;
  bindAttributeBuffer : PuleGpuActionBindAttributeBuffer;
  clearImageColor : PuleGpuActionClearImageColor;
  clearImageDepth : PuleGpuActionClearImageDepth;
  dispatchRender : PuleGpuActionDispatchRender;
  dispatchRenderIndirect : PuleGpuActionDispatchRenderIndirect;
  dispatchRenderElements : PuleGpuActionDispatchRenderElements;
  pushConstants : PuleGpuActionPushConstants;
  dispatchCommandList : PuleGpuActionDispatchCommandList;
  setScissor : PuleGpuActionSetScissor;
  copyImageToImage : PuleGpuActionCopyImageToImage;
};
```

## enums
### PuleGpuElementType
```c
enum {
  u8,
  u16,
  u32,
}
```
### PuleGpuAction

   some amount of action independent of another, such that one complete amount
     of work can be made between the GPU and CPU. Such as blits, allocations,
     initiating a render, etc
   commands are not unique, and as such are treated as primitives and may not
     refer to any specific unique index of a queue
   TODO just rename to GpuCommand_

```c
enum {
  bindPipeline,
  bindBuffer,
  bindTexture,
  resourceBarrier,
  renderPassBegin,
  renderPassEnd,
  bindElementBuffer,
  bindAttributeBuffer,
  bindFramebuffer,
  clearImageColor,
  clearImageDepth,
  dispatchRender,
  dispatchRenderElements,
  dispatchRenderIndirect,
  pushConstants,
  dispatchCommandList,
  setScissor,
  copyImageToImage,
}
```
### PuleGpuResourceBarrierStage
```c
enum {
  top,
  drawIndirect,
  vertexInput,
  shaderFragment,
  shaderVertex,
  shaderCompute,
  outputAttachmentColor,
  outputAttachmentDepth,
  transfer,
  bottom,
}
```
### PuleGpuResourceAccess
```c
enum {
  none,
  indirectCommandRead,
  indexRead,
  vertexAttributeRead,
  uniformRead,
  inputAttachmentRead,
  shaderRead,
  shaderWrite,
  attachmentColorRead,
  attachmentColorWrite,
  attachmentDepthRead,
  attachmentDepthWrite,
  transferRead,
  transferWrite,
  hostRead,
  hostWrite,
  memoryRead,
  memoryWrite,
}
```
### PuleGpuConstantTypeTag
 TODO deprecate this in favor of shader introspection 
```c
enum {
  f32,
  f32v2,
  f32v3,
  f32v4,
  i32,
  i32v2,
  i32v3,
  i32v4,
  f32m44,
}
```
### PuleErrorGfx
```c
enum {
  none,
  creationFailed,
  shaderModuleCompilationFailed,
  invalidDescriptorSet,
  invalidCommandList,
  invalidFramebuffer,
  submissionFenceWaitFailed,
}
```
### PuleGpuBufferUsage
```c
enum {
  attribute,
  element,
  uniform,
  storage,
  accelerationStructure,
  indirect,
}
```
### PuleGpuBufferBindingDescriptor
```c
enum {
  uniform,
  storage,
  accelerationStructure,
}
```
### PuleGpuBufferVisibilityFlag
```c
enum {
  /*  incompatible with rest  */
  deviceOnly,
  hostVisible,
  hostWritable,
}
```
### PuleGpuBufferMapAccess
```c
enum {
  hostVisible,
  hostWritable,
  invalidate,
}
```
### PuleGpuImageMagnification
```c
enum {
  nearest,
}
```
### PuleGpuImageWrap
```c
enum {
  clampToEdge,
}
```
### PuleGpuImageByteFormat
```c
enum {
  undefined,
  bgra8U,
  rgba8U,
  rgb8U,
  r8U,
  depth16,
}
```
### PuleGpuImageTarget
```c
enum {
  i2D,
}
```
### PuleGpuFramebufferAttachment
```c
enum {
  color0,
  color1,
  color3,
  color4,
  depth,
  stencil,
  depthStencil,
}
```
### PuleGpuFramebufferType
```c
enum {
  renderStorage,
  imageStorage,
}
```
### PuleGpuImageAttachmentOpLoad
```c
enum {
  load,
  clear,
  dontCare,
}
```
### PuleGpuImageAttachmentOpStore
```c
enum {
  store,
  dontCare,
}
```
### PuleGpuImageLayout
```c
enum {
  uninitialized,
  storage,
  attachmentColor,
  attachmentDepth,
  transferSrc,
  transferDst,
  presentSrc,
}
```
### PuleGpuPipelineStage
```c
enum {
  top,
  drawIndirect,
  vertexInput,
  vertexShader,
  fragmentShader,
  colorAttachmentOutput,
  computeShader,
  transfer,
  bottom,
}
```
### PuleGpuDescriptorType
```c
enum {
  sampler,
  uniformBuffer,
  /*  not supported  */
  storageBuffer,
}
```
### PuleGpuAttributeDataType
```c
enum {
  f32,
  u8,
}
```
### PuleGpuPipelineDescriptorMax
```c
enum {
  uniform,
  storage,
  attribute,
  texture,
}
```
### PuleGpuDescriptorStage
```c
enum {
  unused,
  vertex,
  fragment,
}
```
### PuleGpuDrawPrimitive
```c
enum {
  triangle,
  triangleStrip,
  point,
  line,
}
```
### PuleGpuFenceConditionFlag
```c
enum {
  all,
}
```
### PuleGpuSignalTime
```c
enum {
  forever,
}
```

## entities
### PuleGpuCommandList
### PuleGpuCommandListRecorder
### PuleGpuCommandListChain

  utility to allow recording to a command list while it is being submitted,
  which occurs if double or triple buffering for present is enabled It's an
  implementation detail if command lists are reused or destroyed

### PuleGpuBuffer
### PuleGpuSampler
### PuleGpuImage
### PuleGpuRenderStorage
### PuleGpuPipeline
### PuleGpuImageChain
### PuleGpuImageReference

   allows you to reference images, even if they're behind a chain, and track
   their lifetimes (e.g. if they get reconstructed)

### PuleGpuShaderModule
### PuleGpuSemaphore
### PuleGpuFence

## functions
### puleGpuCommandListCreate
```c
puleGpuCommandListCreate(
  allocator : PuleAllocator,
  label : PuleStringView
) PuleGpuCommandList;
```
### puleGpuCommandListDestroy
```c
puleGpuCommandListDestroy(
  commandList : PuleGpuCommandList
) void;
```
### puleGpuCommandListName
```c
puleGpuCommandListName(
  commandList : PuleGpuCommandList
) PuleStringView;
```
### puleGpuCommandListRecorder
```c
puleGpuCommandListRecorder(
  commandList : PuleGpuCommandList
) PuleGpuCommandListRecorder;
```
### puleGpuCommandListRecorderFinish
```c
puleGpuCommandListRecorderFinish(
  commandListRecorder : PuleGpuCommandListRecorder
) void;
```
### puleGpuCommandListAppendAction
```c
puleGpuCommandListAppendAction(
  commandListRecorder : PuleGpuCommandListRecorder,
  action : PuleGpuCommand
) void;
```
### puleGpuCommandListSubmit
```c
puleGpuCommandListSubmit(
  info : PuleGpuCommandListSubmitInfo,
  error : PuleError ptr
) void;
```
### puleGpuCommandListSubmitAndPresent
```c
puleGpuCommandListSubmitAndPresent(
  info : PuleGpuCommandListSubmitInfo,
  error : PuleError ptr
) void;
```
### puleGpuCommandListDump
```c
puleGpuCommandListDump(
  commandList : PuleGpuCommandList
) void;
```
### puleGpuCommandListChainCreate
```c
puleGpuCommandListChainCreate(
  allocator : PuleAllocator,
  label : PuleStringView
) PuleGpuCommandListChain;
```
### puleGpuCommandListChainDestroy
```c
puleGpuCommandListChainDestroy(
  commandListChain : PuleGpuCommandListChain
) void;
```
### puleGpuCommandListChainCurrent
```c
puleGpuCommandListChainCurrent(
  commandListChain : PuleGpuCommandListChain
) PuleGpuCommandList;
```
### puleGpuCommandListChainCurrentFence

  when submitting command list, be sure to set 'fenceTargetFinish' to this

```c
puleGpuCommandListChainCurrentFence(
  commandListChain : PuleGpuCommandListChain
) PuleGpuFence;
```
### puleGpuBufferCreate
```c
puleGpuBufferCreate(
  name : PuleStringView,
  optionalInitialData : void const ptr,
  byteLength : size_t,
  usage : PuleGpuBufferUsage,
  visibility : PuleGpuBufferVisibilityFlag
) PuleGpuBuffer;
```
### puleGpuBufferDestroy
```c
puleGpuBufferDestroy(
  buffer : PuleGpuBuffer
) void;
```
### puleGpuBufferMap
```c
puleGpuBufferMap(
  range : PuleGpuBufferMapRange
) void ptr;
```
### puleGpuBufferMappedFlush
```c
puleGpuBufferMappedFlush(
  range : PuleGpuBufferMappedFlushRange
) void;
```
### puleGpuBufferUnmap
```c
puleGpuBufferUnmap(
  buffer : PuleGpuBuffer
) void;
```
### puleGpuInitialize
```c
puleGpuInitialize(
  platform : PulePlatform,
  error : PuleError ptr
) void;
```
### puleGpuShutdown
```c
puleGpuShutdown() void;
```
### puleGpuDebugPrint

  prints all debug information
  TODO this should just stream out or something maybe serialize

```c
puleGpuDebugPrint() void;
```
### puleGpuBufferUsageLabel
```c
puleGpuBufferUsageLabel(
  usage : PuleGpuBufferUsage
) PuleStringView;
```
### puleGpuSamplerCreate
```c
puleGpuSamplerCreate(
  createInfo : PuleGpuSamplerCreateInfo
) PuleGpuSampler;
```
### puleGpuSamplerDestroy
```c
puleGpuSamplerDestroy(
  sampler : PuleGpuSampler
) void;
```
### puleGpuImageLabel
```c
puleGpuImageLabel(
  image : PuleGpuImage
) PuleStringView;
```
### puleGpuImageDestroy
```c
puleGpuImageDestroy(
  image : PuleGpuImage
) void;
```
### puleGpuImageCreate
```c
puleGpuImageCreate(
  imageCreateInfo : PuleGpuImageCreateInfo
) PuleGpuImage;
```
### puleGpuFramebufferCreateInfo
```c
puleGpuFramebufferCreateInfo() PuleGpuFramebufferCreateInfo;
```
### puleGpuFramebufferCreate
```c
puleGpuFramebufferCreate(
  framebufferCreateInfo : PuleGpuFramebufferCreateInfo,
  error : PuleError ptr
) PuleGpuFramebuffer;
```
### puleGpuFramebufferDestroy
```c
puleGpuFramebufferDestroy(
  framebuffer : PuleGpuFramebuffer
) void;
```
### puleGpuFramebufferAttachments
```c
puleGpuFramebufferAttachments(
  framebuffer : PuleGpuFramebuffer
) PuleGpuFramebufferAttachments;
```
### puleGpuImageLayoutLabel
```c
puleGpuImageLayoutLabel(
  layout : PuleGpuImageLayout
) PuleStringView;
```
### puleGpuPipelineDescriptorSetLayout
```c
puleGpuPipelineDescriptorSetLayout() PuleGpuPipelineLayoutDescriptorSet;
```
### puleGpuPipelineCreate
```c
puleGpuPipelineCreate(
  createInfo : PuleGpuPipelineCreateInfo,
  error : PuleError ptr
) PuleGpuPipeline;
```
### puleGpuPipelineDestroy
```c
puleGpuPipelineDestroy(
  pipeline : PuleGpuPipeline
) void;
```
### puleGpuImageChain_create
```c
puleGpuImageChain_create(
  allocator : PuleAllocator,
  label : PuleStringView,
  createInfo : PuleGpuImageCreateInfo
) PuleGpuImageChain;
```
### puleGpuImageChain_destroy
```c
puleGpuImageChain_destroy(
  imageChain : PuleGpuImageChain
) void;
```
### puleGpuImageChain_current
```c
puleGpuImageChain_current(
  imageChain : PuleGpuImageChain
) PuleGpuImage;
```
### puleGpuImageReference_createImageChain
```c
puleGpuImageReference_createImageChain(
  imageChain : PuleGpuImageChain
) PuleGpuImageReference;
```
### puleGpuImageReference_createImage
```c
puleGpuImageReference_createImage(
  image : PuleGpuImage
) PuleGpuImageReference;
```
### puleGpuImageReference_updateImageChain
```c
puleGpuImageReference_updateImageChain(
  baseImageChain : PuleGpuImageChain,
  newImageChain : PuleGpuImageChain
) void;
```
### puleGpuImageReference_updateImage
```c
puleGpuImageReference_updateImage(
  baseImage : PuleGpuImage,
  newImage : PuleGpuImage
) void;
```
### puleGpuImageReference_destroy
```c
puleGpuImageReference_destroy(
  reference : PuleGpuImageReference
) void;
```
### puleGpuImageReference_image
```c
puleGpuImageReference_image(
  reference : PuleGpuImageReference
) PuleGpuImage;
```
### puleGpuWindowSwapchainImage
```c
puleGpuWindowSwapchainImage() PuleGpuImage;
```
### puleGpuWindowSwapchainImageReference
```c
puleGpuWindowSwapchainImageReference() PuleGpuImageReference;
```
### puleGpuShaderModuleCreate
```c
puleGpuShaderModuleCreate(
  vertexShaderBytecode : PuleBufferView,
  fragmentShaderBytecode : PuleBufferView,
  error : PuleError ptr
) PuleGpuShaderModule;
```
### puleGpuShaderModuleDestroy
```c
puleGpuShaderModuleDestroy(
  shaderModule : PuleGpuShaderModule
) void;
```
### puleGpuSemaphoreCreate
```c
puleGpuSemaphoreCreate(
  label : PuleStringView
) PuleGpuSemaphore;
```
### puleGpuSemaphoreDestroy
```c
puleGpuSemaphoreDestroy(
  semaphore : PuleGpuSemaphore
) void;
```
### puleGpuFenceCreate
```c
puleGpuFenceCreate(
  label : PuleStringView
) PuleGpuFence;
```
### puleGpuFenceDestroy
```c
puleGpuFenceDestroy(
  fence : PuleGpuFence
) void;
```
### puleGpuFenceWaitSignal
```c
puleGpuFenceWaitSignal(
  fence : PuleGpuFence,
  timeout : PuleNanosecond
) bool;
```
### puleGpuFenceReset
```c
puleGpuFenceReset(
  fence : PuleGpuFence
) void;
```
### puleGpuFrameStart
```c
puleGpuFrameStart() PuleGpuSemaphore;
```
### puleGpuSwapchainAvailableSemaphore

 same value returned from puleGpuFrameStart

```c
puleGpuSwapchainAvailableSemaphore() PuleGpuSemaphore;
```
### puleGpuFrameEnd
call before pulePlatformSwapFramebuffer
```c
puleGpuFrameEnd(
  waitSemaphoreCount : size_t,
  waitSemaphores : PuleGpuSemaphore const ptr
) void;
```
