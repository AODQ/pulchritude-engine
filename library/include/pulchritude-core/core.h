#pragma once

#define PULE_exportFn

#ifdef __cplusplus
#include <cstdint>
#include <cstddef>
#include <cassert>
#else
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#endif

#define PULE_assert(X) \
  assert(X);

#define PULE_arraySize(ARR) \
  ((int32_t)(sizeof(ARR) / sizeof(*(ARR))))
