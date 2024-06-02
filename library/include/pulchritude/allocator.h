/* auto generated file allocator */
#pragma once
#include "core.h"


#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs
struct PuleAllocateInfo;
struct PuleReallocateInfo;
struct PuleAllocator;

/* a request for an allocation of memory */
typedef struct PuleAllocateInfo {
  /* minimum number of bytes to be allocated */
  size_t numBytes;
  /* alignment of pointer, 0 is allowed */
  size_t alignment;
  /* if memory should be zeroed out */
  bool zeroOut;
} PuleAllocateInfo;
/* a request for reallocation of existing memory */
typedef struct PuleReallocateInfo {
  /* address to reallocate memory for */
  void * allocation;
  /* minimum number of bytes to reallocate */
  size_t numBytes;
  /* alignment of pointer, 0 is allowed */
  size_t alignment;
} PuleReallocateInfo;
/* interface to allocate memory */
typedef struct PuleAllocator {
  /* allocation internal details, can be null */
  void * implementation;
  void *(* allocate)(void *, PuleAllocateInfo);
  void *(* reallocate)(void *, PuleReallocateInfo);
  void(* deallocate)(void *, void *);
} PuleAllocator;

// functions
/* 
  default allocator provided by engine, probably wrapper around C stdlib,
   but could be wrapper to check for memory leaks
 */
PULE_exportFn PuleAllocator puleAllocateDefault();
PULE_exportFn void * puleAllocate(PuleAllocator, PuleAllocateInfo);
PULE_exportFn void * puleMalloc(PuleAllocator allocator, size_t numBytes);
PULE_exportFn void * puleReallocate(PuleAllocator, PuleReallocateInfo);
PULE_exportFn void puleDeallocate(PuleAllocator, void * allocAddress);

#ifdef __cplusplus
} // extern C
#endif
