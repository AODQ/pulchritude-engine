#pragma once
// TODO rename to renderer-3d-mesh ?

#include <pulchritude-ecs/ecs.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-asset/model.h>

// simple 3D renderer for most graphical operations

// there are four phases to render an object, disregarding creation/deletion
//   of system
//  - 'prepare'; load it somehow, tell renderer it will be rendered later
//  - 'attach'; attach prepared model as component
//  - 'update'; update instances, and each instance's dynamic information
//              this happens as part of your component's update phase
//  - 'render'; allow the renderer to render it using information from 'prepare'
//              this happens during renderer's system update
//
// to create the renderer is simple
//  - creating renderer attaches it to your world as a system
//
// 'update' part is possibly the most user-involved. You have to assign a
//   component to a specified model/mesh, then attach that entity to the system.
//
// this by no means will be able to render everything in the scene, this
//   specializes in generic meshes. There are many cases like water, skyboxes,
//   particles, etc that require their own type of renderer.

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t id;
} PuleRenderer3D;

PULE_exportFn PuleRenderer3D puleRenderer3DCreate(
  PuleEcsWorld const world,
  PulePlatform const platform
);

PULE_exportFn PuleEcsSystem puleRenderer3DEcsSystem(
  PuleRenderer3D const renderer3D
);

typedef struct {
  size_t id;
} PuleRenderer3DModel;

// this will prepare rendering for you, but this renderer can only assume so
//   much about how you intend to render. In most cases then you are going to
//   get what you want, but
// in some cases you might need to use your own mesh or material shader. in the
//   default case what's going to happen is that all your attributes are going
//   to be
// collapsed into a single buffer, and the mesh shader will use the element
//   buffer to access these values. A best attempt to creating a mesh shader
//   based off your input data will be used, however to keep a reasonable and
//   performant path, align all your models to use similar attribute types so
//   that many mesh shaders can be reused
PULE_exportFn PuleRenderer3DModel puleRenderer3DPrepareModel(
  PuleRenderer3D renderer3D,
  PuleAssetModel const model
);

// TODO right now just attach model, but in future need to attach model + mesh
//      + material, to form a unique set

PULE_exportFn PuleEcsComponent puleRenderer3DAttachComponentRender(
  PuleEcsWorld const world,
  PuleRenderer3D const renderer3DSystem,
  PuleEcsEntity const entity,
  PuleRenderer3DModel const model
);

#ifdef __cplusplus
} // C
#endif
