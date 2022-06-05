#include <pulchritude-asset/model.h>

#include <pulchritude-asset/pds.h>
#include <pulchritude-gfx/gfx.h>

#include <functional>
#include <string>
#include <string.h>
#include <unordered_map>
#include <vector>

namespace {

struct InternalAssetModel {
  std::vector<uint8_t> dataBuffer;
  std::vector<PuleAssetMesh> meshes;
};

std::unordered_map<uint64_t, InternalAssetModel> internalModels;
size_t internalModelCount = 0;

} // namespace

extern "C" {

size_t puleAssetMeshComponentDataTypeByteLength(
  PuleAssetMeshComponentDataType const dataType
) {
  switch (dataType) {
    default: return 0;
    case PuleAssetMeshComponentDataType_u8: return 1;
    case PuleAssetMeshComponentDataType_u16: return 2;
    case PuleAssetMeshComponentDataType_u32: return 4;
    case PuleAssetMeshComponentDataType_u8Normalized: return 1;
    case PuleAssetMeshComponentDataType_u16Normalized: return 2;
    case PuleAssetMeshComponentDataType_u32Normalized: return 4;
    case PuleAssetMeshComponentDataType_f16: return 2;
    case PuleAssetMeshComponentDataType_f32: return 4;
  }
}

PuleAssetModel puleAssetModelLoadFromStream(
  PuleAssetModelCreateInfo const info,
  PuleError * const error
) {
  auto const returnError = PuleAssetModel {
    .id = 0,
    .materials = nullptr, .materialCount = 0,
    .meshes = nullptr, .meshCount = 0,
  };

  PuleDsValue const head = (
    puleAssetPdsLoadFromStream(info.allocator, info.modelSource, error)
  );
  if (puleErrorConsume(error)) {
    PULE_error(PuleErrorAssetModel_decode, "failed to parse model source");
    return returnError;
  }

  /*
    buffers: [ [ .. ], // pdsBuffer ],
    buffer-views: [ [ buffer:0, offset: 0, size: 0, stride: 0, ], ],
    meshes: [{
      attribute-origin: {
        buffer-view: , component-data-type, component-per-vertex,
      },
      attribute-uv: { .. },
      element: [ buffer-view, component-data-type ],
    },],
  */
  internalModels.emplace(internalModelCount, InternalAssetModel{});
  auto & assetModel = internalModels.at(internalModelCount);

  // -- prepare pds arrays for quick reference
  PuleDsValueArray const bufferArray = (
    puleDsAsArray(puleDsObjectMember(head, "buffers"))
  );
  PuleDsValueArray const bufferViewArray = (
    puleDsAsArray(puleDsObjectMember(head, "buffer-views"))
  );
  PuleDsValueArray const meshArray = (
    puleDsAsArray(puleDsObjectMember(head, "meshes"))
  );

  // -- first, copy all buffers over, keeping note of offsets
  std::vector<size_t> bufferGlobalOffsets;
  for (size_t it = 0; it < bufferArray.length; ++ it) {
    PuleDsValueBuffer const bufferData = puleDsAsBuffer(bufferArray.values[it]);

    bufferGlobalOffsets.emplace_back(assetModel.dataBuffer.size());
    assetModel.dataBuffer.resize(
      assetModel.dataBuffer.size() + bufferData.length
    );
    memcpy(
      assetModel.dataBuffer.data() + it,
      bufferData.data,
      bufferData.length
    );
  }

  // -- create meshes
  for (size_t it = 0; it < meshArray.length; ++ it) {
    PuleDsValue const meshValue = meshArray.values[it];
    assetModel.meshes.emplace_back(PuleAssetMesh {});
    auto & mesh = assetModel.meshes.back();
    memset(&mesh, 0, sizeof(PuleAssetMesh));
    // -- prepare attributes/element
    PuleDsValue const attributeOrigin = (
      puleDsObjectMember(meshValue, "attribute-origin")
    );
    PuleDsValue const elementValue = (
      puleDsObjectMember(meshValue, "element")
    );

    mesh.verticesToDispatch = (
      puleDsAsUSize(
        puleDsObjectMember(meshValue, "vertices-to-dispatch")
      )
    );

    // -- load origin attribute buffer
    if (attributeOrigin.id > 0) {
      PuleDsValue const bufferViewValue = (
        bufferViewArray.values[
          puleDsAsUSize(puleDsObjectMember(attributeOrigin, "buffer-view"))
        ]
      );
      size_t const globalOffset = (
        bufferGlobalOffsets[
          puleDsAsUSize(puleDsObjectMember(bufferViewValue, "buffer"))
        ]
      );
      size_t const relativeOffset = (
        puleDsAsUSize(puleDsObjectMember(bufferViewValue, "offset"))
      );

      mesh.attributes[PuleAssetMeshAttributeType_origin] = (
        PuleAssetMeshAttribute {
          .view = PuleArrayView {
            .data = (
              assetModel.dataBuffer.data() + globalOffset + relativeOffset
            ),
            .elementStride = (
              puleDsAsUSize(puleDsObjectMember(bufferViewValue, "stride"))
            ),
            .elementCount = (
              puleDsAsUSize(puleDsObjectMember(bufferViewValue, "size"))
            ),
          },
          .componentDataType = (
            static_cast<PuleAssetMeshComponentDataType>(
              puleDsAsI32(
                puleDsObjectMember(attributeOrigin, "component-data-type")
              )
            )
          ),
          .componentsPerVertex = (
            puleDsAsU32(
              puleDsObjectMember(attributeOrigin, "components-per-vertex")
            )
          ),
        }
      );
    }

    // load element buffer
    PULE_assert(elementValue.id > 0);
    {
      PuleDsValue const bufferViewValue = (
        bufferViewArray.values[
          puleDsAsUSize(puleDsObjectMember(elementValue, "buffer-view"))
        ]
      );
      size_t const globalOffset = (
        bufferGlobalOffsets[
          puleDsAsUSize(puleDsObjectMember(bufferViewValue, "buffer"))
        ]
      );
      size_t const relativeOffset = (
        puleDsAsUSize(puleDsObjectMember(bufferViewValue, "offset"))
      );

      mesh.element = (
        PuleAssetMeshElement {
          .view = PuleArrayView {
            .data = (
              assetModel.dataBuffer.data() + globalOffset + relativeOffset
            ),
            .elementStride = (
              puleDsAsUSize(puleDsObjectMember(bufferViewValue, "stride"))
            ),
            .elementCount = (
              puleDsAsUSize(puleDsObjectMember(bufferViewValue, "size"))
            ),
          },
          .componentDataType = (
            static_cast<PuleAssetMeshComponentDataType>(
              puleDsAsU32(
                puleDsObjectMember(elementValue, "component-data-type")
              )
            )
          ),
        }
      );
    }
  }

  // -- return a 'view' of internal model
  auto const userModel = PuleAssetModel {
    .id = internalModelCount,
    .materials = nullptr, .materialCount = 0,
    .meshes = assetModel.meshes.data(),
    .meshCount = assetModel.meshes.size(),
  };

  internalModelCount += 1;
  return userModel;
}

} // C
