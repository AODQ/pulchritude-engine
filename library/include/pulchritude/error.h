/* auto generated file error */
#pragma once
// errors out with message, storing the error ID to error as well
#define PULE_error(ErrorId, message, ...) \
  puleErrorPropagate( \
    error, \
    PuleError { \
      .description = ( \
        puleStringFormatDefault(message __VA_OPT__(,) __VA_ARGS__) \
      ), \
      .id = ErrorId, \
      .sourceLocationNullable = __FILE__, \
      .lineNumber = __LINE__, \
      .child = nullptr, \
    } \
  );

// asserts an expression, which on failure will return the error with the
//   expression as it's description
#define PULE_errorAssert(X, ErrorId, RetValue) \
  if (!(X)) { \
    PULE_error(ErrorId, "assertion failed; %s", #X); \
    return RetValue; \
  }

// irrecoverable assert, should only be on for relwithdebinfo builds i guess
#define PULE_assert(X) { \
  if (!(X)) { \
    puleLogError( \
      "assertion failed; %s @ %s:%d", #X, __FILE__, __LINE__ \
    ); \
    abort(); \
  } \
}

#define PULE_assertComparison(X, Y) { \
  if (X != Y) { \
    puleLogError( \
      "assert failed; %s != %s (%d != %d) @ %s:%d", \
      #X, #Y, X, Y, __FILE__, __LINE__ \
    ); \
  } \
}

// pretty print error, format start with ("%s")
#define PULE_prettyError(str, ...) { \
  puleLogError(str, __PRETTY_FUNCTION__, __VA_ARGS__); \
}

#include "core.h"

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs
struct PuleError;

typedef struct PuleError {
  PuleString description;
  uint32_t id;
  char const * sourceLocationNullable;
  size_t lineNumber;
  struct PuleError * child;
} PuleError;

// functions
PULE_exportFn PuleError puleError();
/* 
    deallocates memory used by error, does not log
 */
PULE_exportFn void puleErrorDestroy(PuleError * error);
/* 
    consumes error , logging the error appropiately with puleLog, returning the
    ID of the error, and deallocating the description
 */
PULE_exportFn uint32_t puleErrorConsume(PuleError * error);
PULE_exportFn bool puleErrorExists(PuleError * error);
PULE_exportFn void puleErrorPropagate(PuleError * source, PuleError newError);

#ifdef __cplusplus
} // extern C
#endif
