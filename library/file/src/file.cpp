#include <pulchritude-file/file.h>

#include <pulchritude-time/time.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/file.h>

#include <string>
#include <unordered_map>
#include <vector>

// TODO:
//   - add buffering (configurable, like 256 bytes)

namespace {
struct OpenFileInfo {
  std::string path;
  FILE * file;
};
std::unordered_map<uint64_t, OpenFileInfo> openFiles;
} // namespace

extern "C" {

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
  FILE * const file = fopen(filenameStr.c_str(), fileMode);
  if (!file) {
    auto const strerrorcp = std::string(strerror(errno));
    // in C11 there is nice strerrorlen_s and strerror_s, but not in C++
    // TODO but this doesnt seem to work, possibly bug in my code. Try to fix
    //std::vector<char> strerrorcopy;
    //strerrorcopy.resize(1024);
    //strerror_r(errno, strerrorcopy.data(), 1024);
    PULE_error(
      PuleErrorFile_fileOpen,
      "failed to open file '%s'", filename
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

size_t puleFileReadBytes(PuleFile const file, PuleArrayViewMutable const dst) {
  auto const fileHandle = ::openFiles.at(file.id).file;
  flock(fileno(fileHandle), LOCK_EX); // lock file for process
  flockfile(fileHandle); // lock file for this thread
  uint8_t * data = reinterpret_cast<uint8_t *>(dst.data);
  size_t bytesWritten = 0;
  for (; bytesWritten < dst.elementCount; ++ bytesWritten) {
    int32_t const character = getc_unlocked(fileHandle);
    // we don't write EOF/'\0'
    if (character == EOF) { break; }
    data[bytesWritten*dst.elementStride] = static_cast<uint8_t>(character);
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
  PuleArrayViewMutable buffer;
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
  if (length + stream.bufferIt >= stream.buffer.elementCount) {
    fileWriteStreamFlush(userdata);
  }

  // check if contents can fit in the intermediary buffer
  if (length >= stream.buffer.elementCount) {
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

PuleStreamRead puleFileStreamRead(
  PuleFile const file,
  PuleArrayViewMutable const view
) {
  if (::streams.find(file.id) != ::streams.end()) {
    puleLogError("already streaming file: %d", file.id);
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
  return PuleStreamRead {
    .userdata = reinterpret_cast<void *>(file.id),
    .readByte = fileReadStreamReadByte,
    .peekByte = fileReadStreamPeekByte,
    .isDone   = fileReadStreamIsDone,
    .destroy  = fileReadStreamDestroy,
  };
}

PuleStreamWrite puleFileStreamWrite(
  PuleFile const file,
  PuleArrayViewMutable const view
) {
  if (::streams.find(file.id) != ::streams.end()) {
    puleLogError("already streaming file: %d", file.id);
  }
  if (view.elementStride != 0 && view.elementStride != 1) {
    puleLogError(
      "puleFileStreamWrite(%s) intermediary buffer stride '%zu' must be 0 or 1",
      puleFilePath(file),
      view.elementStride
    );
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
  return PuleStreamWrite {
    .userdata = reinterpret_cast<void *>(file.id),
    .writeBytes = fileWriteStreamWriteBytes,
    .flush      = fileWriteStreamFlush,
    .destroy    = fileWriteStreamDestroy,
  };
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

PuleString puleFilesystemExecutablePath(PuleAllocator const allocator) {
  // TODO[crossplatform] isn't compatible with windows or MacOS.
  auto path = std::filesystem::canonical("/proc/self/exe").remove_filename();
  // remove the last
  return puleString(allocator, path.string().c_str());
}

PuleString puleFilesystemCurrentPath(PuleAllocator const allocator) {
  std::error_code errorCode;
  auto path = std::filesystem::current_path(errorCode);
  if (errorCode) {
    puleLogError(
      "could not fetch current path: %s",
      errorCode.message().c_str()
    );
    return PuleString{.contents = nullptr, .len = 0, .allocator = allocator,};
  }
  return puleString(allocator, path.string().c_str());
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
    auto const currentPath = puleFilesystemCurrentPath(puleAllocateDefault());
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
  if (!puleFilesystemPathExists(path)) {
    return {.value=0,};
  }
  std::error_code errorcode;
  auto const filetime = std::filesystem::last_write_time(
    std::filesystem::path(std::string_view(path.contents, path.len)),
    errorcode
  );
  if (errorcode) {
    return {.value=0,};
  }
  return {
    .value = (
      static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
          filetime.time_since_epoch()
        ).count()
      )
    ),
  };
}

} // C

//-- file watch ----------------------------------------------------------------

namespace {
  struct FileWatcher {
    void (*fileUpdatedCallback)(
      PuleStringView const filename,
      void * const userdata
    );
    std::string filename;
    void * userdata;
    PuleTimestamp lastUpdated;
  };

  std::unordered_map<uint64_t, FileWatcher> fileWatchers;
  size_t fileWatcherHashIt = 1;
}

extern "C" {

PuleFileWatcher puleFileWatch(
  PuleFileWatchCreateInfo const ci
) {
  auto const filename = std::string(ci.filename.contents, ci.filename.len);

  auto const watcher = FileWatcher {
    .fileUpdatedCallback = ci.fileUpdatedCallback,
    .filename = filename,
    .userdata = ci.userdata,
    .lastUpdated = puleFilesystemTimestamp(ci.filename),
  };

  ::fileWatchers.emplace(::fileWatcherHashIt, watcher);
  return {.id = ::fileWatcherHashIt++,};
}

bool puleFileWatchCheckAll() {
  bool anyFilesChanged = false;
  for (auto & filewatchPair : ::fileWatchers) {
    auto & filewatch = std::get<1>(filewatchPair);
    auto const timestamp = (
      puleFilesystemTimestamp(puleCStr(filewatch.filename.c_str()))
    );
    if (filewatch.lastUpdated.value < timestamp.value) {
      anyFilesChanged = true;
      filewatch.lastUpdated = timestamp;
      if (filewatch.fileUpdatedCallback) {
        filewatch.fileUpdatedCallback(
          PuleStringView {
            .contents=filewatch.filename.c_str(),
            .len=filewatch.filename.size(),
          },
          filewatch.userdata
        );
      }
    }
  }
  return anyFilesChanged;
}

} // C
