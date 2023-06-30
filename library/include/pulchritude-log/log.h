#pragma once

#include <pulchritude-core/core.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn bool * puleLogDebugEnabled();
PULE_exportFn bool * puleLogErrorSegfaultsEnabled();


PULE_exportFn void puleLog(char const * const formatCStr, ...);
PULE_exportFn void puleLogDebug(char const * const formatCStr, ...);
PULE_exportFn void puleLogWarn(char const * const formatCStr, ...);
PULE_exportFn void puleLogError(char const * const formatCStr, ...);

// log without newline
PULE_exportFn void puleLogLn(char const * const formatCStr, ...);

PULE_exportFn void puleLogRaw(char const * const formatCStr, ...);

#ifdef __cplusplus
} // extern c
#endif

// a log just meant for development purposes
#define puleLogDev(msg, ...)                     \
  puleLog(                                       \
    "[" __FILE__ ":%d | %s\n    " msg "\n",      \
    __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__ \
  );
