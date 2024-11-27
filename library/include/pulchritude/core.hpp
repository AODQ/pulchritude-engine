/* auto generated file core */
#pragma once
#include "core.hpp"

#include "core.h"
#include <utility>
#include <cstdio>
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
#ifdef __unix__
#include <execinfo.h>
#endif
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

  TUnderlyingValue * fetch(Handle const handle) {
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

  TUnderlyingValue * at(Handle const handle) {
    TUnderlyingValue * ptr = this->fetch(handle);
    if (ptr == nullptr) {
      printf("failed to find resource with handle %p\n", handle);
#if defined(__unix__) // TODO apple
    void * callstack[128];
    int32_t const numFrames = backtrace(callstack, 128);
    char ** const symbols = backtrace_symbols(callstack, numFrames);
    printf("\t");
    if (symbols != NULL) {
      for (int32_t i = 1; i < numFrames; ++ i) { // skip log function
        char * const parenStr = strchr(symbols[i], '(');
        char * const plusStr = strchr(symbols[i], '+');
        if ((int32_t)(plusStr - parenStr) > 1) {
          if (strncmp(parenStr+1, "main", 4) == 0) { break; }
          printf(
            " -> %.*s",
            (int32_t)(plusStr - parenStr-1), parenStr+1
          );
        }
      }
      free(symbols);
    }
    printf("\n\n");
#endif
      abort();
    }
    return ptr;
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


