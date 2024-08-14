/* auto generated file gpu-ir */
#pragma once
#include "core.hpp"

#include "gpu-ir.h"
#include "allocator.hpp"
#include "array.hpp"
#include "error.hpp"
#include "gpu.hpp"
#include "math.hpp"

namespace pule {
using GpuIr_Pipeline = PuleGpuIr_Pipeline;
}
namespace pule {
using GpuIr_Type = PuleGpuIr_Type;
}
namespace pule {
using GpuIr_Shader = PuleGpuIr_Shader;
}
namespace pule {
using GpuIr_Value = PuleGpuIr_Value;
}
namespace pule {
using GpuIr_UniformBuffer = PuleGpuIr_UniformBuffer;
}
namespace pule {
using GpuIr_StorageBuffer = PuleGpuIr_StorageBuffer;
}
namespace pule {
using puIr_Image = PulGpuIr_Image;
}
namespace pule {
using GpuIr_Texture = PuleGpuIr_Texture;
}
namespace pule {
using GpuIr_Sampler = PuleGpuIr_Sampler;
}
namespace pule {
using GpuIr_InputAttachment = PuleGpuIr_InputAttachment;
}
namespace pule {
using GpuIr_PushConstant = PuleGpuIr_PushConstant;
}
namespace pule {
using GpuIr_InputAttribute = PuleGpuIr_InputAttribute;
}
namespace pule {
using GpuIr_StageInput = PuleGpuIr_StageInput;
}
namespace pule {
using GpuIr_StageOutput = PuleGpuIr_StageOutput;
}
namespace pule {
using GpuIr_Parameter = PuleGpuIr_Parameter;
}
namespace pule {
using GpuIr_ShaderStageVertex = PuleGpuIr_ShaderStageVertex;
}
namespace pule {
using GpuIr_ShaderStageFragment = PuleGpuIr_ShaderStageFragment;
}
namespace pule {
using GpuIr_ShaderStage = PuleGpuIr_ShaderStage;
}
namespace pule {
using GpuIr_ShaderDescriptor = PuleGpuIr_ShaderDescriptor;
}
namespace pule {
using GpuIr_Constant = PuleGpuIr_Constant;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleGpuIr_PipelineType const e) {
  switch (e) {
    case PuleGpuIr_PipelineType_renderVertexFragment: return puleString("renderVertexFragment");
    case PuleGpuIr_PipelineType_renderMeshFragment: return puleString("renderMeshFragment");
    case PuleGpuIr_PipelineType_raytrace: return puleString("raytrace");
    case PuleGpuIr_PipelineType_compute: return puleString("compute");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_ParameterType const e) {
  switch (e) {
    case PuleGpuIr_ParameterType_uniformBuffer: return puleString("uniformBuffer");
    case PuleGpuIr_ParameterType_storageBuffer: return puleString("storageBuffer");
    case PuleGpuIr_ParameterType_image: return puleString("image");
    case PuleGpuIr_ParameterType_texture: return puleString("texture");
    case PuleGpuIr_ParameterType_sampler: return puleString("sampler");
    case PuleGpuIr_ParameterType_inputAttachment: return puleString("inputAttachment");
    case PuleGpuIr_ParameterType_pushConstant: return puleString("pushConstant");
    case PuleGpuIr_ParameterType_inputAttribute: return puleString("inputAttribute");
    case PuleGpuIr_ParameterType_stageInput: return puleString("stageInput");
    case PuleGpuIr_ParameterType_stageOutput: return puleString("stageOutput");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_ImageFormat const e) {
  switch (e) {
    case PuleGpuIr_ImageFormat_r8unorm: return puleString("r8unorm");
    case PuleGpuIr_ImageFormat_r16unorm: return puleString("r16unorm");
    case PuleGpuIr_ImageFormat_rgba8ui: return puleString("rgba8ui");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_VertexInputRate const e) {
  switch (e) {
    case PuleGpuIr_VertexInputRate_vertex: return puleString("vertex");
    case PuleGpuIr_VertexInputRate_instance: return puleString("instance");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_ShaderStageType const e) {
  switch (e) {
    case PuleGpuIr_ShaderStageType_vertex: return puleString("vertex");
    case PuleGpuIr_ShaderStageType_fragment: return puleString("fragment");
    case PuleGpuIr_ShaderStageType_mesh: return puleString("mesh");
    case PuleGpuIr_ShaderStageType_rayGen: return puleString("rayGen");
    case PuleGpuIr_ShaderStageType_rayClosestHit: return puleString("rayClosestHit");
    case PuleGpuIr_ShaderStageType_rayAnyHit: return puleString("rayAnyHit");
    case PuleGpuIr_ShaderStageType_rayMiss: return puleString("rayMiss");
    case PuleGpuIr_ShaderStageType_compute: return puleString("compute");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_VertexTopology const e) {
  switch (e) {
    case PuleGpuIr_VertexTopology_points: return puleString("points");
    case PuleGpuIr_VertexTopology_lines: return puleString("lines");
    case PuleGpuIr_VertexTopology_triangles: return puleString("triangles");
    case PuleGpuIr_VertexTopology_triangleStrip: return puleString("triangleStrip");
    case PuleGpuIr_VertexTopology_lineStrip: return puleString("lineStrip");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_StorageClass const e) {
  switch (e) {
    case PuleGpuIr_StorageClass_uniformConstant: return puleString("uniformConstant");
    case PuleGpuIr_StorageClass_input: return puleString("input");
    case PuleGpuIr_StorageClass_uniform: return puleString("uniform");
    case PuleGpuIr_StorageClass_output: return puleString("output");
    case PuleGpuIr_StorageClass_workgroup: return puleString("workgroup");
    case PuleGpuIr_StorageClass_crossWorkgroup: return puleString("crossWorkgroup");
    case PuleGpuIr_StorageClass_private: return puleString("private");
    case PuleGpuIr_StorageClass_function: return puleString("function");
    case PuleGpuIr_StorageClass_generic: return puleString("generic");
    case PuleGpuIr_StorageClass_pushConstant: return puleString("pushConstant");
    case PuleGpuIr_StorageClass_atomicCounter: return puleString("atomicCounter");
    case PuleGpuIr_StorageClass_image: return puleString("image");
    case PuleGpuIr_StorageClass_storageBuffer: return puleString("storageBuffer");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_ImageDim const e) {
  switch (e) {
    case PuleGpuIr_ImageDim_i1d: return puleString("i1d");
    case PuleGpuIr_ImageDim_i2d: return puleString("i2d");
    case PuleGpuIr_ImageDim_i3d: return puleString("i3d");
    case PuleGpuIr_ImageDim_cube: return puleString("cube");
    case PuleGpuIr_ImageDim_rect: return puleString("rect");
    case PuleGpuIr_ImageDim_buffer: return puleString("buffer");
    case PuleGpuIr_ImageDim_subpassData: return puleString("subpassData");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_ImageDepth const e) {
  switch (e) {
    case PuleGpuIr_ImageDepth_noDepth: return puleString("noDepth");
    case PuleGpuIr_ImageDepth_depth: return puleString("depth");
    case PuleGpuIr_ImageDepth_unknown: return puleString("unknown");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_ConstantType const e) {
  switch (e) {
    case PuleGpuIr_ConstantType_bool: return puleString("bool");
    case PuleGpuIr_ConstantType_int: return puleString("int");
    case PuleGpuIr_ConstantType_float: return puleString("float");
    case PuleGpuIr_ConstantType_f32v2: return puleString("f32v2");
    case PuleGpuIr_ConstantType_f32v3: return puleString("f32v3");
    case PuleGpuIr_ConstantType_f32v4: return puleString("f32v4");
    case PuleGpuIr_ConstantType_f32m44: return puleString("f32m44");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_FunctionControl const e) {
  std::string str = "( ";
  if (e & PuleGpuIr_FunctionControl_none) {
    str += "none | ";
  }
  if (e & PuleGpuIr_FunctionControl_inline) {
    str += "inline | ";
  }
  if (e & PuleGpuIr_FunctionControl_noinline) {
    str += "noinline | ";
  }
  if (e & PuleGpuIr_FunctionControl_pure) {
    str += "pure | ";
  }
  if (e & PuleGpuIr_FunctionControl_const) {
    str += "const | ";
  }
  str += ")";
  PuleString strCp = puleString(str.c_str());
  return strCp;
}
inline pule::str toStr(PuleGpuIr_Builtin const e) {
  switch (e) {
    case PuleGpuIr_Builtin_origin: return puleString("origin");
    case PuleGpuIr_Builtin_pointSize: return puleString("pointSize");
    case PuleGpuIr_Builtin_clipDistance: return puleString("clipDistance");
    case PuleGpuIr_Builtin_cullDistance: return puleString("cullDistance");
    case PuleGpuIr_Builtin_vertexId: return puleString("vertexId");
    case PuleGpuIr_Builtin_instanceId: return puleString("instanceId");
    case PuleGpuIr_Builtin_primitiveId: return puleString("primitiveId");
    case PuleGpuIr_Builtin_fragCoord: return puleString("fragCoord");
    case PuleGpuIr_Builtin_pointCoord: return puleString("pointCoord");
    case PuleGpuIr_Builtin_frontFacing: return puleString("frontFacing");
    case PuleGpuIr_Builtin_fragDepth: return puleString("fragDepth");
    case PuleGpuIr_Builtin_workDim: return puleString("workDim");
    case PuleGpuIr_Builtin_globalSize: return puleString("globalSize");
    case PuleGpuIr_Builtin_globalOffset: return puleString("globalOffset");
    case PuleGpuIr_Builtin_globalLinearId: return puleString("globalLinearId");
    case PuleGpuIr_Builtin_subgroupSize: return puleString("subgroupSize");
    case PuleGpuIr_Builtin_subgroupMaxSize: return puleString("subgroupMaxSize");
    case PuleGpuIr_Builtin_numSubgroups: return puleString("numSubgroups");
    case PuleGpuIr_Builtin_vertexIndex: return puleString("vertexIndex");
    case PuleGpuIr_Builtin_instanceIndex: return puleString("instanceIndex");
    case PuleGpuIr_Builtin_baseVertex: return puleString("baseVertex");
    case PuleGpuIr_Builtin_baseInstance: return puleString("baseInstance");
    case PuleGpuIr_Builtin_drawIndex: return puleString("drawIndex");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleGpuIr_Decoration const e) {
  switch (e) {
    case PuleGpuIr_Decoration_block: return puleString("block");
    case PuleGpuIr_Decoration_builtin: return puleString("builtin");
    case PuleGpuIr_Decoration_location: return puleString("location");
    default: return puleString("N/A");
  }
}
}
