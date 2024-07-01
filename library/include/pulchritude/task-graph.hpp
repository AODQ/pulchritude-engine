/* auto generated file task-graph */
#pragma once
#include "core.hpp"

#include "task-graph.h"
#include "string.hpp"

namespace pule {
struct TaskGraph {
  PuleTaskGraph _handle;
  inline operator PuleTaskGraph() const {
    return _handle;
  }
  inline void destroy() {
    return puleTaskGraphDestroy(this->_handle);
  }
  inline void merge(PuleTaskGraph taskGraphSecondary) {
    return puleTaskGraphMerge(this->_handle, taskGraphSecondary);
  }
  inline PuleTaskGraphNode nodeCreate(PuleStringView label) {
    return puleTaskGraphNodeCreate(this->_handle, label);
  }
  inline PuleTaskGraphNode nodeFetch(PuleStringView label) {
    return puleTaskGraphNodeFetch(this->_handle, label);
  }
  static inline TaskGraph create(PuleAllocator allocator) {
    return { ._handle = puleTaskGraphCreate(allocator),};
  }
};
}
  inline void destroy(pule::TaskGraph self) {
    return puleTaskGraphDestroy(self._handle);
  }
  inline void merge(pule::TaskGraph self, PuleTaskGraph taskGraphSecondary) {
    return puleTaskGraphMerge(self._handle, taskGraphSecondary);
  }
  inline PuleTaskGraphNode nodeCreate(pule::TaskGraph self, PuleStringView label) {
    return puleTaskGraphNodeCreate(self._handle, label);
  }
  inline PuleTaskGraphNode nodeFetch(pule::TaskGraph self, PuleStringView label) {
    return puleTaskGraphNodeFetch(self._handle, label);
  }
namespace pule {
struct TaskGraphNode {
  PuleTaskGraphNode _handle;
  inline operator PuleTaskGraphNode() const {
    return _handle;
  }
  inline void remove() {
    return puleTaskGraphNodeRemove(this->_handle);
  }
  inline PuleStringView label() {
    return puleTaskGraphNodeLabel(this->_handle);
  }
  inline void attributeStore(PuleStringView label, void * attributeData) {
    return puleTaskGraphNodeAttributeStore(this->_handle, label, attributeData);
  }
  inline void attributeStoreU64(PuleStringView label, uint64_t attributeData) {
    return puleTaskGraphNodeAttributeStoreU64(this->_handle, label, attributeData);
  }
  inline void * attributeFetch(PuleStringView label) {
    return puleTaskGraphNodeAttributeFetch(this->_handle, label);
  }
  inline uint64_t attributeFetchU64(PuleStringView label) {
    return puleTaskGraphNodeAttributeFetchU64(this->_handle, label);
  }
  inline void attributeRemove(PuleStringView label) {
    return puleTaskGraphNodeAttributeRemove(this->_handle, label);
  }
  inline void relationSet(PuleTaskGraphNodeRelation relation, PuleTaskGraphNode nodeSec) {
    return puleTaskGraphNodeRelationSet(this->_handle, relation, nodeSec);
  }
  inline bool exists() {
    return puleTaskGraphNodeExists(this->_handle);
  }
  static inline TaskGraphNode create(PuleTaskGraph graph, PuleStringView label) {
    return { ._handle = puleTaskGraphNodeCreate(graph, label),};
  }
  static inline TaskGraphNode fetch(PuleTaskGraph graph, PuleStringView label) {
    return { ._handle = puleTaskGraphNodeFetch(graph, label),};
  }
};
}
  inline void remove(pule::TaskGraphNode self) {
    return puleTaskGraphNodeRemove(self._handle);
  }
  inline PuleStringView label(pule::TaskGraphNode self) {
    return puleTaskGraphNodeLabel(self._handle);
  }
  inline void attributeStore(pule::TaskGraphNode self, PuleStringView label, void * attributeData) {
    return puleTaskGraphNodeAttributeStore(self._handle, label, attributeData);
  }
  inline void attributeStoreU64(pule::TaskGraphNode self, PuleStringView label, uint64_t attributeData) {
    return puleTaskGraphNodeAttributeStoreU64(self._handle, label, attributeData);
  }
  inline void * attributeFetch(pule::TaskGraphNode self, PuleStringView label) {
    return puleTaskGraphNodeAttributeFetch(self._handle, label);
  }
  inline uint64_t attributeFetchU64(pule::TaskGraphNode self, PuleStringView label) {
    return puleTaskGraphNodeAttributeFetchU64(self._handle, label);
  }
  inline void attributeRemove(pule::TaskGraphNode self, PuleStringView label) {
    return puleTaskGraphNodeAttributeRemove(self._handle, label);
  }
  inline void relationSet(pule::TaskGraphNode self, PuleTaskGraphNodeRelation relation, PuleTaskGraphNode nodeSec) {
    return puleTaskGraphNodeRelationSet(self._handle, relation, nodeSec);
  }
  inline bool exists(pule::TaskGraphNode self) {
    return puleTaskGraphNodeExists(self._handle);
  }
namespace pule {
using TaskGraphExecuteInfo = PuleTaskGraphExecuteInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleTaskGraphNodeRelation const e) {
  switch (e) {
    case PuleTaskGraphNodeRelation_dependsOn: return puleString("dependsOn");
    default: return puleString("N/A");
  }
}
}
