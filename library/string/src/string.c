#include <pulchritude-string/string.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

PuleString puleString(
  PuleAllocator const allocator,
  char const * const baseContents
) {
  size_t const len = strlen(baseContents);
  printf("len: %zu\n", len);
  PuleAllocateInfo allocateInfo = {
    .zeroOut = false,
    .numBytes = len+1,
    .alignment = 0,
  };
  char * data = (char *)(puleAllocate(allocator, allocateInfo));
  memcpy(data, baseContents, len);
  data[len] = '\0';
  PuleString str = {
    .contents = data,
    .len = len,
  };
  printf("data: %s\n", data);
  return str;
}

PuleString puleStringDefault(char const * const baseContents) {
  return puleString(puleAllocateDefault(), baseContents);
}

void puleStringDeallocate(PuleString * const stringInout) {
  puleDeallocate(stringInout->allocator, stringInout->contents);
  stringInout->contents = NULL;
  stringInout->len = 0;
}

// allocates and formats specified string, using C specifier notations
PuleString puleStringFormat(
  PuleAllocator const allocator,
  char const * const format, ...
) {
  PuleString stringOut;
  stringOut.allocator = allocator;

  int32_t lenOrErr;
  { // get size of formatted string
    va_list argsLen;
    va_start(argsLen, format);
    lenOrErr = vsnprintf(NULL, 0, format, argsLen);
    va_end(argsLen);
  }

  if (lenOrErr == 0) { return puleString(allocator, format); }

  size_t const len = (size_t)(lenOrErr);

  { // allocate memory
    PuleAllocateInfo allocateInfo = {
      .zeroOut = false,
      .numBytes = len+1,
      .alignment = 0,
    };
    stringOut.contents = puleAllocate(allocator, allocateInfo);
  }

  { // format
    va_list argsFmt;
    va_start(argsFmt, format);
    stringOut.len = (size_t)(
      vsnprintf(stringOut.contents, len+1, format, argsFmt)
    );
    va_end(argsFmt);
  }

  return stringOut;
}

PuleString puleStringFormatDefault(char const * format, ...) {
  va_list args;
  va_start(args, format);
  PuleString str = puleStringFormat(puleAllocateDefault(), format, args);
  va_end(args);
  return str;
}

PuleStringView puleStringView(PuleString const string) {
  PuleStringView str = {
    .contents = string.contents,
    .len = string.len,
  };
  return str;
}

PuleStringView puleStringViewCStr(char const * const cstr) {
  PuleStringView str = {
    .contents = cstr,
    .len = strlen(cstr),
  };
  return str;
}
