# task-graph

## structs
### PuleTaskGraph
```c
struct {
  id : uint64_t;
};
```
### PuleTaskGraphNode
```c
struct {
  id : uint64_t;
};
```
### PuleTaskGraphExecuteInfo
```c
struct {
  graph : PuleTaskGraph;
  callback : @fnptr(void, PuleTaskGraphNode, void ptr);
  userdata : void ptr;
  multithreaded : bool;
};
```

## enums
### PuleTaskGraphNodeRelation
```c
enum {
  PuleTaskGraphNodeRelation_dependsOn,
}
```

## functions
### puleTaskGraphCreate
```c
puleTaskGraphCreate(
  allocator : PuleAllocator
) PuleTaskGraph;
```
### puleTaskGraphDestroy
```c
puleTaskGraphDestroy(
  graph : PuleTaskGraph
) void;
```
### puleTaskGraphMerge
```c
puleTaskGraphMerge(
  taskGraphPrimary : PuleTaskGraph,
  taskGraphSecondary : PuleTaskGraph
) void;
```
### puleTaskGraphNodeCreate
```c
puleTaskGraphNodeCreate(
  graph : PuleTaskGraph,
  label : PuleStringView
) PuleTaskGraphNode;
```
### puleTaskGraphNodeRemove
```c
puleTaskGraphNodeRemove(
  node : PuleTaskGraphNode
) void;
```
### puleTaskGraphNodeLabel
```c
puleTaskGraphNodeLabel(
  node : PuleTaskGraphNode
) PuleStringView;
```
### puleTaskGraphNodeFetch
```c
puleTaskGraphNodeFetch(
  graph : PuleTaskGraph,
  label : PuleStringView
) PuleTaskGraphNode;
```
### puleTaskGraphNodeAttributeStore
```c
puleTaskGraphNodeAttributeStore(
  graphNode : PuleTaskGraphNode,
  label : PuleStringView,
  attributeData : void ptr
) void;
```
### puleTaskGraphNodeAttributeStoreU64
```c
puleTaskGraphNodeAttributeStoreU64(
  graphNode : PuleTaskGraphNode,
  label : PuleStringView,
  attributeData : uint64_t
) void;
```
### puleTaskGraphNodeAttributeFetch
```c
puleTaskGraphNodeAttributeFetch(
  graphNode : PuleTaskGraphNode,
  label : PuleStringView
) void ptr;
```
### puleTaskGraphNodeAttributeFetchU64
```c
puleTaskGraphNodeAttributeFetchU64(
  graphNode : PuleTaskGraphNode,
  label : PuleStringView
) uint64_t;
```
### puleTaskGraphNodeAttributeRemove
```c
puleTaskGraphNodeAttributeRemove(
  graphNode : PuleTaskGraphNode,
  label : PuleStringView
) void;
```
### puleTaskGraphNodeRelationSet
```c
puleTaskGraphNodeRelationSet(
  nodePri : PuleTaskGraphNode,
  relation : PuleTaskGraphNodeRelation,
  nodeSec : PuleTaskGraphNode
) void;
```
### puleTaskGraphExecuteInOrder
```c
puleTaskGraphExecuteInOrder(
  execute : PuleTaskGraphExecuteInfo
) void;
```
### puleTaskGraphNodeExists
```c
puleTaskGraphNodeExists(
  node : PuleTaskGraphNode
) bool;
```
