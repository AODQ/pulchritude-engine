#include <pulchritude-allocator/allocator.h>

#include <cstdlib>
#include <cstring>

namespace {

void * releaseAllocate(void * const, PuleAllocateInfo const info) {
  void * allocatedMemory = nullptr;
  if (info.alignment == 0) {
    if (info.zeroOut) {
      allocatedMemory = calloc(info.numBytes, 1);
    } else {
      allocatedMemory = malloc(info.numBytes);
    }
  } else {
    allocatedMemory = aligned_alloc(info.numBytes, info.alignment);
    if (info.zeroOut) {
      memset(allocatedMemory, 0, info.numBytes);
    }
  }
  return allocatedMemory;
}

void * releaseReallocate(void * const, PuleReallocateInfo const info) {
  void * reallocatedMemory = nullptr;
  reallocatedMemory = realloc(info.allocation, info.numBytes);

  // check reallocated memory is aligned if requested, as otherwise need to
  // free/alloc
  if (
       info.alignment > 0
    && reinterpret_cast<size_t>(reallocatedMemory) % info.alignment != 0
  ) {
    free(reallocatedMemory);
    reallocatedMemory = aligned_alloc(info.numBytes, info.alignment);
  }

  return reallocatedMemory;
}

void releaseDeallocate(void * const, void * const allocationNullable) {
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
  return allocator.allocate(allocator.implementation, info);
}

void * puleReallocate(
  PuleAllocator const allocator,
  PuleReallocateInfo const info
) {
  return allocator.reallocate(allocator.implementation, info);
}

void puleDeallocate(
  PuleAllocator const allocator,
  void * const allocationNullable
) {
  return allocator.deallocate(allocator.implementation, allocationNullable);
}

} // extern C
