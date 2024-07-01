/* auto generated file becs */
#pragma once
#include "core.hpp"

#include "becs.h"
#include "error.hpp"
#include "string.hpp"

namespace pule {
struct BecsSystem {
  PuleBecsSystem _handle;
  inline operator PuleBecsSystem() const {
    return _handle;
  }
  inline void destroy() {
    return puleBecsSystemDestroy(this->_handle);
  }
  inline void advance(float msDelta) {
    return puleBecsSystemAdvance(this->_handle, msDelta);
  }
  static inline BecsSystem create(PuleBecsWorld world, PuleBecsSystemCreateInfo ci) {
    return { ._handle = puleBecsSystemCreate(world, ci),};
  }
};
}
  inline void destroy(pule::BecsSystem self) {
    return puleBecsSystemDestroy(self._handle);
  }
  inline void advance(pule::BecsSystem self, float msDelta) {
    return puleBecsSystemAdvance(self._handle, msDelta);
  }
namespace pule {
struct BecsEntity {
  PuleBecsEntity _handle;
  inline operator PuleBecsEntity() const {
    return _handle;
  }
  inline void destroy() {
    return puleBecsEntityDestroy(this->_handle);
  }
  inline void * componentData(PuleBecsComponent component) {
    return puleBecsEntityComponentData(this->_handle, component);
  }
  static inline BecsEntity create(PuleBecsEntityCreateInfo createInfo) {
    return { ._handle = puleBecsEntityCreate(createInfo),};
  }
};
}
  inline void destroy(pule::BecsEntity self) {
    return puleBecsEntityDestroy(self._handle);
  }
  inline void * componentData(pule::BecsEntity self, PuleBecsComponent component) {
    return puleBecsEntityComponentData(self._handle, component);
  }
namespace pule {
struct BecsComponent {
  PuleBecsComponent _handle;
  inline operator PuleBecsComponent() const {
    return _handle;
  }
  static inline BecsComponent create(PuleBecsWorld world, PuleBecsComponentCreateInfo const * createInfo) {
    return { ._handle = puleBecsComponentCreate(world, createInfo),};
  }
};
}
namespace pule {
using BecsBundle = PuleBecsBundle;
}
namespace pule {
using BecsBundleHistory = PuleBecsBundleHistory;
}
namespace pule {
struct BecsWorld {
  PuleBecsWorld _handle;
  inline operator PuleBecsWorld() const {
    return _handle;
  }
  inline void destroy() {
    return puleBecsWorldDestroy(this->_handle);
  }
  inline void advance(float msDelta) {
    return puleBecsWorldAdvance(this->_handle, msDelta);
  }
  static inline BecsWorld create() {
    return { ._handle = puleBecsWorldCreate(),};
  }
};
}
  inline void destroy(pule::BecsWorld self) {
    return puleBecsWorldDestroy(self._handle);
  }
  inline void advance(pule::BecsWorld self, float msDelta) {
    return puleBecsWorldAdvance(self._handle, msDelta);
  }
namespace pule {
using BecsQueryEntityIterator = PuleBecsQueryEntityIterator;
}
namespace pule {
using BecsComponentCreateInfo = PuleBecsComponentCreateInfo;
}
namespace pule {
using BecsSystemCreateInfo = PuleBecsSystemCreateInfo;
}
namespace pule {
using BecsEntityQuery = PuleBecsEntityQuery;
}
namespace pule {
using BecsSystemInputRelationsCreateInfo = PuleBecsSystemInputRelationsCreateInfo;
}
namespace pule {
using BecsEntityCreateInfo = PuleBecsEntityCreateInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorBecs const e) {
  switch (e) {
    case PuleErrorBecs_none: return puleString("none");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleBecsSystemCallbackFrequency const e) {
  switch (e) {
    case PuleBecsSystemCallbackFrequency_vsync: return puleString("vsync");
    case PuleBecsSystemCallbackFrequency_unlimited: return puleString("unlimited");
    case PuleBecsSystemCallbackFrequency_fixed: return puleString("fixed");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleBecsSystemInputRelation const e) {
  switch (e) {
    case PuleBecsSystemInputRelation_current: return puleString("current");
    case PuleBecsSystemInputRelation_previous: return puleString("previous");
    default: return puleString("N/A");
  }
}
}
