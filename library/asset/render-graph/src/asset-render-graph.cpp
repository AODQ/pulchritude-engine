#include <pulchritude/asset-render-graph.h>

#include <pulchritude/asset-pds.h>
#include <pulchritude/gpu.h>

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
  PuleRenderGraph const graph = (
    puleRenderGraphCreate(
      puleDsMemberAsString(dsRenderGraphValue, "render-graph-label"),
      allocator
    )
  );
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
  PuleDsValueArray const dsGraphResources = (
    puleDsMemberAsArray(dsRenderGraphValue, "render-graph-resources")
  );
  for (size_t it = 0; it < dsGraphResources.length; ++ it) {
    PuleDsValue const dsGraphResource = dsGraphResources.values[it];
    PuleStringView const type = puleDsMemberAsString(dsGraphResource, "type");
    PuleStringView const label = puleDsMemberAsString(dsGraphResource, "name");
    if (puleStringViewEqCStr(type, "image")) {
      PuleDsValue const dsDataManagement = (
        puleDsObjectMember(dsGraphResource, "data-management")
      );
      PuleStringView const dataManagementType = (
        puleDsMemberAsString(dsDataManagement, "type")
      );
      PuleRenderGraph_Resource resource;
      if (puleStringViewEqCStr(dataManagementType, "automatic")) {
        PuleRenderGraph_Resource_Image_DataManagement_Automatic dataManagement;
        // clear fields that might not be initialized by pds
        memset(
          &dataManagement,
          0,
          sizeof(PuleRenderGraph_Resource_Image_DataManagement_Automatic)
        );
        PuleStringView const renderGraphUsage = (
          puleDsMemberAsString(dsDataManagement, "render-graph-usage")
        );
        if (puleStringViewEqCStr(renderGraphUsage, "read")) {
          dataManagement.resourceUsage = PuleRenderGraph_ResourceUsage_read;
        } else if (puleStringViewEqCStr(renderGraphUsage, "write")) {
          dataManagement.resourceUsage = PuleRenderGraph_ResourceUsage_write;
        } else if (puleStringViewEqCStr(renderGraphUsage, "read-write")) {
          dataManagement.resourceUsage = (
            PuleRenderGraph_ResourceUsage_readWrite
          );
        } else {
          PULE_assert(false && "unknown usage");
        }
        dataManagement.isAttachment = (
          puleDsMemberAsBool(dsDataManagement, "is-attachment")
        );
        PuleStringView const initialData = (
          puleDsMemberAsString(dsDataManagement, "initial-data")
        );
        (void)initialData;
        // TODO load initial data
        PuleStringView const target = (
          puleDsMemberAsString(dsDataManagement, "target")
        );
        if (puleStringViewEqCStr(target, "2d")) {
          dataManagement.target = PuleGpuImageTarget_i2D;
        } else {
          PULE_assert(false && "unknown target");
        }
        PuleStringView const byteFormat = (
          puleDsMemberAsString(dsDataManagement, "byte-format")
        );
        if (puleStringViewEqCStr(byteFormat, "rgba8u")) {
          dataManagement.byteFormat = PuleGpuImageByteFormat_rgba8U;
        } else if (puleStringViewEqCStr(byteFormat, "rgb8u")) {
          dataManagement.byteFormat = PuleGpuImageByteFormat_rgb8U;
        } else if (puleStringViewEqCStr(byteFormat, "r8u")) {
          dataManagement.byteFormat = PuleGpuImageByteFormat_r8U;
        } else if (puleStringViewEqCStr(byteFormat, "bgra8u")) {
          dataManagement.byteFormat = PuleGpuImageByteFormat_bgra8U;
        } else if (puleStringViewEqCStr(byteFormat, "depth16")) {
          dataManagement.byteFormat = PuleGpuImageByteFormat_depth16;
        } else {
          PULE_assert(false && "unknown byte format");
        }
        dataManagement.mipmapLevels = (
          static_cast<uint32_t>(
            puleDsMemberAsI64(dsDataManagement, "mipmap-levels")
          )
        );
        dataManagement.arrayLayers = (
          static_cast<uint32_t>(
            puleDsMemberAsI64(dsDataManagement, "array-layers")
          )
        );
        PuleDsValue const scaleDimensionsRelative = (
          puleDsObjectMember(dsDataManagement, "scale-dimensions-relative")
        );
        PuleDsValue const scaleDimensionsAbsolute = (
          puleDsObjectMember(dsDataManagement, "scale-dimensions-absolute")
        );
        if (scaleDimensionsRelative.id != 0) {
          dataManagement.dimensions.scaleRelative.referenceResourceLabel = (
            puleStringCopy(
              puleAllocateDefault(),
              puleDsMemberAsString(
                scaleDimensionsRelative, "reference-image"
              )
            )
          );
          dataManagement.dimensions.scaleRelative.scaleHeight = (
            puleDsMemberAsF64(scaleDimensionsRelative, "scale-height")
          );
          dataManagement.dimensions.scaleRelative.scaleWidth = (
            puleDsMemberAsF64(scaleDimensionsRelative, "scale-width")
          );
          PULE_assert(
            dataManagement.dimensions.scaleRelative.scaleHeight != 0.0f
            && dataManagement.dimensions.scaleRelative.scaleWidth != 0.0f
          );
          dataManagement.areDimensionsAbsolute = false;
        } else if (scaleDimensionsAbsolute.id != 0) {
          dataManagement.dimensions.absolute.width = (
            puleDsMemberAsI64(scaleDimensionsAbsolute, "width")
          );
          dataManagement.dimensions.absolute.height = (
            puleDsMemberAsI64(scaleDimensionsAbsolute, "height")
          );
          dataManagement.areDimensionsAbsolute = true;
        } else {
          PULE_assert(false && "needs scale dimensions");
        }
        resource.resource.image.dataManagement.automatic = dataManagement;
        resource.resource.image.isAutomatic = true;
      } else {
        PULE_assert(false && "unknown data management type");
        resource.resource.image.isAutomatic = false;
      }
      resource.resourceLabel = label;
      resource.resourceType = PuleRenderGraph_ResourceType_image;
      resource.resource.image.imageReference = { .id = 0, }; // created for us
      puleRenderGraph_resourceCreate(graph, resource);
    } else {
      PULE_assert(false && "unknown type");
    }
  }

  // -- create resource usage/dependencies
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
        puleRenderGraph_node_resourceAssign(
          graphNode,
          PuleRenderGraph_Node_Resource {
            .resourceLabel = label,
            .access = toPayloadAccess(entranceAccess),
            .accessEntrance = (PuleGpuResourceAccess)0, // later
            .resource = { .image = {
              .layout = toPayloadLayout(entranceLayout),
              .layoutEntrance = (PuleGpuImageLayout)0, // done later
            }},
          }
        );
      }
    }
  }

  return graph;
}

} // extern C
