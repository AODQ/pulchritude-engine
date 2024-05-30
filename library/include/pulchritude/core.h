/* auto generated file core */
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
#include <cstdarg>
#else
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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


#ifdef __cplusplus
extern "C" {
#endif

// enum
/* General purpose Data types */
typedef enum {
  PuleDt_i8 = 0,
  PuleDt_i16 = 1,
  PuleDt_i32 = 2,
  PuleDt_i64 = 3,
  PuleDt_u8 = 4,
  PuleDt_u16 = 5,
  PuleDt_u32 = 6,
  PuleDt_u64 = 7,
  PuleDt_f32 = 8,
  PuleDt_f64 = 9,
  PuleDt_bool = 10,
  PuleDt_ptr = 11,
  PuleDt_f32v2 = 12,
  PuleDt_f32v3 = 13,
  PuleDt_f32v4 = 14,
  PuleDt_f32m44 = 15,
  PuleDt_f64v2 = 16,
  PuleDt_f64v3 = 17,
  PuleDt_f64v4 = 18,
  PuleDt_i32v2 = 19,
  PuleDt_i32v3 = 20,
  PuleDt_i32v4 = 21,
  PuleDt_u32v2 = 22,
  PuleDt_u32v3 = 23,
  PuleDt_u32v4 = 24,
  PuleDt_i64v2 = 25,
  PuleDt_i64v3 = 26,
  PuleDt_i64v4 = 27,
  PuleDt_u64v2 = 28,
  PuleDt_u64v3 = 29,
  PuleDt_u64v4 = 30,
} PuleDt;
const uint32_t PuleDtSize = 31;
/* general purpose Access flags */
typedef enum {
  PuleAccess_none = 0,
  PuleAccess_read = 1,
  PuleAccess_write = 2,
  PuleAccess_readwrite = 3,
} PuleAccess;
const uint32_t PuleAccessSize = 4;

// entities

// structs


// functions

#ifdef __cplusplus
} // extern C
#endif
