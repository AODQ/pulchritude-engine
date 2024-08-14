#include <pulchritude/array.h>

#include <pulchritude/allocator.h>

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
  if (array.content == nullptr) { return; }
  puleDeallocate(array.allocator, array.content);
}

PuleBuffer puleBufferCreate(PuleAllocator const allocator) {
  return {
    .data = nullptr,
    .byteLength = 0,
    .allocator = allocator,
  };
}

PuleBuffer puleBufferCopyWithData(
  PuleAllocator const allocator,
  uint8_t const * const data, size_t const length
) {
  PuleBuffer buffer;
  buffer.data = (uint8_t *)(
    puleAllocate(
      allocator,
      PuleAllocateInfo {
        .numBytes = length,
        .alignment = 0,
      }
    )
  );
  buffer.byteLength = length;
  buffer.allocator = allocator;
  memcpy(buffer.data, data, length);
  return buffer;
}

void puleBufferResize(PuleBuffer * const buffer, size_t const length) {
  if (buffer->byteLength == length) { return; }
  if (buffer->byteLength == 0) {
    buffer->data = (uint8_t *)(
      puleAllocate(
        buffer->allocator,
        PuleAllocateInfo {
          .numBytes = length,
          .alignment = 0,
        }
      )
    );
    buffer->byteLength = length;
    return;
  }

  buffer->byteLength = length;
  buffer->data = (uint8_t *)(
    puleReallocate(
      buffer->allocator,
      PuleReallocateInfo {
        .allocation = buffer->data,
        .numBytes = buffer->byteLength,
        .alignment = 0,
      }
    )
  );
}

void puleBufferAppend(
  PuleBuffer * const buffer,
  uint8_t const * const data,
  size_t const length
) {
  // TODO maybe capacity instead of realloc every time?
  size_t const oldLength = buffer->byteLength;
  puleBufferResize(buffer, buffer->byteLength + length);
  memcpy(buffer->data+oldLength, data, length);
}

void puleBufferDestroy(PuleBuffer const buffer) {
  puleDeallocate(buffer.allocator, buffer.data);
}

PuleBufferView puleBufferView(PuleBuffer buffer) {
  return { .data = buffer.data, .byteLength = buffer.byteLength };
}

} // C
