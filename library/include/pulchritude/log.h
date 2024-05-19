/* auto generated file log */
#pragma once
#include <pulchritude/core.h>


#ifdef __cplusplus
extern "C" {
#endif

// structs

// enum

// entities

// functions
PULE_exportFn bool * puleLogDebugEnabled();
PULE_exportFn bool * puleLogErrorSegfaultsEnabled();
PULE_exportFn void puleLog(char const * formatCStr, ...);
PULE_exportFn void puleLogDebug(char const * formatCStr, ...);
PULE_exportFn void puleLogWarn(char const * formatCStr, ...);
PULE_exportFn void puleLogError(char const * formatCStr, ...);
/* 
  this is just for dev purposes in a branch, not to be committed to master
 */
PULE_exportFn void puleLogDev(char const * formatCStr, ...);
/* 
  log without newline
 */
PULE_exportFn void puleLogLn(char const * formatCStr, ...);
PULE_exportFn void puleLogRaw(char const * formatCStr, ...);

#ifdef __cplusplus
} // extern C
#endif
