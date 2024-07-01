# render-graph

## structs
### PuleRenderGraph_Resource_DimensionsScaleRelative
```c
struct {
  referenceResourceLabel : PuleString;
  scaleWidth : float;
  scaleHeight : float;
};
```
### PuleRenderGraph_Resource_DimensionsAbsolute
```c
struct {
  width : uint32_t;
  height : uint32_t;
};
```
### PuleRenderGraph_Resource_Dimensions
```c
union {
  scaleRelative : PuleRenderGraph_Resource_DimensionsScaleRelative;
  absolute : PuleRenderGraph_Resource_DimensionsAbsolute;
};
```
### PuleRenderGraph_Resource_Image_DataManagement_Automatic
```c
struct {
  dimensions : PuleRenderGraph_Resource_Dimensions;
  areDimensionsAbsolute : bool;
  nullableInitialData : PuleBufferView;
  resourceUsage : PuleRenderGraph_ResourceUsage;
  isAttachment : bool;
  byteFormat : PuleGpuImageByteFormat;
  sampler : PuleGpuSampler;
  target : PuleGpuImageTarget;
  mipmapLevels : uint32_t;
  arrayLayers : uint32_t;
};
```
### PuleRenderGraph_Resource_Image_DataManagement_Manual
 TODO this should be some reference to an external resource handle 
```c
struct {
  reserved : uint8_t;
};
```
### PuleRenderGraph_Resource_Image_DataManagement
```c
union {
  automatic : PuleRenderGraph_Resource_Image_DataManagement_Automatic;
  manual : PuleRenderGraph_Resource_Image_DataManagement_Manual;
};
```
### PuleRenderGraph_Resource_Image
```c
struct {
  dataManagement : PuleRenderGraph_Resource_Image_DataManagement;
  isAutomatic : bool;
  imageReference : PuleGpuImageReference;
};
```
### PuleRenderGraph_Resource_Buffer
```c
struct {
  reserved : uint8_t;
};
```
### PuleRenderGraph_ResourceUnion
```c
union {
  image : PuleRenderGraph_Resource_Image;
  buffer : PuleRenderGraph_Resource_Buffer;
};
```
### PuleRenderGraph_Resource
 tracks all metadata of a resource relating to render graph
```c
struct {
  resourceLabel : PuleStringView;
  resource : PuleRenderGraph_ResourceUnion;
  resourceType : PuleRenderGraph_ResourceType;
};
```
### PuleRenderGraph_Node_Image
```c
struct {
  layout : PuleGpuImageLayout;
  layoutEntrance : PuleGpuImageLayout;
};
```
### PuleRenderGraph_Node_Buffer
```c
struct {
  reserved : uint8_t;
};
```
### PuleRenderGraph_Node_ResourceUnion
```c
union {
  image : PuleRenderGraph_Node_Image;
  buffer : PuleRenderGraph_Node_Buffer;
};
```
### PuleRenderGraph_Node_Resource

  contains relationship of a resource to a node, e.g. if the node's command
  list plans to use it.

```c
struct {
  resourceLabel : PuleStringView;
  access : PuleGpuResourceAccess;
  accessEntrance : PuleGpuResourceAccess;
  resource : PuleRenderGraph_Node_ResourceUnion;
};
```
### PuleRenderGraphNode_RenderPassAttachment
```c
struct {
  label : PuleStringView;
  opLoad : PuleGpuImageAttachmentOpLoad;
  opStore : PuleGpuImageAttachmentOpStore;
  clear : PuleGpuImageAttachmentClear;
};
```
### PuleRenderGraphNode_RenderPass
```c
struct {
  attachmentColor : PuleRenderGraphNode_RenderPassAttachment;
  attachmentDepth : PuleRenderGraphNode_RenderPassAttachment;
};
```
### PuleRenderGraphExecuteInfo
```c
struct {
  graph : PuleRenderGraph;
  callback : @fnptr(void, PuleRenderGraphNode, void ptr);
  userdata : void ptr;
  multithreaded : bool;
};
```

## enums
### PuleRenderGraph_ResourceType
```c
enum {
  image,
  buffer,
}
```
### PuleRenderGraph_ResourceUsage
```c
enum {
  read,
  write,
  readWrite,
}
```
### PuleRenderGraphNodeRelation
```c
enum {
  dependsOn,
}
```

## entities
### PuleRenderGraph
### PuleRenderGraphNode

## functions
### puleRenderGraphCreate
```c
puleRenderGraphCreate(
  label : PuleStringView,
  allocator : PuleAllocator
) PuleRenderGraph;
```
### puleRenderGraphDestroy
```c
puleRenderGraphDestroy(
  graph : PuleRenderGraph
) void;
```
### puleRenderGraphMerge
```c
puleRenderGraphMerge(
  renderGraphPrimary : PuleRenderGraph,
  renderGraphSecondary : PuleRenderGraph
) void;
```
### puleRenderGraphNodeCreate
```c
puleRenderGraphNodeCreate(
  graph : PuleRenderGraph,
  label : PuleStringView
) PuleRenderGraphNode;
```
### puleRenderGraphNodeRemove
```c
puleRenderGraphNodeRemove(
  node : PuleRenderGraphNode
) void;
```
### puleRenderGraphNodeLabel
```c
puleRenderGraphNodeLabel(
  node : PuleRenderGraphNode
) PuleStringView;
```
### puleRenderGraphNodeFetch
```c
puleRenderGraphNodeFetch(
  graph : PuleRenderGraph,
  label : PuleStringView
) PuleRenderGraphNode;
```
### puleRenderGraph_resourceCreate
```c
puleRenderGraph_resourceCreate(
  graph : PuleRenderGraph,
  resource : PuleRenderGraph_Resource
) void;
```
### puleRenderGraph_resource
```c
puleRenderGraph_resource(
  graph : PuleRenderGraph,
  resourceLabel : PuleStringView
) PuleRenderGraph_Resource;
```
### puleRenderGraph_resourceRemove
```c
puleRenderGraph_resourceRemove(
  graph : PuleRenderGraph,
  resourceLabel : PuleStringView
) void;
```
### puleRenderGraph_node_resourceAssign
```c
puleRenderGraph_node_resourceAssign(
  node : PuleRenderGraphNode,
  resourceUsage : PuleRenderGraph_Node_Resource
) void;
```
### puleRenderGraph_commandList
```c
puleRenderGraph_commandList(
  node : PuleRenderGraphNode
) PuleGpuCommandList;
```
### puleRenderGraph_commandListRecorder
```c
puleRenderGraph_commandListRecorder(
  node : PuleRenderGraphNode
) PuleGpuCommandListRecorder;
```
### puleRenderGraphNode_renderPassSet
```c
puleRenderGraphNode_renderPassSet(
  node : PuleRenderGraphNode,
  renderPass : PuleRenderGraphNode_RenderPass
) void;
```
### puleRenderGraphNode_renderPassBegin
```c
puleRenderGraphNode_renderPassBegin(
  node : PuleRenderGraphNode,
  recorder : PuleGpuCommandListRecorder
) void;
```
### puleRenderGraphNode_renderPassEnd
```c
puleRenderGraphNode_renderPassEnd(
  node : PuleRenderGraphNode,
  recorder : PuleGpuCommandListRecorder
) void;
```
### puleRenderGraphNodeRelationSet
```c
puleRenderGraphNodeRelationSet(
  nodePri : PuleRenderGraphNode,
  relation : PuleRenderGraphNodeRelation,
  nodeSec : PuleRenderGraphNode
) void;
```
### puleRenderGraphFrameStart

  prepares for rendering, needs to fetch resources to bake resource barriers
  into command lists

```c
puleRenderGraphFrameStart(
  graph : PuleRenderGraph
) void;
```
### puleRenderGraphFrameSubmit

  submits the render graph's command lists to the GPU, and will present the
  swapchain image. No further GPU work can be done until the next frame.

  TODO
  probably want to return a semaphore that the user can use to manually present
  the swapchain image

```c
puleRenderGraphFrameSubmit(
  swapchainImageSemaphore : PuleGpuSemaphore,
  graph : PuleRenderGraph
) void;
```
### puleRenderGraphExecuteInOrder
```c
puleRenderGraphExecuteInOrder(
  execute : PuleRenderGraphExecuteInfo
) void;
```
### puleRenderGraphNodeExists
```c
puleRenderGraphNodeExists(
  node : PuleRenderGraphNode
) bool;
```
