#pragma once
#include "core.h"

#ifdef __cplusplus

// C++ utilities, not necessary to use library but can be convenient

#include <unordered_map>

namespace pule {


// -- enum to string -----------------------------------------------------------

// enums just need to overload this function in other plugins
inline char const * toStr(PuleDt const dt) {
  #define dtCase(m) case PuleDt_ ## m: return #m
  switch (dt) {
    dtCase(i8); dtCase(i16); dtCase(i32); dtCase(i64);
    dtCase(u8); dtCase(u16); dtCase(u32); dtCase(u64);
    dtCase(f32); dtCase(f64);
    dtCase(bool); dtCase(ptr);

    dtCase(f32v2); dtCase(f32v3); dtCase(f32v4);
    dtCase(f32m44);

    dtCase(f64v2); dtCase(f64v3); dtCase(f64v4);

    dtCase(i32v2); dtCase(i32v3); dtCase(i32v4);
    dtCase(u32v2); dtCase(u32v3); dtCase(u32v4);

    dtCase(i64v2); dtCase(i64v3); dtCase(i64v4);
    dtCase(u64v2); dtCase(u64v3); dtCase(u64v4);
  }
  #undef dtCase
}

inline char const * toStr(PuleAccess const access) {
  #define dtCase(m) case PuleAccess_ ## m: return #m
  switch (access) {
    dtCase(none);
    dtCase(read);
    dtCase(write);
    dtCase(readwrite);
  }
  #undef dtCase
}

// alternatively with pule::string if enums are not string literal

// -- resource container -------------------------------------------------------

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

// -- scope guard --------------------------------------------------------------

// Perhaps someday C++ will have scope guards at the language level
#ifndef puleScopeExit
#define _puleScopeExitConcatImpl(x, y) x##y
#define _puleScopeExitConcat(x, y) _puleScopeExitConcatImpl(x, y)
#define puleScopeExit \
  auto const _puleScopeExitConcat(_puleScopeExit, __COUNTER__) \
    = pule::ScopeGuard() << [&]()
#endif

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
} // namespace pule

#endif // __cplusplus
