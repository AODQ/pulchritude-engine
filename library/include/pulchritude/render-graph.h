/* auto generated file render-graph */
#pragma once
#include "core.h"

#include "data-serializer.h"
#include "error.h"
#include "gpu.h"
#include "platform.h"
#include "render-graph.h"
#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleRenderGraph_ResourceType_image = 0,
  PuleRenderGraph_ResourceType_buffer = 1,
} PuleRenderGraph_ResourceType;
const uint32_t PuleRenderGraph_ResourceTypeSize = 2;
typedef enum {
  PuleRenderGraph_ResourceUsage_read = 0,
  PuleRenderGraph_ResourceUsage_write = 1,
  PuleRenderGraph_ResourceUsage_readWrite = 2,
} PuleRenderGraph_ResourceUsage;
const uint32_t PuleRenderGraph_ResourceUsageSize = 3;
typedef enum {
  PuleRenderGraphNodeRelation_dependsOn = 0,
} PuleRenderGraphNodeRelation;
const uint32_t PuleRenderGraphNodeRelationSize = 1;

// entities
typedef struct PuleRenderGraph { uint64_t id; } PuleRenderGraph;
typedef struct PuleRenderGraphNode { uint64_t id; } PuleRenderGraphNode;

// structs
struct PuleRenderGraph_Resource_DimensionsScaleRelative;
struct PuleRenderGraph_Resource_DimensionsAbsolute;
union PuleRenderGraph_Resource_Dimensions;
struct PuleRenderGraph_Resource_Image_DataManagement_Automatic;
struct PuleRenderGraph_Resource_Image_DataManagement_Manual;
union PuleRenderGraph_Resource_Image_DataManagement;
struct PuleRenderGraph_Resource_Image;
struct PuleRenderGraph_Resource_Buffer;
union PuleRenderGraph_ResourceUnion;
struct PuleRenderGraph_Resource;
struct PuleRenderGraph_Node_Image;
struct PuleRenderGraph_Node_Buffer;
union PuleRenderGraph_Node_ResourceUnion;
struct PuleRenderGraph_Node_Resource;
struct PuleRenderGraphNode_RenderPassAttachment;
struct PuleRenderGraphNode_RenderPass;
struct PuleRenderGraphExecuteInfo;

typedef struct PuleRenderGraph_Resource_DimensionsScaleRelative {
  PuleString referenceResourceLabel;
  float scaleWidth;
  float scaleHeight;
} PuleRenderGraph_Resource_DimensionsScaleRelative;
typedef struct PuleRenderGraph_Resource_DimensionsAbsolute {
  uint32_t width;
  uint32_t height;
} PuleRenderGraph_Resource_DimensionsAbsolute;
typedef union PuleRenderGraph_Resource_Dimensions {
  PuleRenderGraph_Resource_DimensionsScaleRelative scaleRelative;
  PuleRenderGraph_Resource_DimensionsAbsolute absolute;
} PuleRenderGraph_Resource_Dimensions;
typedef struct PuleRenderGraph_Resource_Image_DataManagement_Automatic {
  PuleRenderGraph_Resource_Dimensions dimensions;
  bool areDimensionsAbsolute;
  PuleBufferView nullableInitialData;
  PuleRenderGraph_ResourceUsage resourceUsage;
  bool isAttachment;
  PuleGpuImageByteFormat byteFormat;
  PuleGpuSampler sampler;
  PuleGpuImageTarget target;
  uint32_t mipmapLevels;
  uint32_t arrayLayers;
} PuleRenderGraph_Resource_Image_DataManagement_Automatic;
/*  TODO this should be some reference to an external resource handle  */
typedef struct PuleRenderGraph_Resource_Image_DataManagement_Manual {
  uint8_t reserved;
} PuleRenderGraph_Resource_Image_DataManagement_Manual;
typedef union PuleRenderGraph_Resource_Image_DataManagement {
  PuleRenderGraph_Resource_Image_DataManagement_Automatic automatic;
  PuleRenderGraph_Resource_Image_DataManagement_Manual manual;
} PuleRenderGraph_Resource_Image_DataManagement;
typedef struct PuleRenderGraph_Resource_Image {
  PuleRenderGraph_Resource_Image_DataManagement dataManagement;
  bool isAutomatic;
  PuleGpuImageReference imageReference;
} PuleRenderGraph_Resource_Image;
typedef struct PuleRenderGraph_Resource_Buffer {
  uint8_t reserved;
} PuleRenderGraph_Resource_Buffer;
typedef union PuleRenderGraph_ResourceUnion {
  PuleRenderGraph_Resource_Image image;
  PuleRenderGraph_Resource_Buffer buffer;
} PuleRenderGraph_ResourceUnion;
/*  tracks all metadata of a resource relating to render graph */
typedef struct PuleRenderGraph_Resource {
  PuleStringView resourceLabel;
  PuleRenderGraph_ResourceUnion resource;
  PuleRenderGraph_ResourceType resourceType;
} PuleRenderGraph_Resource;
typedef struct PuleRenderGraph_Node_Image {
  PuleGpuImageLayout layout;
  PuleGpuImageLayout layoutEntrance;
} PuleRenderGraph_Node_Image;
typedef struct PuleRenderGraph_Node_Buffer {
  uint8_t reserved;
} PuleRenderGraph_Node_Buffer;
typedef union PuleRenderGraph_Node_ResourceUnion {
  PuleRenderGraph_Node_Image image;
  PuleRenderGraph_Node_Buffer buffer;
} PuleRenderGraph_Node_ResourceUnion;
/* 
  contains relationship of a resource to a node, e.g. if the node's command
  list plans to use it.
 */
typedef struct PuleRenderGraph_Node_Resource {
  PuleStringView resourceLabel;
  PuleGpuResourceAccess access;
  PuleGpuResourceAccess accessEntrance;
  PuleRenderGraph_Node_ResourceUnion resource;
} PuleRenderGraph_Node_Resource;
typedef struct PuleRenderGraphNode_RenderPassAttachment {
  PuleStringView label;
  PuleGpuImageAttachmentOpLoad opLoad;
  PuleGpuImageAttachmentOpStore opStore;
  PuleGpuImageAttachmentClear clear;
} PuleRenderGraphNode_RenderPassAttachment;
typedef struct PuleRenderGraphNode_RenderPass {
  PuleRenderGraphNode_RenderPassAttachment attachmentColor;
  PuleRenderGraphNode_RenderPassAttachment attachmentDepth;
} PuleRenderGraphNode_RenderPass;
typedef struct PuleRenderGraphExecuteInfo {
  PuleRenderGraph graph;
  void(* callback)(PuleRenderGraphNode, void *);
  void * userdata;
  bool multithreaded;
} PuleRenderGraphExecuteInfo;

// functions
PULE_exportFn PuleRenderGraph puleRenderGraphCreate(PuleStringView label, PuleAllocator allocator);
PULE_exportFn void puleRenderGraphDestroy(PuleRenderGraph graph);
PULE_exportFn void puleRenderGraphMerge(PuleRenderGraph renderGraphPrimary, PuleRenderGraph renderGraphSecondary);
PULE_exportFn PuleRenderGraphNode puleRenderGraphNodeCreate(PuleRenderGraph graph, PuleStringView label);
PULE_exportFn void puleRenderGraphNodeRemove(PuleRenderGraphNode node);
PULE_exportFn PuleStringView puleRenderGraphNodeLabel(PuleRenderGraphNode node);
PULE_exportFn PuleRenderGraphNode puleRenderGraphNodeFetch(PuleRenderGraph graph, PuleStringView label);
PULE_exportFn void puleRenderGraph_resourceCreate(PuleRenderGraph graph, PuleRenderGraph_Resource resource);
PULE_exportFn PuleRenderGraph_Resource puleRenderGraph_resource(PuleRenderGraph graph, PuleStringView resourceLabel);
PULE_exportFn void puleRenderGraph_resourceRemove(PuleRenderGraph graph, PuleStringView resourceLabel);
PULE_exportFn void puleRenderGraph_node_resourceAssign(PuleRenderGraphNode node, PuleRenderGraph_Node_Resource resourceUsage);
PULE_exportFn PuleGpuCommandList puleRenderGraph_commandList(PuleRenderGraphNode node);
PULE_exportFn PuleGpuCommandListRecorder puleRenderGraph_commandListRecorder(PuleRenderGraphNode node);
PULE_exportFn void puleRenderGraphNode_renderPassSet(PuleRenderGraphNode node, PuleRenderGraphNode_RenderPass renderPass);
PULE_exportFn void puleRenderGraphNode_renderPassBegin(PuleRenderGraphNode node, PuleGpuCommandListRecorder recorder);
PULE_exportFn void puleRenderGraphNode_renderPassEnd(PuleRenderGraphNode node, PuleGpuCommandListRecorder recorder);
PULE_exportFn void puleRenderGraphNodeRelationSet(PuleRenderGraphNode nodePri, PuleRenderGraphNodeRelation relation, PuleRenderGraphNode nodeSec);
/* 
  prepares for rendering, needs to fetch resources to bake resource barriers
  into command lists
 */
PULE_exportFn void puleRenderGraphFrameStart(PuleRenderGraph graph);
/* 
  submits the render graph's command lists to the GPU, and will present the
  swapchain image. No further GPU work can be done until the next frame.

  TODO
  probably want to return a semaphore that the user can use to manually present
  the swapchain image
 */
PULE_exportFn void puleRenderGraphFrameSubmit(PuleGpuSemaphore swapchainImageSemaphore, PuleRenderGraph graph);
PULE_exportFn void puleRenderGraphExecuteInOrder(PuleRenderGraphExecuteInfo execute);
PULE_exportFn bool puleRenderGraphNodeExists(PuleRenderGraphNode node);

#ifdef __cplusplus
} // extern C
#endif
