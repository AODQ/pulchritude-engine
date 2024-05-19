# file

## structs
### PuleFileWatchCreateInfo
```c
struct {
  fileUpdatedCallback : @fnptr(void, PuleStringView, void ptr);
  /*   can be a list separated by ''  */
  filename : PuleStringView;
  waitTime : PuleMillisecond;
  checkMd5Sum : bool;
  userdata : void ptr;
};
```

## enums
### PuleErrorFile
```c
enum {
  PuleErrorFile_none,
  PuleErrorFile_fileOpen,
}
```
### PuleFileDataMode
```c
enum {
  PuleFileDataMode_text,
  PuleFileDataMode_binary,
}
```
### PuleFileOpenMode
```c
enum {
  PuleFileOpenMode_read,
  PuleFileOpenMode_writeOverwrite,
  PuleFileOpenMode_writeAppend,
  PuleFileOpenMode_readWriteOverwrite,
  PuleFileOpenMode_readWrite,
}
```

## entities
### PuleFile
### PuleFileStream
### PuleFileWatcher

    will watch files on the filesystem for you, with user-provided callbacks.
    the file-check only happens with 'puleFileWatchCheckAll()', and is not
    currently thread-safe


## functions
### puleFileStreamReadOpen
```c
puleFileStreamReadOpen(
  path : PuleStringView,
  datamode : PuleFileDataMode
) PuleFileStream;
```
### puleFileStreamWriteOpen
```c
puleFileStreamWriteOpen(
  path : PuleStringView,
  datamode : PuleFileDataMode
) PuleFileStream;
```
### puleFileStreamClose
```c
puleFileStreamClose(
  puStream : PuleFileStream
) void;
```
### puleFileStreamReader
```c
puleFileStreamReader(
  stream : PuleFileStream
) PuleStreamRead;
```
### puleFileStreamWriter
```c
puleFileStreamWriter(
  stream : PuleFileStream
) PuleStreamWrite;
```
### puleFileOpen
```c
puleFileOpen(
  filename : PuleStringView,
  dataMode : PuleFileDataMode,
  openMode : PuleFileOpenMode,
  error : PuleError ptr
) PuleFile;
```
### puleFileDump
```c
puleFileDump(
  filename : PuleStringView,
  dataMode : PuleFileDataMode,
  error : PuleError ptr
) PuleString;
```
### puleFileClose
```c
puleFileClose(
  file : PuleFile
) void;
```
### puleFileIsDone
```c
puleFileIsDone(
  file : PuleFile
) bool;
```
### puleFilePath
```c
puleFilePath(
  file : PuleFile
) PuleStringView;
```
### puleFileReadByte
```c
puleFileReadByte(
  file : PuleFile
) uint8_t;
```
### puleFileReadBytes
```c
puleFileReadBytes(
  file : PuleFile,
  destination : PuleBufferViewMutable
) size_t;
```
### puleFileWriteBytes
```c
puleFileWriteBytes(
  file : PuleFile,
  source : PuleArrayView
) void;
```
### puleFileWriteString
```c
puleFileWriteString(
  file : PuleFile,
  source : PuleStringView
) void;
```
### puleFileSize
```c
puleFileSize(
  file : PuleFile
) uint64_t;
```
### puleFileAdvanceFromStart
```c
puleFileAdvanceFromStart(
  file : PuleFile,
  offset : int64_t
) void;
```
### puleFileAdvanceFromEnd
```c
puleFileAdvanceFromEnd(
  file : PuleFile,
  offset : int64_t
) void;
```
### puleFileAdvanceFromCurrent
```c
puleFileAdvanceFromCurrent(
  file : PuleFile,
  offset : int64_t
) void;
```
### puleFileStreamRead

  efficient manner to stream file (such as for parsing), meant to grab one byte
  at a time rather than copying the entire contents the only limitation is that
  you can only have one stream open per file, you can't use the normal file
  mechanisms without closing the stream, and it's not thread-safe

```c
puleFileStreamRead(
  file : PuleFile,
  intermediateBuffer : PuleBufferViewMutable
) PuleStreamRead;
```
### puleFileStreamWrite
```c
puleFileStreamWrite(
  file : PuleFile,
  intermediateBuffer : PuleBufferViewMutable
) PuleStreamWrite;
```
### puleFilesystemPathExists
```c
puleFilesystemPathExists(
  path : PuleStringView
) bool;
```
### puleFileCopy
```c
puleFileCopy(
  sourcePath : PuleStringView,
  destinationPath : PuleStringView
) bool;
```
### puleFileRemove
```c
puleFileRemove(
  filePath : PuleStringView
) bool;
```
### puleFileRemoveRecursive
```c
puleFileRemoveRecursive(
  filePath : PuleStringView
) bool;
```
### puleFileDirectoryCreate
```c
puleFileDirectoryCreate(
  path : PuleStringView
) bool;
```
### puleFileDirectoryCreateRecursive
```c
puleFileDirectoryCreateRecursive(
  path : PuleStringView
) bool;
```
### puleFilesystemDirWalk
```c
puleFilesystemDirWalk(
  dirPath : PuleStringView,
  walker : @fnptr(void, void ptr, PuleStringView, bool),
  userdata : void ptr
) void;
```
### puleFilesystemPathCurrent
```c
puleFilesystemPathCurrent() PuleString;
```
### puleFilesystemExecutablePath
```c
puleFilesystemExecutablePath() PuleStringView;
```
### puleFilesystemAssetPath
```c
puleFilesystemAssetPath() PuleStringView;
```
### puleFilesystemAssetPathSet
```c
puleFilesystemAssetPathSet(
  path : PuleStringView
) void;
```
### puleFilesystemAbsolutePath

    fetches absolute path of the file, first trying the path directly, then the
    executable path, then the asset path. Returns empty string if the file
    doesn't exist

```c
puleFilesystemAbsolutePath(
  path : PuleStringView
) PuleString;
```
### puleFilesystemSymlinkCreate
```c
puleFilesystemSymlinkCreate(
  target : PuleStringView,
  linkDst : PuleStringView
) bool;
```
### puleFilesystemTimestamp

    returns 0 on error; sometimes this might happen if the file is being
    written to, even if 'puleFilesystemPathExists' returns true, in which case
    you might need to check at a later point

```c
puleFilesystemTimestamp(
  path : PuleStringView
) PuleTimestamp;
```
### puleFileWatch
```c
puleFileWatch(
  createinfo : PuleFileWatchCreateInfo
) PuleFileWatcher;
```
### puleFileWatchCheckAll
```c
puleFileWatchCheckAll() bool;
```
### puleFilesystemWriteString
```c
puleFilesystemWriteString(
  path : PuleStringView,
  string : PuleStringView
) void;
```
