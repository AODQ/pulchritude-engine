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

PuleGpuCommandPayloadAccess toPayloadAccess(PuleStringView const view) {
  std::string const access = std::string(view.contents, view.len);
  uint64_t accessFlag = 0;
  if (access == "indirect-command-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_indirectCommandRead;
  }
  if (access == "index-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_indexRead;
  }
  if (access == "vertex-attribute-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_vertexAttributeRead;
  }
  if (access == "uniform-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_uniformRead;
  }
  if (access == "input-attachment-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_inputAttachmentRead;
  }
  if (access == "shader-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_shaderRead;
  }
  if (access == "shader-write") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_shaderWrite;
  }
  if (access == "attachment-color-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_colorAttachmentRead;
  }
  if (access == "attachment-color-write") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_colorAttachmentWrite;
  }
  if (access == "attachment-depth-stencil-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_depthStencilAttachmentRead;
  }
  if (access == "attachment-depth-stencil-write") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_depthStencilAttachmentWrite;
  }
  if (access == "transfer-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_transferRead;
  }
  if (access == "transfer-write") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_transferWrite;
  }
  if (access == "host-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_hostRead;
  }
  if (access == "host-write") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_hostWrite;
  }
  if (access == "memory-read") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_memoryRead;
  }
  if (access == "memory-write") {
    accessFlag |= (uint64_t)PuleGpuCommandPayloadAccess_memoryWrite;
  }
  PULE_assert(accessFlag != 0);
  return (PuleGpuCommandPayloadAccess)accessFlag;
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
        PuleStringView const exittanceLayout = (
          puleDsMemberAsString(dsResourceImage, "exittance-layout")
        );
        PuleStringView const exittanceAccess = (
          puleDsMemberAsString(dsResourceImage, "exittance-access")
        );
        puleRenderGraph_resourceAssign(
          graphNode,
          label,
          PuleRenderGraph_Resource {
            .image = {
              .entrancePayloadAccess = (PuleGpuCommandPayloadAccess)0,
              .exittancePayloadAccess = toPayloadAccess(exittanceAccess),
              .entrancePayloadLayout = (PuleGpuImageLayout)0,
              .exittancePayloadLayout = toPayloadLayout(exittanceLayout),
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
