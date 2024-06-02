#include <pulchritude/file.h>

#include <pulchritude/core.hpp>

#include <pulchritude/time.h>

#include <sys/file.h>

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#if defined(__unix__)
#include <unistd.h>
#endif

// TODO:
//   - add buffering (configurable, like 256 bytes)

namespace {
struct OpenFileInfo {
  std::string path;
  FILE * file;
};
std::unordered_map<uint64_t, OpenFileInfo> openFiles;

struct StreamInfo {
  PuleFile file;
  bool isRead;
  PuleStreamRead streamRead;
  PuleStreamWrite streamWrite;
  PuleBufferViewMutable buffer;
  std::vector<uint8_t> bufferData;
};
pule::ResourceContainer<StreamInfo> fileStreams;

std::string filesystemAssetPath;
std::string filesystemExecutablePath;
} // namespace

extern "C" {

static PuleFileStream fileStreamOpen(
  PuleStringView const path,
  PuleFileDataMode const dataMode,
  bool writemode
) {
  PuleError err = puleError();
  PuleFile const file = puleFileOpen(
    path,
    dataMode,
    (writemode ?  PuleFileOpenMode_writeOverwrite : PuleFileOpenMode_read),
    &err
  );
  if (puleErrorConsume(&err)) { return {0}; }
  auto streamId = fileStreams.create({});
  auto & stream = *fileStreams.at(streamId);
  stream.file = file;
  stream.isRead = !writemode;
  stream.bufferData.resize(512);
  stream.buffer = PuleBufferViewMutable {
    .data = stream.bufferData.data(),
    .byteLength = stream.bufferData.size(),
  };
  if (writemode) {
    stream.streamWrite = puleFileStreamWrite(file, stream.buffer);
  } else {
    stream.streamRead = puleFileStreamRead(file, stream.buffer);
  }
  return {streamId};
}

PuleFileStream puleFileStreamReadOpen(
  PuleStringView const path,
  PuleFileDataMode const dataMode
) {
  return fileStreamOpen(path, dataMode, false);
}
PuleFileStream puleFileStreamWriteOpen(
  PuleStringView const path, PuleFileDataMode const dataMode
) {
  return fileStreamOpen(path, dataMode, true);
}
void puleFileStreamClose(PuleFileStream const puStream) {
  auto & stream = *fileStreams.at(puStream.id);
  if (stream.isRead) {
    puleStreamReadDestroy(stream.streamRead);
  } else {
    puleStreamWriteDestroy(stream.streamWrite);
  }
  puleFileClose(stream.file);
  fileStreams.destroy(puStream.id);
}

PuleStreamRead puleFileStreamReader(PuleFileStream const stream) {
  auto & streamInfo = *fileStreams.at(stream.id);
  return streamInfo.streamRead;
}

PuleStreamWrite puleFileStreamWriter(PuleFileStream const stream) {
  auto & streamInfo = *fileStreams.at(stream.id);
  return streamInfo.streamWrite;
}

PuleFile puleFileOpen(
  PuleStringView const filename,
  PuleFileDataMode const dataMode,
  PuleFileOpenMode const openMode,
  PuleError * const error
) {
  char const * fileMode = "";
  switch (dataMode) {
    default: PULE_assert(false);
    case PuleFileDataMode_text:
      switch (openMode) {
        default: PULE_assert(false); return {};
        case PuleFileOpenMode_read: fileMode = "r"; break;
        case PuleFileOpenMode_writeOverwrite: fileMode = "w"; break;
        case PuleFileOpenMode_writeAppend: fileMode = "a+"; break;
        case PuleFileOpenMode_readWriteOverwrite: fileMode = "w+"; break;
        case PuleFileOpenMode_readWrite: fileMode = "r+"; break;
      }
    break;
    case PuleFileDataMode_binary:
      switch (openMode) {
        default: PULE_assert(false); return {};
        case PuleFileOpenMode_read: fileMode = "rb"; break;
        case PuleFileOpenMode_writeOverwrite: fileMode = "wb"; break;
        case PuleFileOpenMode_writeAppend: fileMode = "a+b"; break;
        case PuleFileOpenMode_readWriteOverwrite: fileMode = "w+b"; break;
        case PuleFileOpenMode_readWrite: fileMode = "r+b"; break;
      }
    break;
  }
  // null terminate the filename
  std::string const filenameStr = std::string(filename.contents, filename.len);
  FILE * file = fopen(filenameStr.c_str(), fileMode);
  if (!file) { // try asset path
    file = fopen((filesystemAssetPath + filenameStr).c_str(), fileMode);
  }
  if (!file) {
    char * err = strerror(errno);
    // TODO for some reason I can't pass err as arg to PULE_error
    puleLogError(
      "failed to open file '%s': %s", filenameStr.c_str(), err
    );
    PULE_error(
      PuleErrorFile_fileOpen, "failed to open file '%s'", filename
    );
    return { 0 };
  }
  uint64_t const id = reinterpret_cast<uint64_t>(file);
  ::openFiles.emplace(
    id,
    OpenFileInfo { .path = filenameStr, .file = file, }
  );
  return {id};
}

PuleString puleFileDump(
  PuleStringView const filename,
  PuleFileDataMode dataMode,
  PuleError * const error
) {
  auto file = puleFileOpen(filename, dataMode, PuleFileOpenMode_read, error);
  if (puleErrorExists(error)) { return {.len=0,}; }
  uint64_t fileSize = puleFileSize(file);
  char * memory = (
    (char *)(puleAllocate(puleAllocateDefault(), {.numBytes = fileSize+1,}))
  );
  puleFileReadBytes(file, { .data = (uint8_t *)memory, .byteLength = fileSize});
  memory[fileSize] = '\0';
  puleFileClose(file);
  return {
    .contents = memory,
    .len = fileSize-1,
    .allocator = puleAllocateDefault()
  };
}

PuleBuffer puleFileDumpContents(
  PuleAllocator const allocator,
  PuleStringView const filename,
  PuleFileDataMode const dataMode,
  PuleError * const error
) {
  auto file = puleFileOpen(filename, dataMode, PuleFileOpenMode_read, error);
  if (puleErrorExists(error)) { return {}; }
  uint64_t fileSize = puleFileSize(file);
  PuleBuffer buffer = puleBufferCreate(allocator);
  puleBufferResize(&buffer, fileSize+1);
  puleFileReadBytes(file, { .data = buffer.data, .byteLength = fileSize});
  buffer.data[fileSize] = '\0';
  puleFileClose(file);
  return buffer;
}

void puleFileClose(PuleFile const file) {
  if (file.id == 0) {
    return;
  }
  auto const fileHandle = ::openFiles.at(file.id).file;
  fclose(fileHandle);
  ::openFiles.erase(file.id);
}

bool puleFileIsDone(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id).file;
  return ungetc(getc(fileHandle), fileHandle) == EOF;
}

PuleStringView puleFilePath(PuleFile const file) {
  auto const & filepath = ::openFiles.at(file.id).path;
  return PuleStringView {
    .contents = filepath.c_str(), .len = filepath.size()
  };
}

uint8_t puleFileReadByte(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id).file;
  return static_cast<uint8_t>(getc(fileHandle));
}

size_t puleFileReadBytes(PuleFile const file, PuleBufferViewMutable const dst) {
  auto const fileHandle = ::openFiles.at(file.id).file;
  flock(fileno(fileHandle), LOCK_EX); // lock file for process
  flockfile(fileHandle); // lock file for this thread
  uint8_t * data = reinterpret_cast<uint8_t *>(dst.data);
  size_t bytesWritten = 0;
  for (; bytesWritten < dst.byteLength; ++ bytesWritten) {
    int32_t const character = getc_unlocked(fileHandle);
    // we don't write EOF/'\0'
    if (character == EOF) { break; }
    data[bytesWritten] = static_cast<uint8_t>(character);
  }
  funlockfile(fileHandle); // unlock for thread
  flock(fileno(fileHandle), LOCK_UN); // unlock for process

  return bytesWritten;
}

void puleFileWriteBytes(PuleFile const file, PuleArrayView const src) {
  PULE_assert(file.id);
  if (src.elementCount == 0) { return; }
  auto const fileHandle = ::openFiles.at(file.id).file;
  flockfile(fileHandle);
  uint8_t const * data = reinterpret_cast<uint8_t const *>(src.data);
  for (size_t it = 0; it < src.elementCount; ++ it) {
    putc_unlocked(data[it*src.elementStride], fileHandle);
  }
  funlockfile(fileHandle);
}

void puleFileWriteString(PuleFile const file, PuleStringView const source) {
  PULE_assert(file.id);
  puleFileWriteBytes(
      file,
      PuleArrayView {
        .data = reinterpret_cast<uint8_t const *>(source.contents),
        .elementStride = 1,
        .elementCount = source.len,
      }
  );
}

uint64_t puleFileSize(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id).file;
  fseek(fileHandle, 0L, SEEK_END);
  uint64_t const fileSize = static_cast<uint64_t>(ftell(fileHandle));
  fseek(fileHandle, 0L, SEEK_SET);
  return fileSize;
}

} // C

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {

struct PdsStream {
  PuleFile file = { 0 };
  PuleBufferViewMutable buffer;
  size_t fetchedBufferLength;
  size_t bufferIt;
};

std::unordered_map<uint64_t, PdsStream> streams;

void refreshPdsStream(PdsStream & stream) {
  // check if there's still data left to read
  if (stream.bufferIt < stream.fetchedBufferLength) {
    return;
  }

  if (puleFileIsDone(stream.file)) {
    stream.bufferIt = 0;
    stream.fetchedBufferLength = 0;
    return;
  }

  stream.bufferIt = 0;
  stream.fetchedBufferLength = puleFileReadBytes(stream.file, stream.buffer);
}

} // namespace

extern "C" {

static bool fileReadStreamIsDone(void * const userdata) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  ::refreshPdsStream(stream);
  return stream.fetchedBufferLength == 0;
}

static uint8_t fileReadStreamPeekByte(void * const userdata) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  ::refreshPdsStream(stream);
  PULE_assert(!fileReadStreamIsDone(userdata));
  PULE_assert(stream.bufferIt < stream.fetchedBufferLength);
  uint8_t const character = stream.buffer.data[stream.bufferIt];
  return character;
}

static uint8_t fileReadStreamReadByte(void * const userdata) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  uint8_t const character = fileReadStreamPeekByte(userdata);
  stream.bufferIt += 1;
  return character;
}

static void fileWriteStreamFlush(void * const userdata) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  puleFileWriteBytes(
    stream.file,
    PuleArrayView {
      .data = stream.buffer.data,
      .elementStride = 1,
      .elementCount = stream.bufferIt,
    }
  );
  stream.bufferIt = 0;
}

static void fileWriteStreamWriteBytes(
  void * const userdata,
  uint8_t const * const bytes,
  size_t const length
) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));

  // check if buffer should be flushed out
  if (length + stream.bufferIt >= stream.buffer.byteLength) {
    fileWriteStreamFlush(userdata);
  }

  // check if contents can fit in the intermediary buffer
  if (length >= stream.buffer.byteLength) {
    puleFileWriteBytes(
      stream.file,
      PuleArrayView {
        .data = bytes, .elementStride = 1, .elementCount = length,
      }
    );
  } else {
    // copy data into the intermediary buffer
    memcpy(stream.buffer.data + stream.bufferIt, bytes, length);
    stream.bufferIt += length;
  }
}

static void fileReadStreamDestroy(void * const userdata) {
  if (!userdata) { return; }
  ::PdsStream const stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  (void)stream; // TODO remove this void
  ::streams.erase(reinterpret_cast<uint64_t>(userdata));

  // note that the file isn't closed -- that's still on the user to do

  // reverse file to where the user last actually read the byte, as otherwise
  //   a segment from file will be lost
  // TODO (needs this function impl.)
  //puleFileAdvanceFromCurrent(
  //  PuleFile{.id = fileStream.id},
  //  -(stream.fetchedBufferLength-stream.bufferIt)
  //);
}

static void fileWriteStreamDestroy(void * const userdata) {
  if (!userdata) { return; }
  ::PdsStream const stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  (void)stream; // TODO remove this void
  ::streams.erase(reinterpret_cast<uint64_t>(userdata));

  // note that the file isn't closed -- that's still on the user to do
}

static void fileStream(
  PuleStreamRead * sread, PuleStreamWrite * swrite,
  PuleFile file,
  PuleBufferViewMutable view,
  bool isWrite
) {
  if (::streams.find(file.id) != ::streams.end()) {
    puleLogError("already streaming file: %llu", file.id);
  }
  ::streams.emplace(
    file.id,
    PdsStream {
      .file = file,
      .buffer = view,
      .fetchedBufferLength = 0,
      .bufferIt = 0,
    }
  );
  if (isWrite) {
    *swrite = PuleStreamWrite {
      .userdata = reinterpret_cast<void *>(file.id),
      .writeBytes = fileWriteStreamWriteBytes,
      .flush      = fileWriteStreamFlush,
      .destroy    = fileWriteStreamDestroy,
    };
  } else {
    *sread =  PuleStreamRead {
      .userdata = reinterpret_cast<void *>(file.id),
      .readByte = fileReadStreamReadByte,
      .peekByte = fileReadStreamPeekByte,
      .isDone   = fileReadStreamIsDone,
      .destroy  = fileReadStreamDestroy,
    };
  }
}

PuleStreamRead puleFileStreamRead(
  PuleFile const file,
  PuleBufferViewMutable const view
) {
  PuleStreamRead sread;
  fileStream(&sread, nullptr, file, view, false);
  return sread;
}

PuleStreamWrite puleFileStreamWrite(
  PuleFile const file,
  PuleBufferViewMutable const view
) {
  PuleStreamWrite swrite;
  fileStream(nullptr, &swrite, file, view, true);
  return swrite;
}

} // C

// -- filesystem

// this will be a fun one to replace :/
#include <filesystem>
#include <string_view>

extern "C" {

bool puleFilesystemPathExists(
  PuleStringView const path
) {
  std::error_code errorCode;
  bool const exists = (
    std::filesystem::exists(
      std::filesystem::path(std::string_view(path.contents, path.len))
    )
  );
  if (errorCode) {
    puleLogError(
      "could not check if file exists for '%s': %s",
      path.contents,
      errorCode.message().c_str()
    );
  }
  return exists;
}

bool puleFileCopy(
  PuleStringView const srcPath,
  PuleStringView const dstPath
) {
  std::error_code errorCode;
  std::filesystem::copy_file(
    std::filesystem::path(std::string_view(srcPath.contents, srcPath.len)),
    std::filesystem::path(std::string_view(dstPath.contents, dstPath.len)),
    std::filesystem::copy_options::overwrite_existing,
    errorCode
  );
  if (errorCode) {
    puleLogError(
      "could not copy file from '%s' to '%s'\n\t-> %s",
      srcPath.contents,
      dstPath.contents,
      errorCode.message().c_str()
    );
    return false;
  }
  return true;
}

bool puleFileRemove(PuleStringView const filePath) {
  std::error_code errorCode;
  std::filesystem::remove(
    std::filesystem::path(std::string_view(filePath.contents, filePath.len)),
    errorCode
  );
  if (errorCode) {
    puleLogError(
      "could not remove file at '%s'\n\t-> %s",
      filePath.contents, errorCode.message().c_str()
    );
    return false;
  }
  return true;
}

bool puleFileRemoveRecursive(PuleStringView const filePath) {
  std::error_code errorCode;
  std::filesystem::remove_all(
    std::filesystem::path(std::string_view(filePath.contents, filePath.len)),
    errorCode
  );
  if (errorCode) {
    puleLogError(
      "could not remove file at '%s'\n\t-> %s",
      filePath.contents, errorCode.message().c_str()
    );
    return false;
  }
  return true;
}

bool puleFileDirectoryCreate(PuleStringView const path) {
  std::error_code errorCode;
  std::filesystem::create_directory(
    std::filesystem::path(std::string_view(path.contents, path.len)),
    errorCode
  );
  if (errorCode) {
    puleLogError(
      "could not create directory at '%s'\n\t-> %s",
      path.contents, errorCode.message().c_str()
    );
    return false;
  }
  return true;
}

bool puleFileDirectoryCreateRecursive(PuleStringView const path) {
  puleLogDebug("creating recursive directories '%s'", path.contents);
  std::error_code errorCode;
  std::filesystem::create_directories(
    std::filesystem::path(std::string_view(path.contents, path.len)),
    errorCode
  );
  if (errorCode) {
    puleLogError(
      "could not create directory at '%s'\n\t-> %s",
      path.contents, errorCode.message().c_str()
    );
    return false;
  }
  return true;
}

PuleString puleFilesystemPathCurrent() {
  std::error_code errorCode;
  auto path = std::filesystem::current_path(errorCode);
  if (errorCode) {
    puleLogError(
      "could not fetch current path: %s",
      errorCode.message().c_str()
    );
    return PuleString {
      .contents = nullptr, .len = 0, .allocator = {.implementation = nullptr},
    };
  }
  return puleStringCopy(puleAllocateDefault(), puleCStr(path.string().c_str()));
}

PuleStringView puleFilesystemExecutablePath() {
  // TODO this needs windows port
  if (::filesystemExecutablePath == "") {
    ::filesystemExecutablePath = (
      std::filesystem::canonical("/proc/self/exe").remove_filename()
    );
  }
  return puleCStr(::filesystemExecutablePath.c_str());
}

bool puleFilesystemSymlinkCreate(
  PuleStringView const target,
  PuleStringView const linkDst
) {
  puleLogDebug(
    "creating symlink '%s' -> '%s'",
    target.contents,
    linkDst.contents
  );
  std::error_code errorCode;
  std::filesystem::create_symlink(
    std::string_view(target.contents, target.len),
    std::string_view(linkDst.contents, linkDst.len),
    errorCode
  );
  if (errorCode) {
    auto const currentPath = puleFilesystemPathCurrent();
    puleLogError(
      "could not create symlink:\n\t'%s'\n\t-> '%s%s%s'\n\t"
      "[%s (current path %s)]",
      target.contents,
      // determine if relative path
      (target.contents[0] == '/' ? "" : currentPath.contents),
      (target.contents[0] == '/' ? "" : "/"),
      linkDst.contents, errorCode.message().c_str(),
      currentPath.contents
    );
    puleStringDestroy(currentPath);
    return false;
  }
  return true;
}

PuleTimestamp puleFilesystemTimestamp(PuleStringView const path) {
  PuleString const pathAbs = puleFilesystemAbsolutePath(path);
  puleScopeExit { puleStringDestroy(pathAbs); };
  if (pathAbs.contents == nullptr) {
    puleLogError(
      "[puleFilesystemTimestamp] file does not exist: '%s'",
      path.contents
    );
    return {.valueUnixTs=0,};
  }
  std::error_code errorcode;
  auto const filetime = std::filesystem::last_write_time(
    std::filesystem::path(std::string_view(pathAbs.contents, pathAbs.len)),
    errorcode
  );
  if (errorcode) {
    return {.valueUnixTs=0,};
  }
  // TODO this is somehow not returning the correct time, but it might just
  //      be on my system as it doesn't have a working clock
  return {
    .valueUnixTs = (
      std::chrono::duration_cast<std::chrono::milliseconds>(
        filetime.time_since_epoch()
      ).count()
    ),
  };
}

} // C

//-- file watch ----------------------------------------------------------------

namespace {

struct FileWatch {
  std::string filename;
  PuleTimestamp lastUpdated;
  PuleTimestamp lastUpdatedNow;
};
struct FileWatcher {
  void (*fileUpdatedCallback)(
    PuleStringView const filename,
    void * const userdata
  );
  std::vector<FileWatch> files;
  void * userdata;
  bool checkMd5Sum;
  PuleMillisecond waitTime;
  bool needsUpdate;
};

std::unordered_map<uint64_t, FileWatcher> fileWatchers;
size_t fileWatcherHashIt = 1;


std::vector<std::string> splitBySemicolon(std::string const & str) {
  std::vector<std::string> result;
  size_t last = 0;
  for (size_t it = 0; it < str.size(); ++ it) {
    if (str[it] == ';') {
      result.emplace_back(str.substr(last, it-last));
      last = it+1;
    }
  }
  result.emplace_back(str.substr(last, str.size()-last));
  return result;
}

}

extern "C" {

PuleFileWatcher puleFileWatch(
  PuleFileWatchCreateInfo const ci
) {
  auto const ciFilename = std::string(ci.filename.contents, ci.filename.len);

  std::vector<::FileWatch> filewatches;
  for (auto const & filename : ::splitBySemicolon(ciFilename )) {
    if (filename == "") { continue; }
    PuleString const filenameAbs = (
      puleFilesystemAbsolutePath(puleCStr(filename.c_str()))
    );
    puleScopeExit { puleStringDestroy(filenameAbs); };
    if (filenameAbs.contents == nullptr) {
      puleLogError(
        "[PuleFileWatcher] file does not exist: '%s'",
        filename.c_str()
      );
      continue;
    }
    filewatches.emplace_back(::FileWatch {
      .filename = std::string(filenameAbs.contents, filenameAbs.len),
      .lastUpdated = puleFilesystemTimestamp(puleStringView(filenameAbs)),
      .lastUpdatedNow = puleTimestampNow(),
    });
  }

  if (filewatches.size() == 0) {
    return {.id = 0,};
  }

  auto const watcher = FileWatcher {
    .fileUpdatedCallback = ci.fileUpdatedCallback,
    .files = filewatches,
    .userdata = ci.userdata,
    .checkMd5Sum = ci.checkMd5Sum,
    .waitTime = ci.waitTime,
    .needsUpdate = false,
  };

  ::fileWatchers.emplace(::fileWatcherHashIt, watcher);
  return {.id = ::fileWatcherHashIt++,};
}

bool puleFileWatchCheckAll() {
  bool anyFilesChanged = false;
  for (auto & filewatchPair : ::fileWatchers) {
    auto & filebundle = filewatchPair.second;
    bool anyFilesInBundleChanged = false;
    for (auto & filewatch : filewatchPair.second.files) {
      // first check that the file exists (e.g. if it's building)
      if (!puleFilesystemPathExists(puleCStr(filewatch.filename.c_str()))) {
        continue;
      }
      auto const timestamp = (
        puleFilesystemTimestamp(puleCStr(filewatch.filename.c_str()))
      );
      auto const now = puleTimestampNow();
      // store the last updated time
      if (filewatch.lastUpdated.valueUnixTs < timestamp.valueUnixTs) {
        filewatch.lastUpdated = timestamp;
        filewatch.lastUpdatedNow = now;
        filebundle.needsUpdate = true;
      }
      if (!filebundle.needsUpdate) { continue; }
      auto const timestampElapsed = (
        now.valueUnixTs - filewatch.lastUpdatedNow.valueUnixTs
      );
      // then check if it's been long enough since the last update time to
      //   trigger callback
      if (timestampElapsed >= filebundle.waitTime.valueMilli) {
        anyFilesInBundleChanged = true;
        filewatch.lastUpdated = timestamp;
        filebundle.needsUpdate = false;
      }
    }
    if (anyFilesInBundleChanged) {
      filebundle.needsUpdate = false;
      if (filebundle.fileUpdatedCallback) {
        filebundle.fileUpdatedCallback(
          PuleStringView {
            .contents="n/a",
            .len=2,
          },
          filebundle.userdata
        );
      }
    }
    anyFilesChanged = anyFilesChanged || anyFilesInBundleChanged;
  }
  return anyFilesChanged;
}

} // C

extern "C" {

void puleFilesystemDirWalk(
  PuleStringView const dirPath,
  void (*walker)(void * userdata, PuleStringView path, bool isFile),
  void * userdata
) {
  std::error_code errorCode;
  std::filesystem::directory_iterator it(
    std::filesystem::path(std::string_view(dirPath.contents, dirPath.len)),
    errorCode
  );
  if (errorCode) {
    puleLogError(
      "could not walk directory '%s': %s",
      dirPath.contents, errorCode.message().c_str()
    );
    return;
  }
  for (; it != std::filesystem::directory_iterator(); ++ it) {
    walker(
      userdata,
      puleCStr(it->path().c_str()),
      std::filesystem::is_regular_file(it->path())
    );
  }
}

PuleStringView puleFilesystemAssetPath() {
  return PuleStringView {
    filesystemAssetPath.c_str(),
    filesystemAssetPath.size(),
  };
}

void puleFilesystemAssetPathSet(PuleStringView const path) {
  filesystemAssetPath = std::string(path.contents, path.len);
}

PuleString puleFilesystemAbsolutePath(
    PuleStringView const path
) {
  if (path.contents == nullptr) { return {}; }
  // check if already abs path
  if (path.contents[0] == '/') {
    return puleStringCopy(puleAllocateDefault(), path);
  }
  { // check current directory
    PuleString const executableDir = puleFilesystemPathCurrent();
    puleScopeExit { puleStringDestroy(executableDir); };
    std::string const executablePath = (
      std::string(executableDir.contents, executableDir.len)
      + "/" + std::string(path.contents, path.len)
    );
    if (puleFilesystemPathExists(puleCStr(executablePath.c_str()))) {
      return puleString(executablePath.c_str());
    }
  }
  { // check executable path
    PuleStringView const executableDir = puleFilesystemExecutablePath();
    std::string const executablePath = (
      std::string(executableDir.contents, executableDir.len)
      + "/" + std::string(path.contents, path.len)
    );
    if (puleFilesystemPathExists(puleCStr(executablePath.c_str()))) {
      return puleString(executablePath.c_str());
    }
  }
  { // check asset path
    PuleStringView const assetDir = puleFilesystemAssetPath();
    std::string const assetPath = (
      std::string(assetDir.contents, assetDir.len)
      + "/" + std::string(path.contents, path.len)
    );
    if (puleFilesystemPathExists(puleCStr(assetPath.c_str()))) {
      return puleString(assetPath.c_str());
    }
  }

  // could not find path
  return PuleString {
    .contents = nullptr,
    .len = 0,
    .allocator = puleAllocateDefault(),
  };
}

} // C

// -- convenience --------------------------------------------------------------
extern "C" {

void puleFilesystemWriteString(
  PuleStringView const path,
  PuleStringView const string
) {
  PuleFile const file = puleFileOpen(
    path, PuleFileDataMode_text, PuleFileOpenMode_writeOverwrite, nullptr
  );
  puleFileWriteString(file, string);
  puleFileClose(file);
}

} // C
