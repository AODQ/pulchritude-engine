#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-log/log.h>

#include <stdarg.h>

// provides string tools. This might not be necessary for many
//   languages/environments beyond just passing strings between plugins

#ifdef __cplusplus
extern "C" {
#endif

// strings in Pulchritude are dynamic arrays of uint8_t that must end with a
//   null-terminating character.
typedef struct {
  char * contents;
  size_t len; // number of bytes excluding null-terminator
  PuleAllocator allocator;
} PuleString;

// String views are important, and *very most likely* preferred over PuleString
// or `char const *` as they already store the size. They aren't able to modify
// a string, thus not only are the contents a string unmodifiable, they can not
// reallocate/deallocate a string. However, since they don't manage the memory
// of a string, they are no longer valid once the string being 'viewed' has
// been modified. If it's not self-evident, they are terrible choices for
// long-term storage, but are excellent to pass immutable data between
// interfaces.
typedef struct PuleStringView {
  char const * contents PULE_param(nullptr);
  size_t len PULE_param(0); // TODO rename to byteLen
} PuleStringView;

PULE_exportFn PuleString puleStringDefault(char const * const baseContents);
PULE_exportFn PuleString puleString(
  PuleAllocator const allocator,
  char const * const baseContents
);
PULE_exportFn PuleString puleStringCopy(
  PuleAllocator const allocator,
  PuleStringView const base
);

PULE_exportFn void puleStringDestroy(PuleString const puleStringInout);

PULE_exportFn void puleStringAppend(
  PuleString * const stringInout,
  PuleStringView const append
);

// formats given string, using C specifier notations
// simple usage is;
//   PuleString const str = puleStringFormatDefault("age: %d", 5);
//   ...
//   puleStringDestroy(str);
PULE_exportFn PuleString puleStringFormat(
  PuleAllocator const allocator, char const * const format, ...
);
PULE_exportFn PuleString puleStringFormatDefault(
  char const * const format, ...
);
PULE_exportFn PuleString puleStringFormatVargs(
  PuleAllocator const allocator, char const * const format, va_list args
);

PULE_exportFn PuleStringView puleStringView(PuleString const string);
// equivalent of puleStringViewCStr but it's used so heavily that
//   I decided to shorten it
PULE_exportFn PuleStringView puleCStr(char const * const cstr);

PULE_exportFn bool puleStringViewEq(
  PuleStringView const v0,
  PuleStringView const v1
);
PULE_exportFn bool puleStringViewContains(
  PuleStringView const base,
  PuleStringView const segment
);
PULE_exportFn bool puleStringViewEnds(
  PuleStringView const base,
  PuleStringView const segment
);
PULE_exportFn bool puleStringViewEqCStr(
  PuleStringView const v0,
  char const * const v1
);

PULE_exportFn size_t puleStringViewHash(PuleStringView const view);

#ifdef __cplusplus
} // extern c
#endif

#ifdef __cplusplus

PuleStringView operator ""_psv(char const * const cstr, size_t const len);

#endif // __cplusplus
