#pragma once

#define PULE_exportFn

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

#define PULE_arraySize(ARR) \
  ((int32_t)(sizeof(ARR) / sizeof(*(ARR))))
