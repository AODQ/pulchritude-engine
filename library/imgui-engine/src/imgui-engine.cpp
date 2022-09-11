#include <pulchritude-imgui-engine/imgui-engine.h>

#include <pulchritude-ecs/ecs.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gfx/commands.h>
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-gfx/image.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-log/log.h>
#include <pulchritude-platform/platform.h>

namespace {

PuleEcsComponent focusedComponent = { .id = 0, };

void displayEcsComponent(
  PuleEcsComponent const component,
  void * const userdata
) {
  auto const info = *reinterpret_cast<PuleImguiEngineDisplayInfo *>(userdata);
  PuleEcsComponentInfo const componentInfo = (
    puleEcsComponentInfo(info.world, component)
  );
  if (!puleImguiBeginSection(componentInfo.label.contents)) {
    return;
  }

  if (puleImguiButton("focus entities")) {
    focusedComponent = component;
  }


  if (componentInfo.imguiOverviewCallback) {
    componentInfo.imguiOverviewCallback();
  }

  puleImguiEndSection();
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
    puleImguiBeginWindow(windowBegin.contents, &opened);
    puleStringDeallocate(windowBegin);
  }

  if (!opened) {
    focusedComponent.id = 0;
    return;
  }

  PuleError err = puleError();

  PuleEcsQuery const query = (
    puleEcsQueryByComponent(info.world, &focusedComponent, 1, &err)
  );
  if (puleErrorConsume(&err)) {
    focusedComponent.id = 0;
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
      puleImguiText(
        "entity %zu name '%s'",
        entities[it].id,
        puleEcsEntityName(info.world, entities[it])
      );
    }
  }
  puleEcsQueryIteratorDestroy(queryIter);
  puleEcsQueryDestroy(query);
}

void displayEcsComponentList(PuleImguiEngineDisplayInfo const info) {
  puleImguiBeginWindow("ecs component list", nullptr);
  puleEcsComponentIterateAll(
    PuleEcsComponentIterateAllCallback {
      .world = info.world,
      .userdata = const_cast<void *>(reinterpret_cast<void const *>(&info)),
      .fn = &displayEcsComponent,
    }
  );

  puleImguiEndWindow();
}

} // namespace

extern "C" {

void puleImguiEngineDisplay(PuleImguiEngineDisplayInfo const info) {
  displayEcsComponentList(info);
  displayEcsEntityList(info);
}

} // C
