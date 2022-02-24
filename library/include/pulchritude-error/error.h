#pragma once

#include <pulchritude-core/core.h>

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  PuleString description;
  uint32_t id;
} PuleError;

// consumes error , logging the error appropiately with puleLog, returning the
//   ID of the error, and deallocating the description
PULE_exportFn uint32_t puleErrorConsume(PuleError * const);

// --- general helpers in C/C++

// asserts an expression, which on failure will return the error with the
//   expression as it's description
#define PULE_errorAssert(X, ErrorId, RetValue, ...) \
  if (!(X)) { \
    error = { \
      .description = puleStringFormatDefault("assertion failed; %s", #X), \
      .id = ErrorId, \
    }; \
    return RetValue; \
  }

#ifdef __cplusplus
} // extern C
#endif
