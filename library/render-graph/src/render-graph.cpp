#include <pulchritude-render-graph/render-graph.h>

#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace {

size_t uniqueThreadId() {
  static std::unordered_map<std::thread::id, size_t> threadIdMap;
  static size_t threadIdCounter = 0;
  static std::mutex threadIdMapMutex;
  std::thread::id const threadId = std::this_thread::get_id();
  std::lock_guard<std::mutex> lock(threadIdMapMutex);
  auto const threadIdIt = threadIdMap.find(threadId);
  if (threadIdIt == threadIdMap.end()) {
    return threadIdMap[threadId] = threadIdCounter ++;
  }
  return threadIdIt->second;
}

}

namespace {

struct RenderGraphNode {
  std::string label;
  std::unordered_map<
    std::thread::id, PuleGpuCommandListChain
  > perThreadCommandList;
  PuleGpuCommandListChain transitionCommandListChainEnter;
  PuleGpuCommandListChain transitionCommandListChainExit;
  std::unordered_map<uint64_t, PuleRenderGraph_Resource> resources;
  std::unordered_map<uint64_t, std::string> resourceLabels;
  std::vector<uint64_t> relationDependsOn;
  bool usesSwapchain;
  bool isLastSwapchainUsage;
};

struct RenderGraph {
  PuleAllocator allocator;
  std::unordered_map<uint64_t, RenderGraphNode> nodes = {};
  std::vector<uint64_t> nodesInRelationOrder = {};
  bool needsResort = false;
  uint64_t lastSwapchainUsageNodeId = 0;
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
    // tie dependNode resource to the current node, so for example
    // if current node resource layout is "attachment-color", then we
    // know that for any nodes that depend on it, they must transition
    // from whatever their current layout is to "attachment-color".
    // one problem with this is that there is a one-to-many relationship
    // between nodes, a node could have many image barriers on the same
    // image, and I have no idea how to handle that.
    // though theoretically this would be illegal behavior as parallel
    // operations would occur on the same resource in this scenario.
    PuleRenderGraph_Resource & resource = resourcePair.second;
    PuleRenderGraph_Resource const & dependResource = (
      dependNodeResourceIt->second
    );
    PULE_assert(resource.resourceType == dependResource.resourceType);
    switch (resource.resourceType) {
      case PuleRenderGraph_ResourceType_image: {
        auto & image = resource.image;
        auto & depImage = dependResource.image;
        image.payloadAccessPreentrance = depImage.payloadAccessEntrance;
        image.payloadLayoutPreentrance = depImage.payloadLayoutEntrance;
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
  PULE_assert(graph.nodes.size() == graph.nodesInRelationOrder.size());
  // calculate resource relationship intersection
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    RenderGraphNode & node = graph.nodes.at(nodeId);
    for (uint64_t const dependNodeId : node.relationDependsOn) {
      RenderGraphNode & dependNode = graph.nodes.at(dependNodeId);
      // TODO it doesn't work if, given node graph A->B->C, A uses
      //      resource X, C uses X, but B does not use X. In this case,
      //      C will not have found X in A, and so will have an undefined
      //      layout.
      nodeDependencyCalculateResourceIntersection(node, dependNode);
    }
    if (node.relationDependsOn.size() == 0) {
      // node has no dependencies, thus it's root. Preentrance resources
      // should be in undefined layouts on the first pass, but in future
      // passes they will have the layout of the last node that uses them.
      // Set preentrance to the entrance layout of the last node whose
      // chain of dependencies leads to this node.

      // To do this, build a list of all nodes that depend on this,
      // by building a temporary list that firstly includes this node,
      // so every other node can check if it's dependent on this node.

      // As the nodes are in relation order, I don't think there will be
      // any nodes skipped which have a dependency on this node.

      std::vector<uint64_t> isDependent;
      isDependent.push_back(nodeId);
      for (uint64_t const relationNodeId : graph.nodesInRelationOrder) {
        RenderGraphNode const & relationNode = graph.nodes.at(relationNodeId);
        for (uint64_t const dependsOn : relationNode.relationDependsOn) {
          if (dependsOn == nodeId) {
            isDependent.push_back(relationNodeId);
            break;
          }
        }
      }

      // TODO CRIT
      // TODO I should just build, for each resource, a chain/list of nodes
      //      that use it, and then I can build the proper preentrance
      //      for all nodes in one pass, instead of having to iterate
      //      twice (once forward for 'forward' and one 'backwards' to complete
      //      the chain)

      // now we have a list of all nodes in execution order that depend on
      // this node, so just iterate backwards for each resource and check
      // if both are used by the same node, and if so, set the preentrance
      // to the entrance of that node (thus completing the lifecycle of that
      // resource in this frame)
      for (auto & resourcePair : node.resources) {
        // check if this resource is a window swapchain image
        if (
          node.resourceLabels.at(resourcePair.first) == "window-swapchain-image"
        ) {
          // we know the layout and access for this, and it can't be
          // computed implicitly as swapping isn't part of the render-graph
          // TODO on the first pass, this should be undefined
          resourcePair.second.image.payloadAccessPreentrance = (
            PuleGpuResourceAccess_none
          );
          resourcePair.second.image.payloadLayoutPreentrance = (
            PuleGpuImageLayout_presentSrc
          );
          continue;
        }

        // handle every other resource
        for (size_t depIt = 0; depIt < isDependent.size(); ++ depIt) {
          uint64_t const dependentNodeId = (
            isDependent[isDependent.size()-depIt-1]
          );
          RenderGraphNode const & dependentNode = (
            graph.nodes.at(dependentNodeId)
          );
          auto dependentNodeResourceIt = (
            dependentNode.resources.find(resourcePair.first)
          );
          if (dependentNodeResourceIt == dependentNode.resources.end()) {
            continue; // resource not used by this node, check next node
          }
          // resource used by this node, set preentrance to entrance of
          // this node
          PuleRenderGraph_Resource & resource = resourcePair.second;
          PuleRenderGraph_Resource const & dependResource = (
            dependentNodeResourceIt->second
          );
          switch (resource.resourceType) {
            case PuleRenderGraph_ResourceType_image: {
              auto & image = resource.image;
              auto & depImage = dependResource.image;
              image.payloadAccessPreentrance = depImage.payloadAccessEntrance;
              image.payloadLayoutPreentrance = depImage.payloadLayoutEntrance;
            } break;
            case PuleRenderGraph_ResourceType_buffer:
            break;
          }
          break; // onto next resource
        }
      }
    }
  }
  // find the last node that uses the swapchain image, and also check
  // if the node uses the swapchain image
  uint64_t lastSwapchainImageNode = 0;
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    RenderGraphNode & node = graph.nodes.at(nodeId);
    node.usesSwapchain = false;
    node.isLastSwapchainUsage = false;
    for (auto const & resourcePair : node.resourceLabels) {
      std::string const & label = resourcePair.second;
      if (label != "window-swapchain-image") { continue; }
      lastSwapchainImageNode = nodeId; // overwrite previous value
      node.usesSwapchain = true;
      break;
    }
  }
  if (lastSwapchainImageNode != 0) {
    graph.nodes.at(lastSwapchainImageNode).isLastSwapchainUsage = true;
    graph.lastSwapchainUsageNodeId = lastSwapchainImageNode;
  }
  puleLogDev(
    "Last swapchain image: %s",
    graph.nodes.at(graph.lastSwapchainUsageNodeId).label.c_str()
  );
  // create command list chains
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    RenderGraphNode & node = graph.nodes.at(nodeId);
    node.transitionCommandListChainEnter = (
      puleGpuCommandListChainCreate(
        graph.allocator,
        puleCStr(
          std::string(
            "transition-command-list-chain-enter-" + node.label
          )
          .c_str()
        )
      )
    );
    node.transitionCommandListChainExit = (
      puleGpuCommandListChainCreate(
        graph.allocator,
        puleCStr(
          std::string(
            "transition-command-list-chain-exit-" + node.label
          )
          .c_str()
        )
      )
    );
  }
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
    auto & node = graph.nodes.at(nodeId);
    puleGpuCommandListChainDestroy(node.transitionCommandListChainEnter);
    puleGpuCommandListChainDestroy(node.transitionCommandListChainExit);
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
      .transitionCommandListChainEnter = { .id = 0, },
      .transitionCommandListChainExit =  { .id = 0, },
      .resources = {},
      .resourceLabels = {},
      .relationDependsOn = {},
      .usesSwapchain = false,
      .isLastSwapchainUsage = false,
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

PuleGpuCommandList puleRenderGraph_commandList(
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
    return (
      puleGpuCommandListChainCurrent(node.perThreadCommandList.at(threadId))
    );
  }
  std::string commandListStr = (
    "render-graph-" + node.label + "-command-list-thread-"
    + std::to_string(uniqueThreadId())
  );
  node.perThreadCommandList.emplace(
    threadId,
    puleGpuCommandListChainCreate(
      puleAllocateDefault(),
      puleCStr(commandListStr.c_str())
    )
  );
  PuleGpuCommandList const commandList = (
    puleGpuCommandListChainCurrent(
      node.perThreadCommandList.at(threadId)
    )
  );

  // create command list recorder, same ID as command list
  PULE_assert(puleGpuCommandListRecorder(commandList).id == commandList.id);
  return commandList;
}

PuleGpuCommandListRecorder puleRenderGraph_commandListRecorder(
  PuleRenderGraphNode const node,
  uint64_t (*fetchResourceHandle)(
    PuleStringView const label, void * const data
  ),
  void * const userdata
) {
  return (
    PuleGpuCommandListRecorder {
      puleRenderGraph_commandList(node, fetchResourceHandle, userdata).id,
    }
  );
}

void puleRenderGraphNodeRelationSet(
  PuleRenderGraphNode const pNodePri,
  PuleRenderGraphNodeRelation const relation,
  PuleRenderGraphNode const pNodeSec
) {
  PULE_assert(relation == PuleRenderGraphNodeRelation_dependsOn);
  RenderGraph & graph = (
    ::renderGraphs.at(renderGraphNodeToGraph.at(pNodePri.id))
  );
  RenderGraphNode & nodePri = graph.nodes.at(pNodePri.id);
  nodePri.relationDependsOn.emplace_back(pNodeSec.id);
}

void puleRenderGraphFrameStart(
  PuleRenderGraph const pGraph,
  uint64_t (* const fetchResourceHandle)(
    PuleStringView const label, void * const data
  )
) {
  auto & graph = renderGraphs.at(pGraph.id);
  sortGraphNodes(graph);

  // create entrance and potentially exittance command list (for last node) for
  // resource transitioning. This is so multiple command lists can be executed
  // post-transition
  for (auto & nodePair : graph.nodes) {
    auto & node = nodePair.second;
    // build up barriers
    std::vector<PuleGpuResourceBarrierImage> barrierImages;
    for (auto & resourcePair : node.resources) {
      PuleRenderGraph_Resource & resource = resourcePair.second;
      switch (resource.resourceType) {
        case PuleRenderGraph_ResourceType_image: {
          auto const imageId = (
            fetchResourceHandle(
              puleCStr(node.resourceLabels.at(resourcePair.first).c_str()),
              nullptr // userdata
            )
          );
          barrierImages.emplace_back(
            PuleGpuResourceBarrierImage {
              .image = PuleGpuImage { .id = imageId, },
              .accessSrc = (
                resource.image.isInitialized
                   ? resource.image.payloadAccessPreentrance
                   : PuleGpuResourceAccess_none
              ),
              .accessDst = resource.image.payloadAccessEntrance,
              .layoutSrc = (
                resource.image.isInitialized
                  ? resource.image.payloadLayoutPreentrance
                  : PuleGpuImageLayout_uninitialized
              ),
              .layoutDst = resource.image.payloadLayoutEntrance,
            }
          );
        } break;
        case PuleRenderGraph_ResourceType_buffer:
        break;
      }
    }
    auto const transitionEntranceRecorder = (
      puleGpuCommandListRecorder(
        puleGpuCommandListChainCurrent(node.transitionCommandListChainEnter)
      )
    );
    puleGpuCommandListAppendAction(
      transitionEntranceRecorder,
      PuleGpuCommand {
        .resourceBarrier {
          .action = PuleGpuAction_resourceBarrier,
          .stageSrc = PuleGpuResourceBarrierStage_transfer,
          .stageDst = PuleGpuResourceBarrierStage_outputAttachmentColor,
          .resourceImageCount = barrierImages.size(),
          .resourceImages = barrierImages.data(),
        },
      }
    );
    puleGpuCommandListRecorderFinish(transitionEntranceRecorder);
  }
  // create exit command list for swapping swapchain images
  for (auto & nodePair : graph.nodes) {
    auto & node = nodePair.second;
    if (!node.isLastSwapchainUsage) { continue; }
    auto const swapchainResource = (
      node.resources.at(puleStringViewHash(puleCStr("window-swapchain-image")))
    );
    auto const transitionExittanceRecorder = (
      puleGpuCommandListRecorder(
        puleGpuCommandListChainCurrent(node.transitionCommandListChainExit)
      )
    );
    auto const barrierImage = (
      PuleGpuResourceBarrierImage {
        .image = PuleGpuImage { .id = puleGpuWindowImage().id, },
        .accessSrc = swapchainResource.image.payloadAccessEntrance,
        .accessDst = PuleGpuResourceAccess_none,
        .layoutSrc = swapchainResource.image.payloadLayoutEntrance,
        .layoutDst = PuleGpuImageLayout_presentSrc,
      }
    );
    puleGpuCommandListAppendAction(
      transitionExittanceRecorder,
      PuleGpuCommand {
        .resourceBarrier {
          .action = PuleGpuAction_resourceBarrier,
          .stageSrc = PuleGpuResourceBarrierStage_outputAttachmentColor,
          .stageDst = PuleGpuResourceBarrierStage_transfer,
          .resourceImageCount = 1,
          .resourceImages = &barrierImage,
        },
      }
    );
    puleGpuCommandListRecorderFinish(transitionExittanceRecorder);
  }
}

void puleRenderGraphFrameSubmit(
  PuleGpuSemaphore const swapchainImageSemaphore,
  PuleRenderGraph const pGraph
) {
  auto & graph = renderGraphs.at(pGraph.id);
  sortGraphNodes(graph);
  PuleError err = puleError();
  for (uint64_t const nodeId : graph.nodesInRelationOrder) {
    auto const & node = graph.nodes.at(nodeId);
    PuleGpuSemaphore entranceSignalEntranceSemaphore = { .id = 0, };
    { // submit entrance command list
      std::vector<PuleGpuSemaphore> waitSemaphores;
      std::vector<PuleGpuPipelineStage> waitSemaphoreStages;
      if (node.usesSwapchain) {
        waitSemaphores.emplace_back(swapchainImageSemaphore.id);
        waitSemaphoreStages.emplace_back(
          PuleGpuPipelineStage_colorAttachmentOutput
        );
      }
      puleGpuCommandListSubmit({
        .commandList = (
          puleGpuCommandListChainCurrent(node.transitionCommandListChainEnter)
        ),
        .waitSemaphoreCount = waitSemaphores.size(),
        .waitSemaphores = waitSemaphores.data(),
        .waitSemaphoreStages = waitSemaphoreStages.data(),
        .signalSemaphoreCount = 1,
        .signalSemaphores = &entranceSignalEntranceSemaphore,
        .fenceTargetFinish = (
          puleGpuCommandListChainCurrentFence(
            node.transitionCommandListChainEnter
          )
        ),
      }, &err);
    }
    puleErrorConsume(&err);
    std::vector<PuleGpuSemaphore> commandListSignalSemaphores;
    std::vector<PuleGpuPipelineStage> commandListSignalSemaphoreStages;
    size_t commandListIter = 0;
    for (auto const commandListChainPair : node.perThreadCommandList) {
      auto const commandListChain = commandListChainPair.second;
      auto const commandList = puleGpuCommandListChainCurrent(commandListChain);
      puleGpuCommandListRecorderFinish(
        PuleGpuCommandListRecorder { .id = commandList.id, }
      );
      puleLogDev("is last swapchain usage? %d", node.isLastSwapchainUsage);
      if (node.isLastSwapchainUsage) {
        std::string const semaphoreLabel = (
          "render-graph-signal-semaphore-" + std::to_string(commandListIter)
        );
        commandListSignalSemaphores.emplace_back(
          puleGpuSemaphoreCreate(puleCStr(semaphoreLabel.c_str()))
        );
        commandListSignalSemaphoreStages.emplace_back( // TODO use correct stage
          PuleGpuPipelineStage_colorAttachmentOutput
        );
        puleLogDev("signal semaphore");
      }
      PuleGpuPipelineStage const waitSemaphoreStage = (
        PuleGpuPipelineStage_bottom
      );
      puleGpuCommandListSubmit( PuleGpuCommandListSubmitInfo{
        .commandList = commandList,
        .waitSemaphoreCount = 1,
        .waitSemaphores = &entranceSignalEntranceSemaphore,
        .waitSemaphoreStages = &waitSemaphoreStage, // TODO
        .signalSemaphoreCount = 1,
        .signalSemaphores = &commandListSignalSemaphores.back(),
        .fenceTargetFinish = (
          puleGpuCommandListChainCurrentFence(commandListChain)
        ),
      }, &err);
      puleErrorConsume(&err);
      ++ commandListIter;
    }
    if (node.transitionCommandListChainExit.id != 0) {
      PuleGpuSemaphore signalSwapSemaphore = { .id = 0, };
      puleGpuCommandListSubmit(PuleGpuCommandListSubmitInfo {
        .commandList = (
          puleGpuCommandListChainCurrent(node.transitionCommandListChainExit)
        ),
        .waitSemaphoreCount = commandListSignalSemaphores.size(),
        .waitSemaphores = commandListSignalSemaphores.data(),
        .waitSemaphoreStages = commandListSignalSemaphoreStages.data(),
        .signalSemaphoreCount = 1,
        .signalSemaphores = &signalSwapSemaphore,
        .fenceTargetFinish = { 0 },
      }, &err);
      puleGpuFrameEnd(1, &signalSwapSemaphore);
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
