#include <pulchritude-file/file.h>

#include <stdio.h>

#include <unordered_map>

// TODO:
//   - add buffering (configurable, like 256 bytes)

namespace {
std::unordered_map<uint64_t, FILE *> openFiles;
} // namespace

extern "C" {

PuleFile puleFileOpen(
  char const * const filename,
  PuleFileDataMode dataMode,
  PuleFileOpenMode openMode,
  PuleError * const error
) {
  char const * fileMode = "";
  switch (dataMode) {
    default: assert(false);
    case PuleFileDataMode_text:
      switch (openMode) {
        default: assert(false);
        case PuleFileOpenMode_read: fileMode = "r"; break;
        case PuleFileOpenMode_writeOverwrite: fileMode = "w"; break;
        case PuleFileOpenMode_writeAppend: fileMode = "a+"; break;
        case PuleFileOpenMode_readWriteOverwrite: fileMode = "w+"; break;
        case PuleFileOpenMode_readWrite: fileMode = "r+"; break;
      }
    break;
    case PuleFileDataMode_binary:
      switch (openMode) {
        default: assert(false);
        case PuleFileOpenMode_read: fileMode = "rb"; break;
        case PuleFileOpenMode_writeOverwrite: fileMode = "wb"; break;
        case PuleFileOpenMode_writeAppend: fileMode = "a+b"; break;
        case PuleFileOpenMode_readWriteOverwrite: fileMode = "w+b"; break;
        case PuleFileOpenMode_readWrite: fileMode = "r+b"; break;
      }
    break;
  }
  FILE * const file = fopen(filename, fileMode);
  PULE_errorAssert(file, PuleErrorFile_fileOpen, return { 0 });
  uint64_t const id = reinterpret_cast<uint64_t>(file);
  ::openFiles.emplace(id, file);
  return id;
}

void puleFileClose(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id);
  fclose(fileHandle);
}

bool puleFileIsDone(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id);
  int32_t character = getc(fileHandle);
  ungetc(fileHandle);
  return character == EOF;
}

uint8_t puleFileReadByte(PuleFile const file) {
  auto const fileHandle = ::openFiles.at(file.id);
  return static_cast<uint8_t>(fileHandle);
}

size_t puleFileReadBytes(PuleFile const file, PuleArrayView const dst) {
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

  return bytesWritten
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
