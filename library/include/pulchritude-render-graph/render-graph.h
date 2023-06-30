#pragma once

#include <pulchritude-string/string.h>
#include <pulchritude-gfx/commands.h>

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
      // this can be filled out for you based off 'depends on' exittance
      PuleGfxCommandPayloadAccess entrancePayloadAccess;
      PuleGfxCommandPayloadAccess exittancePayloadAccess;
      // this can be filled out for you based off 'depends on' exittance
      PuleGfxImageLayout entrancePayloadLayout;
      PuleGfxImageLayout exittancePayloadLayout;
    } image;
    struct {
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

PULE_exportFn PuleGfxCommandList puleRenderGraph_commandList(
  PuleRenderGraphNode const node,
  uint64_t (* const fetchResourceHandle)(
    PuleStringView const label, void * const userdata
  ),
  void * const userdata
);
PULE_exportFn PuleGfxCommandListRecorder puleRenderGraph_commandListRecorder(
  PuleRenderGraphNode const node,
  uint64_t (* const fetchResourceHandle)(
    PuleStringView const label, void * const userdata
  ),
  void * const userdata
);

// *must* manually free PuleGfxCommandPayload
PULE_exportFn PuleGfxCommandPayload puleRenderGraph_commandPayload(
  PuleRenderGraphNode const node,
  PuleAllocator const payloadAllocator,
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

PULE_exportFn void puleRenderGraphFrameStart(PuleRenderGraph const graph);
PULE_exportFn void puleRenderGraphFrameEnd(PuleRenderGraph const graph);

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
