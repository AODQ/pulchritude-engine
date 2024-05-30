/* auto generated file scene */
#pragma once
#include "core.h"

#include "scene.h"
#include "ecs.hpp"
#include "gpu.hpp"
#include "physx.hpp"
#include "platform.hpp"
#include "camera.hpp"

namespace pule {
struct Scene {
  PuleScene _handle;
  inline operator PuleScene() const {
    return _handle;
  }
  inline void destroy() {
    return puleSceneDestroy(this->_handle);
  }
  inline void imageAttachmentsSet(PuleI32v2 viewportMin, PuleI32v2 viewportMax, PuleGpuImage colorAttachment, PuleGpuImage depthAttachment) {
    return puleSceneImageAttachmentsSet(this->_handle, viewportMin, viewportMax, colorAttachment, depthAttachment);
  }
  inline void clearColorSet(PuleF32v4 color) {
    return puleSceneClearColorSet(this->_handle, color);
  }
  inline void cameraSet(PuleCamera camera) {
    return puleSceneCameraSet(this->_handle, camera);
  }
  inline void cameraControllerSet(PuleCameraController controller) {
    return puleSceneCameraControllerSet(this->_handle, controller);
  }
  inline void ecsWorldSet(PuleEcsWorld world) {
    return puleSceneEcsWorldSet(this->_handle, world);
  }
  inline PuleEcsWorld ecsWorld() {
    return puleSceneEcsWorld(this->_handle);
  }
  inline PuleEcsWorld physxWorldSet(PuleScenePhysxWorld world) {
    return puleScenePhysxWorldSet(this->_handle, world);
  }
  inline PuleScenePhysxWorld physxWorld() {
    return puleScenePhysxWorld(this->_handle);
  }
  inline PuleEcsComponent componentModel() {
    return puleSceneComponentModel(this->_handle);
  }
  inline PuleEcsComponent componentPhysics() {
    return puleSceneComponentPhysics(this->_handle);
  }
  static inline Scene create(PuleSceneCreateInfo info) {
    return { ._handle = puleSceneCreate(info),};
  }
};
}
  inline void destroy(pule::Scene self) {
    return puleSceneDestroy(self._handle);
  }
  inline void imageAttachmentsSet(pule::Scene self, PuleI32v2 viewportMin, PuleI32v2 viewportMax, PuleGpuImage colorAttachment, PuleGpuImage depthAttachment) {
    return puleSceneImageAttachmentsSet(self._handle, viewportMin, viewportMax, colorAttachment, depthAttachment);
  }
  inline void clearColorSet(pule::Scene self, PuleF32v4 color) {
    return puleSceneClearColorSet(self._handle, color);
  }
  inline void cameraSet(pule::Scene self, PuleCamera camera) {
    return puleSceneCameraSet(self._handle, camera);
  }
  inline void cameraControllerSet(pule::Scene self, PuleCameraController controller) {
    return puleSceneCameraControllerSet(self._handle, controller);
  }
  inline void ecsWorldSet(pule::Scene self, PuleEcsWorld world) {
    return puleSceneEcsWorldSet(self._handle, world);
  }
  inline PuleEcsWorld ecsWorld(pule::Scene self) {
    return puleSceneEcsWorld(self._handle);
  }
  inline PuleEcsWorld physxWorldSet(pule::Scene self, PuleScenePhysxWorld world) {
    return puleScenePhysxWorldSet(self._handle, world);
  }
  inline PuleScenePhysxWorld physxWorld(pule::Scene self) {
    return puleScenePhysxWorld(self._handle);
  }
  inline PuleEcsComponent componentModel(pule::Scene self) {
    return puleSceneComponentModel(self._handle);
  }
  inline PuleEcsComponent componentPhysics(pule::Scene self) {
    return puleSceneComponentPhysics(self._handle);
  }
namespace pule {
using SceneCreateInfo = PuleSceneCreateInfo;
}
namespace pule {
using ScenePhysxWorld = PuleScenePhysxWorld;
}
namespace pule {
using SceneAdvanceInfo = PuleSceneAdvanceInfo;
}
namespace pule {
using SceneComponentModelData = PuleSceneComponentModelData;
}
namespace pule {
using SceneComponentPhysicsData = PuleSceneComponentPhysicsData;
}
namespace pule {
using SceneNodeCreateInfo = PuleSceneNodeCreateInfo;
}
namespace pule {
inline char const * toStr(PuleSceneDimension const e) {
  switch (e) {
    case PuleSceneDimension_i2d: return "i2d";
    case PuleSceneDimension_i3d: return "i3d";
    default: return "N/A";
  }
}
inline char const * toStr(PuleSceneComponentModelType const e) {
  switch (e) {
    case PuleSceneComponentModelType_none: return "none";
    case PuleSceneComponentModelType_cube: return "cube";
    case PuleSceneComponentModelType_sphere: return "sphere";
    case PuleSceneComponentModelType_plane: return "plane";
    default: return "N/A";
  }
}
}
