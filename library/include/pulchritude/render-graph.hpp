/* auto generated file render-graph */
#pragma once
#include "core.hpp"

#include "render-graph.h"
#include "data-serializer.hpp"
#include "error.hpp"
#include "gpu.hpp"
#include "platform.hpp"
#include "render-graph.hpp"
#include "allocator.hpp"

namespace pule {
struct RenderGraph {
  PuleRenderGraph _handle;
  inline operator PuleRenderGraph() const {
    return _handle;
  }
  inline void destroy() {
    return puleRenderGraphDestroy(this->_handle);
  }
  inline void merge(PuleRenderGraph renderGraphSecondary) {
    return puleRenderGraphMerge(this->_handle, renderGraphSecondary);
  }
  inline PuleRenderGraphNode nodeCreate(PuleStringView label) {
    return puleRenderGraphNodeCreate(this->_handle, label);
  }
  inline PuleRenderGraphNode nodeFetch(PuleStringView label) {
    return puleRenderGraphNodeFetch(this->_handle, label);
  }
  inline void _resourceCreate(PuleRenderGraph_Resource resource) {
    return puleRenderGraph_resourceCreate(this->_handle, resource);
  }
  inline PuleRenderGraph_Resource _resource(PuleStringView resourceLabel) {
    return puleRenderGraph_resource(this->_handle, resourceLabel);
  }
  inline void _resourceRemove(PuleStringView resourceLabel) {
    return puleRenderGraph_resourceRemove(this->_handle, resourceLabel);
  }
  inline void frameStart() {
    return puleRenderGraphFrameStart(this->_handle);
  }
  static inline RenderGraph create(PuleStringView label, PuleAllocator allocator) {
    return { ._handle = puleRenderGraphCreate(label, allocator),};
  }
};
}
  inline void destroy(pule::RenderGraph self) {
    return puleRenderGraphDestroy(self._handle);
  }
  inline void merge(pule::RenderGraph self, PuleRenderGraph renderGraphSecondary) {
    return puleRenderGraphMerge(self._handle, renderGraphSecondary);
  }
  inline PuleRenderGraphNode nodeCreate(pule::RenderGraph self, PuleStringView label) {
    return puleRenderGraphNodeCreate(self._handle, label);
  }
  inline PuleRenderGraphNode nodeFetch(pule::RenderGraph self, PuleStringView label) {
    return puleRenderGraphNodeFetch(self._handle, label);
  }
  inline void _resourceCreate(pule::RenderGraph self, PuleRenderGraph_Resource resource) {
    return puleRenderGraph_resourceCreate(self._handle, resource);
  }
  inline PuleRenderGraph_Resource _resource(pule::RenderGraph self, PuleStringView resourceLabel) {
    return puleRenderGraph_resource(self._handle, resourceLabel);
  }
  inline void _resourceRemove(pule::RenderGraph self, PuleStringView resourceLabel) {
    return puleRenderGraph_resourceRemove(self._handle, resourceLabel);
  }
  inline void frameStart(pule::RenderGraph self) {
    return puleRenderGraphFrameStart(self._handle);
  }
namespace pule {
struct RenderGraphNode {
  PuleRenderGraphNode _handle;
  inline operator PuleRenderGraphNode() const {
    return _handle;
  }
  inline void remove() {
    return puleRenderGraphNodeRemove(this->_handle);
  }
  inline PuleStringView label() {
    return puleRenderGraphNodeLabel(this->_handle);
  }
  inline void _renderPassSet(PuleRenderGraphNode_RenderPass renderPass) {
    return puleRenderGraphNode_renderPassSet(this->_handle, renderPass);
  }
  inline void _renderPassBegin(PuleGpuCommandListRecorder recorder) {
    return puleRenderGraphNode_renderPassBegin(this->_handle, recorder);
  }
  inline void _renderPassEnd(PuleGpuCommandListRecorder recorder) {
    return puleRenderGraphNode_renderPassEnd(this->_handle, recorder);
  }
  inline void relationSet(PuleRenderGraphNodeRelation relation, PuleRenderGraphNode nodeSec) {
    return puleRenderGraphNodeRelationSet(this->_handle, relation, nodeSec);
  }
  inline bool exists() {
    return puleRenderGraphNodeExists(this->_handle);
  }
  static inline RenderGraphNode create(PuleRenderGraph graph, PuleStringView label) {
    return { ._handle = puleRenderGraphNodeCreate(graph, label),};
  }
  static inline RenderGraphNode fetch(PuleRenderGraph graph, PuleStringView label) {
    return { ._handle = puleRenderGraphNodeFetch(graph, label),};
  }
};
}
  inline void remove(pule::RenderGraphNode self) {
    return puleRenderGraphNodeRemove(self._handle);
  }
  inline PuleStringView label(pule::RenderGraphNode self) {
    return puleRenderGraphNodeLabel(self._handle);
  }
  inline void _renderPassSet(pule::RenderGraphNode self, PuleRenderGraphNode_RenderPass renderPass) {
    return puleRenderGraphNode_renderPassSet(self._handle, renderPass);
  }
  inline void _renderPassBegin(pule::RenderGraphNode self, PuleGpuCommandListRecorder recorder) {
    return puleRenderGraphNode_renderPassBegin(self._handle, recorder);
  }
  inline void _renderPassEnd(pule::RenderGraphNode self, PuleGpuCommandListRecorder recorder) {
    return puleRenderGraphNode_renderPassEnd(self._handle, recorder);
  }
  inline void relationSet(pule::RenderGraphNode self, PuleRenderGraphNodeRelation relation, PuleRenderGraphNode nodeSec) {
    return puleRenderGraphNodeRelationSet(self._handle, relation, nodeSec);
  }
  inline bool exists(pule::RenderGraphNode self) {
    return puleRenderGraphNodeExists(self._handle);
  }
namespace pule {
using RenderGraph_Resource_DimensionsScaleRelative = PuleRenderGraph_Resource_DimensionsScaleRelative;
}
namespace pule {
using RenderGraph_Resource_DimensionsAbsolute = PuleRenderGraph_Resource_DimensionsAbsolute;
}
namespace pule {
using RenderGraph_Resource_Dimensions = PuleRenderGraph_Resource_Dimensions;
}
namespace pule {
using RenderGraph_Resource_Image_DataManagement_Automatic = PuleRenderGraph_Resource_Image_DataManagement_Automatic;
}
namespace pule {
using RenderGraph_Resource_Image_DataManagement_Manual = PuleRenderGraph_Resource_Image_DataManagement_Manual;
}
namespace pule {
using RenderGraph_Resource_Image_DataManagement = PuleRenderGraph_Resource_Image_DataManagement;
}
namespace pule {
using RenderGraph_Resource_Image = PuleRenderGraph_Resource_Image;
}
namespace pule {
using RenderGraph_Resource_Buffer = PuleRenderGraph_Resource_Buffer;
}
namespace pule {
using RenderGraph_ResourceUnion = PuleRenderGraph_ResourceUnion;
}
namespace pule {
using RenderGraph_Resource = PuleRenderGraph_Resource;
}
namespace pule {
using RenderGraph_Node_Image = PuleRenderGraph_Node_Image;
}
namespace pule {
using RenderGraph_Node_Buffer = PuleRenderGraph_Node_Buffer;
}
namespace pule {
using RenderGraph_Node_ResourceUnion = PuleRenderGraph_Node_ResourceUnion;
}
namespace pule {
using RenderGraph_Node_Resource = PuleRenderGraph_Node_Resource;
}
namespace pule {
using RenderGraphNode_RenderPassAttachment = PuleRenderGraphNode_RenderPassAttachment;
}
namespace pule {
using RenderGraphNode_RenderPass = PuleRenderGraphNode_RenderPass;
}
namespace pule {
using RenderGraphExecuteInfo = PuleRenderGraphExecuteInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleRenderGraph_ResourceType const e) {
  switch (e) {
    case PuleRenderGraph_ResourceType_image: return puleString("image");
    case PuleRenderGraph_ResourceType_buffer: return puleString("buffer");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleRenderGraph_ResourceUsage const e) {
  switch (e) {
    case PuleRenderGraph_ResourceUsage_read: return puleString("read");
    case PuleRenderGraph_ResourceUsage_write: return puleString("write");
    case PuleRenderGraph_ResourceUsage_readWrite: return puleString("readWrite");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleRenderGraphNodeRelation const e) {
  switch (e) {
    case PuleRenderGraphNodeRelation_dependsOn: return puleString("dependsOn");
    default: return puleString("N/A");
  }
}
}
