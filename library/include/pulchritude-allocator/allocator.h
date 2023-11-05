#pragma once

#include <pulchritude-core/core.h>

// allows allocators to be passed between plugins
// This probably shouldn't be used in isolation, but in conjunction with
//   other plugins. For example, if you pass dynamically allocated memory
//   between plugins, then the plugin can know how to allocate/deallocate/resize
//   on demand

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
  size_t numBytes; // minimum number of bytes that need to be allocated

  // returned pointer will modulate to the alignment, 0 specifies to assume
  // worst-case alignment
  size_t alignment PULE_param(0);
  uint8_t zeroOut PULE_param(0); // if the allocated memory should be zeroed out
} PuleAllocateInfo;

typedef struct {
  // pointer to address to reallocate, must reallocate with the same allocator
  //   that originally allocated this memory
  void * allocation;
  size_t numBytes; // minimum number of bytes necessary to reallocate

  // returned pointer will modulate to the alignment, 0 specifies to assume
  // worst-case alignment
  size_t alignment;
} PuleReallocateInfo;

typedef struct {
  void * implementation; // allocation's internal details; can be null
  void * (* allocate)(
    void * const impldata, PuleAllocateInfo const info
  );
  void * (* reallocate)(
    void * const impldata, PuleReallocateInfo const info
  );
  void (* deallocate)(
    void * const impldata, void * const allocationNullable
  );
} PuleAllocator;

// the default allocator most likely will be provided by C's stdlib, though
//   in debug builds this could be a wrapper that check for memory leaks
PULE_exportFn PuleAllocator puleAllocateDefault();

PULE_exportFn void * puleAllocate(
  PuleAllocator const allocator, PuleAllocateInfo const allocateInfo
);
PULE_exportFn void * puleReallocate(
  PuleAllocator const allocator, PuleReallocateInfo const reallocateInfo
);
PULE_exportFn void puleDeallocate(
  PuleAllocator const allocator, void * const allocationNullable
);

/* typedef struct { */
/*   typeof(puleAllocateDefault) * allocateDefault; */
/*   typeof(puleAllocate) * allocate; */
/*   typeof(puleReallocate) * reallocate; */
/*   typeof(puleDeallocate) * deallocate; */
/* } PuleAllocate; */

#ifdef __cplusplus
} // extern c
#endif
