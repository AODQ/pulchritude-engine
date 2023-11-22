#pragma once

// TODO need to set this to the correct symbol export pragma on windows
#define PULE_exportFn

// common C types such as uint64_t, bool, etc
#ifdef __cplusplus
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#else
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#endif

// gives the size of a static array; T[40] will work, T* will NOT work
#define PULE_arraySize(ARR) ((int32_t)(sizeof(ARR) / sizeof(*(ARR))))

// allows multiline strings to work in C/C++ by converting the entire inputted
//   symbol to a string
#define PULE_multilineString(...) #__VA_ARGS__

// struct initializers for C++
#ifdef __cplusplus
#define PULE_param(...) = __VA_ARGS__
#else
#define PULE_param(...)
#endif

// C++ utilities
#ifdef __cplusplus

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
  Handle create(T && resource) {
    this->data.emplace(this->idCount, std::forward<T>(resource));
    return Handle { (this->idCount++) };
  }

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

} // namespace pule

// Perhaps someday C++ will have scope guards at the language level
#ifndef puleScopeExit
#define puleScopeExit \
  auto const pule__scopeExit_##__COUNTER_ = pule::ScopeGuard() << [&]()
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
} // namespace pule

#endif
