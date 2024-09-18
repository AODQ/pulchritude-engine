/* auto generated file engine-language */
#pragma once
#include "core.hpp"

#include "engine-language.h"
#include "error.hpp"
#include "string.hpp"
#include "time.hpp"

namespace pule {
struct ELModule {
  PuleELModule _handle;
  inline operator PuleELModule() const {
    return _handle;
  }
  inline void destroy() {
    return puleELModuleDestroy(this->_handle);
  }
  static inline ELModule create(PuleELModuleCreateInfo ci) {
    return { ._handle = puleELModuleCreate(ci),};
  }
};
}
  inline void destroy(pule::ELModule self) {
    return puleELModuleDestroy(self._handle);
  }
namespace pule {
struct ELEngine {
  PuleELEngine _handle;
  inline operator PuleELEngine() const {
    return _handle;
  }
  inline void destroy() {
    return puleELEngineDestroy(this->_handle);
  }
  inline void addModule(PuleELModule module) {
    return puleELEngineAddModule(this->_handle, module);
  }
  static inline ELEngine create(PuleELEngineCreateInfo ci) {
    return { ._handle = puleELEngineCreate(ci),};
  }
};
}
  inline void destroy(pule::ELEngine self) {
    return puleELEngineDestroy(self._handle);
  }
  inline void addModule(pule::ELEngine self, PuleELModule module) {
    return puleELEngineAddModule(self._handle, module);
  }
namespace pule {
struct ELFence {
  PuleELFence _handle;
  inline operator PuleELFence() const {
    return _handle;
  }
  inline bool wait(PuleMicrosecond timeout) {
    return puleELFenceWait(this->_handle, timeout);
  }
  inline void destroy() {
    return puleELFenceDestroy(this->_handle);
  }
};
}
  inline bool wait(pule::ELFence self, PuleMicrosecond timeout) {
    return puleELFenceWait(self._handle, timeout);
  }
  inline void destroy(pule::ELFence self) {
    return puleELFenceDestroy(self._handle);
  }
namespace pule {
struct ELQueue {
  PuleELQueue _handle;
  inline operator PuleELQueue() const {
    return _handle;
  }
  inline void destroy() {
    return puleELQueueDestroy(this->_handle);
  }
  inline size_t stackLength() {
    return puleELQueueStackLength(this->_handle);
  }
  inline void stackPush(uint64_t value) {
    return puleELQueueStackPush(this->_handle, value);
  }
  inline uint64_t stackPop() {
    return puleELQueueStackPop(this->_handle);
  }
  inline void submit(PuleStringView functionName) {
    return puleELQueueSubmit(this->_handle, functionName);
  }
  static inline ELQueue create(PuleELEngine engine) {
    return { ._handle = puleELQueueCreate(engine),};
  }
};
}
  inline void destroy(pule::ELQueue self) {
    return puleELQueueDestroy(self._handle);
  }
  inline size_t stackLength(pule::ELQueue self) {
    return puleELQueueStackLength(self._handle);
  }
  inline void stackPush(pule::ELQueue self, uint64_t value) {
    return puleELQueueStackPush(self._handle, value);
  }
  inline uint64_t stackPop(pule::ELQueue self) {
    return puleELQueueStackPop(self._handle);
  }
  inline void submit(pule::ELQueue self, PuleStringView functionName) {
    return puleELQueueSubmit(self._handle, functionName);
  }
namespace pule {
using ELModuleCreateInfo = PuleELModuleCreateInfo;
}
namespace pule {
using ELEngineCreateInfo = PuleELEngineCreateInfo;
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
