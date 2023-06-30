#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-array/array.h>
#include <pulchritude-error/error.h>
#include <pulchritude-stream/stream.h>
#include <pulchritude-string/string.h>
#include <pulchritude-time/time.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorFile_none,
  PuleErrorFile_fileOpen,
} PuleErrorFile;

typedef enum {
  PuleFileDataMode_text,
  PuleFileDataMode_binary,
} PuleFileDataMode;

typedef enum {
  PuleFileOpenMode_read,
  PuleFileOpenMode_writeOverwrite,
  PuleFileOpenMode_writeAppend,
  PuleFileOpenMode_readWriteOverwrite,
  PuleFileOpenMode_readWrite,
} PuleFileOpenMode;

typedef struct {
  uint64_t id;
} PuleFile;

// -- files
PULE_exportFn PuleFile puleFileOpen(
  PuleStringView const filename,
  PuleFileDataMode const dataMode,
  PuleFileOpenMode const openMode,
  PuleError * const error
);
// FULE_exportFn PuleArray puleFileLoadContents(
//   PuleAllocator const allocator,
//   PuleFileDataMode const dataMode,
//   PuleError * const error
// );
PULE_exportFn void puleFileClose(PuleFile const file);
PULE_exportFn bool puleFileIsDone(PuleFile const file);
PULE_exportFn PuleStringView puleFilePath(PuleFile const file);
PULE_exportFn uint8_t puleFileReadByte(PuleFile const file);
PULE_exportFn size_t puleFileReadBytes( // greedy
  PuleFile const file,
  PuleArrayViewMutable const destination
);
//FULE_exportFn size_t puleFileReadBytesWithStride(
//  PuleFile const file,
//  PuleArrayViewMutable const destination
//);
PULE_exportFn void puleFileWriteBytes(
  PuleFile const file,
  PuleArrayView const source
);
PULE_exportFn void puleFileWriteString(
  PuleFile const file,
  PuleStringView const source
);
PULE_exportFn uint64_t puleFileSize(PuleFile const file);
PULE_exportFn void puleFileAdvanceFromStart(
  PuleFile const file,
  int64_t const offset
);
PULE_exportFn void puleFileAdvanceFromEnd(
  PuleFile const file,
  int64_t const offset
);
PULE_exportFn void puleFileAdvanceFromCurrent(
  PuleFile const file,
  int64_t const offset
);

// efficient manner to stream file (such as for parsing), meant to grab
//   one byte at a time rather than copying the entire contents
// the only limitation is that you can only have one stream open per file,
//   you can't use the normal file mechanisms without closing the stream,
//   and it's not thread-safe
PULE_exportFn PuleStreamRead puleFileStreamRead(
  PuleFile const file,
  PuleArrayViewMutable const intermediateBuffer // PULE_lifetimeOfReturnObject
);

PULE_exportFn PuleStreamWrite puleFileStreamWrite(
  PuleFile const file,
  PuleArrayViewMutable const intermediateBuffer
);

// -- filesystem -- TODO probably rename ot puleFilesystem or puleFs
PULE_exportFn bool puleFilesystemPathExists(PuleStringView const path);
PULE_exportFn bool puleFileCopy(
  PuleStringView const sourcePath,
  PuleStringView const destinationPath
);
PULE_exportFn bool puleFileRemove(PuleStringView const filePath);
PULE_exportFn bool puleFileRemoveRecursive(PuleStringView const filePath);
PULE_exportFn bool puleFileDirectoryCreate(PuleStringView const path);
PULE_exportFn bool puleFileDirectoryCreateRecursive(PuleStringView const path);

// TODO rename all puleFilesystem*Path to puleFilesystemPath*
PULE_exportFn PuleStringView puleFilesystemExecutablePath();

PULE_exportFn PuleString puleFilesystemCurrentPath(
  PuleAllocator const allocator
);

PULE_exportFn PuleStringView puleFilesystemAssetPath();
PULE_exportFn void puleFilesystemAssetPathSet(PuleStringView const path);

// fetches absolute path of the file, first trying the path directly,
// then the executable path, then the asset path. Returns empty string if
// the file doesn't exist
PULE_exportFn PuleString puleFilesystemAbsolutePath(
  PuleStringView const path
);

PULE_exportFn bool puleFilesystemSymlinkCreate(
  PuleStringView const target,
  PuleStringView const linkDst
);

// returns 0 on error; sometimes this might happen if the file is being written
//   to, even if `puleFilesystemPathExists` returns true, in which case you
//   might need to check at a later point
PULE_exportFn PuleTimestamp puleFilesystemTimestamp(PuleStringView const path);

#ifdef __cplusplus
}
#endif

//-- file watch ----------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// will watch files on the filesystem for you, with user-provided callbacks.
//   the file-check only happens with 'puleFileWatchCheckAll()', and is not
//   currently thread-safe

typedef struct { uint64_t id; } PuleFileWatcher;
typedef struct {
  void (*fileUpdatedCallback)(
    PuleStringView const filename,
    void * const userdata
  );
  void (*deallocateUserdataCallback)(void * const userdata);
  PuleStringView filename;
  void * userdata;
} PuleFileWatchCreateInfo;
PULE_exportFn PuleFileWatcher puleFileWatch(
  PuleFileWatchCreateInfo const createinfo
);
// returns true if any files changed
PULE_exportFn bool puleFileWatchCheckAll();
// TODO return a filewatch list of changed files since last check
// TODO filewatch destroy


#ifdef __cplusplus
}
#endif
