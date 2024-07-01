# gpu-ir

## structs
### PuleGpuIr_UniformBuffer
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  structType : PuleGpuIr_Type;
  label : PuleStringView;
};
```
### PuleGpuIr_StorageBuffer
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  structType : PuleGpuIr_Type;
  label : PuleStringView;
};
```
### PulGpuIr_Image
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  format : PuleGpuIr_ImageFormat;
  label : PuleStringView;
};
```
### PuleGpuIr_Texture
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  label : PuleStringView;
};
```
### PuleGpuIr_Sampler
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  label : PuleStringView;
};
```
### PuleGpuIr_InputAttachment
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  label : PuleStringView;
};
```
### PuleGpuIr_PushConstant
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  structType : PuleGpuIr_Type;
  label : PuleStringView;
};
```
### PuleGpuIr_InputAttribute
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  bufferBinding : uint32_t;
  byteOffset : uint32_t;
  /* 
    must match with other attributes at bufferBinding
   */
  byteStride : uint32_t;
  /* 
    must match with other attributes at bufferBinding
   */
  inputRate : PuleGpuIr_VertexInputRate;
  format : PuleGpuIr_Type;
  label : PuleStringView;
};
```
### PuleGpuIr_StageInput
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  format : PuleGpuIr_Type;
  label : PuleStringView;
};
```
### PuleGpuIr_StageOutput
```c
struct {
  paramType : PuleGpuIr_ParameterType;
  location : uint32_t;
  format : PuleGpuIr_Type;
  label : PuleStringView;
};
```
### PuleGpuIr_Parameter
```c
union {
  paramType : PuleGpuIr_ParameterType;
  uniformBuffer : PuleGpuIr_UniformBuffer;
  storageBuffer : PuleGpuIr_StorageBuffer;
  texture : PuleGpuIr_Texture;
  sampler : PuleGpuIr_Sampler;
  inputAttachment : PuleGpuIr_InputAttachment;
  pushConstant : PuleGpuIr_PushConstant;
  inputAttribute : PuleGpuIr_InputAttribute;
  stageInput : PuleGpuIr_StageInput;
  stageOutput : PuleGpuIr_StageOutput;
  generic : PuleGpuIr_Type;
};
```
### PuleGpuIr_ShaderStageVertex
```c
struct {
  stageType : PuleGpuIr_ShaderStageType;
  topology : PuleGpuIr_VertexTopology;
};
```
### PuleGpuIr_ShaderStageFragment
```c
struct {
  stageType : PuleGpuIr_ShaderStageType;
};
```
### PuleGpuIr_ShaderStage
```c
union {
  stageType : PuleGpuIr_ShaderStageType;
  vertex : PuleGpuIr_ShaderStageVertex;
  fragment : PuleGpuIr_ShaderStageFragment;
};
```
### PuleGpuIr_ShaderDescriptor
```c
struct {
  label : PuleStringView;
  stage : PuleGpuIr_ShaderStage;
  params : PuleGpuIr_Parameter ptr;
  paramLen : size_t;
};
```
### PuleGpuIr_Constant
```c
union {
  boolean : bool;
  integer : int64_t;
  floating : float;
  f32v2 : PuleF32v2;
  f32v3 : PuleF32v3;
  f32v4 : PuleF32v4;
  f32m44 : PuleF32m44;
};
```

## enums
### PuleGpuIr_PipelineType
```c
enum {
  renderVertexFragment,
  renderMeshFragment,
  raytrace,
  compute,
}
```
### PuleGpuIr_ParameterType
```c
enum {
  uniformBuffer,
  storageBuffer,
  image,
  texture,
  sampler,
  inputAttachment,
  pushConstant,
  inputAttribute,
  stageInput,
  stageOutput,
}
```
### PuleGpuIr_ImageFormat
```c
enum {
  r8unorm,
  r16unorm,
  rgba8ui,
}
```
### PuleGpuIr_VertexInputRate
```c
enum {
  vertex,
  instance,
}
```
### PuleGpuIr_ShaderStageType
```c
enum {
  vertex,
  fragment,
  mesh,
  rayGen,
  rayClosestHit,
  rayAnyHit,
  rayMiss,
  compute,
}
```
### PuleGpuIr_VertexTopology
```c
enum {
  points,
  lines,
  triangles,
  triangleStrip,
  lineStrip,
}
```
### PuleGpuIr_StorageClass
```c
enum {
  uniformConstant,
  input,
  uniform,
  output,
  workgroup,
  crossWorkgroup,
  private,
  function,
  generic,
  pushConstant,
  atomicCounter,
  image,
  storageBuffer,
}
```
### PuleGpuIr_ConstantType
```c
enum {
  bool,
  int,
  float,
  f32v2,
  f32v3,
  f32v4,
  f32m44,
}
```

## entities
### PuleGpuIr_Pipeline


API for shader+pipeline construction

Most graphics APIs decouple shaders and pipelines to allow flexibility in the
  generated SPIR-V during compilation.

This API generates the pipeline alongside the SPIR-V as a single interface.
  The flexibility a pipeline descriptor gives isn't enough to specialize
  the spir-v in a lot of cases, such as dead-code removal if a
  vertex-attribute is unused.

Effectively this is another IR (PGPU-IR) but maps pretty closely to SPIR-V
  and will have a human-readable format at some point.

### PuleGpuIr_Type
### PuleGpuIr_Shader
### PuleGpuIr_Value

## functions
### puleGpuIr_pipeline
```c
puleGpuIr_pipeline(
  label : PuleStringView,
  type : PuleGpuIr_PipelineType
) PuleGpuIr_Pipeline;
```
### puleGpuIr_pipelineDestroy
```c
puleGpuIr_pipelineDestroy(
  pipeline : PuleGpuIr_Pipeline
) void;
```
### puleGpuIr_pipelineCompile
```c
puleGpuIr_pipelineCompile(
  pipeline : PuleGpuIr_Pipeline
) PuleGpuPipeline;
```
### puleGpuIr_pipelineAddShader
```c
puleGpuIr_pipelineAddShader(
  pipeline : PuleGpuIr_Pipeline,
  shaderDescriptor : PuleGpuIr_ShaderDescriptor
) PuleGpuIr_Shader;
```
### puleGpuIr_opLabel
```c
puleGpuIr_opLabel(
  shader : PuleGpuIr_Shader,
  label : PuleStringView
) PuleGpuIr_Value;
```
### puleGpuIr_opTypeFloat
```c
puleGpuIr_opTypeFloat(
  s : PuleGpuIr_Shader,
  bits : size_t
) PuleGpuIr_Type;
```
### puleGpuIr_opTypeVector
```c
puleGpuIr_opTypeVector(
  s : PuleGpuIr_Shader,
  elementType : PuleGpuIr_Type,
  elementSize : size_t
) PuleGpuIr_Type;
```
### puleGpuIr_opTypePointer
```c
puleGpuIr_opTypePointer(
  s : PuleGpuIr_Shader,
  underlyingType : PuleGpuIr_Type,
  storageClass : PuleGpuIr_StorageClass
) PuleGpuIr_Type;
```
### puleGpuIr_opVariable
```c
puleGpuIr_opVariable(
  shader : PuleGpuIr_Shader,
  type : PuleGpuIr_Type,
  storageClass : PuleGpuIr_StorageClass
) PuleGpuIr_Value;
```
### puleGpuIr_opLoad
```c
puleGpuIr_opLoad(
  shader : PuleGpuIr_Shader,
  resultType : PuleGpuIr_Type,
  pointer : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opStore
```c
puleGpuIr_opStore(
  shader : PuleGpuIr_Shader,
  pointer : PuleGpuIr_Value,
  value : PuleGpuIr_Value
) void;
```
### puleGpuIr_opConstant
```c
puleGpuIr_opConstant(
  shader : PuleGpuIr_Shader,
  type : PuleGpuIr_Type,
  constantType : PuleGpuIr_ConstantType,
  constant : PuleGpuIr_Constant
) PuleGpuIr_Value;
```
### puleGpuIr_opLogicalNot
```c
puleGpuIr_opLogicalNot(
  shader : PuleGpuIr_Shader,
  value : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opLogicalAnd
```c
puleGpuIr_opLogicalAnd(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opLogicalOr
```c
puleGpuIr_opLogicalOr(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opLogicalEq
```c
puleGpuIr_opLogicalEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opLogicalNotEq
```c
puleGpuIr_opLogicalNotEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opSelect
```c
puleGpuIr_opSelect(
  shader : PuleGpuIr_Shader,
  condition : PuleGpuIr_Value,
  valueTrue : PuleGpuIr_Value,
  valueFalse : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntEq
```c
puleGpuIr_opIntEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntNotEq
```c
puleGpuIr_opIntNotEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntGreaterThan
```c
puleGpuIr_opIntGreaterThan(
  shader : PuleGpuIr_Shader,
  isSigned : bool,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntGreaterThanEq
```c
puleGpuIr_opIntGreaterThanEq(
  shader : PuleGpuIr_Shader,
  isSigned : bool,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntLessThan
```c
puleGpuIr_opIntLessThan(
  shader : PuleGpuIr_Shader,
  isSigned : bool,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntLessThanEq
```c
puleGpuIr_opIntLessThanEq(
  shader : PuleGpuIr_Shader,
  isSigned : bool,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatEq
```c
puleGpuIr_opFloatEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatNotEq
```c
puleGpuIr_opFloatNotEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatGreaterThan
```c
puleGpuIr_opFloatGreaterThan(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatGreaterThanEq
```c
puleGpuIr_opFloatGreaterThanEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatLessThan
```c
puleGpuIr_opFloatLessThan(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatLessThanEq
```c
puleGpuIr_opFloatLessThanEq(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opTranspose
```c
puleGpuIr_opTranspose(
  shader : PuleGpuIr_Shader,
  value : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntNegate
```c
puleGpuIr_opIntNegate(
  shader : PuleGpuIr_Shader,
  value : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatNegate
```c
puleGpuIr_opFloatNegate(
  shader : PuleGpuIr_Shader,
  value : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntAdd
```c
puleGpuIr_opIntAdd(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntSubtract
```c
puleGpuIr_opIntSubtract(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntMultiply
```c
puleGpuIr_opIntMultiply(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntDivide
```c
puleGpuIr_opIntDivide(
  shader : PuleGpuIr_Shader,
  isSigned : bool,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opIntModulo
```c
puleGpuIr_opIntModulo(
  shader : PuleGpuIr_Shader,
  isSigned : bool,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatAdd
```c
puleGpuIr_opFloatAdd(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatSubtract
```c
puleGpuIr_opFloatSubtract(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatMultiply
```c
puleGpuIr_opFloatMultiply(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatDivide
```c
puleGpuIr_opFloatDivide(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opFloatRem
```c
puleGpuIr_opFloatRem(
  shader : PuleGpuIr_Shader,
  value1 : PuleGpuIr_Value,
  value2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opVectorMulScalar
```c
puleGpuIr_opVectorMulScalar(
  shader : PuleGpuIr_Shader,
  vector : PuleGpuIr_Value,
  scalar : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opMatrixMulScalar
```c
puleGpuIr_opMatrixMulScalar(
  shader : PuleGpuIr_Shader,
  matrix : PuleGpuIr_Value,
  scalar : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opVectorMulMatrix
```c
puleGpuIr_opVectorMulMatrix(
  shader : PuleGpuIr_Shader,
  vector : PuleGpuIr_Value,
  matrix : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opMatrixMulVector
```c
puleGpuIr_opMatrixMulVector(
  shader : PuleGpuIr_Shader,
  matrix : PuleGpuIr_Value,
  vector : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opMatrixMulMatrix
```c
puleGpuIr_opMatrixMulMatrix(
  shader : PuleGpuIr_Shader,
  matrix1 : PuleGpuIr_Value,
  matrix2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opVectorOuterProduct
```c
puleGpuIr_opVectorOuterProduct(
  shader : PuleGpuIr_Shader,
  vector1 : PuleGpuIr_Value,
  vector2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opVectorDotProduct
```c
puleGpuIr_opVectorDotProduct(
  shader : PuleGpuIr_Shader,
  vector1 : PuleGpuIr_Value,
  vector2 : PuleGpuIr_Value
) PuleGpuIr_Value;
```
### puleGpuIr_opBranchJmp
```c
puleGpuIr_opBranchJmp(
  shader : PuleGpuIr_Shader,
  target : PuleGpuIr_Value
) void;
```
### puleGpuIr_opBranchCond
```c
puleGpuIr_opBranchCond(
  shader : PuleGpuIr_Shader,
  condition : PuleGpuIr_Value,
  targetTrue : PuleGpuIr_Value,
  targetFalse : PuleGpuIr_Value
) void;
```
### puleGpuIr_opReturn
```c
puleGpuIr_opReturn(
  shader : PuleGpuIr_Shader
) void;
```
### puleGpuIr_opReturnValue
```c
puleGpuIr_opReturnValue(
  shader : PuleGpuIr_Shader,
  value : PuleGpuIr_Value
) void;
```
### puleGpuIr_opAccessChain
```c
puleGpuIr_opAccessChain(
  shader : PuleGpuIr_Shader,
  base : PuleGpuIr_Value,
  indices : PuleGpuIr_Value ptr,
  indexLen : size_t
) PuleGpuIr_Value;
```
