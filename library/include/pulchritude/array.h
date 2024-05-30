/* auto generated file array */
#pragma once
#include "core.h"

#include "error.h"
#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorArray_none = 0,
  PuleErrorArray_errorAllocation = 1,
} PuleErrorArray;
const uint32_t PuleErrorArraySize = 2;

// entities

// structs
struct PuleArray;
struct PuleArrayCreateInfo;
struct PuleArrayView;
struct PuleArrayViewMutable;
struct PuleBufferView;
struct PuleBufferViewMutable;
struct PuleBuffer;

typedef struct PuleArray {
  void * content;
  size_t elementByteLength;
  size_t elementAlignmentByteLength;
  size_t elementCount;
  PuleAllocator allocator;
} PuleArray;
typedef struct PuleArrayCreateInfo {
  /* must be non-zero */
  size_t elementByteLength;
  /* can be zero */
  size_t elementAlignmentByteLength;
  PuleAllocator allocator;
} PuleArrayCreateInfo;
/* view contents of an array without needing knowledge of its allocation
    detail. */
typedef struct PuleArrayView {
  uint8_t const * data;
  size_t elementStride;
  size_t elementCount;
} PuleArrayView;
/* view contents of an array without needing knowledge of its allocation
    detail. */
typedef struct PuleArrayViewMutable {
  uint8_t * data;
  size_t elementStride;
  size_t elementCount;
} PuleArrayViewMutable;
/* view contents of contiguous memory without needing knowledge of its
    allocation detail. */
typedef struct PuleBufferView {
  uint8_t const * data;
  size_t byteLength;
} PuleBufferView;
typedef struct PuleBufferViewMutable {
  uint8_t * data;
  size_t byteLength;
} PuleBufferViewMutable;
typedef struct PuleBuffer {
  uint8_t * data;
  size_t byteLength;
  PuleAllocator allocator;
} PuleBuffer;

// functions
PULE_exportFn PuleArray puleArray(PuleArrayCreateInfo);
PULE_exportFn void puleArrayDestroy(PuleArray);
PULE_exportFn void * puleArrayAppend(PuleArray *);
PULE_exportFn void * puleArrayElementAt(PuleArray, size_t idx);
PULE_exportFn PuleBuffer puleBufferCreate(PuleAllocator);
PULE_exportFn PuleBuffer puleBufferCopyWithData(PuleAllocator, uint8_t const * data, size_t length);
PULE_exportFn void puleBufferResize(PuleBuffer *, size_t length);
PULE_exportFn void puleBufferAppend(PuleBuffer *, uint8_t const * data, size_t length);
PULE_exportFn void puleBufferDestroy(PuleBuffer);

#ifdef __cplusplus
} // extern C
#endif
