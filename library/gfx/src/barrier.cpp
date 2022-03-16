#include <pulchritude-gfx/barrier.h>

#include <pulchritude-log/log.h>

#include <glad/glad.h>

#include <cstring>

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

extern "C" {

PuleGfxFence puleGfxFenceCreate(PuleGfxFenceConditionFlag const condition) {
  (void)condition;
  return {
    .id = (
      reinterpret_cast<uint64_t>(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0))
    )
  };
}
void puleGfxFenceDestroy(PuleGfxFence const fence) {
  auto const handle = reinterpret_cast<GLsync>(fence.id);
  glDeleteSync(handle);
}

PULE_exportFn bool puleGfxFenceCheckSignal(
  PuleGfxFence const fence,
  PuleNanosecond const timeout
) {
  auto const handle = reinterpret_cast<GLsync>(fence.id);
  switch (glClientWaitSync(handle, GL_SYNC_FLUSH_COMMANDS_BIT, timeout.value)) {
    default: return false;
    case GL_ALREADY_SIGNALED: return true;
    case GL_CONDITION_SATISFIED: return true;
    case GL_TIMEOUT_EXPIRED: return false;
    case GL_WAIT_FAILED: return false;
  }
}

void puleGfxMemoryBarrier(
  PuleGfxMemoryBarrierFlag const barrier
) {
  glMemoryBarrier(memoryBarrierToGl(barrier));
}

}
