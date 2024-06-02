/* auto generated file core */
#pragma once
#include "core.hpp"

#include "core.h"
#ifndef puleScopeExit
#define _puleScopeExitConcatImpl(x, y) x##y
#define _puleScopeExitConcat(x, y) _puleScopeExitConcatImpl(x, y)
#define puleScopeExit \
  auto const _puleScopeExitConcat(_puleScopeExit, __COUNTER__) \
    = pule::ScopeGuard() << [&]()
#endif
namespace pule {
template <typename Fn>
struct ScopeGuardImpl {
  Fn fn;
  explicit ScopeGuardImpl(Fn && fn) : fn(fn) {}
  ScopeGuardImpl(ScopeGuardImpl && other) : fn(std::move(other.fn)) {}
  ScopeGuardImpl(const ScopeGuardImpl &) = delete;
  ScopeGuardImpl & operator=(const ScopeGuardImpl &) = delete;
  ~ScopeGuardImpl() { this->fn(); }
};

struct ScopeGuard {
  template <typename F>
  ScopeGuardImpl<F> operator<<(F && fn) {
    return ScopeGuardImpl<F>(std::move(fn));
  }
};
}

#include <unordered_map>
namespace pule {
// TODO replace unordered_map with own implementation that uses allocator
// though probably safe to use unordered_map if passed in default allocator
template <typename T, typename Handle=uint64_t> struct ResourceContainer {
  std::unordered_map<uint64_t, T> data = {};
  uint64_t idCount = 1;
  using TUnderlyingValue =
    std::conditional_t<std::is_pointer_v<T>, std::remove_pointer_t<T>, T>
  ;

  // emplaces a new resource into the container
  //Handle create(T && resource) {
  //  uint64_t const id = this->idCount ++;
  //  this->data.emplace(this->idCount, resource);
  //  return Handle { id };
  //}

  Handle create(T const & resource) {
    uint64_t const id = this->idCount ++;
    this->data.emplace(id, resource);
    return Handle { id };
  }

  void destroy(Handle const handle) {
    if constexpr (std::is_same<Handle, uint64_t>::value) {
      if (handle == 0) { return; }
      this->data.erase(handle);
    } else {
      if (handle.id == 0) { return; }
      this->data.erase(handle.id);
    }
  }

  auto begin() { return this->data.begin(); }
  auto end() { return this->data.end(); }

  TUnderlyingValue * at(Handle const handle) {
    uint64_t uHandle;
    if constexpr (std::is_same<Handle, uint64_t>::value) {
      uHandle = handle;
    } else {
      uHandle = handle.id;
    }
    auto ptr = this->data.find(uHandle);
    if constexpr(std::is_pointer_v<T>) {
      return ((ptr == this->data.end()) ? nullptr : ptr->second);
    } else {
      return ((ptr == this->data.end()) ? nullptr : &ptr->second);
    }
  }

  TUnderlyingValue const * at(Handle const handle) const {
    uint64_t uHandle;
    if constexpr (std::is_same<Handle, uint64_t>::value) {
      uHandle = handle;
    } else {
      uHandle = handle.id;
    }
    auto ptr = this->data.find(uHandle);
    if (ptr == this->data.end()) {
      return nullptr;
    }
    if constexpr(std::is_pointer_v<T>) {
      return ptr->second;
    } else {
      return &ptr->second;
    }
  }
};
}


namespace pule {
inline char const * toStr(PuleDt const e) {
  switch (e) {
    case PuleDt_i8: return "i8";
    case PuleDt_i16: return "i16";
    case PuleDt_i32: return "i32";
    case PuleDt_i64: return "i64";
    case PuleDt_u8: return "u8";
    case PuleDt_u16: return "u16";
    case PuleDt_u32: return "u32";
    case PuleDt_u64: return "u64";
    case PuleDt_f32: return "f32";
    case PuleDt_f64: return "f64";
    case PuleDt_bool: return "bool";
    case PuleDt_ptr: return "ptr";
    case PuleDt_f32v2: return "f32v2";
    case PuleDt_f32v3: return "f32v3";
    case PuleDt_f32v4: return "f32v4";
    case PuleDt_f32m44: return "f32m44";
    case PuleDt_f64v2: return "f64v2";
    case PuleDt_f64v3: return "f64v3";
    case PuleDt_f64v4: return "f64v4";
    case PuleDt_i32v2: return "i32v2";
    case PuleDt_i32v3: return "i32v3";
    case PuleDt_i32v4: return "i32v4";
    case PuleDt_u32v2: return "u32v2";
    case PuleDt_u32v3: return "u32v3";
    case PuleDt_u32v4: return "u32v4";
    case PuleDt_i64v2: return "i64v2";
    case PuleDt_i64v3: return "i64v3";
    case PuleDt_i64v4: return "i64v4";
    case PuleDt_u64v2: return "u64v2";
    case PuleDt_u64v3: return "u64v3";
    case PuleDt_u64v4: return "u64v4";
    default: return "N/A";
  }
}
inline char const * toStr(PuleAccess const e) {
  switch (e) {
    case PuleAccess_none: return "none";
    case PuleAccess_read: return "read";
    case PuleAccess_write: return "write";
    case PuleAccess_readwrite: return "readwrite";
    default: return "N/A";
  }
}
}
