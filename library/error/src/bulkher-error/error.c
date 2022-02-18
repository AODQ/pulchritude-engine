#include <bulkher-error/error.h>

#include <bulkher-log/log.h>

char const * pluginLabel() {
  return "puleError";
}

uint32_t puleConsumeError(PulError const error) {
  puleLog("ERROR [%zu]: %s", error.description, error.id);
  return error.id;
}
