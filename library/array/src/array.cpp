#include <pulchritude-array/array.h>

namespace {
size_t bytesPerElement(PuleArray const & array) {
  return (
    array.elementAlignmentByteLength > array.elementByteLength
      ? array.elementAlignmentByteLength 
      : array.elementByteLength
  );
}
}

extern "C" {

PuleArray puleArray(PuleArrayCreateInfo const info) {
  PuleArray array;
  array.content = nullptr;
  array.elementCount = 0;
  array.elementByteLength = info.elementByteLength;
  array.elementAlignmentByteLength = info.elementAlignmentByteLength;
  array.allocator = info.allocator;
  return array;
}

void * puleArrayAppend(PuleArray * const array) {
  array->elementCount += 1;
  size_t const stride = bytesPerElement(*array);
  array->content = (
    puleReallocate(
      array->allocator,
      PuleReallocateInfo {
        .allocation = array->content,
        .numBytes = array->elementCount * stride,
        .alignment = array->elementAlignmentByteLength,
      }
    )
  );
  return (
    reinterpret_cast<void *>(
      reinterpret_cast<uint64_t>(array->content)
      + stride*(array->elementCount-1)
    )
  );
}

void * puleArrayElementAt(PuleArray const array, size_t const idx) {
  size_t const stride = bytesPerElement(array);
  return (
    reinterpret_cast<void *>(
      reinterpret_cast<uint8_t *>(array.content)[stride*idx]
    )
  );
}

void puleArrayDestroy(PuleArray const array) {
  puleDeallocate(array.allocator, array.content);
}

} // C
