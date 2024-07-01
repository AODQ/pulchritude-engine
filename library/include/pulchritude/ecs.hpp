/* auto generated file ecs */
#pragma once
#include "core.hpp"

#include "ecs.h"
#include "data-serializer.hpp"
#include "error.hpp"
#include "math.hpp"

namespace pule {
struct EcsWorld {
  PuleEcsWorld _handle;
  inline operator PuleEcsWorld() const {
    return _handle;
  }
  inline void destroy() {
    return puleEcsWorldDestroy(this->_handle);
  }
  inline void advance(float msDelta) {
    return puleEcsWorldAdvance(this->_handle, msDelta);
  }
  static inline EcsWorld create() {
    return { ._handle = puleEcsWorldCreate(),};
  }
};
}
  inline void destroy(pule::EcsWorld self) {
    return puleEcsWorldDestroy(self._handle);
  }
  inline void advance(pule::EcsWorld self, float msDelta) {
    return puleEcsWorldAdvance(self._handle, msDelta);
  }
namespace pule {
struct EcsComponent {
  PuleEcsComponent _handle;
  inline operator PuleEcsComponent() const {
    return _handle;
  }
  static inline EcsComponent create(PuleEcsWorld world, PuleEcsComponentCreateInfo info) {
    return { ._handle = puleEcsComponentCreate(world, info),};
  }
  static inline EcsComponent fetchByLabel(PuleEcsWorld world, PuleStringView label) {
    return { ._handle = puleEcsComponentFetchByLabel(world, label),};
  }
  static inline EcsComponent originF32v3() {
    return { ._handle = puleEcsComponentOriginF32v3(),};
  }
};
}
namespace pule {
struct EcsSystem {
  PuleEcsSystem _handle;
  inline operator PuleEcsSystem() const {
    return _handle;
  }
  static inline EcsSystem create(PuleEcsWorld world, PuleEcsSystemCreateInfo info) {
    return { ._handle = puleEcsSystemCreate(world, info),};
  }
};
}
namespace pule {
struct EcsEntity {
  PuleEcsEntity _handle;
  inline operator PuleEcsEntity() const {
    return _handle;
  }
  static inline EcsEntity create(PuleEcsWorld world, PuleStringView label) {
    return { ._handle = puleEcsEntityCreate(world, label),};
  }
};
}
namespace pule {
struct EcsIterator {
  PuleEcsIterator _handle;
  inline operator PuleEcsIterator() const {
    return _handle;
  }
  inline size_t entityCount() {
    return puleEcsIteratorEntityCount(this->_handle);
  }
  inline size_t relativeOffset() {
    return puleEcsIteratorRelativeOffset(this->_handle);
  }
  inline void * queryComponents(size_t componentIndex, size_t componentByteLength) {
    return puleEcsIteratorQueryComponents(this->_handle, componentIndex, componentByteLength);
  }
  inline PuleEcsEntity * queryEntities() {
    return puleEcsIteratorQueryEntities(this->_handle);
  }
  inline PuleEcsWorld world() {
    return puleEcsIteratorWorld(this->_handle);
  }
  inline PuleEcsSystem system() {
    return puleEcsIteratorSystem(this->_handle);
  }
  inline void * userData() {
    return puleEcsIteratorUserData(this->_handle);
  }
};
}
  inline size_t entityCount(pule::EcsIterator self) {
    return puleEcsIteratorEntityCount(self._handle);
  }
  inline size_t relativeOffset(pule::EcsIterator self) {
    return puleEcsIteratorRelativeOffset(self._handle);
  }
  inline void * queryComponents(pule::EcsIterator self, size_t componentIndex, size_t componentByteLength) {
    return puleEcsIteratorQueryComponents(self._handle, componentIndex, componentByteLength);
  }
  inline PuleEcsEntity * queryEntities(pule::EcsIterator self) {
    return puleEcsIteratorQueryEntities(self._handle);
  }
  inline PuleEcsWorld world(pule::EcsIterator self) {
    return puleEcsIteratorWorld(self._handle);
  }
  inline PuleEcsSystem system(pule::EcsIterator self) {
    return puleEcsIteratorSystem(self._handle);
  }
  inline void * userData(pule::EcsIterator self) {
    return puleEcsIteratorUserData(self._handle);
  }
namespace pule {
using EcsComponentCreateInfo = PuleEcsComponentCreateInfo;
}
namespace pule {
using EcsEntityIterateComponentsInfo = PuleEcsEntityIterateComponentsInfo;
}
namespace pule {
using EcsComponentSerializer = PuleEcsComponentSerializer;
}
namespace pule {
using EcsComponentInfo = PuleEcsComponentInfo;
}
namespace pule {
using EcsComponentIterateAllCallback = PuleEcsComponentIterateAllCallback;
}
namespace pule {
using EcsSystemComponentDesc = PuleEcsSystemComponentDesc;
}
namespace pule {
using EcsSystemCreateInfo = PuleEcsSystemCreateInfo;
}
namespace pule {
struct EcsQuery {
  PuleEcsQuery _handle;
  inline operator PuleEcsQuery() const {
    return _handle;
  }
  inline void destroy() {
    return puleEcsQueryDestroy(this->_handle);
  }
  static inline EcsQuery byComponent(PuleEcsWorld world, PuleEcsComponent * componentList, size_t componentListSize, PuleError * error) {
    return { ._handle = puleEcsQueryByComponent(world, componentList, componentListSize, error),};
  }
  static inline EcsQuery allEntities(PuleEcsWorld world, PuleError * error) {
    return { ._handle = puleEcsQueryAllEntities(world, error),};
  }
};
}
  inline void destroy(pule::EcsQuery self) {
    return puleEcsQueryDestroy(self._handle);
  }
namespace pule {
struct EcsQueryIterator {
  PuleEcsQueryIterator _handle;
  inline operator PuleEcsQueryIterator() const {
    return _handle;
  }
  inline PuleEcsIterator next() {
    return puleEcsQueryIteratorNext(this->_handle);
  }
  inline void destroy() {
    return puleEcsQueryIteratorDestroy(this->_handle);
  }
};
}
  inline PuleEcsIterator next(pule::EcsQueryIterator self) {
    return puleEcsQueryIteratorNext(self._handle);
  }
  inline void destroy(pule::EcsQueryIterator self) {
    return puleEcsQueryIteratorDestroy(self._handle);
  }
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorEcs const e) {
  switch (e) {
    case PuleErrorEcs_none: return puleString("none");
    case PuleErrorEcs_queryFailed: return puleString("queryFailed");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleEcsSystemCallbackFrequency const e) {
  switch (e) {
    case PuleEcsSystemCallbackFrequency_preUpdate: return puleString("preUpdate");
    case PuleEcsSystemCallbackFrequency_onUpdate: return puleString("onUpdate");
    case PuleEcsSystemCallbackFrequency_postUpdate: return puleString("postUpdate");
    default: return puleString("N/A");
  }
}
}
