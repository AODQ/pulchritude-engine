#pragma once

#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/resource.h>
#include <pulchritude-string/string.h>

// A task graph to help parallelize render tasks, and track the usage of
// resources within each task. Can be built up from multiple graphs from
// multiple modules that are merged into a single task graph.

#ifdef __cplusplus
extern "C" {
#endif

// -- render graph
typedef struct { uint64_t id; } PuleRenderGraph;

PULE_exportFn PuleRenderGraph puleRenderGraphCreate(
  PuleStringView const label, PuleAllocator const allocator
);
PULE_exportFn void puleRenderGraphDestroy(PuleRenderGraph const graph);
PULE_exportFn void puleRenderGraphMerge(
  PuleRenderGraph const renderGraphPrimary,
  PuleRenderGraph const renderGraphSecondary
);

// -- render node
typedef struct { uint64_t id; } PuleRenderGraphNode;

PULE_exportFn PuleRenderGraphNode puleRenderGraphNodeCreate(
  PuleRenderGraph const graph, PuleStringView const label
);
PULE_exportFn void puleRenderGraphNodeRemove(
  PuleRenderGraphNode const node
);

PULE_exportFn PuleStringView puleRenderGraphNodeLabel(
  PuleRenderGraphNode const node
);

PULE_exportFn PuleRenderGraphNode puleRenderGraphNodeFetch(
  PuleRenderGraph const graph, PuleStringView const label
);

// -- render node | resources

typedef enum {
  PuleRenderGraph_ResourceType_image,
  PuleRenderGraph_ResourceType_buffer,
} PuleRenderGraph_ResourceType;

typedef enum {
  PuleRenderGraph_ResourceUsage_read,
  PuleRenderGraph_ResourceUsage_write,
  PuleRenderGraph_ResourceUsage_readWrite,
} PuleRenderGraph_ResourceUsage;

typedef struct {
  union {
    struct {
      PuleString referenceResourceLabel; // TODO deallocate
      float scaleWidth;
      float scaleHeight;
    } dimensionsScaleRelative;
    struct {
      uint32_t width;
      uint32_t height;
    } dimensionsAbsolute;
  };
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

typedef struct {
  // TODO this should be some reference to an external resource handle
  uint8_t reserved;
} PuleRenderGraph_Resource_Image_DataManagement_Manual;

// tracks all metadata of a resource relating to the render graph.
typedef struct {
  PuleStringView resourceLabel;
  union {
    struct {
      union {
        PuleRenderGraph_Resource_Image_DataManagement_Automatic automatic;
        PuleRenderGraph_Resource_Image_DataManagement_Manual manual;
      } dataManagement;
      bool isAutomatic;
      PuleGpuImageReference imageReference;
    } image;
    struct {
      uint8_t reserved;
    } buffer;
  };
  PuleRenderGraph_ResourceType resourceType;
} PuleRenderGraph_Resource;

// contains relationship of a resource to a node, e.g. if the node's command
// list plans to use it.
typedef struct {
  PuleStringView resourceLabel;
  PuleGpuResourceAccess access;
  PuleGpuResourceAccess accessEntrance;
  union {
    struct {
      PuleGpuImageLayout layout;
      PuleGpuImageLayout layoutEntrance;
    } image;
    struct {
      uint8_t reserved;
    } buffer;
  };
} PuleRenderGraph_Node_Resource;

PULE_exportFn void puleRenderGraph_resourceCreate(
  PuleRenderGraph const graph,
  PuleRenderGraph_Resource const resource
);

PULE_exportFn PuleRenderGraph_Resource puleRenderGraph_resource(
  PuleRenderGraph const graph,
  PuleStringView const resourceLabel
);

PULE_exportFn void puleRenderGraph_resourceRemove(
  PuleRenderGraph const graph,
  PuleStringView const resourceLabel
);

PULE_exportFn void puleRenderGraph_node_resourceAssign(
  PuleRenderGraphNode const node,
  PuleRenderGraph_Node_Resource const resourceUsage
);

// -- render node | command list

PULE_exportFn PuleGpuCommandList puleRenderGraph_commandList(
  PuleRenderGraphNode const node
);
PULE_exportFn PuleGpuCommandListRecorder puleRenderGraph_commandListRecorder(
  PuleRenderGraphNode const node
);

// -- render node | relations
typedef enum {
  PuleRenderGraphNodeRelation_dependsOn,
} PuleRenderGraphNodeRelation;
PULE_exportFn void puleRenderGraphNodeRelationSet(
  PuleRenderGraphNode const nodePri,
  PuleRenderGraphNodeRelation const relation,
  PuleRenderGraphNode const nodeSec
);

// -- render node | operations

// prepares for rendering, needs to fetch resources to bake resource barriers
// into command lists
PULE_exportFn void puleRenderGraphFrameStart(
  PuleRenderGraph const graph
);
// submits the render graph's command lists to the GPU, and will present the
// swapchain image. No further GPU work can be done until the next frame.
// TODO probably want to return a semaphore that the user can use to
//      manually present the swapchain image
PULE_exportFn void puleRenderGraphFrameSubmit(
  PuleGpuSemaphore const swapchainImageSemaphore,
  PuleRenderGraph const graph
);

typedef struct {
  PuleRenderGraph const graph;
  void (*callback)(PuleRenderGraphNode const node, void * const userdata);
  void * userdata;
  bool multithreaded;
} PuleRenderGraphExecuteInfo;

PULE_exportFn void puleRenderGraphExecuteInOrder(
  PuleRenderGraphExecuteInfo const execute
);

PULE_exportFn bool puleRenderGraphNodeExists(PuleRenderGraphNode const node);

#ifdef __cplusplus
} // extern C
#endif
