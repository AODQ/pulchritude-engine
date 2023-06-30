#include <pulchritude-render-graph/render-graph.h>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace {

struct RenderGraphNode {
  std::string label;
  std::unordered_map<std::thread::id, uint64_t> perThreadCommandList;
  std::vector<PuleGfxCommandList> commandLists;
  std::unordered_map<uint64_t, PuleRenderGraph_Resource> resources;
  std::unordered_map<uint64_t, std::string> resourceLabels;
  std::vector<uint64_t> relationDependsOn;
};

struct RenderGraph {
  PuleAllocator allocator;
  std::unordered_map<uint64_t, RenderGraphNode> nodes = {};
  std::vector<uint64_t> nodesInRelationOrder = {};
  bool needsResort = false;
};

std::unordered_map<uint64_t, RenderGraph> renderGraphs;
uint64_t renderGraphCounter = 1;

std::unordered_map<uint64_t, uint64_t> renderGraphNodeToGraph;

void nodeDependencyCalculateResourceIntersection(
  RenderGraphNode & node,
  RenderGraphNode & dependNode
) {
  for (auto & resourcePair : node.resources) {
    auto dependNodeResourceIt = (
      dependNode.resources.find(resourcePair.first)
    );
    if (dependNodeResourceIt == dependNode.resources.end()) { continue; }
    PuleRenderGraph_Resource & resource = resourcePair.second;
    PuleRenderGraph_Resource const & dependResource = (
      dependNodeResourceIt->second
    );
    PULE_assert(resource.resourceType == dependResource.resourceType);
    switch (resource.resourceType) {
      case PuleRenderGraph_ResourceType_image: {
        auto & image = resource.image;
        auto & depImage = dependResource.image;
        image.entrancePayloadAccess = depImage.exittancePayloadAccess;
        image.entrancePayloadLayout = depImage.exittancePayloadLayout;
      } break;
      case PuleRenderGraph_ResourceType_buffer:
      break;
    }
  }
}

void sortGraphNodes(RenderGraph & graph) {
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
      RenderGraphNode const & node = nodePair.second;
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
  // calculate resource relationship intersection
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    RenderGraphNode & node = graph.nodes.at(nodeId);
    for (uint64_t const dependNodeId : node.relationDependsOn) {
      RenderGraphNode & dependNode = graph.nodes.at(dependNodeId);
      nodeDependencyCalculateResourceIntersection(node, dependNode);
    }
  }
  PULE_assert(graph.nodes.size() == graph.nodesInRelationOrder.size());
}

} // namespace

extern "C" {

PuleRenderGraph puleRenderGraphCreate(PuleAllocator const allocator) {
  RenderGraph graph = {
    .allocator = allocator,
    .nodes = {},
    .nodesInRelationOrder = {},
    .needsResort = false,
  };
  renderGraphs.emplace(renderGraphCounter, graph);
  return PuleRenderGraph { .id = renderGraphCounter ++, };
}

void puleRenderGraphDestroy(PuleRenderGraph const pGraph) {
  if (pGraph.id == 0) { return; }
  auto & graph = ::renderGraphs.at(pGraph.id);
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    ::renderGraphNodeToGraph.erase(nodeId);
  }
  ::renderGraphs.erase(pGraph.id);
}

void puleRenderGraphMerge(
  PuleRenderGraph const puPrimary, PuleRenderGraph const puSecondary
) {
  if (puSecondary.id == 0) {return;}
  ::RenderGraph & primary = ::renderGraphs.at(puPrimary.id);
  ::RenderGraph & secondary = ::renderGraphs.at(puSecondary.id);
  // fix relations to be the future correct node id
  for (auto & secondaryNodePair : secondary.nodes) {
    RenderGraphNode & secondaryNode = secondaryNodePair.second;
    for (size_t rit = 0; rit < secondaryNode.relationDependsOn.size(); ++ rit) {
      uint64_t const relationNodeId = secondaryNode.relationDependsOn[rit];
      auto & relatedNode = secondary.nodes.at(relationNodeId);
      std::string const newRelatedNodeHash = (
        relatedNode.label + "--" + std::to_string(puPrimary.id)
      );
      uint64_t const newRelatedNodeId = (
        puleStringViewHash(puleCStr(newRelatedNodeHash.c_str()))
      );
      secondaryNode.relationDependsOn[rit] = newRelatedNodeId;
    }
  }
  // copy over nodes, reset renderGraphNodeToGraph
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
    RenderGraphNode const secondaryNode = secondaryNodePair.second;
    std::string const newNodeHash = (
      secondaryNode.label + "--" + std::to_string(puPrimary.id)
    );
    uint64_t const newNodeId = (
      puleStringViewHash(puleCStr(newNodeHash.c_str()))
    );
    primary.nodes.emplace(newNodeId, secondaryNodePair.second);
    primary.needsResort = true;
    // move to new node id
    renderGraphNodeToGraph.erase(secondaryNodePair.first);
    renderGraphNodeToGraph.emplace(newNodeId, puPrimary.id);
  }
  puleRenderGraphDestroy(puSecondary);
}

PuleRenderGraphNode puleRenderGraphNodeCreate(
  PuleRenderGraph const pGraph, PuleStringView const label
) {
  RenderGraph & graph = ::renderGraphs.at(pGraph.id);
  // create ID by hashing label with graph ID
  std::string const labelToHash = (
    std::string(label.contents) + "--" + std::to_string(pGraph.id)
  );
  uint64_t const id = puleStringViewHash(puleCStr(labelToHash.c_str()));

  // set ID mappings
  assert(!renderGraphNodeToGraph.contains(id));
  renderGraphNodeToGraph.emplace(id, pGraph.id);
  graph.nodes.emplace(
    id,
    RenderGraphNode {
      .label = std::string(label.contents),
      .perThreadCommandList = {},
      .commandLists = {},
      .resources = {},
      .resourceLabels = {},
      .relationDependsOn = {},
    }
  );
  graph.needsResort = true;
  return PuleRenderGraphNode { .id = id, };
}

void puleRenderGraphNodeRemove(PuleRenderGraphNode const node) {
  RenderGraph & graph = ::renderGraphs.at(renderGraphNodeToGraph.at(node.id));
  renderGraphNodeToGraph.erase(node.id);
  graph.nodes.erase(node.id);
  graph.needsResort = true;
}

PuleStringView puleRenderGraphNodeLabel(PuleRenderGraphNode const pNode) {
  RenderGraph & graph = ::renderGraphs.at(renderGraphNodeToGraph.at(pNode.id));
  RenderGraphNode & node = graph.nodes.at(pNode.id);
  return puleCStr(node.label.c_str());
}

PuleRenderGraphNode puleRenderGraphNodeFetch(
  PuleRenderGraph const pGraph, PuleStringView const label
) {
  [[maybe_unused]] RenderGraph & graph = ::renderGraphs.at(pGraph.id);
  std::string const labelToHash = (
    std::string(label.contents) + "--" + std::to_string(pGraph.id)
  );
  uint64_t const id = puleStringViewHash(puleCStr(labelToHash.c_str()));
  PULE_assert(graph.nodes.contains(id));
  return PuleRenderGraphNode { .id = id, };
}

void puleRenderGraph_resourceAssign(
  PuleRenderGraphNode const pNode,
  PuleStringView const resourceLabel,
  PuleRenderGraph_Resource const resource
) {
  PULE_assert(resourceLabel.len > 0);
  RenderGraph & graph = ::renderGraphs.at(renderGraphNodeToGraph.at(pNode.id));
  RenderGraphNode & node = graph.nodes.at(pNode.id);
  node.resources.emplace(puleStringViewHash(resourceLabel), resource);
  node.resourceLabels.emplace(
    puleStringViewHash(resourceLabel),
    std::string(resourceLabel.contents, resourceLabel.len)
  );
}

PuleRenderGraph_Resource puleRenderGraph_resource(
  PuleRenderGraphNode const pNode,
  PuleStringView const resourceLabel
) {
  RenderGraph & graph = ::renderGraphs.at(renderGraphNodeToGraph.at(pNode.id));
  RenderGraphNode & node = graph.nodes.at(pNode.id);
  return node.resources.at(puleStringViewHash(resourceLabel));
}

void puleRenderGraph_resourceRemove(
  PuleRenderGraphNode const pGraphNode,
  PuleStringView const resourceLabel
) {
  RenderGraph & graph = (
    ::renderGraphs.at(renderGraphNodeToGraph.at(pGraphNode.id))
  );
  RenderGraphNode & node = graph.nodes.at(pGraphNode.id);
  node.resources.erase(puleStringViewHash(resourceLabel));
}

PuleGfxCommandList puleRenderGraph_commandList(
  PuleRenderGraphNode const pNode,
  uint64_t (*fetchResourceHandle)(
    PuleStringView const label, void * const data
  ),
  void * const userdata
) {
  RenderGraph & graph = ::renderGraphs.at(renderGraphNodeToGraph.at(pNode.id));
  RenderGraphNode & node = graph.nodes.at(pNode.id);
  auto threadId = std::this_thread::get_id();
  if (node.perThreadCommandList.contains(threadId)) {
    return node.commandLists[node.perThreadCommandList.at(threadId)];
  }
  std::string commandListStr = (
    "render-graph-" + node.label + "-command-list-thread-"
    + std::to_string(node.commandLists.size())
  );
  node.perThreadCommandList.emplace(threadId, node.commandLists.size());
  node.commandLists.emplace_back(
    puleGfxCommandListCreate(
      puleAllocateDefault(),
      puleCStr(commandListStr.c_str())
    )
  );
  PULE_assert(
    puleGfxCommandListRecorder(
      node.commandLists.back(),
      puleRenderGraph_commandPayload(
        pNode,
        puleAllocateDefault(),
        fetchResourceHandle,
        userdata
      )
    ).id == node.commandLists.back().id
  );
  return node.commandLists.back();
}

PuleGfxCommandListRecorder puleRenderGraph_commandListRecorder(
  PuleRenderGraphNode const node,
  uint64_t (*fetchResourceHandle)(
    PuleStringView const label, void * const data
  ),
  void * const userdata
) {
  return (
    PuleGfxCommandListRecorder {
      puleRenderGraph_commandList(node, fetchResourceHandle, userdata).id,
    }
  );
}

PuleGfxCommandPayload puleRenderGraph_commandPayload(
  PuleRenderGraphNode const pGraphNode,
  PuleAllocator const payloadAllocator,
  uint64_t (*fetchResourceHandle)(PuleStringView const label, void * const data),
  void * const userdata
) {
  RenderGraph & graph = (
    ::renderGraphs.at(renderGraphNodeToGraph.at(pGraphNode.id))
  );
  RenderGraphNode & node = graph.nodes.at(pGraphNode.id);
  PuleGfxCommandPayload payload;
  PuleArray payloadImages = (
    puleArray(
      PuleArrayCreateInfo {
        .elementByteLength = sizeof(PuleGfxCommandPayloadImage),
        .elementAlignmentByteLength = 0,
        .allocator = payloadAllocator,
      }
    )
  );
  for (auto & resourcePair : node.resources) {
    PuleRenderGraph_Resource const & resource = resourcePair.second;
    uint64_t const handle = (
      fetchResourceHandle(
        puleCStr(node.resourceLabels.at(resourcePair.first).c_str()),
        userdata
      )
    );
    // TODO check all potential swapchain images
    // bool const isSwapchainImage = (handle == puleGfxWindowImage().id);
    switch (resource.resourceType) {
      case PuleRenderGraph_ResourceType_image: {
        auto * payloadImage = (
          reinterpret_cast<PuleGfxCommandPayloadImage *>(
            puleArrayAppend(&payloadImages)
          )
        );
        PULE_assert(payloadImage);
        *payloadImage = (
          PuleGfxCommandPayloadImage {
            .image = PuleGfxGpuImage { .id = handle, },
            .access = resource.image.entrancePayloadAccess,
            .layout = resource.image.entrancePayloadLayout,
          }
        );
      } break;
      case PuleRenderGraph_ResourceType_buffer:
      break;
    }
  }
  return payload;
}

void puleRenderGraphNodeRelationSet(
  PuleRenderGraphNode const pNodePri,
  PuleRenderGraphNodeRelation const relation,
  PuleRenderGraphNode const pNodeSec
) {
  PULE_assert(relation == PuleRenderGraphNodeRelation_dependsOn);
  RenderGraph & graph = ::renderGraphs.at(renderGraphNodeToGraph.at(pNodePri.id));
  RenderGraphNode & nodePri = graph.nodes.at(pNodePri.id);
  nodePri.relationDependsOn.emplace_back(pNodeSec.id);
}

void puleRenderGraphFrameStart(PuleRenderGraph const pGraph) {
  auto & graph = renderGraphs.at(pGraph.id);
  sortGraphNodes(graph);
  for (auto & nodePair : graph.nodes) {
    // reset command lists TODO maybe reuse?
    for (size_t it = 0; it < nodePair.second.commandLists.size(); ++ it) {
      puleGfxCommandListDestroy(nodePair.second.commandLists[it]);
    }
    nodePair.second.commandLists = {};
    nodePair.second.perThreadCommandList = {};
  }
}

void puleRenderGraphFrameEnd(PuleRenderGraph const pGraph) {
  auto & graph = renderGraphs.at(pGraph.id);
  sortGraphNodes(graph);
  PuleError err = puleError();
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    auto const & node = graph.nodes.at(nodeId);
    for (auto const commandList : node.commandLists) {
      puleGfxCommandListRecorderFinish(
        PuleGfxCommandListRecorder { .id = commandList.id, }
      );
      puleGfxCommandListSubmit({
        .commandList = commandList,
        .fenceTargetStart = nullptr,
        .fenceTargetFinish = nullptr,
      }, &err);
      puleErrorConsume(&err);
    }
  }
}

void puleRenderGraphExecuteInOrder(PuleRenderGraphExecuteInfo const execute) {
  assert(!execute.multithreaded && "multithreaded support not yet implemented");
  RenderGraph & graph = ::renderGraphs.at(execute.graph.id);
  sortGraphNodes(graph);
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    execute.callback(PuleRenderGraphNode { .id = nodeId, }, execute.userdata);
  }
}

bool puleRenderGraphNodeExists(PuleRenderGraphNode const node) {
  return ::renderGraphNodeToGraph.find(node.id) != ::renderGraphNodeToGraph.end();
}

} // extern C
