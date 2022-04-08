#pragma once

// facilitates PDS assets

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-core/core.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-error/error.h>
#include <pulchritude-stream/stream.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorAssetPds_none,
  PuleErrorAssetPds_decode,
} PuleErrorAssetPds;

PULE_exportFn PuleDsValue puleAssetPdsLoadFromStream(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  PuleError * const error
);

typedef struct {
  PuleDsValue head;
  bool prettyPrint; // PULE_defaultValue(false)
  uint32_t spacesPerTab; // PULE_defaultValue(2) ; only used with pretty-print
  uint32_t initialTabLevel; // PULE_defaultValue(0)
} PuleAssetPdsWriteInfo;
PULE_exportFn void puleAssetPdsWriteToStream(
  PuleStreamWrite const stream,
  PuleAssetPdsWriteInfo const writeInfo
);

// can parse command line arguments from the given PDS
// "help" will be inserted if the user requested help flag `--help`
PULE_exportFn PuleDsValue puleAssetPdsLoadFromCommandLineArguments(
  PuleAllocator const allocator,
  char const * const layout,
  int32_t const argumentLength,
  char const * const * const arguments,
  PuleError * const error
);

#ifdef __cplusplus
}
#endif
