/* auto generated file array */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/allocator.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  void * content;
  size_t elementByteLength;
  size_t elementAlignmentByteLength;
  size_t elementCount;
  PuleAllocator allocator;
} PuleArray;
typedef struct {
  /* must be non-zero */
  size_t elementByteLength;
  /* can be zero */
  size_t elementAlignmentByteLength;
  PuleAllocator allocator;
} PuleArrayCreateInfo;
/* view contents of an array without needing knowledge of its allocation
    detail. */
typedef struct {
  uint8_t const * data;
  size_t elementStride;
  size_t elementCount;
} PuleArrayView;
/* view contents of an array without needing knowledge of its allocation
    detail. */
typedef struct {
  uint8_t * data;
  size_t elementStride;
  size_t elementCount;
} PuleArrayViewMutable;
/* view contents of contiguous memory without needing knowledge of its
    allocation detail. */
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

// enum
typedef enum {
  PuleErrorArray_none,
  PuleErrorArray_errorAllocation,
} PuleErrorArray;

// entities

// functions
PULE_exportFn PuleArray puleArray(PuleArrayCreateInfo _);
PULE_exportFn void puleArrayDestroy(PuleArray _);
PULE_exportFn void * puleArrayAppend(PuleArray * _);
PULE_exportFn void * puleArrayElementAt(PuleArray _, size_t idx);
PULE_exportFn PuleBuffer puleBufferCreate(PuleAllocator _);
PULE_exportFn PuleBuffer puleBufferCopyWithData(PuleAllocator _, uint8_t const * data, size_t length);
PULE_exportFn void puleBufferResize(PuleBuffer * _, size_t length);
PULE_exportFn void puleBufferAppend(PuleBuffer * _, uint8_t const * data, size_t length);
PULE_exportFn void puleBufferDestroy(PuleBuffer _);

#ifdef __cplusplus
} // extern C
#endif
