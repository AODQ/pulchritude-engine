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

//this is just for dev purposes in a branch, not to be committed to master
PULE_exportFn void puleLogDev(char const * const formatCStr, ...);

// log without newline
PULE_exportFn void puleLogLn(char const * const formatCStr, ...);

PULE_exportFn void puleLogRaw(char const * const formatCStr, ...);

#ifdef __cplusplus
} // extern c
#endif
