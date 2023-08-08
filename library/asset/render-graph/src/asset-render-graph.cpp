#include <pulchritude-asset/render-graph.h>

#include <pulchritude-asset/pds.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/commands.h>

#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace {
union GraphResource {
  uint64_t id;
  PuleGpuImage image;
  PuleGpuBuffer buffer;
  PuleGpuFramebuffer framebuffer;
  PuleGpuCommandList commandList;
  PuleGpuCommandListRecorder commandListRecorder;
};

PuleGpuResourceAccess toPayloadAccess(PuleStringView const view) {
  std::string const access = std::string(view.contents, view.len);
  uint64_t accessFlag = 0;
  if (access == "indirect-command-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_indirectCommandRead;
  }
  if (access == "index-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_indexRead;
  }
  if (access == "vertex-attribute-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_vertexAttributeRead;
  }
  if (access == "uniform-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_uniformRead;
  }
  if (access == "input-attachment-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_inputAttachmentRead;
  }
  if (access == "shader-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_shaderRead;
  }
  if (access == "shader-write") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_shaderWrite;
  }
  if (access == "attachment-color-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_attachmentColorRead;
  }
  if (access == "attachment-color-write") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_attachmentColorWrite;
  }
  if (access == "attachment-depth-stencil-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_attachmentDepthRead;
  }
  if (access == "attachment-depth-stencil-write") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_attachmentDepthWrite;
  }
  if (access == "transfer-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_transferRead;
  }
  if (access == "transfer-write") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_transferWrite;
  }
  if (access == "host-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_hostRead;
  }
  if (access == "host-write") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_hostWrite;
  }
  if (access == "memory-read") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_memoryRead;
  }
  if (access == "memory-write") {
    accessFlag |= (uint64_t)PuleGpuResourceAccess_memoryWrite;
  }
  PULE_assert(accessFlag != 0);
  return (PuleGpuResourceAccess)accessFlag;
}

PuleGpuImageLayout toPayloadLayout(PuleStringView const view) {
  std::string const layout = std::string(view.contents, view.len);
  if (layout == "uninitialized" or layout == "undefined") {
    return PuleGpuImageLayout_uninitialized;
  }
  if (layout == "storage") {
    return PuleGpuImageLayout_storage;
  }
  if (layout == "attachment-color") {
    return PuleGpuImageLayout_attachmentColor;
  }
  if (layout == "attachment-depth") {
    return PuleGpuImageLayout_attachmentDepth;
  }
  if (layout == "transfer-src") {
    return PuleGpuImageLayout_transferSrc;
  }
  if (layout == "transfer-dst") {
    return PuleGpuImageLayout_transferDst;
  }
  puleLogError("Unknown image layout: %s", layout.c_str());
  PULE_assert(false);
}

} // namespace

extern "C" {

PuleRenderGraph puleAssetRenderGraphFromPds(
  PuleAllocator const allocator,
  [[maybe_unused]] PulePlatform const platform,
  PuleDsValue const dsRenderGraphValue
) {
  PuleRenderGraph const graph = puleRenderGraphCreate(allocator);
  PuleDsValueArray const dsGraphNodes = (
    puleDsMemberAsArray(dsRenderGraphValue, "render-graph")
  );
  // first create nodes
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    puleRenderGraphNodeCreate(
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
      puleRenderGraphNodeRelationSet(
        puleRenderGraphNodeFetch(
          graph,
          puleDsMemberAsString(dsGraphNode, "label")
        ),
        PuleRenderGraphNodeRelation_dependsOn,
        puleRenderGraphNodeFetch(
          graph,
          puleDsAsString(dsDependsOn.values[depIt])
        )
      );
    }
  }

  // -- create resources
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    PuleRenderGraphNode const graphNode = (
      puleRenderGraphNodeFetch(
        graph,
        puleDsMemberAsString(dsGraphNode, "label")
      )
    );
    PuleDsValueArray const dsResources = (
      puleDsMemberAsArray(dsGraphNode, "resources")
    );
    for (size_t resIt = 0; resIt < dsResources.length; ++ resIt) {
      auto const dsNewResource = dsResources.values[resIt];
      PuleStringView const label = (
        puleDsMemberAsString(dsNewResource, "label")
      );
      auto const dsResourceImage = puleDsObjectMember(dsNewResource, "image");
      if (dsResourceImage.id != 0) {
        PuleStringView const entranceLayout = (
          puleDsMemberAsString(dsResourceImage, "layout")
        );
        PuleStringView const entranceAccess = (
          puleDsMemberAsString(dsResourceImage, "access")
        );
        puleRenderGraph_resourceAssign(
          graphNode,
          label,
          PuleRenderGraph_Resource {
            .image = {
              .payloadAccessEntrance = toPayloadAccess(entranceAccess),
              .payloadAccessPreentrance = (PuleGpuResourceAccess)0, // later
              .payloadLayoutEntrance = toPayloadLayout(entranceLayout),
              .payloadLayoutPreentrance = (PuleGpuImageLayout)0, // done later
              .isInitialized = false, // TODO user can specify
            },
            .resourceType = PuleRenderGraph_ResourceType_image,
          }
        );
      }
    }
  }

  return graph;
}

} // extern C
