#include <pulchritude-error/error.h>

#include <pulchritude-log/log.h>

uint32_t puleErrorConsume(PuleError * const error) {
  if (error->id != 0) {
    puleLog("ERROR [%zu]: %s", error->id, error->description.contents);
    puleStringDeallocate(&error->description);
  }
  return error->id;
}
