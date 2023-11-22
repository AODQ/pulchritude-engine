#include <pulchritude-asset/model-simple.h>

#include <pulchritude-asset/pds.h>

#include <vector>

namespace pint {

struct MeshIntermediate {
  std::vector<PuleF32v3> origins;
  std::vector<PuleF32v3> normals;
  std::vector<PuleF32v2> uvs;
  std::vector<uint32_t> elements;
  size_t verticesToDispatch;
  size_t attributeElementCount;
};

struct ModelIntermediate {
  std::vector<std::vector<uint8_t>> buffers;
  std::vector<MeshIntermediate> meshes;
};

void convertAttributeElement(
  PuleAssetMeshAttributeComponentDataType const sourceType,
  PuleAssetMeshAttributeComponentDataType const dstType,
  uint8_t const * const srcData,
  uint8_t * const dstData
) {
  // convert to an intermediate
  struct {
    union {
      uint64_t u64;
      float f32;
    };
    bool isFloat;
  } conversion = { .u64 = 0, .isFloat = false, };

  switch (sourceType) {
    default: PULE_assert(false && "Invalid/unsupported source type");
    case PuleAssetMeshAttributeComponentDataType_u8:
      conversion.u64 = static_cast<uint64_t>(*srcData);
    break;
    case PuleAssetMeshAttributeComponentDataType_u16:
      conversion.u64 = *reinterpret_cast<uint16_t const *>(srcData);
    break;
    case PuleAssetMeshAttributeComponentDataType_u32:
      conversion.u64 = *reinterpret_cast<uint32_t const *>(srcData);
    break;
    case PuleAssetMeshAttributeComponentDataType_i8:
      conversion.u64 = static_cast<uint64_t>(*srcData);
    break;
    case PuleAssetMeshAttributeComponentDataType_i16:
      conversion.u64 = (
        static_cast<uint64_t>(*reinterpret_cast<int16_t const *>(srcData))
      );
    break;
    case PuleAssetMeshAttributeComponentDataType_i32:
      conversion.u64 = (
        static_cast<uint64_t>(*reinterpret_cast<int32_t const *>(srcData))
      );
    break;
    case PuleAssetMeshAttributeComponentDataType_f32:
      conversion.f32 = *reinterpret_cast<float const *>(srcData);
      conversion.isFloat = true;
    break;
  }

  #define DstConversion(dataType) \
    if (conversion.isFloat) { \
      *reinterpret_cast<dataType *>(dstData) = ( \
        static_cast<dataType>(conversion.f32) \
      ); \
    } else { \
      *reinterpret_cast<dataType *>(dstData) = ( \
        static_cast<dataType>(conversion.u64) \
      ); \
    }
  switch (dstType) {
    default: PULE_assert(false && "Invalid/unsupported destination type");
    case PuleAssetMeshAttributeComponentDataType_u8:
      DstConversion(uint8_t);
    break;
    case PuleAssetMeshAttributeComponentDataType_i8:
      DstConversion(int8_t);
    break;
    case PuleAssetMeshAttributeComponentDataType_u16:
      DstConversion(uint16_t);
    break;
    case PuleAssetMeshAttributeComponentDataType_i16:
      DstConversion(int16_t);
    break;
    case PuleAssetMeshAttributeComponentDataType_u32:
      DstConversion(uint32_t);
    break;
    case PuleAssetMeshAttributeComponentDataType_i32:
      DstConversion(int32_t);
    break;
    case PuleAssetMeshAttributeComponentDataType_f32:
      DstConversion(float);
    break;
  }
  #undef DstConversion
}

} // namespace pint

extern "C" {

static void intermediateLoadBuffer(
  PuleBufferView const bufferView,
  size_t const bufferIndex,
  void * const userData
) {
  auto const modelIntermediate = (
    static_cast<pint::ModelIntermediate *>(userData)
  );
  // copy buffer into model intermediate
  if (bufferIndex >= modelIntermediate->buffers.size()) {
    modelIntermediate->buffers.resize(bufferIndex + 1);
  }
  modelIntermediate->buffers[bufferIndex] = (
    std::vector<uint8_t>(
      bufferView.data,
      bufferView.data + bufferView.byteLength
    )
  );
}

static void intermediateLoadMesh(
  PuleAssetMesh const mesh,
  size_t const meshIndex,
  void * const userdata
) {
  auto & modelIntermediate = *static_cast<pint::ModelIntermediate *>(userdata);
  // -- load mesh into model intermediate
  if (meshIndex >= modelIntermediate.meshes.size()) {
    modelIntermediate.meshes.resize(meshIndex + 1);
  }
  auto & meshIntermediate = modelIntermediate.meshes[meshIndex];
  meshIntermediate.verticesToDispatch = mesh.verticesToDispatch;
  meshIntermediate.attributeElementCount = mesh.attributeElementCount;

  // convert attribute buffers
  std::vector<uint8_t> dstOrigin = {};
  std::vector<uint8_t> dstUv = {};
  std::vector<uint8_t> dstNormal = {};
  dstOrigin.resize(mesh.attributeElementCount * sizeof(PuleF32v3));
  dstUv.resize(mesh.attributeElementCount * sizeof(PuleF32v2));
  dstNormal.resize(mesh.attributeElementCount * sizeof(PuleF32v3));
  auto & attrOrigin = mesh.attributes[PuleAssetMeshAttributeType_origin];
  auto & attrUv = mesh.attributes[PuleAssetMeshAttributeType_uvCoord0];
  auto & attrNormal = mesh.attributes[PuleAssetMeshAttributeType_normal];
  puleAssetModelConvertAttributeBuffer({
    .attribute = attrOrigin,
    .attributeBufferView = {
      .data = modelIntermediate.buffers[attrOrigin.bufferIndex].data(),
      .byteLength = modelIntermediate.buffers[attrOrigin.bufferIndex].size(),
    },
    .dstComponentDataType = PuleAssetMeshAttributeComponentDataType_f32,
    .dstComponentCount = 3,
    .attributeElementCount = mesh.attributeElementCount,
    .dstBufferView = {
      .data = dstOrigin.data(),
      .byteLength = dstOrigin.size(),
    },
  });
  puleAssetModelConvertAttributeBuffer({
    .attribute = attrUv,
    .attributeBufferView = {
      .data = modelIntermediate.buffers[attrUv.bufferIndex].data(),
      .byteLength = modelIntermediate.buffers[attrUv.bufferIndex].size(),
    },
    .dstComponentDataType = PuleAssetMeshAttributeComponentDataType_f32,
    .dstComponentCount = 2,
    .attributeElementCount = mesh.attributeElementCount,
    .dstBufferView = {
      .data = dstUv.data(),
      .byteLength = dstUv.size(),
    },
  });
  puleAssetModelConvertAttributeBuffer({
    .attribute = attrNormal,
    .attributeBufferView = {
      .data = modelIntermediate.buffers[attrNormal.bufferIndex].data(),
      .byteLength = modelIntermediate.buffers[attrNormal.bufferIndex].size(),
    },
    .dstComponentDataType = PuleAssetMeshAttributeComponentDataType_f32,
    .dstComponentCount = 3,
    .attributeElementCount = mesh.attributeElementCount,
    .dstBufferView = {
      .data = dstNormal.data(),
      .byteLength = dstNormal.size(),
    },
  });

  // store attribute buffers to mesh intermediate
  // TODO this uses an additional copy, can instead reserve ahead of time
  //   and pass ptr to convertBuffer
  meshIntermediate.origins.resize(dstOrigin.size() / sizeof(PuleF32v3));
  meshIntermediate.uvs.resize(dstUv.size() / sizeof(PuleF32v2));
  meshIntermediate.normals.resize(dstNormal.size() / sizeof(PuleF32v3));
  memcpy(meshIntermediate.origins.data(), dstOrigin.data(), dstOrigin.size());
  memcpy(meshIntermediate.uvs.data(), dstUv.data(), dstUv.size());
  memcpy(meshIntermediate.normals.data(), dstNormal.data(), dstNormal.size());

  // convert element buffer
  for (size_t elemIt = 0; elemIt < mesh.verticesToDispatch; ++ elemIt) {
    meshIntermediate.elements.push_back(
      *reinterpret_cast<uint32_t const *>(
          modelIntermediate.buffers[mesh.element.bufferIndex].data()
        + mesh.element.bufferByteOffset
        + elemIt*sizeof(uint32_t)
      )
    );
  }
}

void puleAssetModelConvertAttributeBuffer(
  PuleAssetModelConvertAttributeInfo const info
) {
  size_t const srcComponentSize = (
    puleAssetMeshAttributeComponentDataTypeByteSize(
      info.attribute.componentDataType
    )
  );

  size_t const dstComponentSize = (
    puleAssetMeshAttributeComponentDataTypeByteSize(
      info.dstComponentDataType
    )
  );
  // TODO these asserts can be moved to debug layer
  if (info.attributeElementCount == 0) {
    PULE_assert(false && "Attribute element count is zero");
    return;
  }
  size_t const minimumSrcBufferLength = (
    info.attribute.bufferByteStride * (info.attributeElementCount - 1)
    + info.attribute.bufferByteOffset
    + srcComponentSize*info.attribute.componentsPerVertex
  );
  size_t const minimumDstBufferLength = (
      (dstComponentSize*info.dstComponentCount) * info.attributeElementCount
  );
  if (minimumSrcBufferLength > info.attributeBufferView.byteLength) {
    PULE_assert(false && "Attribute buffer view is too small");
    return;
  }
  if (minimumDstBufferLength > info.dstBufferView.byteLength) {
    puleLog("minimumDstBufferLength: %zu", minimumDstBufferLength);
    puleLog("destination byte length %zu", info.dstBufferView.byteLength);
    PULE_assert(false && "Destination buffer view is too small");
    return;
  }

  for (size_t elemIt = 0; elemIt < info.attributeElementCount; ++ elemIt)
  for (size_t compIt = 0; compIt < info.dstComponentCount; ++ compIt) {
    if (compIt >= info.attribute.componentsPerVertex) {
      // pad out component when we run out of source data
      continue;
    }
    pint::convertAttributeElement(
      info.attribute.componentDataType,
      info.dstComponentDataType,
      (
          info.attributeBufferView.data
        + info.attribute.bufferByteOffset
        + info.attribute.bufferByteStride*elemIt
        + srcComponentSize*compIt
      ),
      (
          info.dstBufferView.data
        + elemIt*dstComponentSize*info.dstComponentCount
        + compIt*dstComponentSize
      )
    );
  }
}

size_t puleAssetMeshAttributeComponentDataTypeByteSize(
  PuleAssetMeshAttributeComponentDataType const attributeDt
) {
  switch(attributeDt) {
    default: PULE_assert(false && "Invalid/unsupported attribute data type");
    case PuleAssetMeshAttributeComponentDataType_u8:  return sizeof(uint8_t);
    case PuleAssetMeshAttributeComponentDataType_u16: return sizeof(uint16_t);
    case PuleAssetMeshAttributeComponentDataType_u32: return sizeof(uint32_t);
    case PuleAssetMeshAttributeComponentDataType_i8:  return sizeof(int8_t);
    case PuleAssetMeshAttributeComponentDataType_i16: return sizeof(int16_t);
    case PuleAssetMeshAttributeComponentDataType_i32: return sizeof(int32_t);
    case PuleAssetMeshAttributeComponentDataType_f32: return sizeof(float);
  }
}

} // extern "C"

extern "C" {

PuleRenderer3DModel puleAssetModelSimpleLoad(
  PuleStringView const path,
  PuleAllocator const allocator,
  PuleError * const error
) {
  PuleDsValue const modelData = (
    puleAssetPdsLoadFromFile(allocator, path, error)
  );
  if (error->id > 0) {
    PULE_error(1, "Failed to load model data from file: %s", path.contents);
    return {};
  }
  pint::ModelIntermediate modelIntermediate;
  puleAssetModelLoad(
    PuleAssetModelLoadInfo {
      .modelData = modelData,
      .loadBuffer = &intermediateLoadBuffer,
      .loadMesh = &intermediateLoadMesh,
      .userData = (void *)&modelIntermediate,
    },
    error
  );
  puleDsDestroy(modelData);
  if (error->id > 0) {
    PULE_error(1, "Failed to load model from model data: %s", path.contents);
    return {};
  }
  // we don't need to retain buffers since we have reallocated them
  modelIntermediate.buffers = {};
  puleLog("Loaded model intermediate");
  puleLog("model meshes: %zu", modelIntermediate.meshes.size());
  for (size_t mit = 0; mit < modelIntermediate.meshes.size(); ++ mit) {
    auto & mesh = modelIntermediate.meshes[mit];
    puleLog("--mesh %zu", mit++);
    puleLog("  mesh elements: %zu", mesh.elements.size());
    puleLog("  mesh vertices to dispatch: %zu", mesh.verticesToDispatch);
    puleLog("  mesh attribute elements: %zu", mesh.attributeElementCount);
    puleLog("  mesh origins: %zu", mesh.origins.size());
    puleLog("  mesh normals: %zu", mesh.normals.size());
    puleLog("  mesh uvs: %zu", mesh.uvs.size());
  }

  std::vector<PuleBufferView> intermediateBufferViews;
  for (size_t it = 0; it < modelIntermediate.meshes.size(); ++ it) {
    intermediateBufferViews.emplace_back(
      PuleBufferView {
        .data = (uint8_t *)modelIntermediate.meshes[it].elements.data(),
        .byteLength = modelIntermediate.meshes[it].elements.size(),
      }
    );
    intermediateBufferViews.emplace_back(
      PuleBufferView {
        .data = (uint8_t *)modelIntermediate.meshes[it].origins.data(),
        .byteLength = modelIntermediate.meshes[it].origins.size(),
      }
    );
    intermediateBufferViews.emplace_back(
      PuleBufferView {
        .data = (uint8_t *)modelIntermediate.meshes[it].uvs.data(),
        .byteLength = modelIntermediate.meshes[it].uvs.size(),
      }
    );
    intermediateBufferViews.emplace_back(
      PuleBufferView {
        .data = (uint8_t *)modelIntermediate.meshes[it].normals.data(),
        .byteLength = modelIntermediate.meshes[it].normals.size(),
      }
    );
  }

  std::vector<PuleAssetMesh> intermediateAssetMeshes;
  for (size_t it = 0; it < modelIntermediate.meshes.size(); ++ it) {
    intermediateAssetMeshes.emplace_back(
      PuleAssetMesh {
        .attributes = {
          {
            .bufferIndex = it*4 + 0,
            .bufferByteOffset = 0,
            .bufferByteStride = sizeof(PuleF32v3),
            .componentDataType = PuleAssetMeshAttributeComponentDataType_f32,
            .componentsPerVertex = 3,
          },
          {
            .bufferIndex = it*4 + 1,
            .bufferByteOffset = 0,
            .bufferByteStride = sizeof(PuleF32v2),
            .componentDataType = PuleAssetMeshAttributeComponentDataType_f32,
            .componentsPerVertex = 2,
          },
          {
            .bufferIndex = it*4 + 2,
            .bufferByteOffset = 0,
            .bufferByteStride = sizeof(PuleF32v3),
            .componentDataType = PuleAssetMeshAttributeComponentDataType_f32,
            .componentsPerVertex = 3,
          },
        },
        .element = {
          .bufferIndex = it*4 + 3,
          .bufferByteOffset = 0,
        },
        .verticesToDispatch = modelIntermediate.meshes[it].verticesToDispatch,
        .attributeElementCount = (
          modelIntermediate.meshes[it].attributeElementCount
        ),
      }
    );
  }

  return (
    puleRenderer3DModelCreate({
      .material = puleRenderer3DMaterialCreateDefault(),
      .meshes = intermediateAssetMeshes.data(),
      .meshCount = intermediateAssetMeshes.size(),
      .buffers = intermediateBufferViews.data(),
      .bufferCount = intermediateBufferViews.size(),
      .label = path,
    })
  );
}

} // extern "C"
