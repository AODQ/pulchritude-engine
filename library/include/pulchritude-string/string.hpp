#pragma once
#include "string.h"

#ifdef __cplusplus

namespace pule {

struct string {
  PuleString str = {};
  string();
  string(char const * const cstr, PuleAllocator const allocator = {});
  string(PuleStringView const view, PuleAllocator const allocator = {});
  ~string();
  string(string const & other) = delete;
  string(string && other);
  string & operator=(string const & other) = delete;
  string & operator=(string && other);

  static pule::string format(char const * const fmt, ...);

  inline char const * cstr() const { return str.contents; }
};

} // namespace pule

#endif // __cplusplus
