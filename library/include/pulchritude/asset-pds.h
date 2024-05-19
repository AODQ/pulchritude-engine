/* auto generated file asset-pds */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/error.h>
#include <pulchritude/data-serializer.h>
#include <pulchritude/allocator.h>
#include <pulchritude/stream.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
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
typedef struct {
  PuleAllocator allocator;
  PuleDsValue layout;
  int32_t argumentLength;
  char const * const * arguments;
  bool * userRequestedHelpOutNullable;
} PuleAssetPdsCommandLineArgumentsInfo;
typedef struct {
  void * data;
  char const * label;
  char const * format;
} PuleAssetPdsDescription;
typedef struct {
  PuleDsValue value;
  PuleAssetPdsDescription const * descriptions;
  size_t descriptionCount;
} PuleAssetPdsDeserializeInfo;

// enum
typedef enum {
  PuleErrorAssetPds_none,
  PuleErrorAssetPds_decode,
} PuleErrorAssetPds;

// entities

// functions
PULE_exportFn PuleDsValue puleAssetPdsLoadFromStream(PuleAllocator allocator, PuleStreamRead stream, PuleError * error);
/* 
  convenience function, takes over management of the input stream
 */
PULE_exportFn PuleDsValue puleAssetPdsLoadFromRvalStream(PuleAllocator allocator, PuleStreamRead rvalStream, PuleError * error);
/* 
  convenience function, combined file+stream but disables PDS streaming
 */
PULE_exportFn PuleDsValue puleAssetPdsLoadFromFile(PuleAllocator allocator, PuleStringView filename, PuleError * error);
PULE_exportFn PuleDsValue puleAssetPdsLoadFromString(PuleAllocator allocator, PuleStringView contents, PuleError * error);
PULE_exportFn void puleAssetPdsWriteToStream(PuleStreamWrite stream, PuleAssetPdsWriteInfo writeInfo);
/* convenience function using file+stream but disables PDS streaming */
PULE_exportFn void puleAssetPdsWriteToFile(PuleDsValue head, PuleStringView filename, PuleError * error);
/* 
  convenience function using puleStreamStdoutWrite
 */
PULE_exportFn void puleAssetPdsWriteToStdout(PuleDsValue head);
PULE_exportFn PuleDsValue puleAssetPdsLoadFromCommandLineArguments(PuleAssetPdsCommandLineArgumentsInfo info, PuleError * error);
PULE_exportFn void puleAssetPdsDeserialize(PuleAssetPdsDeserializeInfo info, PuleError * error);

#ifdef __cplusplus
} // extern C
#endif
