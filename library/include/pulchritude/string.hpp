/* auto generated file string */
#pragma once
#include "core.hpp"

#include "string.h"
#include "allocator.hpp"
#include "log.hpp"

namespace pule {
struct String {
  PuleString _handle;
  inline operator PuleString() const {
    return _handle;
  }
  inline void destroy() {
    return puleStringDestroy(this->_handle);
  }
  inline void append(PuleStringView append) {
    return puleStringAppend( &this->_handle, append);
  }
  inline PuleStringView view() {
    return puleStringView(this->_handle);
  }
  static inline String copy(PuleAllocator arg0, PuleStringView base) {
    return { ._handle = puleStringCopy(arg0, base),};
  }
  static inline String formatVargs(PuleAllocator arg0, char const * format, va_list arg2) {
    return { ._handle = puleStringFormatVargs(arg0, format, arg2),};
  }
};
}
  inline void destroy(pule::String self) {
    return puleStringDestroy(self._handle);
  }
  inline void append(pule::String & self, PuleStringView append) {
    return puleStringAppend( &self._handle, append);
  }
  inline PuleStringView view(pule::String self) {
    return puleStringView(self._handle);
  }
namespace pule {
struct StringView {
  PuleStringView _handle;
  inline operator PuleStringView() const {
    return _handle;
  }
  inline bool eq(PuleStringView v1) {
    return puleStringViewEq(this->_handle, v1);
  }
  inline bool contains(PuleStringView seg) {
    return puleStringViewContains(this->_handle, seg);
  }
  inline bool ends(PuleStringView seg) {
    return puleStringViewEnds(this->_handle, seg);
  }
  inline bool eqCStr(char const * v1) {
    return puleStringViewEqCStr(this->_handle, v1);
  }
  inline size_t hash() {
    return puleStringViewHash(this->_handle);
  }
};
}
  inline bool eq(pule::StringView self, PuleStringView v1) {
    return puleStringViewEq(self._handle, v1);
  }
  inline bool contains(pule::StringView self, PuleStringView seg) {
    return puleStringViewContains(self._handle, seg);
  }
  inline bool ends(pule::StringView self, PuleStringView seg) {
    return puleStringViewEnds(self._handle, seg);
  }
  inline bool eqCStr(pule::StringView self, char const * v1) {
    return puleStringViewEqCStr(self._handle, v1);
  }
  inline size_t hash(pule::StringView self) {
    return puleStringViewHash(self._handle);
  }
namespace pule {
}
PuleStringView operator ""_psv(char const * const cstr, size_t const len);

