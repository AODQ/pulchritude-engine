#include <pulchritude/scene.h>

#include <pulchritude/asset-pds.h>
#include <pulchritude/asset-render-graph.h>
#include <pulchritude/core.hpp>
#include <pulchritude/gfx-debug.h>
#include <pulchritude/render-graph.h>
#include <pulchritude/string.hpp>

#include <vector>

// -- scene components ---------------------------------------------------------
extern "C" {

// -- component struct fields

// model
#define structField(fieldtype, member) \
  PULE_dsStructField(PuleSceneComponentModelData, fieldtype, member, 1)
static PuleDsStructField const componentModelFields[] = {
  structField(i64, type),
  PULE_dsStructTerminator,
};
#undef structField

// physics
#define structField(fieldtype, member) \
  PULE_dsStructField(PuleSceneComponentPhysicsData, fieldtype, member, 1)
static PuleDsStructField const componentPhysicsFields[] = {
  // TODO
  PULE_dsStructTerminator,
};
#undef structField

// -- serialize callbacks

#define componentSerialize(label) \
  void component##label##Serialize( \
    [[maybe_unused]] PuleEcsEntity const e, \
    [[maybe_unused]] void const * const cd, \
    [[maybe_unused]] PuleDsValue const wo, \
    [[maybe_unused]] PuleAllocator const a \
  ) { \
    auto const d = ( \
      static_cast<PuleSceneComponent##label##Data const *>(cd) \
    ); \
    puleDsStructSerialize(wo, a, component##label##Fields, d); \
  } \
  void component##label##Deserialize( \
    [[maybe_unused]] PuleEcsWorld const w, \
    [[maybe_unused]] PuleEcsEntity const e, \
    [[maybe_unused]] PuleEcsComponent const component, \
    [[maybe_unused]] PuleDsValue const ro, \
    [[maybe_unused]] PuleAllocator const a \
  ) { \
    PuleSceneComponent##label##Data d; \
    puleDsStructDeserialize(ro, component##label##Fields, &d); \
    puleEcsEntityAttachComponent(w, e, component, &d); \
  }

componentSerialize(Model)
componentSerialize(Physics)

#undef componentSerialize

} // extern "C"


// -- scene --------------------------------------------------------------------

namespace pint {

void cameraUpdate(PuleCamera const camera, float const msDelta) {
  static float timer = 0.0f;
  timer += 0.000001f * msDelta;
  puleCameraLookAt(
    camera,
    PuleF32v3{sinf(timer)*2.0f, 1.0f, cosf(timer)*2.0f},
    PuleF32v3{0, 0, 0},
    PuleF32v3{0, 0, 1}
  );
  puleCameraPerspectiveSet(
    camera,
    PuleCameraPerspective {
      .nearCutoff = 0.001f,
      .farCutoff = 100.0f,
      .aspectRatio = 800.0f / 600.0f, // TODO
      .fieldOfViewRadians = 2.0f,
    }
  );
}

struct Scene {
  PulePlatform platform;
  bool createdEcsWorld;
  bool createdPhysxWorld;
  PuleEcsWorld worldEcs;
  PuleSceneDimension dimension;
  union {
    PulePhysx3DWorld worldPhysx3D;
    PulePhysx2DWorld worldPhysx2D;
  };

  PuleEcsComponent ecsComponentModel;
  PuleEcsComponent ecsComponentPhysics;

  PuleEcsSystem ecsSystemModelRender;

  PuleCamera camera;
  PuleCameraController cameraController = { .id = 0, };

  PuleGpuSemaphore gpuFrameWaitSemaphore;
};

pule::ResourceContainer<Scene, PuleScene> scenes;
// to lookup Scene from EcsWorld ID in ecs update callback

} // namespace pint

extern "C" {

void systemPhysicsUpdateCallback(PuleEcsIterator const iter);
void systemModelRenderCallback(PuleEcsIterator const iter);

PuleScene puleSceneCreate(PuleSceneCreateInfo const info) {
  // // for now use gfx-debug to render, however I should move to a more
  // //   refined renderer
  // puleGfxDebugInitialize(info.platform);
  // pint::Scene scene;
  // // fill in scene data
  // scene.platform = info.platform;
  // scene.createdEcsWorld = true;
  // scene.createdPhysxWorld = info.createPhysxWorld;
  // scene.worldEcs = PuleEcsWorld { .id = 0 };
  // if (scene.createdEcsWorld) {
  //   scene.worldEcs = puleEcsWorldCreate();
  // }
  // scene.dimension = info.dimension;
  // if (info.createPhysxWorld && scene.dimension == PuleSceneDimension_i3d) {
  //   scene.worldPhysx3D = pulePhysx3DWorldCreate();
  // } else if (info.createPhysxWorld) {
  //   scene.worldPhysx2D = pulePhysx2DWorldCreate();
  // } else {
  //   scene.worldPhysx3D = PulePhysx3DWorld { .id = 0 }; // assign 0
  // }
  //
  // // create ecs components
  // if (scene.worldEcs.id) {
  //   #define componentCreate(_label, _tagLabel) \
  //     scene.ecsComponent##_label = ( \
  //       puleEcsComponentCreate( \
  //         scene.worldEcs, \
  //         PuleEcsComponentCreateInfo { \
  //           .label = "pule-scene-" #_tagLabel, \
  //           .byteLength = sizeof(PuleSceneComponent##_label##Data), \
  //           .byteAlignment = alignof(PuleSceneComponent##_label##Data), \
  //           .serializeComponentCallback = component##_label##Serialize, \
  //           .deserializeComponentCallback = component##_label##Deserialize, \
  //         } \
  //       ) \
  //     );
  //   componentCreate(Model, model);
  //   componentCreate(Physics, physics);
  //   #undef componentCreate
  // }
  //
  // // create ecs systems
  // if (scene.worldEcs.id) {
  //   std::vector<PuleEcsSystemComponentDesc> systemModelRenderComponentDesc = {
  //     PuleEcsSystemComponentDesc
  //     { .component = scene.ecsComponentModel,  .access = PuleAccess_read },
  //     { .component = scene.ecsComponentPhysics, .access = PuleAccess_read, },
  //   };
  //   scene.ecsSystemModelRender = (
  //     puleEcsSystemCreate(
  //       scene.worldEcs,
  //       PuleEcsSystemCreateInfo {
  //         .label = "pule-scene-model-render",
  //         .componentCount = systemModelRenderComponentDesc.size(),
  //         .components = systemModelRenderComponentDesc.data(),
  //         .callback = systemModelRenderCallback,
  //         .callbackFrequency = PuleEcsSystemCallbackFrequency_postUpdate,
  //       }
  //     )
  //   );
  // }
  //
  // // create camera
  // scene.camera = puleCameraCreate();
  // scene.cameraController = (
  //   puleCameraControllerOrbit(
  //     scene.platform, scene.camera, puleF32v3(0.0f), 5.0f
  //   )
  // );
  //
  // //
  // PuleScene puScene = pint::scenes.create(scene);
  // return puScene;
}

void puleSceneDestroy(PuleScene const scene) {
  // if (scene.id == 0) { return; }
  // auto const s = pint::scenes.at(scene);
  // if (s->createdEcsWorld) {
  //   puleEcsWorldDestroy(s->worldEcs);
  // }
  // if (s->createdPhysxWorld) {
  //   if (s->dimension == PuleSceneDimension_i3d) {
  //     //pulePhysx3DWorldDestroy(s->worldPhysx3D);
  //   } else {
  //     pulePhysx2DWorldDestroy(s->worldPhysx2D);
  //   }
  // }
}

PuleEcsWorld puleSceneEcsWorld(PuleScene const scene) {
  // auto const s = pint::scenes.at(scene);
  // return s->worldEcs;
}

PuleScenePhysxWorld puleScenePhysxWorld(PuleScene const scene) {
  // PuleScenePhysxWorld result;
  // auto const s = pint::scenes.at(scene);
  // if (s->dimension == PuleSceneDimension_i3d) {
  //   result.i3D = s->worldPhysx3D;
  // } else {
  //   result.i2D = s->worldPhysx2D;
  // }
  // return result;
}

void puleSceneAdvance(PuleSceneAdvanceInfo const info){
  // auto const s = pint::scenes.at(info.scene);
  // // advance physx system first
  // if (s->worldPhysx3D.id != 0 && info.advancePhysxWorld) {
  //   if (s->dimension == PuleSceneDimension_i3d) {
  //     //pulePhysx3DWorldAdvance(s->worldPhysx3D, info.msDelta);
  //   } else {
  //     pulePhysx2DWorldStep(s->worldPhysx2D, info.msDelta, 8, 3);
  //   }
  // }
  // // then update camera controls
  // puleCameraControllerPollEvents();
  // // then advance ECS (which should update objects + rendering)
  // if (s->worldEcs.id != 0 && info.advanceEcsWorld) {
  //   s->gpuFrameWaitSemaphore = info.waitSemaphore;
  //   puleEcsSystemAdvance(
  //     s->worldEcs, s->ecsSystemModelRender, info.msDelta, (void *)info.scene.id
  //   );
  // }
}

PuleEcsComponent puleSceneComponentModel(PuleScene const scene) {
  // auto const s = pint::scenes.at(scene);
  // return s->ecsComponentModel;
}
PuleEcsComponent puleSceneComponentPhysics(PuleScene const scene) {
  // auto const s = pint::scenes.at(scene);
  // return s->ecsComponentPhysics;
}

void puleSceneNodeAttachComponents(
  PuleSceneNodeCreateInfo const info
) {
  // auto const s = pint::scenes.at(info.scene);
  // PuleSceneComponentModelData modelData;
  // PuleSceneComponentPhysicsData physicsData;
  // modelData.type = info.modelType;
  // physicsData = info.physicsData;
  //
  // puleEcsEntityAttachComponent(
  //   s->worldEcs, info.entity, s->ecsComponentModel, &modelData
  // );
  // puleEcsEntityAttachComponent(
  //   s->worldEcs, info.entity, s->ecsComponentPhysics, &physicsData
  // );
}

} // extern "C"

// -- system callbacks

namespace pint {

struct SystemRenderInfo {
  PuleRenderGraph graph;
};

std::unordered_map<uint64_t, SystemRenderInfo> systemRenderInfos;

static PuleStringView renderGraphSource = R"(
  render-graph-label: "pule-scene-render-graph",
  render-graph: [
    {
      label: "pule-scene-draw",
      depends-on: [],
      resources: [
        {
          label: "pule-scene-framebuffer-image",
          image: {
            layout: "attachment-color", access: "attachment-color-write",
          },
        },
        {
          label: "pule-scene-framebuffer-depth",
          image: {
            layout: "attachment-depth",
            access: "attachment-depth-stencil-write",
          },
        },
      ],
    },
    {
      label: "pule-scene-blit",
      depends-on: ["pule-scene-draw",],
      resources: [
        {
          label: "pule-scene-framebuffer-image",
          image: {
            layout: "transfer-src", access: "transfer-read",
          },
        },
        {
          label: "window-swapchain-image",
          image: {
            layout: "transfer-dst", access: "transfer-write",
          },
        },
      ],
    },
  ],
  render-graph-resources: [
    {
      type: "image",
      name: "pule-scene-framebuffer-image",
      data-management: {
        type: "automatic",
        render-graph-usage: "read-write",
        is-attachment: true,
        scale-dimensions-relative: {
          reference-image: "window-swapchain-image",
          scale-width: 1.0,
          scale-height: 1.0,
        },
        initial-data: "",
        target: "2d",
        byte-format: "rgba8u",
        mipmap-levels: 1,
        array-layers: 1,
      },
    },
    {
      type: "image",
      name: "pule-scene-framebuffer-depth",
      data-management: {
        type: "automatic",
        render-graph-usage: "read-write",
        is-attachment: true,
        scale-dimensions-relative: {
          reference-image: "window-swapchain-image",
          scale-width: 1.0,
          scale-height: 1.0,
        },
        initial-data: "",
        target: "2d",
        byte-format: "depth16",
        mipmap-levels: 1,
        array-layers: 1,
      },
    },
  ],
)"_psv;

void createSystemRender(pint::Scene & scene, PuleScene const puScene) {
  // SystemRenderInfo info;
  // PuleError err = puleError();
  // PuleDsValue const renderGraphValue = (
  //   puleAssetPdsLoadFromString(puleAllocateDefault(), renderGraphSource, &err)
  // );
  // if (puleErrorConsume(&err)) {
  //   puleLogError("failed to create PuleScene system-render graph");
  // }
  // info.graph = (
  //   puleAssetRenderGraphFromPds(
  //     puleAllocateDefault(),
  //     scene.platform,
  //     renderGraphValue
  //   )
  // );
  // systemRenderInfos.emplace(puScene.id, info);
}

} // namespace pint

extern "C" {

void systemModelRenderCallback(PuleEcsIterator const iter) {
  // // "pule-scene-model,pule-scene-object",
  // PuleSceneComponentModelData const * const modelData = (
  //   static_cast<PuleSceneComponentModelData const *>(
  //     puleEcsIteratorQueryComponents(
  //       iter, 0, sizeof(PuleSceneComponentModelData)
  //     )
  //   )
  // );
  // PuleSceneComponentPhysicsData const * const physxData = (
  //   static_cast<PuleSceneComponentPhysicsData const *>(
  //     puleEcsIteratorQueryComponents(
  //       iter, 1, sizeof(PuleSceneComponentPhysicsData)
  //     )
  //   )
  // );
  // PuleScene const puScene = { (uint64_t)puleEcsIteratorUserData(iter) };
  // auto & scene = *pint::scenes.at(puScene);
  // auto const world = scene.worldEcs;
  //
  // if (pint::systemRenderInfos.count(puScene.id) == 0) {
  //   puleLog("creating system render");
  //   pint::createSystemRender(scene, puScene);
  // }
  //
  // auto system = pint::systemRenderInfos.at(puScene.id);
  //
  // puleRenderGraphFrameStart(system.graph);
  //
  // PuleRenderGraphNode renderNodeDraw = (
  //   puleRenderGraphNodeFetch(system.graph, "pule-scene-draw"_psv)
  // );
  // PuleRenderGraphNode renderNodeBlit = (
  //   puleRenderGraphNodeFetch(system.graph, "pule-scene-blit"_psv)
  // );
  // PuleGpuImage const framebufferImage = (
  //   puleGpuImageReference_image(
  //     puleRenderGraph_resource(
  //       system.graph, "pule-scene-framebuffer-image"_psv
  //     ).resource.image.imageReference
  //   )
  // );
  // PuleGpuImage const framebufferDepth = (
  //   puleGpuImageReference_image(
  //     puleRenderGraph_resource(
  //       system.graph, "pule-scene-framebuffer-depth"_psv
  //     ).resource.image.imageReference
  //   )
  // );
  // PuleGpuImage const windowImage = (
  //   puleGpuImageReference_image(
  //     puleRenderGraph_resource(
  //       system.graph, puleCStr("window-swapchain-image")
  //     ).resource.image.imageReference
  //   )
  // );
  //
  // PuleGpuCommandListRecorder commandDraw = (
  //   puleRenderGraph_commandListRecorder(renderNodeDraw)
  // );
  //
  // auto debugRecorder = (
  //   puleGfxDebugStart(
  //     puleF32m44(1.0f),
  //     puleCameraView(scene.camera),
  //     puleCameraProj(scene.camera)
  //   )
  // );
  //
  // size_t const entityCount = puleEcsIteratorEntityCount(iter);
  // for (size_t it = 0; it < entityCount; ++it) {
  //   auto const entity = puleEcsIteratorQueryEntities(iter)[it];
  //   auto const & model = modelData[it];
  //   auto const & physx = physxData[it];
  //   struct {
  //     PuleF32v3 position;
  //     PuleF32m33 rotation;
  //     PuleF32v3 scale;
  //   } object;
  //   //object.position = pulePhysx3DBodyOrigin(physx.body);
  //   //object.rotation = pulePhysx3DBodyOrientation(physx.body);
  //   //object.scale = puleF32v3(1.0f);
  //   //auto shape = pulePhysx3DShape(physx.body);
  //   //if (shape.type == PulePhysx3DShape_sphere) {
  //   //  object.scale = puleF32v3(shape.sphere.radius);
  //   //}
  //   //(void)entity;
  //
  //   switch (model.type) {
  //     case PuleSceneComponentModelType_none: break;
  //     case PuleSceneComponentModelType_cube:
  //       puleGfxDebugRender(
  //         debugRecorder,
  //         PuleGfxDebugRenderParam {
  //           .cube = {
  //             .originCenter = object.position,
  //             .rotationMatrix = object.rotation,
  //             .dimensionsHalf = object.scale,
  //             .color = {1.0f, 0.2f, 0.2f},
  //           },
  //         }
  //       );
  //     break;
  //     case PuleSceneComponentModelType_sphere:
  //       puleGfxDebugRender(
  //         debugRecorder,
  //         PuleGfxDebugRenderParam { // TODO change this to sphere
  //           .sphere = {
  //             .originCenter = object.position,
  //             .radius = object.scale.x,
  //             .color = {1.0f, 0.2f, 0.2f},
  //           },
  //         }
  //       );
  //     break;
  //     case PuleSceneComponentModelType_plane:
  //       puleGfxDebugRender(
  //         debugRecorder,
  //         PuleGfxDebugRenderParam {
  //           .plane = {
  //             .originCenter = object.position,
  //             .rotationMatrix = object.rotation,
  //             .color = {0.8f, 0.8f, 0.8f},
  //           },
  //         }
  //       );
  //     break;
  //   }
  // }
  //
  // puleGfxDebugSubmit({
  //   .recorder = debugRecorder,
  //   .commandListRecorder = commandDraw,
  //   .renderPassBegin = (
  //     PuleGpuActionRenderPassBegin {
  //       .viewportMin = {0, 0},
  //       .viewportMax = {800, 600}, // TODO
  //       .attachmentColor = {
  //         PuleGpuImageAttachment {
  //           .opLoad = PuleGpuImageAttachmentOpLoad_clear,
  //           .opStore = PuleGpuImageAttachmentOpStore_store,
  //           .layout = PuleGpuImageLayout_attachmentColor,
  //           .clear = { .color = {0.0f, 0.0f, 0.0f, 1.0f}},
  //           .imageView = {
  //             PuleGpuImageView {
  //               .image = framebufferImage,
  //               .mipmapLevelStart = 0, .mipmapLevelCount = 1,
  //               .arrayLayerStart = 0, .arrayLayerCount = 1,
  //               .byteFormat = PuleGpuImageByteFormat_rgba8U,
  //             }
  //           }
  //         }
  //       },
  //       .attachmentColorCount = 1,
  //       .attachmentDepth = {
  //         .opLoad = PuleGpuImageAttachmentOpLoad_clear,
  //         .opStore = PuleGpuImageAttachmentOpStore_store,
  //         .layout = PuleGpuImageLayout_attachmentDepth,
  //         .clear = { .depth = 1.0f, },
  //         .imageView = {
  //           PuleGpuImageView {
  //             .image = framebufferDepth,
  //             .mipmapLevelStart = 0, .mipmapLevelCount = 1,
  //             .arrayLayerStart = 0, .arrayLayerCount = 1,
  //             .byteFormat = PuleGpuImageByteFormat_depth16,
  //           }
  //         },
  //       },
  //     }
  //   ),
  // });
  //
  // PuleGpuCommandListRecorder commandBlit = (
  //   puleRenderGraph_commandListRecorder(renderNodeBlit)
  // );
  //
  // puleGpuCommandListAppendAction(
  //   commandBlit,
  //   PuleGpuCommand {
  //     .copyImageToImage = {
  //       .action = PuleGpuAction_copyImageToImage,
  //       .srcImage = framebufferImage,
  //       .dstImage = windowImage,
  //       .srcOffset = { .x = 0, .y = 0, },
  //       .dstOffset = { .x = 0, .y = 0, },
  //       .extent = {
  //         .x = 800, .y = 600, .z = 1, // TODO
  //       },
  //     },
  //   }
  // );
  //
  // PuleGpuSemaphore waitSemaphore = puleGpuSwapchainAvailableSemaphore();
  // if (scene.gpuFrameWaitSemaphore.id != 0) {
  //   waitSemaphore = scene.gpuFrameWaitSemaphore;
  // }
  //
  // puleRenderGraphFrameSubmit(waitSemaphore, system.graph);
}

} // extern "C"
