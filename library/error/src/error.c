#include <pulchritude-error/error.h>

#include <pulchritude-log/log.h>

PuleError puleError() {
  PuleError error;
  error.id = 0;
  return error;
}

uint32_t puleErrorConsume(PuleError * const error) {
  if (error->id != 0) {
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
    puleStringDestroy(error->description);
    *error = puleError();
  }
  return error->id;
}

bool puleErrorExists(PuleError * const error) {
  return error->id != 0;
}
