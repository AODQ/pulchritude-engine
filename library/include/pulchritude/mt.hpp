/* auto generated file mt */
#pragma once
#include "core.hpp"

#include "mt.h"
#include "error.hpp"
#include "allocator.hpp"

namespace pule {
struct MtThread {
  PuleMtThread _handle;
  inline operator PuleMtThread() const {
    return _handle;
  }
  inline void * join() {
    return puleMtThreadJoin(this->_handle);
  }
  inline void detach() {
    return puleMtThreadDetach(this->_handle);
  }
  inline void destroy() {
    return puleMtThreadDestroy(this->_handle);
  }
  static inline MtThread create(PuleMtThreadCreateInfo ci) {
    return { ._handle = puleMtThreadCreate(ci),};
  }
};
}
  inline void * join(pule::MtThread self) {
    return puleMtThreadJoin(self._handle);
  }
  inline void detach(pule::MtThread self) {
    return puleMtThreadDetach(self._handle);
  }
  inline void destroy(pule::MtThread self) {
    return puleMtThreadDestroy(self._handle);
  }
namespace pule {
struct MtMutex {
  PuleMtMutex _handle;
  inline operator PuleMtMutex() const {
    return _handle;
  }
  inline void lock() {
    return puleMtMutexLock(this->_handle);
  }
  inline void unlock() {
    return puleMtMutexUnlock(this->_handle);
  }
  inline bool tryLock() {
    return puleMtMutexTryLock(this->_handle);
  }
  static inline MtMutex create() {
    return { ._handle = puleMtMutexCreate(),};
  }
};
}
  inline void lock(pule::MtMutex self) {
    return puleMtMutexLock(self._handle);
  }
  inline void unlock(pule::MtMutex self) {
    return puleMtMutexUnlock(self._handle);
  }
  inline bool tryLock(pule::MtMutex self) {
    return puleMtMutexTryLock(self._handle);
  }
namespace pule {
using MtThreadCreateInfo = PuleMtThreadCreateInfo;
}
