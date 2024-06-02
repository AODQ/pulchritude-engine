/* auto generated file array */
#pragma once
#include "core.hpp"

#include "array.h"
#include "error.hpp"
#include "allocator.hpp"

namespace pule {
struct Array {
  PuleArray _handle;
  inline operator PuleArray() const {
    return _handle;
  }
  inline void destroy() {
    return puleArrayDestroy(this->_handle);
  }
  inline void * append() {
    return puleArrayAppend( &this->_handle);
  }
  inline void * elementAt(size_t idx) {
    return puleArrayElementAt(this->_handle, idx);
  }
};
}
  inline void destroy(pule::Array self) {
    return puleArrayDestroy(self._handle);
  }
  inline void * append(pule::Array & self) {
    return puleArrayAppend( &self._handle);
  }
  inline void * elementAt(pule::Array self, size_t idx) {
    return puleArrayElementAt(self._handle, idx);
  }
namespace pule {
using ArrayCreateInfo = PuleArrayCreateInfo;
}
namespace pule {
using ArrayView = PuleArrayView;
}
namespace pule {
using ArrayViewMutable = PuleArrayViewMutable;
}
namespace pule {
using BufferView = PuleBufferView;
}
namespace pule {
using BufferViewMutable = PuleBufferViewMutable;
}
namespace pule {
struct Buffer {
  PuleBuffer _handle;
  inline operator PuleBuffer() const {
    return _handle;
  }
  inline void resize(size_t length) {
    return puleBufferResize( &this->_handle, length);
  }
  inline void append(uint8_t const * data, size_t length) {
    return puleBufferAppend( &this->_handle, data, length);
  }
  inline void destroy() {
    return puleBufferDestroy(this->_handle);
  }
  static inline Buffer create(PuleAllocator arg0) {
    return { ._handle = puleBufferCreate(arg0),};
  }
  static inline Buffer copyWithData(PuleAllocator arg0, uint8_t const * data, size_t length) {
    return { ._handle = puleBufferCopyWithData(arg0, data, length),};
  }
};
}
  inline void resize(pule::Buffer & self, size_t length) {
    return puleBufferResize( &self._handle, length);
  }
  inline void append(pule::Buffer & self, uint8_t const * data, size_t length) {
    return puleBufferAppend( &self._handle, data, length);
  }
  inline void destroy(pule::Buffer self) {
    return puleBufferDestroy(self._handle);
  }
namespace pule {
inline char const * toStr(PuleErrorArray const e) {
  switch (e) {
    case PuleErrorArray_none: return "none";
    case PuleErrorArray_errorAllocation: return "errorAllocation";
    default: return "N/A";
  }
}
}
