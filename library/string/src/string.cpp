#include <pulchritude-string/string.h>

#include <string_view>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern "C" {

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
    .allocator = allocator,
  };
  return str;
}

PuleString puleStringDefault(char const * const baseContents) {
  return puleString(puleAllocateDefault(), baseContents);
}

void puleStringDestroy(PuleString const stringInout) {
  puleDeallocate(stringInout.allocator, stringInout.contents);
}

PuleString puleStringFormatVargs(
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
    stringOut.contents = (
      reinterpret_cast<char *>(puleAllocate(allocator, allocateInfo))
    );
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

PuleStringView puleCStr(char const * const cstr) {
  PuleStringView str = {
    .contents = cstr,
    .len = strlen(cstr),
  };
  return str;
}

bool puleStringViewEq(PuleStringView const v0, PuleStringView const v1) {
  if (v0.len != v1.len) {
    return false;
  }
  for (size_t it = 0; it < v0.len; ++ it) {
    if (v0.contents[it] != v1.contents[it]) {
      return false;
    }
  }
  return true;
}

bool puleStringViewEqCStr(PuleStringView const v0, char const * const v1) {
  for (size_t it = 0; it < v0.len; ++ it) {
    if (v0.contents[it] != v1[it]) {
      return false;
    }
  }
  return true;
}

bool puleStringViewContains(PuleStringView const v0, PuleStringView const v1) {
  for (size_t it0 = 0; it0 < v0.len; ++ it0) {
    // ex {ABC, DEF}, iter checks will be (3<0+3), (3<1+3)
    if (v0.len < it0 + v1.len) { break; }
    bool has = true;
    for (size_t it1 = 0; it1 < v1.len; ++ it1) {
      if (v0.contents[it0+it1] != v1.contents[it1]) {
        has = false;
        break;
      }
    }
    if (has) {
      return true;
    }
  }
  return false;
}

size_t puleStringViewHash(PuleStringView const view) {
  auto stringView = std::string_view(view.contents, view.len);
  auto hash = std::hash<std::string_view>{}(stringView);
  return hash;
}

} // C
