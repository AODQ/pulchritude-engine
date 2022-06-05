#include <pulchritude-file/file.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <string>

#include <unordered_map>

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
  char const * const filename,
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
  FILE * const file = fopen(filename, fileMode);
  if (!file) {
    PULE_error(
      PuleErrorFile_fileOpen,
      "failed to open file '%s' (%s)", filename, strerror(errno)
    );
    return { 0 };
  }
  uint64_t const id = reinterpret_cast<uint64_t>(file);
  ::openFiles.emplace(
    id,
    OpenFileInfo { .path = std::string(filename), .file = file, }
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
  flockfile(fileHandle);
  uint8_t * data = reinterpret_cast<uint8_t *>(dst.data);
  size_t bytesWritten = 0;
  for (; bytesWritten < dst.elementCount; ++ bytesWritten) {
    int32_t const character = getc_unlocked(fileHandle);
    // we don't write EOF/'\0'
    if (character == EOF) { break; }
    data[bytesWritten*dst.elementStride] = static_cast<uint8_t>(character);
  }
  funlockfile(fileHandle);

  return bytesWritten;
}

void puleFileWriteBytes(PuleFile const file, PuleArrayView const src) {
  if (src.elementCount == 0) { return; }
  auto const fileHandle = ::openFiles.at(file.id).file;
  flockfile(fileHandle);
  uint8_t const * data = reinterpret_cast<uint8_t const *>(src.data);
  for (size_t it = 0; it < src.elementCount; ++ it) {
    putc_unlocked(data[it*src.elementStride], fileHandle);
  }
  funlockfile(fileHandle);
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

} // C
