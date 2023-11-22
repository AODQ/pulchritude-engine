#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-compiler/compiler.h>
#include <pulchritude-error/error.h>
#include <pulchritude-stream/stream.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorEL_none,
  PuleErrorEL_compileError,
} PuleErrorEL;

PULE_exportFn PuleIRModule puleELModuleCreate(
  PuleStreamRead const stream,
  PuleError * const error
);

#ifdef __cplusplus
} // extern "C"
#endif
