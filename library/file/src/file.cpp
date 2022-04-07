#include <pulchritude-file/file.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unordered_map>

// TODO:
//   - add buffering (configurable, like 256 bytes)

namespace {
std::unordered_map<uint64_t, FILE *> openFiles;
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
        default: PULE_assert(false);
        case PuleFileOpenMode_read: fileMode = "r"; break;
        case PuleFileOpenMode_writeOverwrite: fileMode = "w"; break;
        case PuleFileOpenMode_writeAppend: fileMode = "a+"; break;
        case PuleFileOpenMode_readWriteOverwrite: fileMode = "w+"; break;
        case PuleFileOpenMode_readWrite: fileMode = "r+"; break;
      }
    break;
    case PuleFileDataMode_binary:
      switch (openMode) {
        default: PULE_assert(false);
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
  ::openFiles.emplace(id, file);
  return {id};
}

void puleFileClose(PuleFile const file) {
  if (file.id == 0) {
    return;
  }
  auto const fileHandle = ::openFiles.at(file.id);
  fclose(fileHandle);
}

bool puleFileIsDone(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id);
  return ungetc(getc(fileHandle), fileHandle) == EOF;
}

uint8_t puleFileReadByte(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id);
  return static_cast<uint8_t>(getc(fileHandle));
}

size_t puleFileReadBytes(PuleFile const file, PuleArrayViewMutable const dst) {
  auto const fileHandle = ::openFiles.at(file.id);
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
  auto const fileHandle = ::openFiles.at(file.id);
  flockfile(fileHandle);
  uint8_t const * data = reinterpret_cast<uint8_t const *>(src.data);
  for (size_t it = 0; it < src.elementCount; ++ it) {
    putc_unlocked(data[it*src.elementStride], fileHandle);
  }
  funlockfile(fileHandle);
}

uint64_t puleFileSize(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id);
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

static bool fileStreamIsDone(void * const userdata) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  ::refreshPdsStream(stream);
  return stream.fetchedBufferLength == 0;
}

static uint8_t fileStreamPeekByte(void * const userdata) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  ::refreshPdsStream(stream);
  PULE_assert(!fileStreamIsDone(userdata));
  PULE_assert(stream.bufferIt < stream.fetchedBufferLength);
  uint8_t const character = stream.buffer.data[stream.bufferIt];
  return character;
}

static uint8_t fileStreamReadByte(void * const userdata) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));
  uint8_t const character = fileStreamPeekByte(userdata);
  stream.bufferIt += 1;
  return character;
}

static void fileStreamFlush(void * const userdata) {
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

static void fileStreamWriteBytes(
  void * const userdata,
  uint8_t const * const bytes,
  size_t const length
) {
  ::PdsStream & stream = ::streams.at(reinterpret_cast<uint64_t>(userdata));

  // check if buffer should be flushed out
  if (length + stream.bufferIt >= stream.buffer.elementCount) {
    fileStreamFlush(userdata);
  }

  // check if contents can fit in the intermediary buffer
  if (length >= stream.buffer.elementCount) {
    puleFileWriteBytes(
      stream.file,
      PuleArrayView {
        .data = bytes, .elementStride = 1, .elementCount = length,
      }
    );
  }

  // copy data into the intermediary buffer
  memcpy(stream.buffer.data + stream.bufferIt, bytes, length);
  stream.bufferIt += length;
}

static void fileStreamReadDestroy(void * const userdata) {
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

static void fileStreamWriteDestroy(void * const userdata) {
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
    .readByte = fileStreamReadByte,
    .peekByte = fileStreamPeekByte,
    .isDone = fileStreamIsDone,
    .destroy = fileStreamReadDestroy,
  };
}

PuleStreamWrite puleFileStreamWrite(
  PuleFile const file,
  PuleArrayViewMutable const view
) {
  if (::streams.find(file.id) != ::streams.end()) {
    puleLogError("already streaming file: %d", file.id);
  }
  if (view.elementStride != 0 || view.elementStride != 1) {
    puleLogError("stream write intermediary buffer must have no stride");
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
    .writeBytes = fileStreamWriteBytes,
    .flush = fileStreamFlush,
    .destroy = fileStreamWriteDestroy,
  };
}

} // C
