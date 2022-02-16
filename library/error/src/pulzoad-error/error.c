#include <pulzoad-error/error.h>

#include <pulzoad-log/log.h>

uint32_t puleConsumeError(PulError const error) {
  puleLog("ERROR [%zu]: %s", error.description, error.id);
  return error.id;
}
