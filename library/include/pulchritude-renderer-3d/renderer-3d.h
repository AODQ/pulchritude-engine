#pragma once

#include <pulchritude-ecs/ecs.h>

// simple 3D renderer for most graphical operations

#ifdef __cplusplus
extern "C" {
#endif

PuleEcsSystem puleRenderer3DSystem(
  PuleEcsWorld const world,
  PuleGfxContext const gfx
);

PuleEcsComponent puleRenderer3DAttachComponentRender(
  PuleEcsWorld const world,
  PuleEcsSystem const renderer3DSystem,
  PuleEcsEntity const entity,
  PuleAssetModel const model
  /* PuleAssetModelInstance const assetModel */
);
/* PuleEcsComponent puleRenderer3DModelInstanceCreate( */
/*   PuleAssetModelInstance const instance */
/* ); */

/* void puleRenderer3DAddModel(PuleAssetModelInstance const instance); */

#ifdef __cplusplus
} // C
#endif
