#pragma once

// TODO need to set this to the correct symbol export pragma on windows
#define PULE_exportFn

// common C types such as uint64_t, bool, etc
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#else
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#endif

// gives the size of a static array; T[40] will work, T* will NOT work
#define PULE_arraySize(ARR) \
  ((int32_t)(sizeof(ARR) / sizeof(*(ARR))))

// allows multiline strings to work in C/C++ by converting the entire inputted
//   symbol to a string
#define PULE_multilineString(...) #__VA_ARGS__
