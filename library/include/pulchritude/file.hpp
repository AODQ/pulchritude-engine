/* auto generated file file */
#pragma once
#include "core.hpp"

#include "file.h"
#include "allocator.hpp"
#include "array.hpp"
#include "error.hpp"
#include "stream.hpp"
#include "string.hpp"
#include "time.hpp"

namespace pule {
struct File {
  PuleFile _handle;
  inline operator PuleFile() const {
    return _handle;
  }
  inline void close() {
    return puleFileClose(this->_handle);
  }
  inline bool isDone() {
    return puleFileIsDone(this->_handle);
  }
  inline PuleStringView path() {
    return puleFilePath(this->_handle);
  }
  inline uint8_t readByte() {
    return puleFileReadByte(this->_handle);
  }
  inline size_t readBytes(PuleBufferViewMutable destination) {
    return puleFileReadBytes(this->_handle, destination);
  }
  inline void writeBytes(PuleArrayView source) {
    return puleFileWriteBytes(this->_handle, source);
  }
  inline void writeString(PuleStringView source) {
    return puleFileWriteString(this->_handle, source);
  }
  inline uint64_t size() {
    return puleFileSize(this->_handle);
  }
  inline void advanceFromStart(int64_t offset) {
    return puleFileAdvanceFromStart(this->_handle, offset);
  }
  inline void advanceFromEnd(int64_t offset) {
    return puleFileAdvanceFromEnd(this->_handle, offset);
  }
  inline void advanceFromCurrent(int64_t offset) {
    return puleFileAdvanceFromCurrent(this->_handle, offset);
  }
  inline PuleStreamRead streamRead(PuleBufferViewMutable intermediateBuffer) {
    return puleFileStreamRead(this->_handle, intermediateBuffer);
  }
  inline PuleStreamWrite streamWrite(PuleBufferViewMutable intermediateBuffer) {
    return puleFileStreamWrite(this->_handle, intermediateBuffer);
  }
  static inline File open(PuleStringView filename, PuleFileDataMode dataMode, PuleFileOpenMode openMode, PuleError * error) {
    return { ._handle = puleFileOpen(filename, dataMode, openMode, error),};
  }
};
}
  inline void close(pule::File self) {
    return puleFileClose(self._handle);
  }
  inline bool isDone(pule::File self) {
    return puleFileIsDone(self._handle);
  }
  inline PuleStringView path(pule::File self) {
    return puleFilePath(self._handle);
  }
  inline uint8_t readByte(pule::File self) {
    return puleFileReadByte(self._handle);
  }
  inline size_t readBytes(pule::File self, PuleBufferViewMutable destination) {
    return puleFileReadBytes(self._handle, destination);
  }
  inline void writeBytes(pule::File self, PuleArrayView source) {
    return puleFileWriteBytes(self._handle, source);
  }
  inline void writeString(pule::File self, PuleStringView source) {
    return puleFileWriteString(self._handle, source);
  }
  inline uint64_t size(pule::File self) {
    return puleFileSize(self._handle);
  }
  inline void advanceFromStart(pule::File self, int64_t offset) {
    return puleFileAdvanceFromStart(self._handle, offset);
  }
  inline void advanceFromEnd(pule::File self, int64_t offset) {
    return puleFileAdvanceFromEnd(self._handle, offset);
  }
  inline void advanceFromCurrent(pule::File self, int64_t offset) {
    return puleFileAdvanceFromCurrent(self._handle, offset);
  }
  inline PuleStreamRead streamRead(pule::File self, PuleBufferViewMutable intermediateBuffer) {
    return puleFileStreamRead(self._handle, intermediateBuffer);
  }
  inline PuleStreamWrite streamWrite(pule::File self, PuleBufferViewMutable intermediateBuffer) {
    return puleFileStreamWrite(self._handle, intermediateBuffer);
  }
namespace pule {
struct FileStream {
  PuleFileStream _handle;
  inline operator PuleFileStream() const {
    return _handle;
  }
  inline void close() {
    return puleFileStreamClose(this->_handle);
  }
  inline PuleStreamRead reader() {
    return puleFileStreamReader(this->_handle);
  }
  inline PuleStreamWrite writer() {
    return puleFileStreamWriter(this->_handle);
  }
  static inline FileStream readOpen(PuleStringView path, PuleFileDataMode datamode) {
    return { ._handle = puleFileStreamReadOpen(path, datamode),};
  }
  static inline FileStream writeOpen(PuleStringView path, PuleFileDataMode datamode) {
    return { ._handle = puleFileStreamWriteOpen(path, datamode),};
  }
};
}
  inline void close(pule::FileStream self) {
    return puleFileStreamClose(self._handle);
  }
  inline PuleStreamRead reader(pule::FileStream self) {
    return puleFileStreamReader(self._handle);
  }
  inline PuleStreamWrite writer(pule::FileStream self) {
    return puleFileStreamWriter(self._handle);
  }
namespace pule {
using FileWatcher = PuleFileWatcher;
}
namespace pule {
using FileWatchCreateInfo = PuleFileWatchCreateInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorFile const e) {
  switch (e) {
    case PuleErrorFile_none: return puleString("none");
    case PuleErrorFile_fileOpen: return puleString("fileOpen");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleFileDataMode const e) {
  switch (e) {
    case PuleFileDataMode_text: return puleString("text");
    case PuleFileDataMode_binary: return puleString("binary");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleFileOpenMode const e) {
  switch (e) {
    case PuleFileOpenMode_read: return puleString("read");
    case PuleFileOpenMode_writeOverwrite: return puleString("writeOverwrite");
    case PuleFileOpenMode_writeAppend: return puleString("writeAppend");
    case PuleFileOpenMode_readWriteOverwrite: return puleString("readWriteOverwrite");
    case PuleFileOpenMode_readWrite: return puleString("readWrite");
    default: return puleString("N/A");
  }
}
}
