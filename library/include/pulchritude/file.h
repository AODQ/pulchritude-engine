/* auto generated file file */
#pragma once
#include "core.h"

#include "allocator.h"
#include "array.h"
#include "error.h"
#include "stream.h"
#include "string.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorFile_none = 0,
  PuleErrorFile_fileOpen = 1,
} PuleErrorFile;
const uint32_t PuleErrorFileSize = 2;
typedef enum {
  PuleFileDataMode_text = 0,
  PuleFileDataMode_binary = 1,
} PuleFileDataMode;
const uint32_t PuleFileDataModeSize = 2;
typedef enum {
  PuleFileOpenMode_read = 0,
  PuleFileOpenMode_writeOverwrite = 1,
  PuleFileOpenMode_writeAppend = 2,
  PuleFileOpenMode_readWriteOverwrite = 3,
  PuleFileOpenMode_readWrite = 4,
} PuleFileOpenMode;
const uint32_t PuleFileOpenModeSize = 5;

// entities
typedef struct PuleFile { uint64_t id; } PuleFile;
typedef struct PuleFileStream { uint64_t id; } PuleFileStream;
/* 
    will watch files on the filesystem for you, with user-provided callbacks.
    the file-check only happens with 'puleFileWatchCheckAll()', and is not
    currently thread-safe
 */
typedef struct PuleFileWatcher { uint64_t id; } PuleFileWatcher;

// structs
struct PuleFileWatchCreateInfo;

typedef struct PuleFileWatchCreateInfo {
  void(* fileUpdatedCallback)(PuleStringView, void *);
  /*   can be a list separated by ''  */
  PuleStringView filename;
  PuleMillisecond waitTime;
  bool checkMd5Sum;
  void * userdata;
} PuleFileWatchCreateInfo;

// functions
PULE_exportFn PuleFileStream puleFileStreamReadOpen(PuleStringView path, PuleFileDataMode datamode);
PULE_exportFn PuleFileStream puleFileStreamWriteOpen(PuleStringView path, PuleFileDataMode datamode);
PULE_exportFn void puleFileStreamClose(PuleFileStream puStream);
PULE_exportFn PuleStreamRead puleFileStreamReader(PuleFileStream stream);
PULE_exportFn PuleStreamWrite puleFileStreamWriter(PuleFileStream stream);
PULE_exportFn PuleFile puleFileOpen(PuleStringView filename, PuleFileDataMode dataMode, PuleFileOpenMode openMode, struct PuleError * error);
PULE_exportFn PuleString puleFileDump(PuleStringView filename, PuleFileDataMode dataMode, struct PuleError * error);
PULE_exportFn void puleFileClose(PuleFile file);
PULE_exportFn bool puleFileIsDone(PuleFile file);
PULE_exportFn PuleStringView puleFilePath(PuleFile file);
PULE_exportFn uint8_t puleFileReadByte(PuleFile file);
PULE_exportFn size_t puleFileReadBytes(PuleFile file, PuleBufferViewMutable destination);
PULE_exportFn void puleFileWriteBytes(PuleFile file, PuleArrayView source);
PULE_exportFn void puleFileWriteString(PuleFile file, PuleStringView source);
PULE_exportFn uint64_t puleFileSize(PuleFile file);
PULE_exportFn void puleFileAdvanceFromStart(PuleFile file, int64_t offset);
PULE_exportFn void puleFileAdvanceFromEnd(PuleFile file, int64_t offset);
PULE_exportFn void puleFileAdvanceFromCurrent(PuleFile file, int64_t offset);
/* 
  efficient manner to stream file (such as for parsing), meant to grab one byte
  at a time rather than copying the entire contents the only limitation is that
  you can only have one stream open per file, you can't use the normal file
  mechanisms without closing the stream, and it's not thread-safe
 */
PULE_exportFn PuleStreamRead puleFileStreamRead(PuleFile file, PuleBufferViewMutable intermediateBuffer);
PULE_exportFn PuleStreamWrite puleFileStreamWrite(PuleFile file, PuleBufferViewMutable intermediateBuffer);
PULE_exportFn bool puleFilesystemPathExists(PuleStringView path);
PULE_exportFn bool puleFileCopy(PuleStringView sourcePath, PuleStringView destinationPath);
PULE_exportFn bool puleFileRemove(PuleStringView filePath);
PULE_exportFn bool puleFileRemoveRecursive(PuleStringView filePath);
PULE_exportFn bool puleFileDirectoryCreate(PuleStringView path);
PULE_exportFn bool puleFileDirectoryCreateRecursive(PuleStringView path);
PULE_exportFn void puleFilesystemDirWalk(PuleStringView dirPath, void(* walker)(void *, PuleStringView, bool), void * userdata);
PULE_exportFn PuleString puleFilesystemPathCurrent();
PULE_exportFn PuleStringView puleFilesystemExecutablePath();
PULE_exportFn PuleStringView puleFilesystemAssetPath();
PULE_exportFn void puleFilesystemAssetPathSet(PuleStringView path);
/* 
    fetches absolute path of the file, first trying the path directly, then the
    executable path, then the asset path. Returns empty string if the file
    doesn't exist
 */
PULE_exportFn PuleString puleFilesystemAbsolutePath(PuleStringView path);
PULE_exportFn bool puleFilesystemSymlinkCreate(PuleStringView target, PuleStringView linkDst);
/* 
    returns 0 on error; sometimes this might happen if the file is being
    written to, even if 'puleFilesystemPathExists' returns true, in which case
    you might need to check at a later point
 */
PULE_exportFn PuleTimestamp puleFilesystemTimestamp(PuleStringView path);
PULE_exportFn PuleFileWatcher puleFileWatch(PuleFileWatchCreateInfo createinfo);
PULE_exportFn bool puleFileWatchCheckAll();
PULE_exportFn void puleFilesystemWriteString(PuleStringView path, PuleStringView string);

#ifdef __cplusplus
} // extern C
#endif
