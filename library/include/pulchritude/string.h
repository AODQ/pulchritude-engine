/* auto generated file string */
#pragma once
#include "core.h"

#include "allocator.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs
struct PuleString;
struct PuleStringView;

/* ASCII/UTF8 encoded string that must end with null-terminating character */
typedef struct PuleString {
  char * contents;
  /* number of bytes excluding null-terminator */
  size_t len;
  PuleAllocator allocator;
} PuleString;
/* light-weight view into a string, with no knowledge of its underlying
    allocation. Unable to modify the contents of the string. The convention
    is to pass immutable data between interfaces with the string-view, and
    make copies if the local system needs to retain the information. */
typedef struct PuleStringView {
  char const * contents;
  size_t len;
} PuleStringView;

// functions
/* 
  create string with default allocator
 */
PULE_exportFn PuleString puleString(char const * baseContents);
PULE_exportFn PuleString puleStringCopy(PuleAllocator, PuleStringView base);
PULE_exportFn void puleStringDestroy(PuleString);
PULE_exportFn void puleStringAppend(PuleString * str, PuleStringView append);
PULE_exportFn PuleString puleStringFormat(PuleAllocator, char const * format, ...);
PULE_exportFn PuleString puleStringFormatDefault(char const * format, ...);
PULE_exportFn PuleString puleStringFormatVargs(PuleAllocator, char const * format, va_list);
PULE_exportFn PuleStringView puleStringView(PuleString string);
PULE_exportFn PuleStringView puleCStr(char const * cstr);
PULE_exportFn bool puleStringViewEq(PuleStringView v0, PuleStringView v1);
PULE_exportFn bool puleStringViewContains(PuleStringView base, PuleStringView seg);
PULE_exportFn bool puleStringViewEnds(PuleStringView base, PuleStringView seg);
PULE_exportFn bool puleStringViewEqCStr(PuleStringView v0, char const * v1);
PULE_exportFn size_t puleStringViewHash(PuleStringView view);

#ifdef __cplusplus
} // extern C
#endif
