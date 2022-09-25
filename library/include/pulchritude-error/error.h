#pragma once

#include <assert.h>

#include <pulchritude-core/core.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  PuleString description; // default nil
  uint32_t id; // default 0
  char const * sourceLocationNullable; // default nil
  size_t lineNumber; // default 0
} PuleError;

PULE_exportFn PuleError puleError();

// consumes error , logging the error appropiately with puleLog, returning the
//   ID of the error, and deallocating the description
PULE_exportFn uint32_t puleErrorConsume(PuleError * const error);

PULE_exportFn bool puleErrorExists(PuleError * const error);

// --- general helpers in C/C++

// errors out with message, storing the error ID to error as well
#define PULE_error(ErrorId, message, ...) \
  *error = { \
    .description = puleStringFormatDefault(message __VA_OPT__(,) __VA_ARGS__), \
    .id = ErrorId, \
    .sourceLocationNullable = __FILE__, \
    .lineNumber = __LINE__, \
  };

// asserts an expression, which on failure will return the error with the
//   expression as it's description
#define PULE_errorAssert(X, ErrorId, RetValue) \
  if (!(X)) { \
    *error = { \
      .description = ( \
        puleStringFormatDefault( \
          "assertion failed; %s", #X \
        ) \
      ), \
      .id = ErrorId, \
      .sourceLocationNullable = __FILE__, \
      .lineNumber = __LINE__, \
    }; \
    return RetValue; \
  }

// irrecoverable assert, should only be on for relwithdebinfo builds i guess
#define PULE_assert(X) { \
  if (!(X)) { \
    puleLogError("assertion failed; %s @ %s:%d", #X, __FILE__, __LINE__); \
    abort(); \
  } \
}

#ifdef __cplusplus
} // extern C
#endif
