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

// convenience function, takes over management of the input stream
PULE_exportFn PuleDsValue puleAssetPdsLoadFromRvalStream(
  PuleAllocator const allocator,
  PuleStreamRead const rvalStream,
  PuleError * const error
);

// convenience function, combined file+stream but disables PDS streaming
PULE_exportFn PuleDsValue puleAssetPdsLoadFromFile(
  PuleAllocator const allocator,
  PuleStringView const filename,
  PuleError * const error
);

PULE_exportFn PuleDsValue puleAssetPdsLoadFromString(
  PuleAllocator const allocator,
  PuleStringView const contents,
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

// convenience function using file+stream but disables PDS streaming
PULE_exportFn void puleAssetPdsWriteToFile(
  PuleDsValue const head,
  PuleStringView const filename,
  PuleError * const error
);

// convenience function using puleStreamStdoutWrite
PULE_exportFn void puleAssetPdsWriteToStdout(PuleDsValue const head);

// can parse command line arguments from the given PDS
// "help" will be inserted if the user requested help flag `--help`
typedef struct {
  PuleAllocator allocator;
  PuleDsValue layout;
  int32_t argumentLength;
  char const * const * arguments;
  bool * userRequestedHelpOutNullable;
} PuleAssetPdsCommandLineArgumentsInfo;
PULE_exportFn PuleDsValue puleAssetPdsLoadFromCommandLineArguments(
  PuleAssetPdsCommandLineArgumentsInfo const info,
  PuleError * const error
);

typedef struct {
  void * data;
  char const * label;
  char const * format;
} PuleAssetPdsDescription;

typedef struct {
  PuleDsValue const value;
  PuleAssetPdsDescription const * const descriptions;
  size_t const descriptionCount;
} PuleAssetPdsDeserializeInfo;
PULE_exportFn void puleAssetPdsDeserialize(
  PuleAssetPdsDeserializeInfo const info,
  PuleError * const error
);

#ifdef __cplusplus
} // extern C
#endif
