#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  PuleString description; // default nil
  uint32_t id; // default 0
} PuleError;

PULE_exportFn PuleError puleError();

// consumes error , logging the error appropiately with puleLog, returning the
//   ID of the error, and deallocating the description
PULE_exportFn uint32_t puleErrorConsume(PuleError * const error);

// --- general helpers in C/C++

// errors out with message, storing the error ID to error as well
#define PULE_error(ErrorId, message, ...) \
  *error = { \
    .description = puleStringFormatDefault(message), \
    .id = ErrorId, \
  };

// asserts an expression, which on failure will return the error with the
//   expression as it's description
#define PULE_errorAssert(X, ErrorId, RetValue, ...) \
  if (!(X)) { \
    *error = { \
      .description = ( \
        puleStringFormatDefault( \
          "assertion failed; %s:%d; %s", __FILE__, __LINE__, #X \
        ) \
      ), \
      .id = ErrorId, \
    }; \
    return RetValue; \
  }

// irrecoverable assert, should only be on for relwithdebinfo builds i guess
#define PULE_assert(X) \
  if (!(X)) { \
    puleLogError("assertion failed at %s:%d; %s", __FILE__, __LINE__, #X); \
    exit(-1); \
  }

#ifdef __cplusplus
} // extern C
#endif
