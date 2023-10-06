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
template <typename T> struct ResourceContainer {
  std::unordered_map<uint64_t, T> data = {};
  uint64_t idCount = 1;
  using TUnderlyingValue =
    std::conditional_t<std::is_pointer_v<T>, std::remove_pointer_t<T>, T>
  ;

  // emplaces a new resource into the container
  uint64_t create(T && resource) {
    this->data.emplace(this->idCount, std::forward<T>(resource));
    return (this->idCount++);
  }

  uint64_t create(T const & resource) {
    uint64_t const id = this->idCount ++;
    this->data.emplace(id, resource);
    return id;
  }

  void destroy(uint64_t const handle) {
    if (handle == 0) { return; }
    this->data.erase(handle);
  }

  TUnderlyingValue * at(uint64_t const handle) {
    auto ptr = this->data.find(handle);
    if constexpr(std::is_pointer_v<T>) {
      return ((ptr == this->data.end()) ? nullptr : ptr->second);
    } else {
      return ((ptr == this->data.end()) ? nullptr : &ptr->second);
    }
  }

  template <typename U> T * at(U const object) {
    return this->at(object.id);
  }

  TUnderlyingValue const * at(uint64_t const handle) const {
    auto ptr = this->data.find(handle);
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

#endif
