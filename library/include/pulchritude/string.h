/* auto generated file string */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/allocator.h>
#include <pulchritude/log.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
/* ASCII/UTF8 encoded string that must end with null-terminating character */
typedef struct {
  char * contents;
  /* number of bytes excluding null-terminator */
  size_t len;
  PuleAllocator allocator;
} PuleString;
/* light-weight view into a string, with no knowledge of its underlying
    allocation. Unable to modify the contents of the string. The convention
    is to pass immutable data between interfaces with the string-view, and
    make copies if the local system needs to retain the information. */
typedef struct {
  char const * contents;
  size_t len;
} PuleStringView;

// enum

// entities

// functions
/* 
  create string with default allocator
 */
PULE_exportFn void puleStringDefault(char const * baseContents);
PULE_exportFn PuleString puleString(PuleAllocator _, char const * contents);
PULE_exportFn PuleString puleStringCopy(PuleAllocator _, PuleStringView base);
PULE_exportFn void puleStringDestroy(PuleString _);
PULE_exportFn void puleStringAppend(PuleString * str, PuleStringView append);
PULE_exportFn PuleString puleStringFormat(PuleAllocator _, char const * format, vaArgs _);
PULE_exportFn PuleString puleStringFormatDefault(char const * format, vaArgs _);
PULE_exportFn PuleString puleStringFormatVargs(PuleAllocator _, char const * format, vaList _);
PULE_exportFn PuleStringView puleStringView(PuleString string);
PULE_exportFn PuleStringView puleCStr(char const * cstr);
PULE_exportFn bool puleStringViewEq(PuleStringView v0, PuleStringView v1);
PULE_exportFn bool puleStringViewContains(PuleStringView base, PuleStringView seg);
PULE_exportFn bool puleStringViewEqCstr(PuleStringView v0, char const * v1);
PULE_exportFn size_t puleStringViewHash(PuleStringView view);

#ifdef __cplusplus
} // extern C
#endif
