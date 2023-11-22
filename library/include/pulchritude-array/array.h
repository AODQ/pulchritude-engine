#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-log/log.h>

// provides array and buffer tools. This might not be necessary for many
//   languages/environments beyond just passing arrays between plugins

// Arrays hold elements of a type, so they store the element stride (as both
//    the element byte-length, and the element's alignment byte-length)
// Buffers are just raw binary data, and have no concept of an element type

// Views into these can have mutable or immutable data, but they have no ability
//   to allocate or deallocate memory

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorArray_none,
  PuleErrorArray_errorAllocation,
} PuleErrorArray;

typedef struct {
  void * content;
  size_t elementByteLength;
  size_t elementAlignmentByteLength;
  size_t elementCount;
  PuleAllocator allocator;
} PuleArray;

typedef struct {
  size_t elementByteLength; // must be >0
  size_t elementAlignmentByteLength; // can be zero
  PuleAllocator allocator;
} PuleArrayCreateInfo;

PULE_exportFn PuleArray puleArray(PuleArrayCreateInfo const info);
// this is equivalent to puleDeallocate(array.allocator, array.content)
PULE_exportFn void puleArrayDestroy(PuleArray const array);

// appends single element to array and returns it
PULE_exportFn void * puleArrayAppend(PuleArray * const array);

// best to use this to access elements to take stride into account
PULE_exportFn void * puleArrayElementAt(
  PuleArray const array,
  size_t const idx
);

// Buffer views let you view the contents of an array, without having ownership
//   of data, but have no indication on its stride, element count, etc. Useful
//   for raw binary data
typedef struct {
  uint8_t const * data;
  size_t byteLength;
} PuleBufferView;

typedef struct {
  uint8_t * data;
  size_t byteLength;
} PuleBufferViewMutable;

typedef struct {
  uint8_t * data;
  size_t byteLength;
  PuleAllocator allocator;
} PuleBuffer;

PuleBuffer puleBufferCreate(PuleAllocator const allocator);
void puleBufferResize(PuleBuffer * const buffer, size_t const length);
void puleBufferAppend(
  PuleBuffer * const buffer,
  uint8_t const * const data, size_t const length
);
void puleBufferDestroy(PuleBuffer const buffer);

// Array views let you view the contents of an array (with specific element
// stride), without modifying the contents nor modifying it's allocation.
typedef struct {
  uint8_t const * data; // PULE_hintNonnull
  size_t elementStride; // must be nonzero, accounts for bytelength&alignment
  size_t elementCount;
} PuleArrayView;

typedef struct {
  uint8_t * data; // PULE_hintNonnull
  size_t elementStride; // must be nonzero, accounts for bytelength&alignment
  size_t elementCount;
} PuleArrayViewMutable;

#ifdef __cplusplus
} // extern c
#endif
