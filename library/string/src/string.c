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

static PuleString puleStringFormatVargs(
  PuleAllocator const allocator,
  char const * const format,
  va_list args
) {
  int32_t lenOrErr;
  { // get size of formatted string
    va_list argsLen;
    va_copy(argsLen, args);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    lenOrErr = vsnprintf(NULL, 0, format, argsLen);
#pragma GCC diagnostic pop
    va_end(argsLen);
  }

  if (lenOrErr == 0) { return puleString(allocator, format); }

  size_t const len = (size_t)(lenOrErr);

  PuleString stringOut;
  stringOut.allocator = allocator;

  { // allocate memory
    PuleAllocateInfo allocateInfo = {
      .zeroOut = false,
      .numBytes = len+1,
      .alignment = 0,
    };
    stringOut.contents = puleAllocate(allocator, allocateInfo);
  }

  { // format (consumes args)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    stringOut.len = (size_t)(
      vsnprintf(stringOut.contents, len+1, format, args)
    );
#pragma GCC diagnostic pop
  }

  va_end(args);
  return stringOut;
}

// allocates and formats specified string, using C specifier notations
PuleString puleStringFormat(
  PuleAllocator const allocator,
  char const * const format, ...
) {
  va_list args;
  va_start(args, format);
  PuleString str = puleStringFormatVargs(allocator, format, args);
  va_end(args);
  return str;
}

PuleString puleStringFormatDefault(char const * const format, ...) {
  va_list args;
  va_start(args, format);
  PuleString str = puleStringFormatVargs(puleAllocateDefault(), format, args);
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
