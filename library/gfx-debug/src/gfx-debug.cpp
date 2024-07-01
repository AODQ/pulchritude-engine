#include <pulchritude/gfx-debug.h>

#include <pulchritude/core.hpp>
#include <pulchritude/gpu.h>
#include <pulchritude/shape.h>

#include <string>
#include <vector>

// Premise for how this works is for user to start  debug shapes,
//   they have us create a decord recorder, fill in the shapes,
//   then fill in a command list that they can use to dispatch
// Preferably there is one debug-recorder throughout the lifetime of a frame,
//   but realistically needs to be one per-view or render-pass, or for
//   buffering

// -- recording data --
namespace pint {

constexpr PuleGpuPipelineLayoutPushConstant pipelineLayoutPushConstants = {
  .stage = PuleGpuDescriptorStage_vertex,
  .byteLength = sizeof(float)*4*4*2, // 4x4 transform + viewProj
  .byteOffset = 0,
};

constexpr size_t attributeScratchBufferSize = 1024*1024; // 1MB

struct TriangleAttribute {
  PuleF32v3 origin;
  PuleF32v3 uv;
  PuleF32v3 color;
};
constexpr size_t maxTrianglesPerCommandList = (
  attributeScratchBufferSize/(sizeof(TriangleAttribute)*3)
);

struct LineInfo {
  PuleF32v2 origin;
  PuleF32v3 color;
};
constexpr size_t maxLinesPerCommandList = (
  attributeScratchBufferSize/(sizeof(LineInfo)*2)
);

struct DebugRecorder {
  std::vector<TriangleAttribute> triangleAttributes;
  std::vector<LineInfo> lineAttributes;
  PuleF32m44 transform;
  PuleF32m44 viewProjection;
};

pule::ResourceContainer<DebugRecorder, PuleGfxDebugRecorder> debugRecorders;
} // namespace pint

// -- gpu data --
// only keep one instance of GPU data for maximal reuse between recorders;
//   this means that a command-list must be submitted immediately as it
//   is recorded
namespace pint {
struct DebugGpuData {
  PuleGpuPipeline linePipeline;
  PuleGpuShaderModule lineShaderModule;
  PuleGpuPipeline triPipeline;
  PuleGpuShaderModule triShaderModule;

  PuleGpuBuffer attributeScratchBuffer;
};
static DebugGpuData debugGpuData;
void debugGpuDataInitializeLine();
void debugGpuDataInitializeTriangle();
void debugGpuDataInitialize();

void renderTriangles(PuleGfxDebugSubmitInfo const submitInfo);
void renderLines(
  PuleGfxDebugSubmitInfo const submitInfo
);
} // namespace pint

void pint::debugGpuDataInitializeLine() {
  #include "autogen-debug-gfx-line.frag.spv"
  #include "autogen-debug-gfx-line.vert.spv"

  PuleError err = puleError();

  pint::debugGpuData.lineShaderModule = (
    puleGpuShaderModuleCreate(
      PuleBufferView {
        .data = debugGfxLineVert,
        .byteLength = sizeof(debugGfxLineVert),
      },
      PuleBufferView {
        .data = debugGfxLineFrag,
        .byteLength = sizeof(debugGfxLineFrag),
      },
      &err
    )
  );
  if (puleErrorConsume(&err)) {
    return;
  }

  auto layoutDescriptorSet = puleGpuPipelineDescriptorSetLayout();
  layoutDescriptorSet.attributeBindings[0] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 0,
    .numComponents = 2,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(pint::LineInfo, origin)
  };
  layoutDescriptorSet.attributeBindings[1] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 0,
    .numComponents = 3,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(pint::LineInfo, color)
  };
  layoutDescriptorSet.attributeBufferBindings[0] = {
    .stridePerElement = sizeof(pint::LineInfo),
  };

  auto pipelineInfo = (
    PuleGpuPipelineCreateInfo {
      .shaderModule = pint::debugGpuData.lineShaderModule,
      .layoutDescriptorSet = layoutDescriptorSet,
      .layoutPushConstants = pipelineLayoutPushConstants,
      .config = {
        .depthTestEnabled = false,
        .blendEnabled = false,
        .scissorTestEnabled = false,
        .viewportMin = {0, 0},
        .viewportMax = {800, 600}, // TODO FIX
        .scissorMin = {0, 0},
        .scissorMax = {0, 0},
        .drawPrimitive = PuleGpuDrawPrimitive_line,
        .colorAttachmentCount = 1,
        .colorAttachmentFormats = {
          PuleGpuImageByteFormat_rgba8U,
        },
        .depthAttachmentFormat = PuleGpuImageByteFormat_undefined,
      },
    }
  );

  pint::debugGpuData.linePipeline = puleGpuPipelineCreate(pipelineInfo, &err);

  if (puleErrorConsume(&err)) {
    return;
  }
}

void pint::debugGpuDataInitializeTriangle() {
  #include "autogen-debug-gfx-tri.frag.spv"
  #include "autogen-debug-gfx-tri.vert.spv"

  PuleError err = puleError();

  pint::debugGpuData.triShaderModule = (
    puleGpuShaderModuleCreate(
      PuleBufferView {
        .data = debugGfxTriVert,
        .byteLength = sizeof(debugGfxTriVert),
      },
      PuleBufferView {
        .data = debugGfxTriFrag,
        .byteLength = sizeof(debugGfxTriFrag),
      },
      &err
    )
  );
  if (puleErrorConsume(&err)) \
    return;

  auto layoutDescriptorSet = puleGpuPipelineDescriptorSetLayout();
  layoutDescriptorSet.attributeBindings[0] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 0,
    .numComponents = 3,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(pint::TriangleAttribute, origin)
  };
  layoutDescriptorSet.attributeBindings[1] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 0,
    .numComponents = 3,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(pint::TriangleAttribute, uv)
  };
  layoutDescriptorSet.attributeBindings[2] = {
    .dataType = PuleGpuAttributeDataType_f32,
    .bufferIndex = 0,
    .numComponents = 3,
    .convertFixedDataTypeToNormalizedFloating = false,
    .relativeOffset = offsetof(pint::TriangleAttribute, color)
  };
  layoutDescriptorSet.attributeBufferBindings[0] = {
    .stridePerElement = sizeof(pint::TriangleAttribute),
  };

  auto pipelineInfo = (
    PuleGpuPipelineCreateInfo {
      .shaderModule = pint::debugGpuData.triShaderModule,
      .layoutDescriptorSet = layoutDescriptorSet,
      .layoutPushConstants = pipelineLayoutPushConstants,
      .config = {
        .depthTestEnabled = true,
        .blendEnabled = false,
        .scissorTestEnabled = false,
        .viewportMin = {0, 0},
        .viewportMax = {800, 600}, // TODO FIX
        .scissorMin = {0, 0},
        .scissorMax = {0, 0},
        .drawPrimitive = PuleGpuDrawPrimitive_triangle,
        .colorAttachmentCount = 1,
        .colorAttachmentFormats = {
          PuleGpuImageByteFormat_rgba8U,
        },
        .depthAttachmentFormat = PuleGpuImageByteFormat_depth16,
      },
    }
  );

  pint::debugGpuData.triPipeline = puleGpuPipelineCreate(pipelineInfo, &err);

  if (puleErrorConsume(&err)) \
    return;
}

void pint::debugGpuDataInitialize() {
  static bool initialized = false;
  if (initialized) \
    return;
  initialized = true;
  pint::debugGpuDataInitializeLine();
  pint::debugGpuDataInitializeTriangle();

  // create buffer
  pint::debugGpuData.attributeScratchBuffer = (
    puleGpuBufferCreate(
      puleCStr("debug-line-attributes"),
      pint::maxLinesPerCommandList*sizeof(pint::LineInfo),
      PuleGpuBufferUsage_attribute,
      PuleGpuBufferVisibilityFlag_hostWritable
    )
  );
}

void pint::renderTriangles(
  PuleGfxDebugSubmitInfo const submitInfo
) {
  pint::DebugRecorder & debugRecorder = (
    *pint::debugRecorders.at(submitInfo.recorder)
  );
  auto & triangleAttrs = debugRecorder.triangleAttributes;
  void * const  scratchBuffer = (
    puleGpuBufferMap({
      .buffer = pint::debugGpuData.attributeScratchBuffer,
      .byteOffset = 0,
      .byteLength = pint::attributeScratchBufferSize,
    })
  );

  struct {
    PuleF32m44 transform;
    PuleF32m44 viewProj;
  } pushConstantData;
  pushConstantData.transform = debugRecorder.transform;
  pushConstantData.viewProj = debugRecorder.viewProjection;

  // first write out data
  size_t const countAttribute = (
    std::min(
      maxTrianglesPerCommandList*3,
      triangleAttrs.size()
    )
  );
  PULE_assert(countAttribute % 3 == 0);
  if (countAttribute == 0) \
    return;
  for (size_t it = 0; it < countAttribute; ++ it) {
    auto const & tri = triangleAttrs[it];
  }
  memcpy(
    scratchBuffer,
    triangleAttrs.data(),
    countAttribute*sizeof(pint::TriangleAttribute)
  );
  puleGpuBufferMappedFlush({
    .buffer = pint::debugGpuData.attributeScratchBuffer,
    .byteOffset = 0,
    .byteLength = countAttribute*sizeof(pint::TriangleAttribute),
  });
  puleGpuBufferUnmap(pint::debugGpuData.attributeScratchBuffer);

  // record command list
  PuleGpuCommandListRecorder recorder = submitInfo.commandListRecorder;

  PuleGpuResourceBarrierBuffer const barrier = {
    .buffer = pint::debugGpuData.attributeScratchBuffer,
    .accessSrc = PuleGpuResourceAccess_hostWrite,
    .accessDst = PuleGpuResourceAccess_vertexAttributeRead,
    .byteOffset = 0,
    .byteLength = countAttribute*sizeof(pint::TriangleAttribute),
  };
  puleGpuCommandListAppendAction(
    recorder,
    PuleGpuCommand {
      .resourceBarrier = {
        .stageSrc = PuleGpuResourceBarrierStage_top,
        .stageDst = PuleGpuResourceBarrierStage_vertexInput,
        .resourceImageCount = 0,
        .resourceImages = nullptr,
        .resourceBufferCount = 1,
        .resourceBuffers = &barrier,
      }
    }
  );

  // resource barrier can't exist inside a render-pass

  puleGpuCommandListAppendAction(
    submitInfo.commandListRecorder,
    { .renderPassBegin = submitInfo.renderPassBegin, }
  );

  // SIGH -- this needs to G T F O
  // the renderpass should be calling this by default to be honest
  puleGpuCommandListAppendAction(
    submitInfo.commandListRecorder,
    {
      .setScissor = {
        .action = PuleGpuAction_setScissor,
        .scissorMin = { .x = 0, .y = 0, },
        .scissorMax = { .x = 800, .y = 600, },
      },
    }
  );

  puleGpuCommandListAppendAction(
    recorder,
    PuleGpuCommand {
      .bindPipeline = {
        .pipeline = pint::debugGpuData.triPipeline,
      }
    }
  );
  puleGpuCommandListAppendAction(
    recorder,
    PuleGpuCommand {
      .pushConstants = {
        .stage = PuleGpuDescriptorStage_vertex,
        .byteLength = sizeof(pushConstantData),
        .byteOffset = 0,
        .data = &pushConstantData,
      }
    }
  );
  puleGpuCommandListAppendAction(
    recorder,
    PuleGpuCommand {
      .bindAttributeBuffer = {
        .bindingIndex = 0,
        .buffer = pint::debugGpuData.attributeScratchBuffer,
        .offset = 0,
        .stride = sizeof(pint::TriangleAttribute),
      }
    }
  );
  puleGpuCommandListAppendAction(
    recorder,
    PuleGpuCommand {
      .dispatchRender = {
        .vertexOffset = 0,
        .numVertices = countAttribute,
      }
    }
  );

  puleGpuCommandListAppendAction(
    submitInfo.commandListRecorder,
    { .renderPassEnd = {}, }
  );
}

void pint::renderLines(PuleGfxDebugSubmitInfo const submitInfo) {
  // TODO
  (void)submitInfo;
}

extern "C" {
void puleGfxDebugInitialize(PulePlatform const platform) {
  (void)platform;
  pint::debugGpuDataInitialize();
}

void puleGfxDebugShutdown() {
  // just don't do anything (to avoid bug init -> deinit -> init)
  // can fix later
}

PuleGfxDebugRecorder puleGfxDebugStart(
  PuleF32m44 const transform,
  PuleF32m44 const view,
  PuleF32m44 const projection
) {
  pint::DebugRecorder debugRecorder = {
    .transform = transform,
    .viewProjection = puleF32m44Mul(view, projection),
  };
  return pint::debugRecorders.create(debugRecorder);
}

void puleGfxDebugSubmit(
  PuleGfxDebugSubmitInfo const submitInfo
) {
  // in the case that there are no triangles or lines, just draw a dummy
  //   triangle to prevent command list from being empty
  auto & debugRecorder = *pint::debugRecorders.at(submitInfo.recorder);
  if (
       debugRecorder.triangleAttributes.empty()
    && debugRecorder.lineAttributes.empty()
  ) {
    debugRecorder.triangleAttributes.push_back({
      .origin = {0.0f, 0.0f, 0.0f},
      .uv = {0.0f, 0.0f},
      .color = {1.0f, 1.0f, 1.0f},
    });
    debugRecorder.triangleAttributes.push_back({
      .origin = {0.0f, 1.0f, 1.0f},
      .uv = {0.0f, 1.0f},
      .color = {1.0f, 1.0f, 1.0f},
    });
    debugRecorder.triangleAttributes.push_back({
      .origin = {1.0f, 1.0f, 1.0f},
      .uv = {1.0f, 1.0f},
      .color = {1.0f, 1.0f, 1.0f},
    });
  }

  pint::renderTriangles(submitInfo);
  pint::renderLines(submitInfo);

  pint::debugRecorders.destroy(submitInfo.recorder);
}

} // extern C

// -- render params
extern "C" {

void puleGfxDebugRender(
  PuleGfxDebugRecorder const debugRecorder,
  PuleGfxDebugRenderParam const param
) {
  auto & recorder = *pint::debugRecorders.at(debugRecorder);

  switch (param.type) {
    case PuleGfxDebugRenderType_line:
      recorder.lineAttributes.push_back({
        .origin = param.line.a,
        .color = param.line.color,
      });
      recorder.lineAttributes.push_back({
        .origin = param.line.b,
        .color = param.line.color,
      });
    break;
    case PuleGfxDebugRenderType_quad:
      puleLogError("Unimplemented: quad");
      break;
    case PuleGfxDebugRenderType_cube: {
      #include "cube-vertices.inl"
      PuleF32m44 const rotation = (
        puleF32m33AsM44(param.cube.rotationMatrix)
      );
      for (size_t it = 0; it < cubeTriVertices.size(); ++ it) {
        // rotate point
        PuleF32v4 vtx = cubeTriVertices[it];
        vtx = puleF32m44MulV4(rotation, vtx);
        // scale
        vtx.x *= param.cube.dimensionsHalf.x;
        vtx.y *= param.cube.dimensionsHalf.y;
        vtx.z *= param.cube.dimensionsHalf.z;
        // add origin
        vtx = puleF32v4Add(vtx, puleF32v3to4(param.cube.originCenter, 0.0f));
        recorder.triangleAttributes.push_back({
          .origin = PuleF32v3 {vtx.x, vtx.y, vtx.z},
          .uv = {0.0f, 0.0f, 0.0f},
          .color = param.cube.color,
        });
      }
    } break;
    case PuleGfxDebugRenderType_sphere: {
      // in the future I can do a custom renderer, that uses instancing
      // for now just render a sphere as a cube
      static PuleBuffer icosphereBuffer = {.data = nullptr};
      if (icosphereBuffer.data == nullptr) {
        icosphereBuffer = puleShapeCreateIcosphere(3);
      }
      size_t const numVertices = icosphereBuffer.byteLength/sizeof(float)/3;
      for (size_t it = 0; it < numVertices; ++ it) {
        PuleF32v3 vtx = {
          ((float *)icosphereBuffer.data)[it*3 + 0],
          ((float *)icosphereBuffer.data)[it*3 + 1],
          ((float *)icosphereBuffer.data)[it*3 + 2],
        };
        // calculate uv
        // scale by radius
        vtx = puleF32v3MulScalar(vtx, param.sphere.radius);
        PuleF32v3 uv = vtx;
        // translate by origin, after UV calculation
        vtx = puleF32v3Add(vtx, param.sphere.originCenter);
        recorder.triangleAttributes.push_back({
          .origin = vtx,
          .uv = uv,
          .color = param.sphere.color,
        });
      }
    } break;
    case PuleGfxDebugRenderType_plane: {
      // create a quad with infinite dimensions
      std::vector<PuleF32v2> const quadVertices = {
        { 1.0f,  1.0f},
        { 1.0f, -1.0f},
        {-1.0f, -1.0f},

        { 1.0f,  1.0f},
        {-1.0f, -1.0f},
        {-1.0f,  1.0f},
      };
      PuleF32m44 const rotation = (
        puleF32m33AsM44(param.plane.rotationMatrix)
      );

      for (size_t it = 0; it < quadVertices.size(); ++ it) {
        PuleF32v2 const vtx = quadVertices[it];
        PuleF32v4 originV4 = (
          puleF32m44MulV4(rotation, PuleF32v4 { vtx.x, 0.0f, vtx.y, 1.0f })
        );
        auto origin = PuleF32v3 {originV4.x, originV4.y, originV4.z,};
        // scale it up to infinity
        origin = puleF32v3MulScalar(origin, 20.0f);
        // then translate by origin
        origin = puleF32v3Add(origin, param.plane.originCenter);
        recorder.triangleAttributes.push_back({
          .origin = origin,
          .uv = { vtx.x*0.5f + 0.5f, vtx.y*0.5f + 0.5f, 0.0f },
          .color = param.plane.color,
        });
      }
    } break;
  }
}

} // extern C
