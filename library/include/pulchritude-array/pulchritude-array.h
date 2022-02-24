#pragma once

#include <pulchritude-core/core.h>

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#endif

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-log/log.h>

// provides array tools. This might not be necessary for many
//   languages/environments beyond just passing arrays between plugins

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorArray_none,
  PuleErrorArray_errorAllocation,
} PuleErrorArray;

// Arrays by themselves are probably useless since you can just pass allocations
//   around. So they are not here for now.

// Array views let you view the contents of an array, without modifying the
//   contents nor modifying it's allocation
typedef struct {
  void * const data;
  size_t len;
} PuleArrayView;

#ifdef __plusplus
} // extern c
#endif
