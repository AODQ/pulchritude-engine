/* auto generated file task-graph */
#pragma once
#include "core.h"

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleTaskGraphNodeRelation_dependsOn = 0,
} PuleTaskGraphNodeRelation;
const uint32_t PuleTaskGraphNodeRelationSize = 1;

// entities

// structs
struct PuleTaskGraph;
struct PuleTaskGraphNode;
struct PuleTaskGraphExecuteInfo;

typedef struct PuleTaskGraph {
  uint64_t id;
} PuleTaskGraph;
typedef struct PuleTaskGraphNode {
  uint64_t id;
} PuleTaskGraphNode;
typedef struct PuleTaskGraphExecuteInfo {
  PuleTaskGraph graph;
  void(* callback)(PuleTaskGraphNode, void *);
  void * userdata;
  bool multithreaded;
} PuleTaskGraphExecuteInfo;

// functions
PULE_exportFn PuleTaskGraph puleTaskGraphCreate(PuleAllocator allocator);
PULE_exportFn void puleTaskGraphDestroy(PuleTaskGraph graph);
PULE_exportFn void puleTaskGraphMerge(PuleTaskGraph taskGraphPrimary, PuleTaskGraph taskGraphSecondary);
PULE_exportFn PuleTaskGraphNode puleTaskGraphNodeCreate(PuleTaskGraph graph, PuleStringView label);
PULE_exportFn void puleTaskGraphNodeRemove(PuleTaskGraphNode node);
PULE_exportFn PuleStringView puleTaskGraphNodeLabel(PuleTaskGraphNode node);
PULE_exportFn PuleTaskGraphNode puleTaskGraphNodeFetch(PuleTaskGraph graph, PuleStringView label);
PULE_exportFn void puleTaskGraphNodeAttributeStore(PuleTaskGraphNode graphNode, PuleStringView label, void * attributeData);
PULE_exportFn void puleTaskGraphNodeAttributeStoreU64(PuleTaskGraphNode graphNode, PuleStringView label, uint64_t attributeData);
PULE_exportFn void * puleTaskGraphNodeAttributeFetch(PuleTaskGraphNode graphNode, PuleStringView label);
PULE_exportFn uint64_t puleTaskGraphNodeAttributeFetchU64(PuleTaskGraphNode graphNode, PuleStringView label);
PULE_exportFn void puleTaskGraphNodeAttributeRemove(PuleTaskGraphNode graphNode, PuleStringView label);
PULE_exportFn void puleTaskGraphNodeRelationSet(PuleTaskGraphNode nodePri, PuleTaskGraphNodeRelation relation, PuleTaskGraphNode nodeSec);
PULE_exportFn void puleTaskGraphExecuteInOrder(PuleTaskGraphExecuteInfo execute);
PULE_exportFn bool puleTaskGraphNodeExists(PuleTaskGraphNode node);

#ifdef __cplusplus
} // extern C
#endif
