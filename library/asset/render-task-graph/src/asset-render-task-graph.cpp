#include <pulchritude-asset/render-task-graph.h>

#include <pulchritude-asset/pds.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/commands.h>


#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace {
union GraphResource {
  uint64_t id;
  PuleGfxGpuImage image;
  PuleGfxGpuBuffer buffer;
  PuleGfxFramebuffer framebuffer;
  PuleGfxCommandList commandList;
  PuleGfxCommandListRecorder commandListRecorder;
};

PuleGfxCommandListRecorder createCommandListRecorder(
  PuleDsValue const resourceValue,
  std::unordered_map<std::string, GraphResource> const & resources
) {
  PuleStringView const label = (
    puleDsMemberAsString(resourceValue, "label")
  );
  if (resources.contains(std::string(label.contents))) {
    return resources.at(std::string(label.contents)).commandListRecorder;
  }
  PuleStringView const commandListLabel = (
    puleDsMemberAsString(resourceValue, "command-list")
  );
  (void)commandListLabel;
  puleLogDebug(
    "%s: Creating command-list-recorder resource: '%s'",
    __FUNCTION__, label.contents
  );
  /* resources[std::string(label.contents)] = resource; */
  assert(false && "unimplemented");
}
PuleGfxCommandList createCommandList(
  PuleAllocator const allocator,
  PuleDsValue const resourceValue,
  std::unordered_map<std::string, GraphResource> & resources
) {
  (void)resources;
  PuleStringView const label = (
    puleDsMemberAsString(resourceValue, "label")
  );
  if (resources.contains(std::string(label.contents))) {
    return resources.at(std::string(label.contents)).commandList;
  }
  puleLogDebug(
    "%s Creating command-list-recorder resource: '%s'",
    __FUNCTION__, label.contents
  );
  PuleGfxCommandList const commandList = (
    puleGfxCommandListCreate(
      allocator,
      label
    )
  );
  resources[std::string(label.contents)].commandList = commandList;
  return commandList;
}

void actionClearFramebufferColor(
  PuleDsValue const actionValue,
  std::unordered_map<std::string, GraphResource> const & resources,
  PuleGfxCommandListRecorder const recorder
) {
  PuleStringView const framebufferLabel = (
    puleDsMemberAsString(actionValue, "framebuffer")
  );

  PuleDsValueArray const rgba = (
    puleDsMemberAsArray(actionValue, "rgba")
  );

  PuleGfxFramebuffer framebuffer = { .id = 0, };
  if (!puleStringViewEqCStr(framebufferLabel, "window-framebuffer")) {
    framebuffer = (
      resources.at(std::string(framebufferLabel.contents)).framebuffer
    );
  }

  puleGfxCommandListAppendAction(
    recorder,
    PuleGfxCommand {
      .clearFramebufferColor = {
        .action = PuleGfxAction_clearFramebufferColor,
        .framebuffer = framebuffer,
        .color = {
          .x = puleDsAsF32(rgba.values[0]),
          .y = puleDsAsF32(rgba.values[1]),
          .z = puleDsAsF32(rgba.values[2]),
          .w = puleDsAsF32(rgba.values[3]),
        },
      },
    }
  );
}

void actionClearFramebufferDepth(
  PuleDsValue const actionValue,
  std::unordered_map<std::string, GraphResource> const & resources,
  PuleGfxCommandListRecorder const recorder
) {
  PuleStringView const framebufferLabel = (
    puleDsMemberAsString(actionValue, "framebuffer")
  );

  float const depth = (
    puleDsMemberAsF32(actionValue, "depth")
  );

  PuleGfxFramebuffer framebuffer = { .id = 0, };
  if (!puleStringViewEqCStr(framebufferLabel, "window-framebuffer")) {
    framebuffer = (
      resources.at(std::string(framebufferLabel.contents)).framebuffer
    );
  }

  puleGfxCommandListAppendAction(
    recorder,
    PuleGfxCommand {
      .clearFramebufferDepth = {
        .action = PuleGfxAction_clearFramebufferDepth,
        .framebuffer = framebuffer,
        .depth = depth,
      },
    }
  );
}

} // namespace

PULE_exportFn PuleTaskGraph puleAssetRenderTaskGraphFromPds(
  PuleAllocator const allocator,
  [[maybe_unused]] PulePlatform const platform,
  PuleDsValue const dsRenderGraphValue
) {
  PuleTaskGraph const graph = puleTaskGraphCreate(allocator);
  PuleDsValueArray const dsGraphNodes = (
    puleDsMemberAsArray(dsRenderGraphValue, "task-graph")
  );
  // first create nodes
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    puleTaskGraphNodeCreate(
      graph,
      puleDsMemberAsString(dsGraphNode, "label")
    );
  }

  // create relationships
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    PuleDsValueArray const dsDependsOn = (
      puleDsMemberAsArray(dsGraphNode, "depends-on")
    );
    for (size_t depIt = 0; depIt < dsDependsOn.length; ++ depIt) {
      puleTaskGraphNodeRelationSet(
        puleTaskGraphNodeFetch(
          graph,
          puleDsMemberAsString(dsGraphNode, "label")
        ),
        PuleTaskGraphNodeRelation_dependsOn,
        puleTaskGraphNodeFetch(
          graph,
          puleDsAsString(dsDependsOn.values[depIt])
        )
      );
    }
  }

  // -- create attributess

  // create new resources
  std::unordered_map<std::string, GraphResource> resources;

  // create default command lists
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleStringView const label = (
      puleDsMemberAsString(dsGraphNodes.values[it], "label")
    );
    PuleGfxCommandList const commandListPrimary = (
      puleGfxCommandListCreate(
        allocator,
        puleCStr((std::string(label.contents) + "-primary").c_str())
      )
    );
    PuleGfxCommandList const commandListStartup = (
      puleGfxCommandListCreate(
        allocator,
        puleCStr((std::string(label.contents) + "-startup").c_str())
      )
    );
    PuleGfxCommandListRecorder const commandListStartupRecorder = (
      puleGfxCommandListRecorder(commandListStartup)
    );
    PuleGfxCommandListRecorder const commandListPrimaryRecorder = (
      puleGfxCommandListRecorder(commandListPrimary)
    );
    resources.emplace(
      std::string(label.contents) + "-primary",
      GraphResource { .commandList = commandListPrimary, }
    );
    resources.emplace(
      std::string(label.contents) + "-startup",
      GraphResource { .commandList = commandListStartup, }
    );
    resources.emplace(
      std::string(label.contents) + "-primary",
      GraphResource { .commandListRecorder = commandListPrimaryRecorder, }
    );
    resources.emplace(
      std::string(label.contents) + "-startup",
      GraphResource { .commandListRecorder = commandListStartupRecorder, }
    );
  }

  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    PuleTaskGraphNode const graphNode = (
      puleTaskGraphNodeFetch(
        graph,
        puleDsMemberAsString(dsGraphNode, "label")
      )
    );
    PuleDsValueArray const dsAttributes = (
      puleDsMemberAsArray(dsGraphNode, "attributes")
    );
    for (size_t attrIt = 0; attrIt < dsAttributes.length; ++ attrIt) {
      PuleDsValue const dsNewResource = (
        puleDsObjectMember(dsAttributes.values[attrIt], "resource")
      );
      if (puleDsIsNull(dsNewResource)) { continue; }
      PuleStringView const dsNewResourceType = (
        puleDsMemberAsString(dsNewResource, "type")
      );
      PuleStringView const label = (
        puleDsMemberAsString(dsNewResource, "label")
      );
      GraphResource resource;
      if (puleStringViewEqCStr(dsNewResourceType, "command-list-recorder")) {
        resource.commandListRecorder = (
          ::createCommandListRecorder(dsNewResource, resources)
        );
      } else if (puleStringViewEqCStr(dsNewResourceType, "command-list")) {
        resource.commandList = (
          ::createCommandList(allocator, dsNewResource, resources)
        );
      } else {
        puleLogError(
          "unsupported new-resource type: %s", dsNewResourceType.contents
        );
        PULE_assert(false && "unsupported new-resource type");
        // .. TODO buffer, image, etc
      }
      puleTaskGraphNodeAttributeStoreU64(graphNode, label, resource.id);
    }
  }

  // assign resources
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleTaskGraphNode const graphNode = (
      puleTaskGraphNodeFetch(
        graph,
        puleDsMemberAsString(dsGraphNodes.values[it], "label")
      )
    );
    PuleDsValueArray const dsAttributes = (
      puleDsMemberAsArray(dsGraphNodes.values[it], "attributes")
    );
    for (size_t attrIt = 0; attrIt < dsAttributes.length; ++ attrIt) {
      PuleDsValue const dsRes = (
        puleDsObjectMember(dsAttributes.values[attrIt], "resource")
      );
      if (puleDsIsNull(dsRes)) { continue; }
      PuleStringView const dsResType = puleDsMemberAsString(dsRes, "type");
      PuleStringView const label = puleDsMemberAsString(dsRes, "label");
      (void)dsResType; // TODO assert this is correct
      puleTaskGraphNodeAttributeStoreU64(
        graphNode, label, resources.at(std::string(label.contents)).id
      );
    }
  }

  // create command lists
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleStringView const label = (
      puleDsMemberAsString(dsGraphNodes.values[it], "label")
    );
    PuleTaskGraphNode const graphNode = puleTaskGraphNodeFetch(graph, label);
    PuleDsValueArray const dsAttributes = (
      puleDsMemberAsArray(dsGraphNodes.values[it], "attributes")
    );
    auto const commandListPrimary = PuleGfxCommandList {
      resources.at(std::string(label.contents) + "-primary").commandList
    };
    auto const commandListStartup = PuleGfxCommandList {
      resources.at(std::string(label.contents) + "-startup").commandList
    };
    auto const commandListPrimaryRecorder = PuleGfxCommandListRecorder {
      resources.at(std::string(label.contents) + "-primary").commandListRecorder
    };
    auto const commandListStartupRecorder = PuleGfxCommandListRecorder {
      resources.at(std::string(label.contents) + "-startup").commandListRecorder
    };
    for (size_t attrIt = 0; attrIt < dsAttributes.length; ++ attrIt) {
      PuleDsValue const dsAction = (
        puleDsObjectMember(dsAttributes.values[attrIt], "action")
      );
      if (puleDsIsNull(dsAction)) { continue; }
      PuleStringView const dsActionType = (
        puleDsMemberAsString(dsAction, "type")
      );
      if (puleStringViewEqCStr(dsActionType, "clear-framebuffer-color")) {
        ::actionClearFramebufferColor(
          dsAction, resources, commandListStartupRecorder
        );
      }
      if (puleStringViewEqCStr(dsActionType, "clear-framebuffer-depth")) {
        ::actionClearFramebufferDepth(
          dsAction, resources, commandListStartupRecorder
        );
      }
    }
    puleGfxCommandListRecorderFinish(commandListStartupRecorder);
    puleTaskGraphNodeAttributeStoreU64(
      graphNode, puleCStr("command-list-startup"),
      commandListStartup.id
    );
    puleTaskGraphNodeAttributeStoreU64(
      graphNode, puleCStr("command-list-primary"),
      commandListPrimary.id
    );
    puleTaskGraphNodeAttributeStoreU64(
      graphNode, puleCStr("command-list-primary-recorder"),
      commandListPrimaryRecorder.id
    );
  }

  return graph;
}
