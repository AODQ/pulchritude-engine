#include <pulchritude-allocator/allocator.h>

#include <pulchritude-log/log.h>

#include <cstdlib>
#include <cstring>

namespace {

void * releaseAllocate(void * const, PuleAllocateInfo const info) {
  void * allocatedMemory = nullptr;
  if (info.alignment == 0) {
    if (info.zeroOut) {
      //allocatedMemory = calloc(info.numBytes, 1);
      allocatedMemory = malloc(info.numBytes);
      memset(allocatedMemory, 0, info.numBytes);
      puleLogDebug("[%p] calloc %zu bytes", allocatedMemory, info.numBytes);
    } else {
      allocatedMemory = malloc(info.numBytes);
      puleLogDebug("[%p] malloc %zu", allocatedMemory, info.numBytes);
    }
  } else {
    allocatedMemory = aligned_alloc(info.numBytes, info.alignment);
    puleLogDebug(
      "[%p] aligned alloc %zu alignment %zu",
      allocatedMemory, info.numBytes, info.alignment
    );
    if (info.zeroOut) {
      memset(allocatedMemory, 0, info.numBytes);
    }
  }
  return allocatedMemory;
}

void * releaseReallocate(void * const, PuleReallocateInfo const info) {
  void * reallocatedMemory = nullptr;
  puleLogDebug("[%p] realloc %zu", info.allocation, info.numBytes);
  reallocatedMemory = realloc(info.allocation, info.numBytes);
  puleLogDebug("[%p] new address [%p]", info.allocation, reallocatedMemory);

  // check reallocated memory is aligned if requested, as otherwise need to
  // free/alloc
  if (
       info.alignment > 0
    && reinterpret_cast<size_t>(reallocatedMemory) % info.alignment != 0
  ) {
    puleLogDebug("freeing memory at ", info.numBytes, info.alignment);
    free(reallocatedMemory);
    reallocatedMemory = aligned_alloc(info.numBytes, info.alignment);
    puleLogDebug(
      "[%p] aligned alloc",
      reallocatedMemory, info.numBytes, info.alignment
    );
  }

  return reallocatedMemory;
}

void releaseDeallocate(void * const, void * const allocationNullable) {
  puleLogDebug("[%p] free", allocationNullable);
  free(allocationNullable);
}

} // namespace

extern "C" {

PuleAllocator puleAllocateDefault() {
  return PuleAllocator {
    .implementation = nullptr,
    .allocate = &::releaseAllocate,
    .reallocate = &::releaseReallocate,
    .deallocate = &::releaseDeallocate,
  };
}

void * puleAllocate(
  PuleAllocator const allocator,
  PuleAllocateInfo const info
) {
  if (allocator.allocate == nullptr) {
    puleLogError("null allocator!");
    return nullptr;
  }

  return allocator.allocate(allocator.implementation, info);
}

void * puleReallocate(
  PuleAllocator const allocator,
  PuleReallocateInfo const info
) {
  if (allocator.reallocate == nullptr) {
    puleLogError("null reallocator!");
    return nullptr;
  }
  return allocator.reallocate(allocator.implementation, info);
}

void puleDeallocate(
  PuleAllocator const allocator,
  void * const allocationNullable
) {
  if (allocator.deallocate == nullptr) {
    return;
  }
  return allocator.deallocate(allocator.implementation, allocationNullable);
}

} // extern C
