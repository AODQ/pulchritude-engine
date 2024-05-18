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
#define PULE_defaultField(...) = __VA_ARGS__
#else
#define PULE_param(...)
#define PULE_defaultField(...)
#endif

// these are data-type enums for general usage, other plugins might use their
//   own enums if they support a different set of data types
typedef enum {
  PuleDt_i8,
  PuleDt_i16,
  PuleDt_i32,
  PuleDt_i64,
  PuleDt_u8,
  PuleDt_u16,
  PuleDt_u32,
  PuleDt_u64,
  PuleDt_f32,
  PuleDt_f64,
  PuleDt_bool,
  PuleDt_ptr,

  PuleDt_f32v2,
  PuleDt_f32v3,
  PuleDt_f32v4,
  PuleDt_f32m44,

  PuleDt_f64v2,
  PuleDt_f64v3,
  PuleDt_f64v4,

  PuleDt_i32v2,
  PuleDt_i32v3,
  PuleDt_i32v4,
  PuleDt_u32v2,
  PuleDt_u32v3,
  PuleDt_u32v4,

  PuleDt_i64v2,
  PuleDt_i64v3,
  PuleDt_i64v4,
  PuleDt_u64v2,
  PuleDt_u64v3,
  PuleDt_u64v4,
} PuleDt;

// simple in/out read/write enum
typedef enum {
  PuleAccess_none,
  PuleAccess_read,
  PuleAccess_write,
  PuleAccess_readwrite,
} PuleAccess;


#include "core.hpp"
