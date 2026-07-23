/* auto generated file pecs */
#pragma once
#include "core.hpp"

#include "pecs.h"
#include "data-serializer.hpp"
#include "math.hpp"
#include "time.hpp"
#include "asset-sprite.hpp"

namespace pule {
struct PecsScene {
  PulePecsScene _handle;
  inline operator PulePecsScene() const {
    return _handle;
  }
  inline void destroy() {
    return pulePecsSceneDestroy(this->_handle);
  }
  inline void advance(PuleMicrosecond deltaTime) {
    return pulePecsSceneAdvance(this->_handle, deltaTime);
  }
  static inline PecsScene create() {
    return { ._handle = pulePecsSceneCreate(),};
  }
};
}
  inline void destroy(pule::PecsScene self) {
    return pulePecsSceneDestroy(self._handle);
  }
  inline void advance(pule::PecsScene self, PuleMicrosecond deltaTime) {
    return pulePecsSceneAdvance(self._handle, deltaTime);
  }
namespace pule {
struct PecsEntity {
  PulePecsEntity _handle;
  inline operator PulePecsEntity() const {
    return _handle;
  }
  static inline PecsEntity create(PulePecsEntityCreateInfo createInfo) {
    return { ._handle = pulePecsEntityCreate(createInfo),};
  }
};
}
namespace pule {
struct PecsComponent {
  PulePecsComponent _handle;
  inline operator PulePecsComponent() const {
    return _handle;
  }
  static inline PecsComponent create(PulePecsScene scene, PulePecsComponentCreateInfo createInfo) {
    return { ._handle = pulePecsComponentCreate(scene, createInfo),};
  }
  static inline PecsComponent fetch(PulePecsScene scene, PuleStringView label) {
    return { ._handle = pulePecsComponentFetch(scene, label),};
  }
  static inline PecsComponent engine_Transform2D(PulePecsScene scene) {
    return { ._handle = pulePecsComponentEngine_Transform2D(scene),};
  }
  static inline PecsComponent engine_Camera(PulePecsScene scene) {
    return { ._handle = pulePecsComponentEngine_Camera(scene),};
  }
  static inline PecsComponent engine_2dSprite(PulePecsScene scene) {
    return { ._handle = pulePecsComponentEngine_2dSprite(scene),};
  }
};
}
namespace pule {
struct PecsSystem {
  PulePecsSystem _handle;
  inline operator PulePecsSystem() const {
    return _handle;
  }
  static inline PecsSystem create(PulePecsSystemCreateInfo createInfo) {
    return { ._handle = pulePecsSystemCreate(createInfo),};
  }
};
}
namespace pule {
struct PecsQueryEntity {
  PulePecsQueryEntity _handle;
  inline operator PulePecsQueryEntity() const {
    return _handle;
  }
  static inline PecsQueryEntity create(PulePecsQueryEntityCreateInfo createInfo) {
    return { ._handle = pulePecsQueryEntityCreate(createInfo),};
  }
};
}
namespace pule {
struct PecsQueryEntityIterator {
  PulePecsQueryEntityIterator _handle;
  inline operator PulePecsQueryEntityIterator() const {
    return _handle;
  }
  static inline PecsQueryEntityIterator create(PulePecsScene scene, PulePecsQueryEntity query) {
    return { ._handle = pulePecsQueryEntityIteratorCreate(scene, query),};
  }
};
}
namespace pule {
struct PecsEntityTemplate {
  PulePecsEntityTemplate _handle;
  inline operator PulePecsEntityTemplate() const {
    return _handle;
  }
  static inline PecsEntityTemplate create(PulePecsEntityTemplateCreateInfo createInfo) {
    return { ._handle = pulePecsEntityTemplateCreate(createInfo),};
  }
};
}
namespace pule {
using PecsComponentCreateInfo = PulePecsComponentCreateInfo;
}
namespace pule {
using PecsSystemCreateInfo = PulePecsSystemCreateInfo;
}
namespace pule {
using PecsQueryEntityCreateInfo = PulePecsQueryEntityCreateInfo;
}
namespace pule {
using PecsEntityTemplateCreateInfo = PulePecsEntityTemplateCreateInfo;
}
namespace pule {
using PecsEntityCreateInfo = PulePecsEntityCreateInfo;
}
namespace pule {
using PecsComponent_Transform2D = PulePecsComponent_Transform2D;
}
namespace pule {
using PecsComponent_Camera = PulePecsComponent_Camera;
}
namespace pule {
using PecsComponent_2dSprite = PulePecsComponent_2dSprite;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PulePecsSystemCallbackFrequency const e) {
  switch (e) {
    case PulePecsSystemCallbackFrequency_vsync: return puleString("vsync");
    case PulePecsSystemCallbackFrequency_unlimited: return puleString("unlimited");
    case PulePecsSystemCallbackFrequency_fixed: return puleString("fixed");
    default: return puleString("N/A");
  }
}
}
