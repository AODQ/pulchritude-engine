/* auto generated file plugin */
#pragma once
#include "core.h"

#include "plugin.h"
#include "string.hpp"

namespace pule {
struct PluginPayload {
  PulePluginPayload _handle;
  inline operator PulePluginPayload() const {
    return _handle;
  }
  inline void destroy() {
    return pulePluginPayloadDestroy(this->_handle);
  }
  inline void * fetch(PuleStringView handle) {
    return pulePluginPayloadFetch(this->_handle, handle);
  }
  inline uint64_t fetchU64(PuleStringView handle) {
    return pulePluginPayloadFetchU64(this->_handle, handle);
  }
  inline void store(PuleStringView handle, void * data) {
    return pulePluginPayloadStore(this->_handle, handle, data);
  }
  inline void storeU64(PuleStringView handle, uint64_t data) {
    return pulePluginPayloadStoreU64(this->_handle, handle, data);
  }
  inline void remove(PuleStringView handle) {
    return pulePluginPayloadRemove(this->_handle, handle);
  }
  static inline PluginPayload create(PuleAllocator allocator) {
    return { ._handle = pulePluginPayloadCreate(allocator),};
  }
};
}
  inline void destroy(pule::PluginPayload self) {
    return pulePluginPayloadDestroy(self._handle);
  }
  inline void * fetch(pule::PluginPayload self, PuleStringView handle) {
    return pulePluginPayloadFetch(self._handle, handle);
  }
  inline uint64_t fetchU64(pule::PluginPayload self, PuleStringView handle) {
    return pulePluginPayloadFetchU64(self._handle, handle);
  }
  inline void store(pule::PluginPayload self, PuleStringView handle, void * data) {
    return pulePluginPayloadStore(self._handle, handle, data);
  }
  inline void storeU64(pule::PluginPayload self, PuleStringView handle, uint64_t data) {
    return pulePluginPayloadStoreU64(self._handle, handle, data);
  }
  inline void remove(pule::PluginPayload self, PuleStringView handle) {
    return pulePluginPayloadRemove(self._handle, handle);
  }
namespace pule {
using PluginInfo = PulePluginInfo;
}
namespace pule {
inline char const * toStr(PulePluginType const e) {
  switch (e) {
    case PulePluginType_library: return "library";
    case PulePluginType_component: return "component";
    default: return "N/A";
  }
}
}
