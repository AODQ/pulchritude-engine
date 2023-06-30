#include <pulchritude-imgui-engine/imgui-engine.h>

#include <pulchritude-ecs/ecs.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gpu/commands.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-gpu/image.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-log/log.h>
#include <pulchritude-platform/platform.h>

// only needed for pulcRenderGraph which should probably
// be moved later
#include <pulchritude-task-graph/task-graph.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-asset/render-graph.h>

#include <string>

namespace {

PuleEcsComponent focusedComponent = { .id = 0, };
PuleEcsEntity focusedEntity = { .id = 0, };

void displayEcsComponent(
  PuleEcsComponent const component,
  void * const userdata
) {
  auto const info = *reinterpret_cast<PuleImguiEngineDisplayInfo *>(userdata);
  PuleEcsComponentInfo const componentInfo = (
    puleEcsComponentInfo(info.world, component)
  );
  if (!puleImguiSectionBegin(componentInfo.label.contents)) {
    return;
  }

  if (puleImguiButton("focus entities")) {
    focusedComponent = component;
  }

  puleImguiSectionEnd();
}

struct FocusedEntityComponentCallbackInfo {
  PuleEcsWorld world;
  PuleEcsEntity entity;
};

void displayEcsFocusedEntityComponentCallback(
  PuleEcsComponent const component,
  void const * const userdata
) {
  auto const info = (
    *reinterpret_cast<FocusedEntityComponentCallbackInfo const *>(userdata)
  );
  PuleEcsComponentInfo const componentInfo = (
    puleEcsComponentInfo(info.world, component)
  );
  if (componentInfo.imguiEntityCallback) {
    componentInfo.imguiEntityCallback(
      info.world,
      info.entity,
      component
    );
  }
}

void displayEcsFocusedEntity(PuleImguiEngineDisplayInfo const info) {
  if (focusedEntity.id == 0) {
    return;
  }
  bool opened = true;
  puleImguiWindowBegin("entity", &opened);
  if (!opened) {
    puleImguiWindowEnd();
    focusedEntity.id = 0;
    return;
  }

  auto const callbackInfo = FocusedEntityComponentCallbackInfo {
    .world = info.world,
    .entity = focusedEntity,
  };

  puleEcsEntityIterateComponents( PuleEcsEntityIterateComponentsInfo {
    .world = info.world,
    .entity = focusedEntity,
    .userdata = &callbackInfo,
    .callback = &displayEcsFocusedEntityComponentCallback,
  });

  puleImguiWindowEnd();
}

void displayEcsEntityList(PuleImguiEngineDisplayInfo const info) {
  if (focusedComponent.id == 0) {
    return;
  }

  PuleEcsComponentInfo const focusedComponentInfo = (
    puleEcsComponentInfo(info.world, focusedComponent)
  );

  bool opened = true;
  { // open window
    PuleString const windowBegin = (
      puleStringFormat(
        puleAllocateDefault(),
        "entity list for '%s'", focusedComponentInfo.label.contents
      )
    );
    puleImguiWindowBegin(windowBegin.contents, &opened);
    puleStringDestroy(windowBegin);
  }

  if (!opened) {
    focusedComponent.id = 0;
    puleImguiWindowEnd();
    return;
  }

  PuleError err = puleError();

  PuleEcsQuery const query = (
    puleEcsQueryByComponent(info.world, &focusedComponent, 1, &err)
  );
  if (puleErrorConsume(&err)) {
    focusedComponent.id = 0;
    puleImguiWindowEnd();
    return;
  }

  PuleEcsQueryIterator const queryIter = (
    puleEcsQueryIterator(info.world, query)
  );
  for (
    PuleEcsIterator iter = puleEcsQueryIteratorNext(queryIter);
    iter.id != 0;
    iter = puleEcsQueryIteratorNext(queryIter)
  ) {
    size_t const entityCount = puleEcsIteratorEntityCount(iter);
    PuleEcsEntity * const entities = puleEcsIteratorQueryEntities(iter);
    for (size_t it = 0; it < entityCount; ++ it) {
      if (puleImguiButton("o")) {
        focusedEntity = entities[it];
      }
      puleImguiJoinNext();
      puleImguiText(
        "entity %zu name '%s'",
        entities[it].id,
        puleEcsEntityName(info.world, entities[it])
      );
    }
  }
  puleEcsQueryIteratorDestroy(queryIter);
  puleEcsQueryDestroy(query);
  puleImguiWindowEnd();
}

void displayEcsComponentList(PuleImguiEngineDisplayInfo const info) {
  puleImguiWindowBegin("ecs component list", nullptr);
  puleEcsComponentIterateAll(
    PuleEcsComponentIterateAllCallback {
      .world = info.world,
      .userdata = const_cast<void *>(reinterpret_cast<void const *>(&info)),
      .fn = &displayEcsComponent,
    }
  );

  puleImguiWindowEnd();
}

} // namespace

extern "C" {

void puleImguiEngineDisplay(PuleImguiEngineDisplayInfo const info) {
  displayEcsComponentList(info);
  displayEcsEntityList(info);
  displayEcsFocusedEntity(info);
}

////////////////////////////////////////////////////////////////////////////////
//-- RENDER TASK GRAPH ---------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////

extern "C" {
PuleRenderGraph pulcRenderGraph(PulePlatform const platform) {
  #include "imgui-engine-asset-render-graph.mixin"
  PuleError err = puleError();
  PuleDsValue const assetRenderPds = (
    puleAssetPdsLoadFromString(
      puleAllocateDefault(),
      PuleStringView{assetRenderGraphStr.c_str(), assetRenderGraphStr.size(),},
      &err
    )
  );
  if (puleErrorConsume(&err)) { return { .id = 0, }; }
  PuleRenderGraph const renderGraph = (
    puleAssetRenderGraphFromPds(
      puleAllocateDefault(),
      platform,
      assetRenderPds
    )
  );
  puleDsDestroy(assetRenderPds);
  return renderGraph;
}
} // extern C

} // C
