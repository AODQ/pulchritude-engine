#include <pulchritude-gfx/barrier.h>

#include <pulchritude-log/log.h>

#include <volk.h>

#if 0
namespace {
  GLenum memoryBarrierFlagToGl(PuleGfxMemoryBarrierFlag const barrier) {
    switch (barrier) {
      default: return 0;
      case PuleGfxMemoryBarrierFlag_all:
        return GL_ALL_BARRIER_BITS;
      case PuleGfxMemoryBarrierFlag_attribute:
        return GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
      case PuleGfxMemoryBarrierFlag_element:
        return GL_ELEMENT_ARRAY_BARRIER_BIT;
      case PuleGfxMemoryBarrierFlag_bufferUpdate:
        return GL_BUFFER_UPDATE_BARRIER_BIT;
    }
  }

  GLenum memoryBarrierToGl(PuleGfxMemoryBarrierFlag const barrier) {
    GLenum flags = 0;
    if (barrier == PuleGfxMemoryBarrierFlag_all) {
      return GL_ALL_BARRIER_BITS;
    }
    for (uint64_t it = 1; it < PuleGfxMemoryBarrierFlagEnd; it <<= 1) {
      if (barrier & static_cast<PuleGfxMemoryBarrierFlag>(it)) {
        flags |= (
          memoryBarrierFlagToGl(
            static_cast<PuleGfxMemoryBarrierFlag>(it)
          )
        );
      }
    }
    return flags;
  }
}
#endif

extern "C" {

PuleGfxFence puleGfxFenceCreate(PuleGfxFenceConditionFlag const condition) {
  #if 0
  (void)condition;
  return {
    .id = (
      reinterpret_cast<uint64_t>(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0))
    )
  };
  #endif
  return { 0 };
}
void puleGfxFenceDestroy(PuleGfxFence const fence) {
  #if 0
  if (fence.id == 0) { return; }
  auto const handle = reinterpret_cast<GLsync>(fence.id);
  glDeleteSync(handle);
  #endif
}

PULE_exportFn bool puleGfxFenceCheckSignal(
  PuleGfxFence const fence,
  PuleNanosecond const timeout
) {
  #if 0
  auto const handle = reinterpret_cast<GLsync>(fence.id);
  switch (glClientWaitSync(handle, GL_SYNC_FLUSH_COMMANDS_BIT, timeout.value)) {
    default: return false;
    case GL_ALREADY_SIGNALED: return true;
    case GL_CONDITION_SATISFIED: return true;
    case GL_TIMEOUT_EXPIRED: return false;
    case GL_WAIT_FAILED: return false;
  }
  #endif
  return false;
}

void puleGfxMemoryBarrier(
  PuleGfxMemoryBarrierFlag const barrier
) {
  #if 0
  glMemoryBarrier(memoryBarrierToGl(barrier));
  #endif
}

}
