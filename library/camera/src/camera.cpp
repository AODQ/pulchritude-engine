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
        sizeof(CameraGpuData)*4 + sizeof(uint32_t),
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

  uint32_t * const uniformBufferSizeMemory = (
    reinterpret_cast<uint32_t *>(set.uniformBufferMemory)
  );
  *uniformBufferSizeMemory = set.uniformBufferCapacity;
  for (size_t it = 0; it < set.cameras.size(); ++ it) {
    PuleCamera const camera = set.cameras[it];
    CameraGpuData * gpuMemory = (
      reinterpret_cast<CameraGpuData *>(uniformBufferSizeMemory+1) + it
    );
    gpuMemory->projection = puleCameraProj(camera);
    gpuMemory->view = puleCameraView(camera);
  }
  puleGfxGpuBufferMappedFlush({
    .buffer = set.uniformBuffer,
    .byteOffset = 0,
    .byteLength = sizeof(uint32_t) + sizeof(CameraGpuData)*set.cameras.size(),
  });
  return puleGfxFenceCreate(PuleGfxFenceConditionFlag_all);
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
  float dirCos, dirSin;
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

  uint32_t const moveH = (
    (uint32_t)(puleInputKey(controller.platform, PuleInputKey_d))
    -
    (uint32_t)(puleInputKey(controller.platform, PuleInputKey_a))
  );

  uint32_t const moveD = (
    (uint32_t)(puleInputKey(controller.platform, PuleInputKey_w))
    -
    (uint32_t)(puleInputKey(controller.platform, PuleInputKey_s))
  );
  uint32_t const moveV = (
    (uint32_t)(puleInputKey(controller.platform, PuleInputKey_r))
    -
    (uint32_t)(puleInputKey(controller.platform, PuleInputKey_f))
  );

  // TODO move relative to up+forward
  controller.origin.x += moveH;
  controller.origin.y += moveV;
  controller.origin.z += moveD;

  PuleI32v2 const mouse = pulePlatformMouseOrigin(controller.platform);
  auto const mouseDelta = (
    PuleF32v2 {
      .x = static_cast<float>(controller.prevMouseOrigin.x - mouse.x),
      .y = static_cast<float>(controller.prevMouseOrigin.y - mouse.y),
    }
  );
  controller.prevMouseOrigin = mouse;

  controller.dirCos += mouseDelta.x * 0.05f;
  controller.dirSin += mouseDelta.y * 0.05f;
  controller.forward = (
    puleF32v3Normalize( PuleF32v3 {
      (float)cos(controller.dirCos),
      (float)sin(controller.dirSin),
      (float)sin(controller.dirCos),
    })
  );
  controller.up = PuleF32v3 { 0.0f, 1.0f, 0.0f, };

  puleCameraLookAt(
    controller.camera, controller.origin, controller.forward, controller.up
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
  ::cameraControllers.emplace(::cameraControllerIt, controllerContainer);
  return PuleCameraController { .id=::cameraControllerIt++, };
}
PULE_exportFn void puleCameraControllerDestroy(
  PuleCameraController const pController
) {
  /* auto & controller = ::cameraControllers.at(pController.id); */
  ::cameraControllers.erase(pController.id);
}

void puleCamereaControllerPollEvents() {
  for (auto & controller : ::cameraControllers) {
    if (controller.second.update) {
      controller.second.update(controller.second.cameraData.data());
    }
  }
}

} // extern C
