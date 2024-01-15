#include <pulchritude-scene/scene.h>

#include <pulchritude-asset/pds.h>
#include <pulchritude-asset/render-graph.h>
#include <pulchritude-gfx-debug/gfx-debug.h>
#include <pulchritude-render-graph/render-graph.h>

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
  structField(i64, type),
  PULE_dsStructTerminator,
};
#undef structField

// object
#define structField(fieldtype, member) \
  PULE_dsStructField(PuleSceneComponentObjectData, fieldtype, member, 1)
static PuleDsStructField const componentObjectFields[] = {
  structField(f32v3, position),
  structField(f32v3, rotation),
  structField(f32v3, scale),
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
componentSerialize(Object)

#undef componentSerialize

} // extern "C"


// -- scene --------------------------------------------------------------------

namespace pint {

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
  PuleEcsComponent ecsComponentObject;

  PuleEcsSystem ecsSystemPhysicsUpdate; // sets position/rotation of object
  PuleEcsSystem ecsSystemModelRender;
};

pule::ResourceContainer<Scene, PuleScene> scenes;
// to lookup Scene from EcsWorld ID in ecs update callback
std::unordered_map<uint64_t, pint::Scene> ecsWorldToScene;

} // namespace pint

extern "C" {

void systemPhysicsUpdateCallback(PuleEcsIterator const iter);
void systemModelRenderCallback(PuleEcsIterator const iter);

PuleScene puleSceneCreate(PuleSceneCreateInfo const info) {
  pint::Scene scene;
  // fill in scene data
  scene.platform = info.platform;
  scene.createdEcsWorld = true;
  scene.createdPhysxWorld = info.createPhysxWorld;
  scene.worldEcs = PuleEcsWorld { .id = 0 };
  if (scene.createdEcsWorld) {
    scene.worldEcs = puleEcsWorldCreate();
  }
  scene.dimension = info.dimension;
  if (info.createPhysxWorld && scene.dimension == PuleSceneDimension_3d) {
    scene.worldPhysx3D = pulePhysx3DWorldCreate();
  } else if (info.createPhysxWorld) {
    scene.worldPhysx2D = pulePhysx2DWorldCreate();
  } else {
    scene.worldPhysx3D = PulePhysx3DWorld { .id = 0 };
  }

  // create ecs components
  if (scene.worldEcs.id) {
    #define componentCreate(_label, _tagLabel) \
      scene.ecsComponent##_label = ( \
        puleEcsComponentCreate( \
          scene.worldEcs, \
          PuleEcsComponentCreateInfo { \
            .label = "pule-scene-" #_tagLabel, \
            .byteLength = sizeof(PuleSceneComponent##_label##Data), \
            .byteAlignment = alignof(PuleSceneComponent##_label##Data), \
            .serializeComponentCallback = component##_label##Serialize, \
            .deserializeComponentCallback = component##_label##Deserialize, \
          } \
        ) \
      );
    componentCreate(Model, model);
    componentCreate(Physics, physics);
    componentCreate(Object, object);
    #undef componentCreate
  }

  // create ecs systems
  if (scene.worldEcs.id) {
    scene.ecsSystemModelRender = (
      puleEcsSystemCreate(
        scene.worldEcs,
        PuleEcsSystemCreateInfo {
          .label = "pule-scene-model-render",
          .commaSeparatedComponentLabels = "pule-scene-model,pule-scene-object",
          .callback = systemModelRenderCallback,
          .callbackFrequency = PuleEcsSystemCallbackFrequency_postUpdate,
        }
      )
    );
    scene.ecsSystemPhysicsUpdate = (
      puleEcsSystemCreate(
        scene.worldEcs,
        PuleEcsSystemCreateInfo {
          .label = "pule-scene-physics-update",
          .commaSeparatedComponentLabels = (
            "pule-scene-physics,pule-scene-object"
          ),
          .callback = systemPhysicsUpdateCallback,
          .callbackFrequency = PuleEcsSystemCallbackFrequency_preUpdate,
        }
      )
    );
  }

  //
  PuleScene puScene = pint::scenes.create(scene);
  pint::ecsWorldToScene.emplace(scene.worldEcs.id, scene);
  return puScene;
}

void puleSceneDestroy(PuleScene const scene) {
  if (scene.id == 0) { return; }
  auto const s = pint::scenes.at(scene);
  if (s->createdEcsWorld) {
    puleEcsWorldDestroy(s->worldEcs);
  }
  if (s->createdPhysxWorld) {
    if (s->dimension == PuleSceneDimension_3d) {
      pulePhysx3DWorldDestroy(s->worldPhysx3D);
    } else {
      pulePhysx2DWorldDestroy(s->worldPhysx2D);
    }
  }
}

PULE_exportFn PuleEcsWorld puleSceneEcsWorld(PuleScene const scene) {
  auto const s = pint::scenes.at(scene);
  return s->worldEcs;
}

PULE_exportFn PuleScenePhysxWorld puleScenePhysxWorld(PuleScene const scene) {
  PuleScenePhysxWorld result;
  auto const s = pint::scenes.at(scene);
  if (s->dimension == PuleSceneDimension_3d) {
    result.i3D = s->worldPhysx3D;
  } else {
    result.i2D = s->worldPhysx2D;
  }
  return result;
}

void puleSceneAdvance(PuleSceneAdvanceInfo const info){
  auto const s = pint::scenes.at(info.scene);
  // advance physx system first
  if (s->worldPhysx3D.id != 0 && info.advancePhysxWorld) {
    if (s->dimension == PuleSceneDimension_3d) {
      pulePhysx3DWorldAdvance(s->worldPhysx3D, info.msDelta);
    } else {
      pulePhysx2DWorldStep(s->worldPhysx2D, info.msDelta, 8, 3);
    }
  }
  // then update camera controls
  // TODO
  // then advance ECS (which should update objects + rendering)
  if (s->worldEcs.id != 0 && info.advanceEcsWorld) {
    puleEcsWorldAdvance(s->worldEcs, info.msDelta);
  }
}

PuleEcsComponent puleSceneComponentObject(PuleScene const scene) {
  auto const s = pint::scenes.at(scene);
  return s->ecsComponentObject;
}
PuleEcsComponent puleSceneComponentModel(PuleScene const scene) {
  auto const s = pint::scenes.at(scene);
  return s->ecsComponentModel;
}
PuleEcsComponent puleSceneComponentPhysics(PuleScene const scene) {
  auto const s = pint::scenes.at(scene);
  return s->ecsComponentPhysics;
}

} // extern "C"

// -- system callbacks

namespace pint {

struct SystemRenderInfo {
  PuleRenderGraph graph;
};

std::unordered_map<uint64_t, SystemRenderInfo> systemRenderInfos;

static PuleStringView renderGraphSource = R"(
  render-graph: [
    {
      label: "pule-scene-graph-draw",
      resources: [
        {
          label: "pule-sccene-graph-framebuffer-image",
          image: {
            layout: "attachment-color", access: "attachment-color-write",
          },
        },
      ],
      depends-on: [],
    }
    {
      label: "pule-scene-graph-blit",
      resources: [
        {
          label: "pule-sccene-graph-framebuffer-image",
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
  ],
)"_pcs;

void createSystemRender(PuleEcsWorld const world) {
  auto scene = ecsWorldToScene.at(world.id);
  SystemRenderInfo info;
  PuleError err = puleError();
  PuleDsValue const renderGraphValue = (
    puleAssetPdsLoadFromString(puleAllocateDefault(), renderGraphSource, &err)
  );
  if (puleErrorConsume(&err)) {
    puleLogError("failed to create PuleScene system-render graph");
  }
  info.graph = (
    puleAssetRenderGraphFromPds(
      puleAllocateDefault(),
      scene.platform,
      renderGraphValue
    )
  );
  systemRenderInfos.emplace(world.id, info);
}

} // namespace pint

extern "C" {

void systemModelRenderCallback(PuleEcsIterator const iter) {
  // "pule-scene-model,pule-scene-object",
  PuleSceneComponentModelData const * const modelData = (
    static_cast<PuleSceneComponentModelData const *>(
      puleEcsIteratorQueryComponents(
        iter, 0, sizeof(PuleSceneComponentModelData)
      )
    )
  );
  PuleSceneComponentObjectData const * const objectData = (
    static_cast<PuleSceneComponentObjectData const *>(
      puleEcsIteratorQueryComponents(
        iter, 1, sizeof(PuleSceneComponentObjectData)
      )
    )
  );
  auto const world = puleEcsIteratorWorld(iter);

  if (pint::systemRenderInfos.count(world.id) == 0) {
    pint::createSystemRender(world);
  }

  auto system = pint::systemRenderInfos.at(world.id);

  PuleRenderGraphNode renderNodeDraw = (
    puleRenderGraphNodeFetch(system.graph, "physx-draw"_pcs)
  );
  PuleRenderGraphNode renderNodeBlit = (
    puleRenderGraphNodeFetch(system.graph, "physx-blit"_pcs)
  );
  PuleGpuImage const framebufferImage = (
    puleGpuImageReference_image(
      puleRenderGraph_resource(
        system.graph, "pule-scene-framebuffer-image"_pcs
      ).image.imageReference
    )
  );
  PuleGpuImage const windowImage = (
    puleGpuImageReference_image(
      puleRenderGraph_resource(
        system.graph, puleCStr("window-swapchain-image")
      ).image.imageReference
    )
  );

  PuleGpuCommandListRecorder commandDraw = (
    puleRenderGraph_commandListRecorder(renderNodeDraw)
  );

  auto debugRecorder = (
    puleGfxDebugStart(
      commandDraw,
      PuleGpuActionRenderPassBegin {
        .viewportMin = {0, 0},
        .viewportMax = {800, 600}, // TODO
        .attachmentColor = {
          PuleGpuImageAttachment {
            .opLoad = PuleGpuImageAttachmentOpLoad_clear,
            .opStore = PuleGpuImageAttachmentOpStore_store,
            .layout = PuleGpuImageLayout_attachmentColor,
            .clearColor = {0.0f, 0.0f, 0.0f, 1.0f},
            .imageView = {
              PuleGpuImageView {
                .image = framebufferImage,
                .mipmapLevelStart = 0, .mipmapLevelCount = 1,
                .arrayLayerStart = 0, .arrayLayerCount = 1,
                .byteFormat = PuleGpuImageByteFormat_rgba8U,
              }
            }
          }
        },
        .attachmentColorCount = 1,
      },
      puleF32m44(1.0f)
    )
  );

  size_t const entityCount = puleEcsIteratorEntityCount(iter);
  for (size_t it = 0; it < entityCount; ++it) {
    auto const entity = puleEcsIteratorQueryEntities(iter)[it];
    auto const & model = modelData[it];
    auto const & object = objectData[it];
    (void)entity; (void)model; (void)object; (void)world;

    switch (model.type) {
      case PuleSceneComponentModelType_none: break;
      case PuleSceneComponentModelType_cube:
        puleGfxDebugRender(
          debugRecorder,
          PuleGfxDebugRenderParam {
            .cube = {
              .originCenter = object.position,
              .rotation = object.rotation,
              .dimensionsHalf = object.scale,
              .color = {1.0f, 0.2f, 0.2f},
            },
          }
        );
      break;
      case PuleSceneComponentModelType_sphere:
        puleLogError("sphere not implemented");
      break;
      case PuleSceneComponentModelType_plane:
        puleLogError("plane not implemented");
      break;
    }
  }
  puleGfxDebugEnd(debugRecorder);

  PuleGpuCommandListRecorder commandBlit = (
    puleRenderGraph_commandListRecorder(renderNodeBlit)
  );

  puleGpuCommandListAppendAction(
    commandBlit,
    PuleGpuCommand {
      .copyImageToImage = {
        .action = PuleGpuAction_copyImageToImage,
        .srcImage = framebufferImage,
        .dstImage = windowImage,
        .srcOffset = { .x = 0, .y = 0, },
        .dstOffset = { .x = 0, .y = 0, },
        .extent = {
          .x = 800, .y = 600, .z = 1, // TODO
        },
      },
    }
  );
}

void systemPhysicsUpdateCallback(PuleEcsIterator const iter) {
  // "pule-scene-physics,pule-scene-object",
  PuleSceneComponentPhysicsData const * const physicsData = (
    static_cast<PuleSceneComponentPhysicsData const *>(
      puleEcsIteratorQueryComponents(
        iter, 0, sizeof(PuleSceneComponentPhysicsData)
      )
    )
  );
  PuleSceneComponentObjectData * const objectData = (
    static_cast<PuleSceneComponentObjectData *>(
      puleEcsIteratorQueryComponents(
        iter, 1, sizeof(PuleSceneComponentObjectData)
      )
    )
  );
  auto const world = puleEcsIteratorWorld(iter);

  size_t const entityCount = puleEcsIteratorEntityCount(iter);
  for (size_t it = 0; it < entityCount; ++it) {
    auto const entity = puleEcsIteratorQueryEntities(iter)[it];
    auto const & physics = physicsData[it];
    auto & object = objectData[it];

    // TODO update physics
    puleLog("object position: %f", object.position.y);
    object.position = puleF32v3(0.0f);
    object.rotation = puleF32v3(0.0f);
    object.scale = puleF32v3(1.0f);
  }
}

} // extern "C"
