/* auto generated file stream */
#pragma once
#include "core.hpp"

#include "stream.h"
#include "array.hpp"
#include "string.hpp"

namespace pule {
struct StreamRead {
  PuleStreamRead _handle;
  inline operator PuleStreamRead() const {
    return _handle;
  }
  inline uint8_t byte() {
    return puleStreamReadByte(this->_handle);
  }
  inline bool isDone() {
    return puleStreamReadIsDone(this->_handle);
  }
  inline void destroy() {
    return puleStreamReadDestroy(this->_handle);
  }
  static inline StreamRead fromString(PuleStringView stringView) {
    return { ._handle = puleStreamReadFromString(stringView),};
  }
  static inline StreamRead fromBuffer(PuleBufferView bufferView) {
    return { ._handle = puleStreamReadFromBuffer(bufferView),};
  }
};
}
  inline uint8_t byte(pule::StreamRead self) {
    return puleStreamReadByte(self._handle);
  }
  inline bool isDone(pule::StreamRead self) {
    return puleStreamReadIsDone(self._handle);
  }
  inline void destroy(pule::StreamRead self) {
    return puleStreamReadDestroy(self._handle);
  }
namespace pule {
struct StreamWrite {
  PuleStreamWrite _handle;
  inline operator PuleStreamWrite() const {
    return _handle;
  }
  inline void bytes(uint8_t const * bytes, size_t length) {
    return puleStreamWriteBytes(this->_handle, bytes, length);
  }
  inline void flush() {
    return puleStreamWriteFlush(this->_handle);
  }
  inline void destroy() {
    return puleStreamWriteDestroy(this->_handle);
  }
};
}
  inline void bytes(pule::StreamWrite self, uint8_t const * bytes, size_t length) {
    return puleStreamWriteBytes(self._handle, bytes, length);
  }
  inline void flush(pule::StreamWrite self) {
    return puleStreamWriteFlush(self._handle);
  }
  inline void destroy(pule::StreamWrite self) {
    return puleStreamWriteDestroy(self._handle);
  }
namespace pule {
}
