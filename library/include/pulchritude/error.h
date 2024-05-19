/* auto generated file error */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/string.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleString description;
  uint32_t id;
  char const * sourceLocationNullable;
  size_t lineNumber;
  PuleError * child;
} PuleError;

// enum

// entities

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
