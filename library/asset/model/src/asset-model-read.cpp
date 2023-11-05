#include <pulchritude-asset/model.h>

#include <pulchritude-asset/pds.h>
#include <pulchritude-gpu/gpu.h>

#include <functional>
#include <string>
#include <string.h>
#include <unordered_map>
#include <vector>

extern "C" {

void puleAssetModelLoad(
  PuleAssetModelLoadInfo const info,
  PuleError * const error
) {
  /*
    buffers: [ [ .. ], // pdsBuffer ],
    meshes: [{
      attribute-origin: {
        buffer-index: #,
        buffer-byte-offset: #,
        buffer-byte-stride: #,
        component-data-type: #,
        components-per-vertex: #,
      },
      attribute-uv: { .. },
      element: { bufferIndex: #, bufferByteOffset: #, componentDataType: # },
    },],
  */
  // -- prepare pds arrays for quick reference
  PuleDsValueArray const buffers = (
    puleDsAsArray(puleDsObjectMember(info.modelData, "buffers"))
  );
  PuleDsValueArray const meshes = (
    puleDsAsArray(puleDsObjectMember(info.modelData, "meshes"))
  );

  // load buffers
  for (size_t bufferIt = 0; bufferIt < buffers.length; ++ bufferIt) {
    PuleDsValue const bufferValue = buffers.values[bufferIt];
    PuleDsValueBuffer buffer = puleDsAsBuffer(bufferValue);
    info.loadBuffer(
      PuleBufferView {
        .data = buffer.data,
        .byteLength = buffer.byteLength,
      },
      bufferIt,
      info.userData
    );
  }

  // load meshes
  for (size_t meshIt = 0; meshIt < meshes.length; ++ meshIt) {
    PuleAssetMesh mesh = {};
    for (auto attributeIt : { std::pair<PuleAssetMeshAttributeType, std::string>
      { PuleAssetMeshAttributeType_origin, "attribute-origin", },
      { PuleAssetMeshAttributeType_uvCoord0, "attribute-uv", },
      { PuleAssetMeshAttributeType_normal, "attribute-normal", },
    }) {
      PuleDsValue const attributeValue = (
        puleDsObjectMember(meshes.values[meshIt], attributeIt.second.c_str())
      );
      auto & attribute = mesh.attributes[attributeIt.first];
      std::vector<PuleAssetPdsDescription> serializeDescription = {
        { &attribute.bufferIndex, "buffer-index", "{usize}", },
        { &attribute.bufferByteOffset, "buffer-byte-offset", "{usize}", },
        { &attribute.bufferByteStride, "buffer-byte-stride", "{size}", },
        { &attribute.componentDataType, "component-data-type", "{i32}", },
        { &attribute.elementCount, "element-count", "{usize}", },
      };
      puleAssetPdsDeserialize({
        .value = attributeValue,
        .descriptions = serializeDescription.data(),
        .descriptionCount = serializeDescription.size(),
        }, error
      );
      if (error->id > 0) {
        PULE_error(PuleErrorAssetModel_decode, "failed to deserialize mesh");
        return;
      }
    }
    PuleDsValue const elementValue = (
      puleDsObjectMember(meshes.values[meshIt], "element")
    );
    std::vector<PuleAssetPdsDescription> serializeDescription = {
      { &mesh.element.bufferIndex, "buffer-index", "{usize}", },
      { &mesh.element.bufferByteOffset, "buffer-byte-offset", "{usize}", },
    };
    puleAssetPdsDeserialize({
      .value = elementValue,
      .descriptions = serializeDescription.data(),
      .descriptionCount = serializeDescription.size(),
      }, error
    );
    if (error->id > 0) {
      PULE_error(PuleErrorAssetModel_decode, "failed to deserialize mesh");
      return;
    }
    mesh.verticesToDispatch = (
      puleDsAsUSize(
        puleDsObjectMember(meshes.values[meshIt], "vertices-to-dispatch")
      )
    );
    info.loadMesh(mesh, meshIt, info.userData);
  }
  // TODO load textures, materials, animations, nodes, scenes, etc
}

} // C
