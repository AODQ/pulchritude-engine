#include <pulchritude-task-graph/task-graph.h>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct TaskGraphNode {
  std::string label;
  std::unordered_map<uint64_t, void *> attributes;
  std::vector<uint64_t> relationDependsOn;
};

struct TaskGraph {
  PuleAllocator allocator;
  std::unordered_map<uint64_t, TaskGraphNode> nodes = {};
  std::vector<uint64_t> nodesInRelationOrder = {};
  bool needsResort = false;
};

std::unordered_map<uint64_t, TaskGraph> taskGraphs;
uint64_t taskGraphCounter = 1;

std::unordered_map<uint64_t, uint64_t> taskGraphNodeToGraph;

void sortGraphNodes(TaskGraph & graph) {
  if (!graph.needsResort) { return; }
  graph.needsResort = false;
  // iterate all nodes, inserting either at end, or when all relations have
  // been satisfied, greedily. Have to loop multiple times to resolve
  // dependencies that are iterated in reverse order
  // let's look at {A C B} and {B C A}, where B relies on A to execute
  // for {A C B}, on first iter {A}, then {A C}, then {A B C}
  // for {B C A}, on first iter {}, then {A}, then {A C} then {A B C}
  graph.nodesInRelationOrder = {};
  while (graph.nodes.size() > graph.nodesInRelationOrder.size()) {
    for (auto const & nodePair : graph.nodes) {
      uint64_t const nodeId = nodePair.first;
      TaskGraphNode const & node = nodePair.second;
      std::vector<uint8_t> satisfiedRelations;
      satisfiedRelations.resize(node.relationDependsOn.size(), false);
      // check node exists already
      for (size_t it = 0; it < graph.nodesInRelationOrder.size(); ++ it) {
        if (graph.nodesInRelationOrder[it] == nodeId) {
          goto NODE_EXISTS;
        }
      }
      // try to find a place to insert it
      for (size_t it = 0; it < graph.nodesInRelationOrder.size(); ++ it) {
        uint64_t const relatedNode = graph.nodesInRelationOrder[it];
        // if node already exists, skip.
        // check if this node satisfies a relation
        for (
          size_t relIt = 0;
          relIt < node.relationDependsOn.size();
          ++ relIt
        ) {
          if (relatedNode == node.relationDependsOn[relIt]) {
            satisfiedRelations[relIt] = true;
          }
        }
        // check if we are satisfied here
        bool satisfied = true;
        for (uint8_t const relation : satisfiedRelations) {
          satisfied = satisfied && relation;
        }
        if (satisfied) {
          graph.nodesInRelationOrder.insert(
            graph.nodesInRelationOrder.begin() + it + 1,
            nodeId
          );
          goto NODE_EXISTS;
        }
      }
      // see if we can insert at end
      if (satisfiedRelations.size() == 0) {
        graph.nodesInRelationOrder.emplace_back(nodeId);
      }
      NODE_EXISTS:;
    }
  }
  puleLogDebug(
    "task-graph sortGraphNodes finished processing %lu nodes",
    graph.nodes.size()
  );
  PULE_assert(graph.nodes.size() == graph.nodesInRelationOrder.size());
}

} // namespace

extern "C" {

PuleTaskGraph puleTaskGraphCreate(PuleAllocator const allocator) {
  TaskGraph graph = {
    .allocator = allocator,
    .nodes = {},
    .nodesInRelationOrder = {},
    .needsResort = false,
  };
  taskGraphs.emplace(taskGraphCounter, graph);
  return PuleTaskGraph { .id = taskGraphCounter ++, };
}


void puleTaskGraphDestroy(PuleTaskGraph const pGraph) {
  if (pGraph.id == 0) { return; }
  auto & graph = ::taskGraphs.at(pGraph.id);
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    ::taskGraphNodeToGraph.erase(nodeId);
  }
  ::taskGraphs.erase(pGraph.id);
}

void puleTaskGraphMerge(
  PuleTaskGraph const puPrimary, PuleTaskGraph const puSecondary
) {
  if (puSecondary.id == 0) {return;}
  ::TaskGraph & primary = ::taskGraphs.at(puPrimary.id);
  ::TaskGraph & secondary = ::taskGraphs.at(puSecondary.id);
  // copy over nodes, reset taskGraphNodeToGraph
  for (auto secondaryNodePair : secondary.nodes) {
    if (primary.nodes.contains(secondaryNodePair.first)) {
      puleLogError(
        "Merging graph with conflicting node ID, label '%s' vs '%s'",
        secondaryNodePair.first,
        secondaryNodePair.second.label.c_str(),
        primary.nodes.at(secondaryNodePair.first).label.c_str()
      );
      continue;
    }
    TaskGraphNode const secondaryNode = secondaryNodePair.second;
    std::string const newNodeHash = (
      secondaryNode.label + "--" + std::to_string(puPrimary.id)
    );
    uint64_t const newNodeId = (
      puleStringViewHash(puleCStr(newNodeHash.c_str()))
    );
    primary.nodes.emplace(newNodeId, secondaryNodePair.second);
    primary.needsResort = true;
    // move to new node id
    taskGraphNodeToGraph.erase(secondaryNodePair.first);
    taskGraphNodeToGraph.emplace(newNodeId, puPrimary.id);
  }
  puleTaskGraphDestroy(puSecondary);
}

PuleTaskGraphNode puleTaskGraphNodeCreate(
  PuleTaskGraph const pGraph, PuleStringView const label
) {
  TaskGraph & graph = ::taskGraphs.at(pGraph.id);
  // create ID by hashing label with graph ID
  std::string const labelToHash = (
    std::string(label.contents) + "--" + std::to_string(pGraph.id)
  );
  uint64_t const id = puleStringViewHash(puleCStr(labelToHash.c_str()));

  // set ID mappings
  assert(!taskGraphNodeToGraph.contains(id));
  taskGraphNodeToGraph.emplace(id, pGraph.id);
  graph.nodes.emplace(
    id,
    TaskGraphNode {
      .label = std::string(label.contents),
      .attributes = {},
      .relationDependsOn = {},
    }
  );
  graph.needsResort = true;
  return PuleTaskGraphNode { .id = id, };
}

void puleTaskGraphNodeRemove(PuleTaskGraphNode const node) {
  TaskGraph & graph = ::taskGraphs.at(taskGraphNodeToGraph.at(node.id));
  taskGraphNodeToGraph.erase(node.id);
  graph.nodes.erase(node.id);
  graph.needsResort = true;
}

PuleStringView puleTaskGraphNodeLabel(PuleTaskGraphNode const pNode) {
  TaskGraph & graph = ::taskGraphs.at(taskGraphNodeToGraph.at(pNode.id));
  TaskGraphNode & node = graph.nodes.at(pNode.id);
  return puleCStr(node.label.c_str());
}

PuleTaskGraphNode puleTaskGraphNodeFetch(
  PuleTaskGraph const pGraph, PuleStringView const label
) {
  TaskGraph & graph = ::taskGraphs.at(pGraph.id);
  std::string const labelToHash = (
    std::string(label.contents) + "--" + std::to_string(pGraph.id)
  );
  uint64_t const id = puleStringViewHash(puleCStr(labelToHash.c_str()));
  assert(graph.nodes.contains(id));
  return PuleTaskGraphNode { .id = id, };
}

void * puleTaskGraphNodeAttributeFetch(
  PuleTaskGraphNode const pNode,
  PuleStringView const label
) {
  TaskGraph & graph = ::taskGraphs.at(taskGraphNodeToGraph.at(pNode.id));
  TaskGraphNode & node = graph.nodes.at(pNode.id);
  return node.attributes.at(puleStringViewHash(label));
}

uint64_t puleTaskGraphNodeAttributeFetchU64(
  PuleTaskGraphNode const graphNode,
  PuleStringView const label
) {
  return (
    reinterpret_cast<uint64_t>(
      puleTaskGraphNodeAttributeFetch(graphNode, label)
    )
  );
}

void puleTaskGraphNodeAttributeStore(
  PuleTaskGraphNode const pNode,
  PuleStringView const label,
  void * const attributeData
) {
  TaskGraph & graph = ::taskGraphs.at(taskGraphNodeToGraph.at(pNode.id));
  TaskGraphNode & node = graph.nodes.at(pNode.id);
  node.attributes.emplace(puleStringViewHash(label), attributeData);
}
void puleTaskGraphNodeAttributeStoreU64(
  PuleTaskGraphNode const pGraphNode,
  PuleStringView const label,
  uint64_t const attributeData
) {
  puleTaskGraphNodeAttributeStore(
    pGraphNode, label, reinterpret_cast<void *>(attributeData)
  );
}

void puleTaskGraphNodeAttributeRemove(
  PuleTaskGraphNode const pGraphNode,
  PuleStringView const label
) {
  TaskGraph & graph = ::taskGraphs.at(taskGraphNodeToGraph.at(pGraphNode.id));
  TaskGraphNode & node = graph.nodes.at(pGraphNode.id);
  node.attributes.erase(puleStringViewHash(label));
}

// void puleTaskGraphNodeAttributeLabels(
//   PuleTaskGraphNode const pGraphNode,
//   size_t * viewsArrayLength,
//   PuleStringView * viewsArray
// ) {
//   TaskGraph & graph = ::taskGraphs.at(taskGraphNodeToGraph.at(pGraphNode.id));
//   TaskGraphNode & node = graph.nodes.at(pGraphNode.id);
//   if (viewsArray == nullptr) {
//     PULE_assert(viewsArrayLength != nullptr);
//     viewsArrayLength = node.attributes
//   }
//   node.at
// }

void puleTaskGraphNodeRelationSet(
  PuleTaskGraphNode const pNodePri,
  PuleTaskGraphNodeRelation const relation,
  PuleTaskGraphNode const pNodeSec
) {
  assert(relation == PuleTaskGraphNodeRelation_dependsOn);
  TaskGraph & graph = ::taskGraphs.at(taskGraphNodeToGraph.at(pNodePri.id));
  TaskGraphNode & nodePri = graph.nodes.at(pNodePri.id);
  nodePri.relationDependsOn.emplace_back(pNodeSec.id);
}

void puleTaskGraphExecuteInOrder(PuleTaskGraphExecuteInfo const execute) {
  assert(!execute.multithreaded && "multithreaded support not yet implemented");
  TaskGraph & graph = ::taskGraphs.at(execute.graph.id);
  sortGraphNodes(graph);
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    execute.callback(PuleTaskGraphNode { .id = nodeId, }, execute.userdata);
  }
}

bool puleTaskGraphNodeExists(PuleTaskGraphNode const node) {
  return ::taskGraphNodeToGraph.find(node.id) != ::taskGraphNodeToGraph.end();
}

} // extern C
