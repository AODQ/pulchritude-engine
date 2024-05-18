#include <pulchritude-camera/camera.h>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

// -- camera -------------------------------------------------------------------

struct Camera {
  PuleF32m44 projection;
  PuleCameraPerspective perspective;
  // orthogonal
  bool isPerspective;

  PuleF32m44 view;
  PuleF32v3 origin; PuleF32v3 forward; PuleF32v3 up;
};

struct CameraGpuData {
  PuleF32m44 projection;
  PuleF32m44 view;
};

namespace {
std::unordered_map<uint64_t, Camera> internalCameras;
uint64_t internalCameraCount = 1;
} // namespace

extern "C" {

PuleCamera puleCameraCreate() {
  Camera defaultCamera;
  memset(&defaultCamera, 0, sizeof(Camera));
  defaultCamera.up = PuleF32v3{0.0f, 1.0f, 0.0f};
  ::internalCameras.emplace(::internalCameraCount, defaultCamera);
  return PuleCamera { .id = ::internalCameraCount ++, };
}

void puleCameraDestroy(PuleCamera const camera) {
  ::internalCameras.erase(camera.id);
}

PuleF32m44 puleCameraView(PuleCamera const pCamera) {
  auto & camera = ::internalCameras.at(pCamera.id);
  return camera.view;
}
PuleF32m44 puleCameraProj(PuleCamera const pCamera) {
  auto & camera = ::internalCameras.at(pCamera.id);
  return camera.projection;
}

void puleCameraLookAt(
  PuleCamera const pCamera,
  PuleF32v3 const origin, PuleF32v3 const normalizedTarget, PuleF32v3 const up
) {
  auto & camera = ::internalCameras.at(pCamera.id);
  camera.origin = origin;
  camera.forward = normalizedTarget;
  camera.up = up;
  camera.view = puleViewLookAt(camera.origin, camera.forward, camera.up);
}

PuleCameraPerspective puleCameraPerspective(
  PuleCamera const pCamera
) {
  auto & camera = ::internalCameras.at(pCamera.id);
  return camera.perspective;
}
void puleCameraPerspectiveSet(
  PuleCamera const pCamera,
  PuleCameraPerspective const perspective
) {
  auto & camera = ::internalCameras.at(pCamera.id);
  camera.perspective = perspective;
  camera.isPerspective = true;
  camera.projection = (
    puleProjectionPerspective(
      camera.perspective.fieldOfViewRadians,
      camera.perspective.aspectRatio,
      camera.perspective.nearCutoff,
      camera.perspective.farCutoff
    )
  );
}

} // extern C

// -- camera set ---------------------------------------------------------------

struct CameraSet {
  std::string label;
  std::vector<PuleCamera> cameras;
  PuleGpuBuffer uniformBuffer;
  size_t uniformBufferCapacity;
  void * uniformBufferMemory;
};

namespace {
std::unordered_map<uint64_t, CameraSet> internalCameraSets;
uint64_t internalCameraSetCount = 1;

void puleCameraSetRefreshUniformBuffer(CameraSet & set) {
  // deallocate if need to grow or shrink space [TODO for shrink]
  bool const needsGrow = set.cameras.size() > set.uniformBufferCapacity;
  bool const needsShrink = false; // TODO

  // deallocate
  if (needsGrow || needsShrink) {
    if (set.uniformBufferMemory) {
      puleGpuBufferUnmap(set.uniformBuffer);
      set.uniformBufferMemory = nullptr;
    }
    if (set.uniformBuffer.id != 0) {
      puleGpuBufferDestroy(set.uniformBuffer);
      set.uniformBuffer.id = 0;
    }
  }

  // set capacity
  if (needsGrow) {
    set.uniformBufferCapacity = std::min(4, (int)set.cameras.size()*2+1);
  } else if (needsShrink) {
    // TODO
  }

  // allocate more space (this could be first allocation)
  if (!set.uniformBuffer.id) {
    std::string const label = set.label + "-uniform-buffer";
    set.uniformBuffer = (
      puleGpuBufferCreate(
        puleCStr(label.c_str()),
        nullptr,
        sizeof(CameraGpuData)*4,
        PuleGpuBufferUsage_uniform,
        PuleGpuBufferVisibilityFlag_hostWritable
      )
    );
  }

  if (!set.uniformBufferMemory) {
    set.uniformBufferMemory = (
      puleGpuBufferMap(
        PuleGpuBufferMapRange {
          .buffer = set.uniformBuffer,
          .access = PuleGpuBufferMapAccess_hostWritable,
          .byteOffset = 0,
          .byteLength = sizeof(CameraGpuData)*4
        }
      )
    );
  }
}

} // namespace

extern "C" {

PuleCameraSet puleCameraSetCreate(PuleStringView const label) {
  CameraSet set;
  set.label = std::string(label.contents, label.len);
  set.uniformBufferCapacity = 4;
  set.uniformBuffer = { .id = 0, };
  set.uniformBufferMemory = nullptr;
  puleCameraSetRefreshUniformBuffer(set);
  ::internalCameraSets.emplace(::internalCameraSetCount, set);
  return PuleCameraSet { .id = ::internalCameraSetCount ++, };
}

void puleCameraSetDestroy(PuleCameraSet const pSet) {
  auto & set = ::internalCameraSets.at(pSet.id);
  puleGpuBufferUnmap(set.uniformBuffer);
  puleGpuBufferDestroy(set.uniformBuffer);
   ::internalCameraSets.erase(pSet.id);
}

void puleCameraSetAdd(PuleCameraSet const pSet, PuleCamera const pCamera) {
  auto & set = ::internalCameraSets.at(pSet.id);
  set.cameras.emplace_back(pCamera);
}

void puleCameraSetRemove(PuleCameraSet const pSet, PuleCamera const pCamera) {
  auto & set = ::internalCameraSets.at(pSet.id);
  for (size_t it = 0; it < set.cameras.size(); ++ it) {
    if (set.cameras[it].id == pCamera.id) {
      set.cameras.erase(set.cameras.begin() + it);
      return;
    }
  }
}

PuleCameraSetArray puleCameraSetArray(PuleCameraSet const pSet) {
  auto & set = ::internalCameraSets.at(pSet.id);
  return (
    PuleCameraSetArray {
      .cameras = set.cameras.data(),
      .cameraCount = set.cameras.size(),
    }
  );
}

PuleGpuBuffer puleCameraSetUniformBuffer(PuleCameraSet const pSet) {
  auto & set = ::internalCameraSets.at(pSet.id);
  return set.uniformBuffer;
}

PuleGpuFence puleCameraSetRefresh(PuleCameraSet const pSet) {
  auto & set = ::internalCameraSets.at(pSet.id);
  puleCameraSetRefreshUniformBuffer(set);

  CameraGpuData * const cameraMemory = (
    reinterpret_cast<CameraGpuData *>(set.uniformBufferMemory)
  );
  /* *uniformBufferSizeMemory = set.uniformBufferCapacity; */
  for (size_t it = 0; it < set.cameras.size(); ++ it) {
    PuleCamera const camera = set.cameras[it];
    CameraGpuData * gpuMemory = cameraMemory + it;
    gpuMemory->projection = puleCameraProj(camera);
    gpuMemory->view = puleCameraView(camera);
  }
  puleGpuBufferMappedFlush({
    .buffer = set.uniformBuffer,
    .byteOffset = 0,
    .byteLength = sizeof(CameraGpuData)*set.cameras.size(),
  });
  return puleGpuFenceCreate(puleCStr("nil"));
}

} // extern C

// -- camera controller---------------------------------------------------------
// TODO make this its own API? expose to user how to append to list?

struct CameraControllerFps {
  PuleF32v3 origin;
  PuleF32v3 forward;
  PuleF32v3 up;

  PuleCamera camera;
  PulePlatform platform;

  PuleI32v2 prevMouseOrigin;
  float dirTheta, dirPhi;
};

struct CameraControllerOrbit {
  PuleF32v3 targetOrigin;
  float radius;
  float radiusOriginal;
  PuleF32v3 up;

  PuleCamera camera;
  PulePlatform platform;

  PuleI32v2 prevMouseOrigin;
  float dirTheta, dirPhi;
};

struct CameraControllerGeneric {
  std::vector<uint8_t> cameraData;
  void (*update)(void * const userdata);
};

namespace {
std::unordered_map<uint64_t, CameraControllerGeneric> cameraControllers;
uint64_t cameraControllerIt = 1;

void cameraControllerFpsUpdate(void * const userdata) {
  CameraControllerFps & controller = (
    *reinterpret_cast<CameraControllerFps *>(userdata)
  );

  // calculate view direction
  PuleI32v2 const mouse = pulePlatformMouseOrigin(controller.platform);
  auto const mouseDelta = (
    PuleF32v2 {
      .x = static_cast<float>(controller.prevMouseOrigin.x - mouse.x),
      .y = static_cast<float>(controller.prevMouseOrigin.y - mouse.y),
    }
  );
  controller.prevMouseOrigin = mouse;

  if (
       puleInputKey(controller.platform, PuleInputKey_escape)
    || puleInputKey(controller.platform, PuleInputKey_graveAccent)
  ) {
    pulePlatformCursorShow(controller.platform);
  }
  if (
       !pulePlatformCursorEnabled(controller.platform)
    && puleInputMouse(controller.platform, PuleInputMouse_left)
  ) {
    pulePlatformCursorHide(controller.platform);
  }

  controller.dirTheta -= mouseDelta.x * 0.005f;
  controller.dirPhi += mouseDelta.y * 0.005f;
  if (controller.dirPhi > 1.3f)
    controller.dirPhi = 1.3f;
  if (controller.dirPhi < -1.3f)
    controller.dirPhi = -1.3f;
  controller.forward = (
    puleF32v3Normalize( PuleF32v3 {
      (float)sin(controller.dirTheta),
      (float)controller.dirPhi,
      (float)cos(controller.dirTheta),
    })
  );
  controller.up = PuleF32v3 { 0.0f, 1.0f, 0.0f, };

  // calculate movement
  PuleF32v3 const moveDirection = PuleF32v3 {
    (
      (float)(puleInputKey(controller.platform, PuleInputKey_d))
    - (float)(puleInputKey(controller.platform, PuleInputKey_a))
    ),
    (
      (float)(puleInputKey(controller.platform, PuleInputKey_r))
    - (float)(puleInputKey(controller.platform, PuleInputKey_f))
    ),
    (
      (float)(puleInputKey(controller.platform, PuleInputKey_w))
    - (float)(puleInputKey(controller.platform, PuleInputKey_s))
    ),
  };

  // horizontal
  controller.origin.x += (
    moveDirection.x*0.01f*(float)cos(controller.dirTheta)
    + moveDirection.z*0.01f*(float)sin(controller.dirTheta)
  );
  controller.origin.z -= (
    moveDirection.x*0.01f*(float)sin(controller.dirTheta)
    + -moveDirection.z*0.01f*(float)cos(controller.dirTheta)
  );

  // veretical
  controller.origin.y += moveDirection.y*0.02f;
  /* controller.origin.y += ( */
  /*   moveDirection.z*0.01f*(float)controller.dirPhi */
  /* ); */

  puleCameraPerspectiveSet(
    controller.camera,
    PuleCameraPerspective {
      .nearCutoff = 0.01f,
      .farCutoff = 100.0f,
      .aspectRatio = 800.0f/600.0f,
      .fieldOfViewRadians = 70.0f,
    }
  );

  puleCameraLookAt(
    controller.camera,
    controller.origin,
    puleF32v3Add(controller.forward, controller.origin),
    controller.up
  );
}

void cameraControllerOrbitUpdate(void * const userdata) {
  CameraControllerOrbit & controller = (
    *reinterpret_cast<CameraControllerOrbit *>(userdata)
  );
  PuleI32v2 const mouse = pulePlatformMouseOrigin(controller.platform);
  auto mouseDelta = (
    PuleF32v2 {
      .x = static_cast<float>(controller.prevMouseOrigin.x - mouse.x),
      .y = static_cast<float>(controller.prevMouseOrigin.y - mouse.y),
    }
  );
  controller.prevMouseOrigin = mouse;

  // only apply mouse delta if mouse left is down
  if (!puleInputMouse(controller.platform, PuleInputMouse_left)) {
    mouseDelta = PuleF32v2{0.0f, 0.0f};
  }

  // calculate movement
  PuleF32v2 const moveDirection = PuleF32v2 {
    (
      (float)(puleInputKey(controller.platform, PuleInputKey_d))
    - (float)(puleInputKey(controller.platform, PuleInputKey_a))
    ),
    (
      (float)(puleInputKey(controller.platform, PuleInputKey_s))
    - (float)(puleInputKey(controller.platform, PuleInputKey_w))
    ),
  };

  // translate, use the mouse and move directrion to modify phi/theta
  controller.dirTheta -= mouseDelta.x * 0.005f + moveDirection.x * 0.005f;
  controller.dirPhi += mouseDelta.y * 0.005f + moveDirection.y * 0.005f;
  if (controller.dirPhi > 1.3f)
    controller.dirPhi = 1.3f;
  if (controller.dirPhi < -1.3f)
    controller.dirPhi = -1.3f;

  // calculate view direction

  // first get the normalized direction from theta/phi
  PuleF32v3 modifiedOrigin = (
    puleF32v3Normalize({
      (float)sin(controller.dirTheta),
      (float)controller.dirPhi,
      (float)cos(controller.dirTheta),
    })
  );

  // control radius by radius original
  float const radiusDelta = puleInputScroll(controller.platform) * 0.1f;

  controller.radius += radiusDelta * controller.radiusOriginal;
  controller.radius = (
    std::clamp(controller.radius, 0.1f, controller.radiusOriginal*10.0f)
  );
  if (puleInputKey(controller.platform, PuleInputKey_space)) {
    controller.radius = controller.radiusOriginal;
  }

  // scale by radius, offfset by target
  puleCameraLookAt(
    controller.camera,
    puleF32v3Add(
      controller.targetOrigin,
      puleF32v3MulScalar(modifiedOrigin, controller.radius)
    ),
    controller.targetOrigin,
    controller.up
  );

  puleCameraPerspectiveSet(
    controller.camera,
    PuleCameraPerspective {
      .nearCutoff = 0.01f,
      .farCutoff = 10'000.0f,
      .aspectRatio = 800.0f/600.0f,
      .fieldOfViewRadians = 70.0f,
    }
  );
}

} // namespace

extern "C" {

PuleCameraController puleCameraControllerFirstPerson(
  PulePlatform const platform,
  PuleCamera const camera
) {
  CameraControllerGeneric controllerContainer;
  controllerContainer.cameraData.resize(sizeof(CameraControllerFps));
  controllerContainer.update = &cameraControllerFpsUpdate;
  CameraControllerFps & controller = (
    *reinterpret_cast<CameraControllerFps *>(
      controllerContainer.cameraData.data()
    )
  );
  controller.origin = puleF32v3(1);
  controller.forward = PuleF32v3{1.0f, 0.0f, 1.0f,};
  controller.up = PuleF32v3{0.0f, 1.0f, 0.0f,};
  controller.camera = camera;
  controller.platform = platform;
  pulePlatformCursorHide(platform);
  ::cameraControllers.emplace(::cameraControllerIt, controllerContainer);
  return PuleCameraController { .id=::cameraControllerIt++, };
}

PuleCameraController puleCameraControllerOrbit(
  PulePlatform const platform,
  PuleCamera const camera,
  PuleF32v3 const origin,
  float const radius
) {
  CameraControllerGeneric controllerContainer;
  controllerContainer.cameraData.resize(sizeof(CameraControllerOrbit));
  controllerContainer.update = &cameraControllerOrbitUpdate;
  CameraControllerOrbit & controller = (
    *reinterpret_cast<CameraControllerOrbit *>(
      controllerContainer.cameraData.data()
    )
  );
  controller.targetOrigin = origin;
  controller.radius = controller.radiusOriginal = radius;
  controller.up = PuleF32v3{0.0f, 1.0f, 0.0f,};
  controller.dirPhi = 0.0f;
  controller.dirTheta = 0.0f;
  controller.prevMouseOrigin = pulePlatformMouseOrigin(platform);

  controller.camera = camera;
  controller.platform = platform;
  ::cameraControllers.emplace(::cameraControllerIt, controllerContainer);
  return PuleCameraController { .id=::cameraControllerIt++, };
}

void puleCameraControllerDestroy(
  PuleCameraController const pController
) {
  /* auto & controller = ::cameraControllers.at(pController.id); */
  ::cameraControllers.erase(pController.id);
}

void puleCameraControllerPollEvents() {
  for (auto & controller : ::cameraControllers) {
    if (controller.second.update) {
      controller.second.update(controller.second.cameraData.data());
    }
  }
}

} // extern C
