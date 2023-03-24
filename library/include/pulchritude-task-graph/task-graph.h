#pragma once

#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

// -- task graph
typedef struct {
  uint64_t id;
} PuleTaskGraph;

PULE_exportFn PuleTaskGraph puleTaskGraphCreate(PuleAllocator const allocator);
PULE_exportFn void puleTaskGraphDestroy(PuleTaskGraph const graph);

// -- task node
typedef struct {
  uint64_t id;
} PuleTaskGraphNode;

PULE_exportFn PuleTaskGraphNode puleTaskGraphNodeCreate(
  PuleTaskGraph const graph, PuleStringView const label
);
PULE_exportFn void puleTaskGraphNodeRemove(
  PuleTaskGraphNode const node
);

PULE_exportFn PuleStringView puleTaskGraphNodeLabel(
  PuleTaskGraphNode const node
);

PULE_exportFn PuleTaskGraphNode puleTaskGraphNodeFetch(
  PuleTaskGraph const graph, PuleStringView const label
);

// -- task node | attributes
PULE_exportFn void puleTaskGraphNodeAttributeStore(
  PuleTaskGraphNode const graphNode,
  PuleStringView const label,
  void * const attributeData
);
PULE_exportFn void puleTaskGraphNodeAttributeStoreU64(
  PuleTaskGraphNode const graphNode,
  PuleStringView const label,
  uint64_t const attributeData
);

PULE_exportFn void * puleTaskGraphNodeAttributeFetch(
  PuleTaskGraphNode const graphNode,
  PuleStringView const label
);
PULE_exportFn uint64_t puleTaskGraphNodeAttributeFetchU64(
  PuleTaskGraphNode const graphNode,
  PuleStringView const label
);

PULE_exportFn void puleTaskGraphNodeAttributeRemove(
  PuleTaskGraphNode const graphNode,
  PuleStringView const label
);

// -- task node | relations
typedef enum {
  PuleTaskGraphNodeRelation_dependsOn,
} PuleTaskGraphNodeRelation;
PULE_exportFn void puleTaskGraphNodeRelationSet(
  PuleTaskGraphNode const nodePri,
  PuleTaskGraphNodeRelation const relation,
  PuleTaskGraphNode const nodeSec
);

typedef struct {
  PuleTaskGraph const graph;
  void (*callback)(PuleTaskGraphNode const node, void * const userdata);
  void * userdata;
  bool multithreaded;
} PuleTaskGraphExecuteInfo;

PULE_exportFn void puleTaskGraphExecuteInOrder(
  PuleTaskGraphExecuteInfo const execute
);

PULE_exportFn bool puleTaskGraphNodeExists(PuleTaskGraphNode const node);

#ifdef __cplusplus
} // extern C
#endif
