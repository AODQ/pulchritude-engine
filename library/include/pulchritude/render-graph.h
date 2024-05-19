/* auto generated file render-graph */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/data-serializer.h>
#include <pulchritude/error.h>
#include <pulchritude/gpu.h>
#include <pulchritude/platform.h>
#include <pulchritude/render-graph.h>
#include <pulchritude/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleString referenceResourceLabel;
  float scaleWidth;
  float scaleHeight;
} PuleRenderGraph_Resource_DimensionsScaleRelative;
typedef struct {
  uint32_t width;
  uint32_t height;
} PuleRenderGraph_Resource_DimensionsAbsolute;
typedef union {
  PuleRenderGraph_Resource_DimensionsScaleRelative scaleRelative;
  PuleRenderGraph_Resource_DimensionsAbsolute absolute;
} PuleRenderGraph_Resource_Dimensions;
typedef struct {
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
typedef struct {
  uint8_t reserved;
} PuleRenderGraph_Resource_Image_DataManagement_Manual;
typedef union {
  PuleRenderGraph_Resource_Image_DataManagement_Automatic automatic;
  PuleRenderGraph_Resource_Image_DataManagement_Manual manual;
} PuleRenderGraph_Resource_Image_DataManagement;
typedef struct {
  PuleRenderGraph_Resource_Image_DataManagement dataManagement;
  bool isAutomatic;
  PuleGpuImageReference imageReference;
} PuleRenderGraph_Resource_Image;
typedef struct {
  uint8_t reserved;
} PuleRenderGraph_Resource_Buffer;
typedef union {
  image PuleRenderGraph_Resource_Image;
  buffer PuleRenderGraph_Resource_Buffer;
} PuleRenderGraph_ResourceUnion;
/*  tracks all metadata of a resource relating to render graph */
typedef struct {
  PuleStringView resourceLabel;
  PuleRenderGraph_ResourceUnion resource;
  PuleRenderGraph_ResourceType resourceType;
} PuleRenderGraph_Resource;
typedef union {
  PuleGpuImageLayout layout;
  PuleGpuImageLayout layoutEntrance;
} PuleRenderGraph_Node_Image;
typedef union {
  uint8_t reserved;
} PuleRenderGraph_Node_Buffer;
typedef union {
  PuleRenderGraph_Node_Image image;
  PuleRenderGraph_Node_Buffer buffer;
} PuleRenderGraph_Node_ResourceUnion;
/* 
  contains relationship of a resource to a node, e.g. if the node's command
  list plans to use it.
 */
typedef struct {
  PuleStringView resourceLabel;
  PuleGpuResourceAccess access;
  PuleGpuResourceAccess accessEntrance;
  PuleRenderGraph_Node_ResourceUnion resource;
} PuleRenderGraph_Node_Resource;
typedef struct {
  PuleRenderGraph graph;
  void(* callback)(PuleRenderGraphNode, void *);
  void * userdata;
  bool multithreaded;
} PuleRenderGraphExecuteInfo;

// enum
typedef enum {
  PuleRenderGraph_ResourceType_image,
  PuleRenderGraph_ResourceType_buffer,
} PuleRenderGraph_ResourceType;
typedef enum {
  PuleRenderGraph_ResourceUsage_read,
  PuleRenderGraph_ResourceUsage_write,
  PuleRenderGraph_ResourceUsage_readWrite,
} PuleRenderGraph_ResourceUsage;
typedef enum {
  PuleRenderGraphNodeRelation_dependsOn,
} PuleRenderGraphNodeRelation;

// entities
typedef struct { uint64_t id; } PuleRenderGraph;
typedef struct { uint64_t id; } PuleRenderGraphNode;

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
