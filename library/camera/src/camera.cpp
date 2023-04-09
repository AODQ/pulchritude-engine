#include <pulchritude-camera/camera.h>

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
  defaultCamera.up = PuleF32v3(0.0f, 1.0f, 0.0f);
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
  PuleGfxGpuBuffer uniformBuffer;
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
      puleGfxGpuBufferUnmap(set.uniformBuffer);
      set.uniformBufferMemory = nullptr;
    }
    if (set.uniformBuffer.id != 0) {
      puleGfxGpuBufferDestroy(set.uniformBuffer);
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
    set.uniformBuffer = (
      puleGfxGpuBufferCreate(
        nullptr,
        sizeof(CameraGpuData)*4,
        PuleGfxGpuBufferUsage_bufferUniform,
        PuleGfxGpuBufferVisibilityFlag_hostWritable
      )
    );
  }

  if (!set.uniformBufferMemory) {
    set.uniformBufferMemory = (
      puleGfxGpuBufferMap(
        PuleGfxGpuBufferMapRange {
          .buffer = set.uniformBuffer,
          .access = PuleGfxGpuBufferMapAccess_hostWritable,
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
  puleGfxGpuBufferUnmap(set.uniformBuffer);
  puleGfxGpuBufferDestroy(set.uniformBuffer);
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

PuleGfxGpuBuffer puleCameraSetGfxUniformBuffer(PuleCameraSet const pSet) {
  auto & set = ::internalCameraSets.at(pSet.id);
  return set.uniformBuffer;
}

PuleGfxFence puleCameraSetRefresh(PuleCameraSet const pSet) {
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
  puleGfxGpuBufferMappedFlush({
    .buffer = set.uniformBuffer,
    .byteOffset = 0,
    .byteLength = sizeof(CameraGpuData)*set.cameras.size(),
  });
  return puleGfxFenceCreate(PuleGfxFenceConditionFlag_all);
}

} // extern C

// -- camera controller---------------------------------------------------------
// TODO make this its own API? expose to user how to append to list?
struct CameraControllerRts {
  PuleF32v3 origin;
  PuleF32v3 forward;
  PuleF32v3 up;

  PulePlatform platform;
};

struct CameraControllerFps {
  PuleF32v3 origin;
  PuleF32v3 forward;
  PuleF32v3 up;

  PulePlatform platform;

  PuleI32v2 prevMouseOrigin;
  float dirTheta, dirPhi;
};

struct CameraControllerGeneric {
  PuleCamera camera;
  std::vector<uint8_t> cameraData;
  void (*update)(PuleCamera camera, void * const userdata);
};

namespace {
std::unordered_map<uint64_t, CameraControllerGeneric> cameraControllers;
uint64_t cameraControllerIt = 1;

void cameraControllerRtsUpdate(PuleCamera camera, void * const userdata) {
  CameraControllerRts & controller = (
    *reinterpret_cast<CameraControllerRts *>(userdata)
  );

  puleCameraPerspectiveSet(
    camera,
    PuleCameraPerspective {
      .nearCutoff = 0.01f,
      .farCutoff = 100.0f,
      .aspectRatio = 800.0f/600.0f,
      .fieldOfViewRadians = 70.0f,
    }
  );

  // calculate movement
  float const mag = 0.01;
  PuleF32v3 const velocity = PuleF32v3 {
    mag * (
      (float)(puleInputKey(controller.platform, PuleInputKey_d))
    - (float)(puleInputKey(controller.platform, PuleInputKey_a))
    ),
    mag * (
      (float)(puleInputKey(controller.platform, PuleInputKey_r))
    - (float)(puleInputKey(controller.platform, PuleInputKey_f))
    ),
    mag * (
      (float)(puleInputKey(controller.platform, PuleInputKey_w))
    - (float)(puleInputKey(controller.platform, PuleInputKey_s))
    ),
  };

  controller.origin = puleF32v3Add(controller.origin, velocity);

  puleCameraLookAt(
    camera,
    controller.origin,
    puleF32v3Add(controller.forward, controller.origin),
    controller.up
  );
}

void cameraControllerFpsUpdate(PuleCamera camera, void * const userdata) {
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
    camera,
    PuleCameraPerspective {
      .nearCutoff = 0.01f,
      .farCutoff = 100.0f,
      .aspectRatio = 800.0f/600.0f,
      .fieldOfViewRadians = 70.0f,
    }
  );

  puleCameraLookAt(
    camera,
    controller.origin,
    puleF32v3Add(controller.forward, controller.origin),
    controller.up
  );
}
} // namespace

extern "C" {

PuleCameraController puleCameraControllerCreate(
  PuleCamera camera,
  void (*update)(PuleCamera camera, void * const userdata),
  PuleArrayView controllerData
) {
  auto data = static_cast<uint8_t const * const>(controllerData.data);
  CameraControllerGeneric controllerContainer {
    .camera = camera,
    .cameraData = std::vector<uint8_t> (
      controllerData.data, 
      controllerData.data 
        + controllerData.elementStride * controllerData.elementCount
    ),
    .update = update
  };
  ::cameraControllers.emplace(::cameraControllerIt, controllerContainer);
  return PuleCameraController { .id=::cameraControllerIt++, };
}

PuleCameraController puleCameraControllerFirstPerson(
  PulePlatform const platform,
  PuleCamera const camera
) {
  CameraControllerFps controller {
    .origin = puleF32v3(1),
    .forward = PuleF32v3{1.0f, 0.0f, 1.0f,},
    .up = PuleF32v3{0.0f, 1.0f, 0.0f,},
    .platform = platform
  };
  pulePlatformCursorHide(platform);
  return puleCameraControllerCreate(
    camera,
    &cameraControllerFpsUpdate,
    PuleArrayView {
      .data = reinterpret_cast<const uint8_t *>(&controller),
      .elementStride = sizeof(CameraControllerFps),
      .elementCount = 1
    }
  );
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
      controller.second.update(
        controller.second.camera,
        controller.second.cameraData.data()
      );
    }
  }
}

} // extern C
