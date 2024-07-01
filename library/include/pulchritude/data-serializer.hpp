/* auto generated file data-serializer */
#pragma once
#include "core.hpp"

#include "data-serializer.h"
#include "allocator.hpp"
#include "error.hpp"
#include "file.hpp"
#include "math.hpp"
#include "string.hpp"

namespace pule {
struct DsValue {
  PuleDsValue _handle;
  inline operator PuleDsValue() const {
    return _handle;
  }
  inline PuleDsValue cloneRecursively(PuleAllocator allocator) {
    return puleDsValueCloneRecursively(this->_handle, allocator);
  }
};
}
  inline PuleDsValue cloneRecursively(pule::DsValue self, PuleAllocator allocator) {
    return puleDsValueCloneRecursively(self._handle, allocator);
  }
namespace pule {
using DsValueArray = PuleDsValueArray;
}
namespace pule {
using DsValueObject = PuleDsValueObject;
}
namespace pule {
using DsValueBuffer = PuleDsValueBuffer;
}
namespace pule {
using DsStructField = PuleDsStructField;
}
namespace pule {
using _DS_TODODOMP = PULE_DS_TODODOMP;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorDataSerializer const e) {
  switch (e) {
    case PuleErrorDataSerializer_none: return puleString("none");
    case PuleErrorDataSerializer_invalidFormat: return puleString("invalidFormat");
    default: return puleString("N/A");
  }
}
}
