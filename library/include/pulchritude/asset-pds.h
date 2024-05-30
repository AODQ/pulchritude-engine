/* auto generated file asset-pds */
#pragma once
#include "core.h"

#include "error.h"
#include "data-serializer.h"
#include "allocator.h"
#include "stream.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorAssetPds_none = 0,
  PuleErrorAssetPds_decode = 1,
} PuleErrorAssetPds;
const uint32_t PuleErrorAssetPdsSize = 2;

// entities

// structs
struct PuleAssetPdsWriteInfo;
struct PuleAssetPdsCommandLineArgumentsInfo;
struct PuleAssetPdsDescription;
struct PuleAssetPdsDeserializeInfo;

typedef struct PuleAssetPdsWriteInfo {
  PuleDsValue head;
  bool prettyPrint;
  /*  pretty-print only  */
  uint32_t spacesPerTab;
  uint32_t initialTabLevel;
} PuleAssetPdsWriteInfo;
/* 
  can parse command line arguments from the given PDS "help" will be inserted
  if the user requested help flag '--help'
 */
typedef struct PuleAssetPdsCommandLineArgumentsInfo {
  PuleAllocator allocator;
  PuleDsValue layout;
  int32_t argumentLength;
  char const * const * arguments;
  bool * userRequestedHelpOutNullable;
} PuleAssetPdsCommandLineArgumentsInfo;
typedef struct PuleAssetPdsDescription {
  void * data;
  char const * label;
  char const * format;
} PuleAssetPdsDescription;
typedef struct PuleAssetPdsDeserializeInfo {
  PuleDsValue value;
  PuleAssetPdsDescription const * descriptions;
  size_t descriptionCount;
} PuleAssetPdsDeserializeInfo;

// functions
PULE_exportFn PuleDsValue puleAssetPdsLoadFromStream(PuleAllocator allocator, PuleStreamRead stream, struct PuleError * error);
/* 
  convenience function, takes over management of the input stream
 */
PULE_exportFn PuleDsValue puleAssetPdsLoadFromRvalStream(PuleAllocator allocator, PuleStreamRead rvalStream, struct PuleError * error);
/* 
  convenience function, combined file+stream but disables PDS streaming
 */
PULE_exportFn PuleDsValue puleAssetPdsLoadFromFile(PuleAllocator allocator, PuleStringView filename, struct PuleError * error);
PULE_exportFn PuleDsValue puleAssetPdsLoadFromString(PuleAllocator allocator, PuleStringView contents, struct PuleError * error);
PULE_exportFn void puleAssetPdsWriteToStream(PuleStreamWrite stream, PuleAssetPdsWriteInfo writeInfo);
/* convenience function using file+stream but disables PDS streaming */
PULE_exportFn void puleAssetPdsWriteToFile(PuleDsValue head, PuleStringView filename, struct PuleError * error);
/* 
  convenience function using puleStreamStdoutWrite
 */
PULE_exportFn void puleAssetPdsWriteToStdout(PuleDsValue head);
PULE_exportFn PuleDsValue puleAssetPdsLoadFromCommandLineArguments(PuleAssetPdsCommandLineArgumentsInfo info, struct PuleError * error);
PULE_exportFn void puleAssetPdsDeserialize(PuleAssetPdsDeserializeInfo info, struct PuleError * error);

#ifdef __cplusplus
} // extern C
#endif
