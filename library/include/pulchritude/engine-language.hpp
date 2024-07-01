/* auto generated file engine-language */
#pragma once
#include "core.hpp"

#include "engine-language.h"
#include "error.hpp"
#include "stream.hpp"

namespace pule {
struct ELModule {
  PuleELModule _handle;
  inline operator PuleELModule() const {
    return _handle;
  }
  inline void destroy() {
    return puleELModuleDestroy(this->_handle);
  }
  static inline ELModule create(PuleStreamRead stream, PuleStringView name, PuleError * error) {
    return { ._handle = puleELModuleCreate(stream, name, error),};
  }
};
}
  inline void destroy(pule::ELModule self) {
    return puleELModuleDestroy(self._handle);
  }
namespace pule {
struct ELJitEngine {
  PuleELJitEngine _handle;
  inline operator PuleELJitEngine() const {
    return _handle;
  }
  inline void destroy() {
    return puleELJitEngineDestroy(this->_handle);
  }
  inline void addModule(PuleELModule module) {
    return puleELJitEngineAddModule(this->_handle, module);
  }
  inline void * functionAddress(PuleStringView functionName) {
    return puleELJitEngineFunctionAddress(this->_handle, functionName);
  }
  static inline ELJitEngine create(PuleELJitEngineCreateInfo ci) {
    return { ._handle = puleELJitEngineCreate(ci),};
  }
};
}
  inline void destroy(pule::ELJitEngine self) {
    return puleELJitEngineDestroy(self._handle);
  }
  inline void addModule(pule::ELJitEngine self, PuleELModule module) {
    return puleELJitEngineAddModule(self._handle, module);
  }
  inline void * functionAddress(pule::ELJitEngine self, PuleStringView functionName) {
    return puleELJitEngineFunctionAddress(self._handle, functionName);
  }
namespace pule {
using ELJitEngineCreateInfo = PuleELJitEngineCreateInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorEL const e) {
  switch (e) {
    case PuleErrorEL_none: return puleString("none");
    case PuleErrorEL_compileError: return puleString("compileError");
    default: return puleString("N/A");
  }
}
}
