#include <pulchritude/error.h>

#include <pulchritude/log.h>

#include <string>

PuleError puleError() {
  PuleError error;
  error.id = 0;
  error.sourceLocationNullable = nullptr;
  error.lineNumber = 0;
  error.description = (
    PuleString { .contents = nullptr, .len = 0, .allocator = {} }
  );
  error.child = nullptr;
  return error;
}

static std::string puleErrorPrettyPrint(PuleError * const error) {
  std::string asCstdStr = "";
  if (error->sourceLocationNullable) {
    #define fileRelLength 6
    // TODO make an attempt at unwrapping the source location
    auto str = puleStringFormatDefault(
      "(%s:%zu [%zu]): %s",
      error->sourceLocationNullable,
      error->lineNumber,
      error->id,
      error->description.contents
    );
    asCstdStr = std::string(str.contents, str.len);
    puleStringDestroy(str);
  } else {
    auto str = puleStringFormatDefault(
      "(%zu): %s",
      error->id,
      error->description.contents
    );
    asCstdStr = std::string(str.contents, str.len);
    puleStringDestroy(str);
  }
  return asCstdStr;
}

void puleErrorDestroy(PuleError * const error) {
  puleStringDestroy(error->description);
  // deallocate all errors (the first error was not allocated by us)
  PuleError * errorIter = error->child;
  while (errorIter != nullptr) {
    PuleError * const errorIterNext = errorIter->child;
    puleStringDestroy(errorIter->description);
    puleDeallocate(puleAllocateDefault(), errorIter);
    errorIter = errorIterNext;
  }
  *error = puleError();
}

uint32_t puleErrorConsume(PuleError * const error) {
  if (error->id == 0) { return 0; }
  // always take first error, as it aligns to whatever API error the user called
  uint32_t const errorCode = error->id;
  PuleError * errorIter = error;
  // first log all errors, destroying strings
  std::string errorString = "";
  while (errorIter != nullptr) {
    if (errorIter->id == 0) {
      errorIter = errorIter->child;
      continue;
    }
    errorString += "\n\t->";
    errorString += puleErrorPrettyPrint(errorIter);
    errorIter = errorIter->child;
  }
  if (errorString.length() > 4) {
    puleLogError("%s", errorString.c_str()+4);
  }
  puleErrorDestroy(error);
  return errorCode;
}

bool puleErrorExists(PuleError * const error) {
  return error->id != 0;
}

void puleErrorPropagate(
  PuleError * const error,
  PuleError const source
) {
  if (error == nullptr) {
    PuleError sourceCopy = source;
    puleErrorConsume(&sourceCopy);
    return;
  }
  if (source.id == 0) {
    return;
  }
  PuleError * errorIter = error;
  // find empty error (either first element if no errors at this point, or last
  //   null child)
  while (true) {
    if (errorIter->id == 0) { break; }
    if (errorIter->child == nullptr) {
      PuleAllocateInfo allocInfo = {
        .numBytes = sizeof(PuleError),
        .alignment = 0,
        .zeroOut = true,
      };
      errorIter->child = (
        (PuleError *)puleAllocate(puleAllocateDefault(), allocInfo)
      );
      errorIter = errorIter->child;
      break;
    }
    errorIter = errorIter->child;
  }
  // copies to either `error`, or the newly allocated `error->...->child`
  *errorIter = source;
}
