#pragma once

#include <pulchritude-string/string.h>
#include <pulchritude-gpu/commands.h>

// A task graph to help parallelize render tasks, and track the usage of
// resources within each task. Can be built up from multiple graphs from
// multiple modules that are merged into a single task graph.

#ifdef __cplusplus
extern "C" {
#endif

// -- render graph
typedef struct { uint64_t id; } PuleRenderGraph;

PULE_exportFn PuleRenderGraph puleRenderGraphCreate(
  PuleAllocator const allocator
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

typedef struct {
  union {
    struct {
      // TODO rename 'entrance' to '', and 'preentrance' to 'entrance'
      // TODO possibly add an optional 'exittance' layout/access, in case
      //      user modifies the access/layout, however this shouldn't
      //      be necessary if the render graph is robust enough
      PuleGpuResourceAccess payloadAccessEntrance;
      // this will be filled out for you based off 'depends on' entrance
      PuleGpuResourceAccess payloadAccessPreentrance;
      PuleGpuImageLayout payloadLayoutEntrance;
      // this will be filled out for you based off 'depends on' entrance
      PuleGpuImageLayout payloadLayoutPreentrance;

      bool isInitialized;
    } image;
    struct {
      PuleGpuResourceAccess entrancePayloadAccess;
      PuleGpuResourceAccess exittancePayloadAccess;
    } buffer;
  };
  PuleRenderGraph_ResourceType resourceType;
} PuleRenderGraph_Resource;

PULE_exportFn void puleRenderGraph_resourceAssign(
  PuleRenderGraphNode const node,
  PuleStringView const resourceLabel,
  PuleRenderGraph_Resource const resource
);

PULE_exportFn PuleRenderGraph_Resource puleRenderGraph_resource(
  PuleRenderGraphNode const node,
  PuleStringView const resourceLabel
);

PULE_exportFn void puleRenderGraph_resourceRemove(
  PuleRenderGraphNode const node,
  PuleStringView const resourceLabel
);

// -- render node | command list

PULE_exportFn PuleGpuCommandList puleRenderGraph_commandList(
  PuleRenderGraphNode const node,
  uint64_t (* const fetchResourceHandle)(
    PuleStringView const label, void * const userdata
  ),
  void * const userdata
);
PULE_exportFn PuleGpuCommandListRecorder puleRenderGraph_commandListRecorder(
  PuleRenderGraphNode const node,
  uint64_t (* const fetchResourceHandle)(
    PuleStringView const label, void * const userdata
  ),
  void * const userdata
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
  PuleRenderGraph const graph,
  uint64_t (* const fetchResourceHandle)(
    PuleStringView const label, void * const userdata
  )
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
