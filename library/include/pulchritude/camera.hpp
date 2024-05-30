/* auto generated file camera */
#pragma once
#include "core.h"

#include "camera.h"
#include "data-serializer.hpp"
#include "gpu.hpp"
#include "math.hpp"
#include "platform.hpp"

namespace pule {
struct Camera {
  PuleCamera _handle;
  inline operator PuleCamera() const {
    return _handle;
  }
  inline void destroy() {
    return puleCameraDestroy(this->_handle);
  }
  inline PuleF32m44 view() {
    return puleCameraView(this->_handle);
  }
  inline PuleF32m44 proj() {
    return puleCameraProj(this->_handle);
  }
  inline void lookAt(PuleF32v3 origin, PuleF32v3 normalizedTarget, PuleF32v3 up) {
    return puleCameraLookAt(this->_handle, origin, normalizedTarget, up);
  }
  inline PuleCameraPerspective perspective() {
    return puleCameraPerspective(this->_handle);
  }
  inline void perspectiveSet(PuleCameraPerspective perspective) {
    return puleCameraPerspectiveSet(this->_handle, perspective);
  }
  static inline Camera create() {
    return { ._handle = puleCameraCreate(),};
  }
};
}
  inline void destroy(pule::Camera self) {
    return puleCameraDestroy(self._handle);
  }
  inline PuleF32m44 view(pule::Camera self) {
    return puleCameraView(self._handle);
  }
  inline PuleF32m44 proj(pule::Camera self) {
    return puleCameraProj(self._handle);
  }
  inline void lookAt(pule::Camera self, PuleF32v3 origin, PuleF32v3 normalizedTarget, PuleF32v3 up) {
    return puleCameraLookAt(self._handle, origin, normalizedTarget, up);
  }
  inline PuleCameraPerspective perspective(pule::Camera self) {
    return puleCameraPerspective(self._handle);
  }
  inline void perspectiveSet(pule::Camera self, PuleCameraPerspective perspective) {
    return puleCameraPerspectiveSet(self._handle, perspective);
  }
namespace pule {
struct CameraSet {
  PuleCameraSet _handle;
  inline operator PuleCameraSet() const {
    return _handle;
  }
  inline void destroy() {
    return puleCameraSetDestroy(this->_handle);
  }
  inline void add(PuleCamera camera) {
    return puleCameraSetAdd(this->_handle, camera);
  }
  inline void remove(PuleCamera camera) {
    return puleCameraSetRemove(this->_handle, camera);
  }
  inline PuleCameraSetArray array() {
    return puleCameraSetArray(this->_handle);
  }
  inline PuleGpuBuffer uniformBuffer() {
    return puleCameraSetUniformBuffer(this->_handle);
  }
  inline PuleGpuFence refresh() {
    return puleCameraSetRefresh(this->_handle);
  }
  static inline CameraSet create(PuleStringView label) {
    return { ._handle = puleCameraSetCreate(label),};
  }
};
}
  inline void destroy(pule::CameraSet self) {
    return puleCameraSetDestroy(self._handle);
  }
  inline void add(pule::CameraSet self, PuleCamera camera) {
    return puleCameraSetAdd(self._handle, camera);
  }
  inline void remove(pule::CameraSet self, PuleCamera camera) {
    return puleCameraSetRemove(self._handle, camera);
  }
  inline PuleCameraSetArray array(pule::CameraSet self) {
    return puleCameraSetArray(self._handle);
  }
  inline PuleGpuBuffer uniformBuffer(pule::CameraSet self) {
    return puleCameraSetUniformBuffer(self._handle);
  }
  inline PuleGpuFence refresh(pule::CameraSet self) {
    return puleCameraSetRefresh(self._handle);
  }
namespace pule {
struct CameraController {
  PuleCameraController _handle;
  inline operator PuleCameraController() const {
    return _handle;
  }
  inline void destroy() {
    return puleCameraControllerDestroy(this->_handle);
  }
  static inline CameraController firstPerson(PulePlatform platform, PuleCamera camera) {
    return { ._handle = puleCameraControllerFirstPerson(platform, camera),};
  }
  static inline CameraController orbit(PulePlatform platform, PuleCamera camera, PuleF32v3 origin, float radius) {
    return { ._handle = puleCameraControllerOrbit(platform, camera, origin, radius),};
  }
};
}
  inline void destroy(pule::CameraController self) {
    return puleCameraControllerDestroy(self._handle);
  }
namespace pule {
using CameraPerspective = PuleCameraPerspective;
}
namespace pule {
using CameraSetArray = PuleCameraSetArray;
}
namespace pule {
}
