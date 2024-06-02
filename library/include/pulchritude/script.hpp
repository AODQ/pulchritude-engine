/* auto generated file script */
#pragma once
#include "core.hpp"

#include "script.h"
#include "string.hpp"
#include "error.hpp"
#include "array.hpp"
#include "file.hpp"

namespace pule {
struct ScriptContext {
  PuleScriptContext _handle;
  inline operator PuleScriptContext() const {
    return _handle;
  }
  inline void destroy() {
    return puleScriptContextDestroy(this->_handle);
  }
  static inline ScriptContext create() {
    return { ._handle = puleScriptContextCreate(),};
  }
};
}
  inline void destroy(pule::ScriptContext self) {
    return puleScriptContextDestroy(self._handle);
  }
namespace pule {
struct ScriptModule {
  PuleScriptModule _handle;
  inline operator PuleScriptModule() const {
    return _handle;
  }
  static inline ScriptModule createFromSource(PuleScriptContext context, PuleScriptModuleFromSourceCreateInfo createInfo, PuleError * error) {
    return { ._handle = puleScriptModuleCreateFromSource(context, createInfo, error),};
  }
  static inline ScriptModule createFromBinary(PuleScriptContext context, PuleArrayView scriptBinary, PuleError * error) {
    return { ._handle = puleScriptModuleCreateFromBinary(context, scriptBinary, error),};
  }
};
}
namespace pule {
struct ScriptArrayF32 {
  PuleScriptArrayF32 _handle;
  inline operator PuleScriptArrayF32() const {
    return _handle;
  }
  inline void destroy() {
    return puleScriptArrayF32Destroy(this->_handle);
  }
  inline void * ptr() {
    return puleScriptArrayF32Ptr(this->_handle);
  }
  inline void append(float value) {
    return puleScriptArrayF32Append(this->_handle, value);
  }
  inline void remove(size_t index) {
    return puleScriptArrayF32Remove(this->_handle, index);
  }
  inline float at(size_t index) {
    return puleScriptArrayF32At(this->_handle, index);
  }
  inline size_t length() {
    return puleScriptArrayF32Length(this->_handle);
  }
  inline size_t elementByteSize() {
    return puleScriptArrayF32ElementByteSize(this->_handle);
  }
  static inline ScriptArrayF32 create(PuleAllocator allocator) {
    return { ._handle = puleScriptArrayF32Create(allocator),};
  }
};
}
  inline void destroy(pule::ScriptArrayF32 self) {
    return puleScriptArrayF32Destroy(self._handle);
  }
  inline void * ptr(pule::ScriptArrayF32 self) {
    return puleScriptArrayF32Ptr(self._handle);
  }
  inline void append(pule::ScriptArrayF32 self, float value) {
    return puleScriptArrayF32Append(self._handle, value);
  }
  inline void remove(pule::ScriptArrayF32 self, size_t index) {
    return puleScriptArrayF32Remove(self._handle, index);
  }
  inline float at(pule::ScriptArrayF32 self, size_t index) {
    return puleScriptArrayF32At(self._handle, index);
  }
  inline size_t length(pule::ScriptArrayF32 self) {
    return puleScriptArrayF32Length(self._handle);
  }
  inline size_t elementByteSize(pule::ScriptArrayF32 self) {
    return puleScriptArrayF32ElementByteSize(self._handle);
  }
namespace pule {
using ScriptModuleFromSourceCreateInfo = PuleScriptModuleFromSourceCreateInfo;
}
namespace pule {
using ScriptModuleFileWatchReturn = PuleScriptModuleFileWatchReturn;
}
namespace pule {
inline char const * toStr(PuleErrorScript const e) {
  switch (e) {
    case PuleErrorScript_none: return "none";
    case PuleErrorScript_executeRuntimeError: return "executeRuntimeError";
    case PuleErrorScript_internalError: return "internalError";
    case PuleErrorScript_memoryError: return "memoryError";
    case PuleErrorScript_syntaxError: return "syntaxError";
    default: return "N/A";
  }
}
inline char const * toStr(PuleScriptDebugSymbols const e) {
  switch (e) {
    case PuleScriptDebugSymbols_enable: return "enable";
    case PuleScriptDebugSymbols_disable: return "disable";
    default: return "N/A";
  }
}
}
