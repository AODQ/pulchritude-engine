#include <pulchritude-error/error.h>

#include <pulchritude-log/log.h>

PuleError puleError() {
  PuleError error;
  error.id = 0;
  return error;
}

uint32_t puleErrorConsume(PuleError * const error) {
  if (error->id != 0) {
    puleLog("ERROR [%zu]: %s", error->id, error->description.contents);
    puleStringDeallocate(&error->description);
  }
  return error->id;
}
