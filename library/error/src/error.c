#include <pulchritude-error/error.h>

#include <pulchritude-log/log.h>

PuleError puleError() {
  PuleError error;
  error.id = 0;
  return error;
}

static void puleErrorLog(PuleError * const error) {
  if (error->sourceLocationNullable) {
    #define fileRelLength 6
    char const * fileRel[fileRelLength] = {
      error->sourceLocationNullable, error->sourceLocationNullable,
      error->sourceLocationNullable, error->sourceLocationNullable,
      error->sourceLocationNullable, error->sourceLocationNullable,
    };
    for (char const * fileIt = fileRel[0]; *fileIt != '\0'; ++ fileIt) {
      if (*fileIt == '/' || *fileIt == '\\') {
        for (int it = 0; it < fileRelLength-1; ++ it) {
          fileRel[it] = fileRel[it+1];
        }
        fileRel[fileRelLength-1] = fileIt+1;
      }
    }
    #undef fileRelLength
    puleLogError(
      "(%s:%zu [%zu]): %s",
      fileRel[0],
      error->lineNumber,
      error->id,
      error->description.contents
    );
  } else {
    puleLogError("[%zu]: %s", error->id, error->description.contents);
  }
}

uint32_t puleErrorConsume(PuleError * const error) {
  // always take first error, as it aligns to whatever API error the user called
  uint32_t const errorCode = error->id;
  PuleError * errorIter = error;
  // first log all errors, destroying strings
  while (errorIter != NULL) {
    if (errorIter->id == 0) { break; }
    puleErrorLog(errorIter);
    puleStringDestroy(errorIter->description);
    errorIter = errorIter->child;
  }
  // then deallocate all errors (the first error was not allocated by us)
  errorIter = error->child;
  while (errorIter != NULL) {
    PuleError * const errorIterNext = errorIter->child;
    puleDeallocate(puleAllocateDefault(), errorIter);
    errorIter = errorIterNext;
  }
  *error = puleError();
  return errorCode;
}

bool puleErrorExists(PuleError * const error) {
  return error->id != 0;
}

void puleErrorPropagate(
  PuleError * const error,
  PuleError const source
) {
  if (source.id == 0) {
    return;
  }
  PuleError * errorIter = error;
  // find empty error (either first element if no errors at this point, or last
  //   null child)
  while (true) {
    if (errorIter->id == 0) { break; }
    if (errorIter->child == NULL) {
      PuleAllocateInfo allocInfo = {
        .numBytes = sizeof(PuleError *),
        .alignment = 0,
        .zeroOut = false,
      };
      errorIter->child = puleAllocate(puleAllocateDefault(), allocInfo);
      errorIter = errorIter->child;
      break;
    }
    errorIter = errorIter->child;
  }
  // copies to either `error`, or the newly allocated `error->...->child`
  *errorIter = source;
}
