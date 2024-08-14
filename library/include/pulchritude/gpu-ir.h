/* auto generated file gpu-ir */
#pragma once
#include "core.h"

#include "allocator.h"
#include "array.h"
#include "error.h"
#include "gpu.h"
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleGpuIr_PipelineType_renderVertexFragment = 0,
  PuleGpuIr_PipelineType_renderMeshFragment = 1,
  PuleGpuIr_PipelineType_raytrace = 2,
  PuleGpuIr_PipelineType_compute = 3,
} PuleGpuIr_PipelineType;
const uint32_t PuleGpuIr_PipelineTypeSize = 4;
typedef enum {
  PuleGpuIr_ParameterType_uniformBuffer = 0,
  PuleGpuIr_ParameterType_storageBuffer = 1,
  PuleGpuIr_ParameterType_image = 2,
  PuleGpuIr_ParameterType_texture = 3,
  PuleGpuIr_ParameterType_sampler = 4,
  PuleGpuIr_ParameterType_inputAttachment = 5,
  PuleGpuIr_ParameterType_pushConstant = 6,
  PuleGpuIr_ParameterType_inputAttribute = 7,
  PuleGpuIr_ParameterType_stageInput = 8,
  PuleGpuIr_ParameterType_stageOutput = 9,
} PuleGpuIr_ParameterType;
const uint32_t PuleGpuIr_ParameterTypeSize = 10;
typedef enum {
  PuleGpuIr_ImageFormat_r8unorm = 0,
  PuleGpuIr_ImageFormat_r16unorm = 1,
  PuleGpuIr_ImageFormat_rgba8ui = 2,
} PuleGpuIr_ImageFormat;
const uint32_t PuleGpuIr_ImageFormatSize = 3;
typedef enum {
  PuleGpuIr_VertexInputRate_vertex = 0,
  PuleGpuIr_VertexInputRate_instance = 1,
} PuleGpuIr_VertexInputRate;
const uint32_t PuleGpuIr_VertexInputRateSize = 2;
typedef enum {
  PuleGpuIr_ShaderStageType_vertex = 0,
  PuleGpuIr_ShaderStageType_fragment = 1,
  PuleGpuIr_ShaderStageType_mesh = 2,
  PuleGpuIr_ShaderStageType_rayGen = 3,
  PuleGpuIr_ShaderStageType_rayClosestHit = 4,
  PuleGpuIr_ShaderStageType_rayAnyHit = 5,
  PuleGpuIr_ShaderStageType_rayMiss = 6,
  PuleGpuIr_ShaderStageType_compute = 7,
} PuleGpuIr_ShaderStageType;
const uint32_t PuleGpuIr_ShaderStageTypeSize = 8;
typedef enum {
  PuleGpuIr_VertexTopology_points = 0,
  PuleGpuIr_VertexTopology_lines = 1,
  PuleGpuIr_VertexTopology_triangles = 2,
  PuleGpuIr_VertexTopology_triangleStrip = 3,
  PuleGpuIr_VertexTopology_lineStrip = 4,
} PuleGpuIr_VertexTopology;
const uint32_t PuleGpuIr_VertexTopologySize = 5;
typedef enum {
  PuleGpuIr_StorageClass_uniformConstant = 0,
  PuleGpuIr_StorageClass_input = 1,
  PuleGpuIr_StorageClass_uniform = 2,
  PuleGpuIr_StorageClass_output = 3,
  PuleGpuIr_StorageClass_workgroup = 4,
  PuleGpuIr_StorageClass_crossWorkgroup = 5,
  PuleGpuIr_StorageClass_private = 6,
  PuleGpuIr_StorageClass_function = 7,
  PuleGpuIr_StorageClass_generic = 8,
  PuleGpuIr_StorageClass_pushConstant = 9,
  PuleGpuIr_StorageClass_atomicCounter = 10,
  PuleGpuIr_StorageClass_image = 11,
  PuleGpuIr_StorageClass_storageBuffer = 12,
} PuleGpuIr_StorageClass;
const uint32_t PuleGpuIr_StorageClassSize = 13;
typedef enum {
  PuleGpuIr_ImageDim_i1d = 0,
  PuleGpuIr_ImageDim_i2d = 1,
  PuleGpuIr_ImageDim_i3d = 2,
  PuleGpuIr_ImageDim_cube = 3,
  PuleGpuIr_ImageDim_rect = 4,
  PuleGpuIr_ImageDim_buffer = 5,
  PuleGpuIr_ImageDim_subpassData = 6,
} PuleGpuIr_ImageDim;
const uint32_t PuleGpuIr_ImageDimSize = 7;
typedef enum {
  PuleGpuIr_ImageDepth_noDepth = 0,
  PuleGpuIr_ImageDepth_depth = 1,
  PuleGpuIr_ImageDepth_unknown = 2,
} PuleGpuIr_ImageDepth;
const uint32_t PuleGpuIr_ImageDepthSize = 3;
typedef enum {
  PuleGpuIr_ConstantType_bool = 0,
  PuleGpuIr_ConstantType_int = 1,
  PuleGpuIr_ConstantType_float = 2,
  PuleGpuIr_ConstantType_f32v2 = 3,
  PuleGpuIr_ConstantType_f32v3 = 4,
  PuleGpuIr_ConstantType_f32v4 = 5,
  PuleGpuIr_ConstantType_f32m44 = 6,
} PuleGpuIr_ConstantType;
const uint32_t PuleGpuIr_ConstantTypeSize = 7;
typedef enum {
  PuleGpuIr_FunctionControl_none = 0,
  PuleGpuIr_FunctionControl_inline = 1,
  PuleGpuIr_FunctionControl_noinline = 2,
  PuleGpuIr_FunctionControl_pure = 3,
  PuleGpuIr_FunctionControl_const = 4,
} PuleGpuIr_FunctionControl;
const uint32_t PuleGpuIr_FunctionControlSize = 5;
typedef enum {
  PuleGpuIr_Builtin_origin = 0,
  PuleGpuIr_Builtin_pointSize = 1,
  PuleGpuIr_Builtin_clipDistance = 2,
  PuleGpuIr_Builtin_cullDistance = 3,
  PuleGpuIr_Builtin_vertexId = 4,
  PuleGpuIr_Builtin_instanceId = 5,
  PuleGpuIr_Builtin_primitiveId = 6,
  PuleGpuIr_Builtin_fragCoord = 7,
  PuleGpuIr_Builtin_pointCoord = 8,
  PuleGpuIr_Builtin_frontFacing = 9,
  PuleGpuIr_Builtin_fragDepth = 10,
  PuleGpuIr_Builtin_workDim = 11,
  PuleGpuIr_Builtin_globalSize = 12,
  PuleGpuIr_Builtin_globalOffset = 13,
  PuleGpuIr_Builtin_globalLinearId = 14,
  PuleGpuIr_Builtin_subgroupSize = 15,
  PuleGpuIr_Builtin_subgroupMaxSize = 16,
  PuleGpuIr_Builtin_numSubgroups = 17,
  PuleGpuIr_Builtin_vertexIndex = 18,
  PuleGpuIr_Builtin_instanceIndex = 19,
  PuleGpuIr_Builtin_baseVertex = 20,
  PuleGpuIr_Builtin_baseInstance = 21,
  PuleGpuIr_Builtin_drawIndex = 22,
} PuleGpuIr_Builtin;
const uint32_t PuleGpuIr_BuiltinSize = 23;
typedef enum {
  PuleGpuIr_Decoration_block = 0,
  PuleGpuIr_Decoration_builtin = 1,
  PuleGpuIr_Decoration_location = 2,
} PuleGpuIr_Decoration;
const uint32_t PuleGpuIr_DecorationSize = 3;

// entities
/* 

API for shader+pipeline construction

Most graphics APIs decouple shaders and pipelines to allow flexibility in the
  generated SPIR-V during compilation.

This API generates the pipeline alongside the SPIR-V as a single interface.
  The flexibility a pipeline descriptor gives isn't enough to specialize
  the spir-v in a lot of cases, such as dead-code removal if a
  vertex-attribute is unused.

Effectively this is another IR (PGPU-IR) but maps pretty closely to SPIR-V
  and will have a human-readable format at some point.
 */
typedef struct PuleGpuIr_Pipeline { uint64_t id; } PuleGpuIr_Pipeline;
typedef struct PuleGpuIr_Type { uint64_t id; } PuleGpuIr_Type;
typedef struct PuleGpuIr_Shader { uint64_t id; } PuleGpuIr_Shader;
typedef struct PuleGpuIr_Value { uint64_t id; } PuleGpuIr_Value;

// structs
struct PuleGpuIr_UniformBuffer;
struct PuleGpuIr_StorageBuffer;
struct PulGpuIr_Image;
struct PuleGpuIr_Texture;
struct PuleGpuIr_Sampler;
struct PuleGpuIr_InputAttachment;
struct PuleGpuIr_PushConstant;
struct PuleGpuIr_InputAttribute;
struct PuleGpuIr_StageInput;
struct PuleGpuIr_StageOutput;
union PuleGpuIr_Parameter;
struct PuleGpuIr_ShaderStageVertex;
struct PuleGpuIr_ShaderStageFragment;
union PuleGpuIr_ShaderStage;
struct PuleGpuIr_ShaderDescriptor;
union PuleGpuIr_Constant;

typedef struct PuleGpuIr_UniformBuffer {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleGpuIr_Type structType;
  PuleStringView label;
} PuleGpuIr_UniformBuffer;
typedef struct PuleGpuIr_StorageBuffer {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleGpuIr_Type structType;
  PuleStringView label;
} PuleGpuIr_StorageBuffer;
typedef struct PulGpuIr_Image {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleGpuIr_ImageFormat format;
  PuleStringView label;
} PulGpuIr_Image;
typedef struct PuleGpuIr_Texture {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleStringView label;
} PuleGpuIr_Texture;
typedef struct PuleGpuIr_Sampler {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleStringView label;
} PuleGpuIr_Sampler;
typedef struct PuleGpuIr_InputAttachment {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleStringView label;
} PuleGpuIr_InputAttachment;
typedef struct PuleGpuIr_PushConstant {
  PuleGpuIr_ParameterType paramType;
  PuleGpuIr_Type structType;
  PuleStringView label;
} PuleGpuIr_PushConstant;
typedef struct PuleGpuIr_InputAttribute {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  uint32_t bufferBinding;
  uint32_t byteOffset;
  /* 
    must match with other attributes at bufferBinding
   */
  uint32_t byteStride;
  /* 
    must match with other attributes at bufferBinding
   */
  PuleGpuIr_VertexInputRate inputRate;
  PuleGpuIr_Type format;
  PuleStringView label;
} PuleGpuIr_InputAttribute;
typedef struct PuleGpuIr_StageInput {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleGpuIr_Type format;
  PuleStringView label;
} PuleGpuIr_StageInput;
typedef struct PuleGpuIr_StageOutput {
  PuleGpuIr_ParameterType paramType;
  uint32_t location;
  PuleGpuIr_Type format;
  PuleStringView label;
} PuleGpuIr_StageOutput;
typedef union PuleGpuIr_Parameter {
  PuleGpuIr_ParameterType paramType;
  PuleGpuIr_UniformBuffer uniformBuffer;
  PuleGpuIr_StorageBuffer storageBuffer;
  PuleGpuIr_Texture texture;
  PuleGpuIr_Sampler sampler;
  PuleGpuIr_InputAttachment inputAttachment;
  PuleGpuIr_PushConstant pushConstant;
  PuleGpuIr_InputAttribute inputAttribute;
  PuleGpuIr_StageInput stageInput;
  PuleGpuIr_StageOutput stageOutput;
  PuleGpuIr_Type generic;
} PuleGpuIr_Parameter;
typedef struct PuleGpuIr_ShaderStageVertex {
  PuleGpuIr_ShaderStageType stageType;
  PuleGpuIr_VertexTopology topology;
} PuleGpuIr_ShaderStageVertex;
typedef struct PuleGpuIr_ShaderStageFragment {
  PuleGpuIr_ShaderStageType stageType;
} PuleGpuIr_ShaderStageFragment;
typedef union PuleGpuIr_ShaderStage {
  PuleGpuIr_ShaderStageType stageType;
  PuleGpuIr_ShaderStageVertex vertex;
  PuleGpuIr_ShaderStageFragment fragment;
} PuleGpuIr_ShaderStage;
typedef struct PuleGpuIr_ShaderDescriptor {
  PuleStringView label;
  PuleGpuIr_ShaderStage stage;
  PuleGpuIr_Parameter * params;
  size_t paramLen;
} PuleGpuIr_ShaderDescriptor;
typedef union PuleGpuIr_Constant {
  bool boolean;
  int64_t integer;
  float floating;
  PuleF32v2 f32v2;
  PuleF32v3 f32v3;
  PuleF32v4 f32v4;
  PuleF32m44 f32m44;
} PuleGpuIr_Constant;

// functions
PULE_exportFn PuleGpuIr_Pipeline puleGpuIr_pipeline(PuleStringView label, PuleGpuIr_PipelineType type);
PULE_exportFn void puleGpuIr_pipelineDestroy(PuleGpuIr_Pipeline pipeline);
PULE_exportFn PuleGpuPipeline puleGpuIr_pipelineCompile(PuleGpuIr_Pipeline pipeline);
/*  TODO this is for debug purpose & need remove  */
PULE_exportFn void puleGpuIr_compileSpirv(PuleStringView str);
PULE_exportFn PuleGpuIr_Shader puleGpuIr_pipelineAddShader(PuleGpuIr_Pipeline pipeline, PuleGpuIr_ShaderDescriptor shaderDescriptor);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opLabel(PuleGpuIr_Shader shader);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypeVoid(PuleGpuIr_Shader s);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypeFloat(PuleGpuIr_Shader s, size_t bits);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypeVector(PuleGpuIr_Shader s, PuleGpuIr_Type elementType, size_t elementSize);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypePointer(PuleGpuIr_Shader s, PuleGpuIr_Type underlyingType, PuleGpuIr_StorageClass storageClass);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypeFunction(PuleGpuIr_Shader s, PuleGpuIr_Type returnType, PuleGpuIr_Type const * paramTypes, size_t paramTypeLen);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypeStruct(PuleGpuIr_Shader s, PuleGpuIr_Type const * memberTypes, size_t memberTypeLen);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypeImage(PuleGpuIr_Shader s, PuleGpuIr_Type sampledType, PuleGpuIr_ImageDim dim, PuleGpuIr_ImageDepth depth, bool arrayed, bool multisampled);
PULE_exportFn PuleGpuIr_Type puleGpuIr_opTypeSampledImage(PuleGpuIr_Shader s, PuleGpuIr_Type imageType);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opImageSampleImplicitLod(PuleGpuIr_Shader s, PuleGpuIr_Type resultType, PuleGpuIr_Value sampledImage, PuleGpuIr_Value coordinate);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opImageSampleExplicitLod(PuleGpuIr_Shader s, PuleGpuIr_Type resultType, PuleGpuIr_Value sampledImage, PuleGpuIr_Value coordinate, PuleGpuIr_Value lod);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opVariable(PuleGpuIr_Shader shader, PuleGpuIr_Type type);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opVariableStorage(PuleGpuIr_Shader shader, PuleGpuIr_Type type, PuleGpuIr_StorageClass storageClass, size_t layoutIndex);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opLoad(PuleGpuIr_Shader shader, PuleGpuIr_Type resultType, PuleGpuIr_Value pointer);
PULE_exportFn void puleGpuIr_opStore(PuleGpuIr_Shader shader, PuleGpuIr_Value pointer, PuleGpuIr_Value value);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opConstant(PuleGpuIr_Shader shader, PuleGpuIr_Type type, PuleGpuIr_ConstantType constantType, PuleGpuIr_Constant constant);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opLogicalNot(PuleGpuIr_Shader shader, PuleGpuIr_Value value);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opLogicalAnd(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opLogicalOr(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opLogicalEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opLogicalNotEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opSelect(PuleGpuIr_Shader shader, PuleGpuIr_Value condition, PuleGpuIr_Value valueTrue, PuleGpuIr_Value valueFalse);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntNotEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntGreaterThan(PuleGpuIr_Shader shader, bool isSigned, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntGreaterThanEq(PuleGpuIr_Shader shader, bool isSigned, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntLessThan(PuleGpuIr_Shader shader, bool isSigned, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntLessThanEq(PuleGpuIr_Shader shader, bool isSigned, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatNotEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatGreaterThan(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatGreaterThanEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatLessThan(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatLessThanEq(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opTranspose(PuleGpuIr_Shader shader, PuleGpuIr_Value value);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntNegate(PuleGpuIr_Shader shader, PuleGpuIr_Value value);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatNegate(PuleGpuIr_Shader shader, PuleGpuIr_Value value);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntAdd(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntSubtract(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntMultiply(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntDivide(PuleGpuIr_Shader shader, bool isSigned, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opIntModulo(PuleGpuIr_Shader shader, bool isSigned, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatAdd(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatSubtract(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatMultiply(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatDivide(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFloatRem(PuleGpuIr_Shader shader, PuleGpuIr_Value value1, PuleGpuIr_Value value2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opVectorMulScalar(PuleGpuIr_Shader shader, PuleGpuIr_Value vector, PuleGpuIr_Value scalar);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opMatrixMulScalar(PuleGpuIr_Shader shader, PuleGpuIr_Value matrix, PuleGpuIr_Value scalar);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opVectorMulMatrix(PuleGpuIr_Shader shader, PuleGpuIr_Value vector, PuleGpuIr_Value matrix);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opMatrixMulVector(PuleGpuIr_Shader shader, PuleGpuIr_Value matrix, PuleGpuIr_Value vector);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opMatrixMulMatrix(PuleGpuIr_Shader shader, PuleGpuIr_Value matrix1, PuleGpuIr_Value matrix2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opVectorOuterProduct(PuleGpuIr_Shader shader, PuleGpuIr_Value vector1, PuleGpuIr_Value vector2);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opVectorDotProduct(PuleGpuIr_Shader shader, PuleGpuIr_Value vector1, PuleGpuIr_Value vector2);
PULE_exportFn void puleGpuIr_opBranchJmp(PuleGpuIr_Shader shader, PuleGpuIr_Value target);
PULE_exportFn void puleGpuIr_opBranchCond(PuleGpuIr_Shader shader, PuleGpuIr_Value condition, PuleGpuIr_Value targetTrue, PuleGpuIr_Value targetFalse);
PULE_exportFn void puleGpuIr_opReturn(PuleGpuIr_Shader shader);
PULE_exportFn void puleGpuIr_opReturnValue(PuleGpuIr_Shader shader, PuleGpuIr_Value value);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opAccessChain(PuleGpuIr_Shader shader, PuleGpuIr_Value base, PuleGpuIr_Value * indices, size_t indexLen);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFunction(PuleGpuIr_Shader s, PuleGpuIr_Type returnType, PuleGpuIr_FunctionControl functionControl, PuleGpuIr_Type fnType, PuleStringView functionLabel);
PULE_exportFn void puleGpuIr_opFunctionEnd(PuleGpuIr_Shader s);
PULE_exportFn void puleGpuIr_opEntryPoint(PuleGpuIr_Shader s, PuleGpuIr_Value functionEntry, PuleGpuIr_Value const * globals, size_t globalLen);
PULE_exportFn void puleGpuIr_opDecorate(PuleGpuIr_Shader s, PuleGpuIr_Type type, uint32_t const * values, size_t valueLen);
PULE_exportFn void puleGpuIr_opDecorateMember(PuleGpuIr_Shader s, PuleGpuIr_Type type, int64_t memberIndex, uint32_t const * values, size_t valueLen);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opCompositeExtract(PuleGpuIr_Shader s, PuleGpuIr_Type type, PuleGpuIr_Value value, size_t index);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opCompositeConstruct(PuleGpuIr_Shader s, PuleGpuIr_Type type, PuleGpuIr_Value const * constituents, size_t constituentLen);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opVectorShuffle(PuleGpuIr_Shader s, PuleGpuIr_Type type, PuleGpuIr_Value vec0, PuleGpuIr_Value vec1, uint32_t const * components, size_t componentLen);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opExtInst(PuleGpuIr_Shader s, PuleGpuIr_Type type, PuleStringView instruction, PuleGpuIr_Value const * operands, size_t operandLen);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opFunctionCall(PuleGpuIr_Shader s, PuleGpuIr_Type returnType, PuleGpuIr_Value function, PuleGpuIr_Value const * arguments, size_t argumentLen);
PULE_exportFn PuleGpuIr_Value puleGpuIr_opConvertSignedToFloat(PuleGpuIr_Shader s, PuleGpuIr_Type type, PuleGpuIr_Value value);

#ifdef __cplusplus
} // extern C
#endif
