#include <pulchritude/gpu-ir.h>

#include <pulchritude/core.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#include <spirv-tools/libspirv.hpp>
#include <spirv-tools/optimizer.hpp>

// -- utilities ----------------------------------------------------------------

namespace priv {

enum struct Opcode {
  typeFloat,
  typeVector,
  typePointer,
  variable,
  load,
  store,
  constant,
  logicalNot,
  logicalAnd,
  logicalOr,
  logicalEq,
  logicalNotEq,
  select,
  intEq,
  intNotEq,
  intGreaterThan,
  intGreaterThanEq,
  intLessThan,
  intLessThanEq,
  floatEq,
  floatNotEq,
  floatGreaterThan,
  floatGreaterThanEq,
  floatLessThan,
  floatLessThanEq,
  transpose,
  intNegate,
  floatNegate,
  intAdd,
  intSubtract,
  intMultiply,
  intDivide,
  intModulo,
  floatAdd,
  floatSubtract,
  floatMultiply,
  floatDivide,
  floatRem,
  vectorMulScalar,
  matrixMulScalar,
  vectorMulMatrix,
  matrixMulVector,
  matrixMulMatrix,
  vectorOuterProduct,
  vectorDotProduct,
  branchJmp,
  branchCond,
  returnVoid,
  returnValue,
  accessChain,
  label,
};

struct Instruction {
  Instruction() {}
  ~Instruction() {
    if (this->opcode == Opcode::accessChain) {
      this->opAccessChain.indices.~vector(); // manually destruct
    }
  }
  Instruction(Instruction const & instr) {
    memcpy(this, &instr, sizeof(Instruction));
    if (instr.opcode == Opcode::accessChain) {
      this->opAccessChain.indices = instr.opAccessChain.indices;
    }
  }
  Instruction(Instruction && instr) {
    memcpy(this, &instr, sizeof(Instruction));
    if (instr.opcode == Opcode::accessChain) {
      this->opAccessChain.indices = std::move(instr.opAccessChain.indices);
    }
  }
  Instruction & operator=(Instruction const & instr) {
    if (this->opcode == Opcode::accessChain) {
      this->opAccessChain.indices.~vector(); // manually destruct
    }
    memcpy(this, &instr, sizeof(Instruction));
    if (instr.opcode == Opcode::accessChain) {
      this->opAccessChain.indices = instr.opAccessChain.indices;
    }
    return *this;
  }
  Instruction & operator=(Instruction && instr) {
    if (this->opcode == Opcode::accessChain) {
      this->opAccessChain.indices.~vector(); // manually destruct
    }
    memcpy(this, &instr, sizeof(Instruction));
    if (instr.opcode == Opcode::accessChain) {
      this->opAccessChain.indices = std::move(instr.opAccessChain.indices);
    }
    return *this;
  }

  Opcode opcode;
  union {
    struct {
      size_t bits;
      PuleGpuIr_Type returnType;
    } opTypeFloat;
    struct {
      PuleGpuIr_Type elementType;
      size_t elementSize;
      PuleGpuIr_Type returnType;
    } opTypeVector;
    struct {
      PuleGpuIr_Type underlyingType;
      PuleGpuIr_StorageClass storageClass;
      PuleGpuIr_Type returnType;
    } opTypePointer;
    struct {
      PuleGpuIr_Type type;
      PuleGpuIr_StorageClass storageClass;
      PuleGpuIr_Value returnValue;
    } opVariable;
    struct {
      PuleGpuIr_Type resultType;
      PuleGpuIr_Value pointer;
      PuleGpuIr_Value returnValue;
    } opLoad;
    struct {
      PuleGpuIr_Value pointer;
      PuleGpuIr_Value value;
    } opStore;
    struct {
      PuleGpuIr_Type type;
      PuleGpuIr_ConstantType constantType;
      PuleGpuIr_Constant constant;
      PuleGpuIr_Value returnValue;
    } opConstant;
    struct {
      PuleGpuIr_Value value;
      PuleGpuIr_Value returnValue;
    } opLogicalNot;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opLogicalAnd;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opLogicalOr;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opLogicalEq;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opLogicalNotEq;
    struct {
      PuleGpuIr_Value condition;
      PuleGpuIr_Value valueTrue;
      PuleGpuIr_Value valueFalse;
      PuleGpuIr_Value returnValue;
    } opSelect;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntEq;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntNotEq;
    struct {
      bool isSigned;
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntGreaterThan;
    struct {
      bool isSigned;
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntGreaterThanEq;
    struct {
      bool isSigned;
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntLessThan;
    struct {
      bool isSigned;
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntLessThanEq;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatEq;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatNotEq;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatGreaterThan;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatGreaterThanEq;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatLessThan;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatLessThanEq;
    struct {
      PuleGpuIr_Value value;
      PuleGpuIr_Value returnValue;
    } opTranspose;
    struct {
      PuleGpuIr_Value value;
      PuleGpuIr_Value returnValue;
    } opIntNegate;
    struct {
      PuleGpuIr_Value value;
      PuleGpuIr_Value returnValue;
    } opFloatNegate;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntAdd;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntSubtract;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntMultiply;
    struct {
      bool isSigned;
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntDivide;
    struct {
      bool isSigned;
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opIntModulo;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatAdd;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatSubtract;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatMultiply;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatDivide;
    struct {
      PuleGpuIr_Value value1;
      PuleGpuIr_Value value2;
      PuleGpuIr_Value returnValue;
    } opFloatRem;
    struct {
      PuleGpuIr_Value vector;
      PuleGpuIr_Value scalar;
      PuleGpuIr_Value returnValue;
    } opVectorMulScalar;
    struct {
      PuleGpuIr_Value matrix;
      PuleGpuIr_Value scalar;
      PuleGpuIr_Value returnValue;
    } opMatrixMulScalar;
    struct {
      PuleGpuIr_Value vector;
      PuleGpuIr_Value matrix;
      PuleGpuIr_Value returnValue;
    } opVectorMulMatrix;
    struct {
      PuleGpuIr_Value matrix;
      PuleGpuIr_Value vector;
      PuleGpuIr_Value returnValue;
    } opMatrixMulVector;
    struct {
      PuleGpuIr_Value matrix1;
      PuleGpuIr_Value matrix2;
      PuleGpuIr_Value returnValue;
    } opMatrixMulMatrix;
    struct {
      PuleGpuIr_Value vector1;
      PuleGpuIr_Value vector2;
      PuleGpuIr_Value returnValue;
    } opVectorOuterProduct;
    struct {
      PuleGpuIr_Value vector1;
      PuleGpuIr_Value vector2;
      PuleGpuIr_Value returnValue;
    } opVectorDotProduct;
    struct {
      PuleGpuIr_Value target;
    } opBranchJmp;
    struct {
      PuleGpuIr_Value condition;
      PuleGpuIr_Value targetTrue;
      PuleGpuIr_Value targetFalse;
    } opBranchCond;
    struct {
    } opReturnVoid;
    struct {
      PuleGpuIr_Value value;
    } opReturnValue;
    struct {
      PuleGpuIr_Value base;
      std::vector<PuleGpuIr_Value> indices;
      PuleGpuIr_Value returnValue;
    } opAccessChain;
    struct {
      PuleGpuIr_Value target;
      std::string label;
    } opLabel;
  };
};

struct Pipeline;

struct Shader {
  std::string label;
  size_t irId;
  PuleGpuIr_ShaderStage stage;
  std::vector<PuleGpuIr_Parameter> parameters;
  std::vector<PuleGpuIr_Value> values;
  std::vector<PuleGpuIr_Type> types;

  std::vector<Instruction> instructions;

  size_t irIdCounter = 0;

  PuleGpuIr_Value newValue() {
    values.push_back(PuleGpuIr_Value { .id = irIdCounter, });
    ++ irIdCounter;
    return values.back();
  }

  PuleGpuIr_Type newType() {
    types.push_back(PuleGpuIr_Type { .id = irIdCounter, });
    ++ irIdCounter;
    return types.back();
  }
};

struct Pipeline {
  std::string label;
  PuleGpuIr_PipelineType type;
  std::vector<PuleGpuIr_Shader> entryPoints;
};

pule::ResourceContainer<Pipeline, PuleGpuIr_Pipeline> pipelines;
pule::ResourceContainer<Shader, PuleGpuIr_Shader> shaders;

} // namespace priv

// -- pipeline -----------------------------------------------------------------

namespace priv {

void compilePipelineInstruction(
  priv::Instruction const & instr,
  std::string & data
) {
  data += "\n";
  auto toStr = [](PuleGpuIr_Value const value) -> std::string {
    return "%" + std::to_string(value.id);
  };
  auto toTypeStr = [](PuleGpuIr_Type const value) -> std::string {
    return "%" + std::to_string(value.id);
  };
  auto toStorageClassStr = [](
    PuleGpuIr_StorageClass const storageClass
  ) -> std::string {
    switch (storageClass) {
      default: return "Unknown";
      case PuleGpuIr_StorageClass_generic: return "";
      case PuleGpuIr_StorageClass_function: return "Function";
      case PuleGpuIr_StorageClass_input: return "Input";
      case PuleGpuIr_StorageClass_output: return "Output";
      case PuleGpuIr_StorageClass_uniform: return "Uniform";
      case PuleGpuIr_StorageClass_storageBuffer: return "StorageBuffer";
      case PuleGpuIr_StorageClass_pushConstant: return "PushConstant";
    }
  };
  switch (instr.opcode) {
    case priv::Opcode::typeFloat: {
      auto & op = instr.opTypeFloat;
      data += (
          toTypeStr(op.returnType)
        + " = OpTypeFloat " + std::to_string(op.bits)
      );
    } break;
    case priv::Opcode::typeVector: {
      auto & op = instr.opTypeVector;
      data += (
          toTypeStr(op.returnType)
        + " = OpTypeVector "
        + toTypeStr(op.elementType)
        + " " + std::to_string(op.elementSize)
      );
    } break;
    case priv::Opcode::typePointer: {
      auto & op = instr.opTypePointer;
      data += (
          toTypeStr(op.returnType)
        + " = OpTypePointer "
        + toStorageClassStr(op.storageClass)
        + " " + toTypeStr(op.underlyingType)
      );
    } break;
    case priv::Opcode::variable: {
      auto & op = instr.opVariable;
      data += (
          toStr(op.returnValue)
        + " = OpVariable "
        + toTypeStr(op.type)
        + " " + toStorageClassStr(op.storageClass)
      );
    } break;
    case priv::Opcode::load: {
      auto & op = instr.opLoad;
      data += (
          "%" + std::to_string(op.returnValue.id)
        + " = OpLoad "
        + toTypeStr(op.resultType)
        + " " + toStr(op.pointer)
      );
      data += " = OpLoad ";
    } break;
    case priv::Opcode::store: {
      auto & op = instr.opStore;
      data += (
        "OpStore " + toStr(op.pointer) + " " + toStr(op.value)
      );
    } break;
    case priv::Opcode::constant: {
      auto & op = instr.opConstant;
      data += (
          toStr(op.returnValue)
        + " = OpConstant "
        + toTypeStr(op.type)
        + " "
      );
      switch (op.constantType) {
        default: PULE_assert(false && "unsupported type");
        case PuleGpuIr_ConstantType_bool: {
          data += (op.constant.boolean ? "true" : "false");
        } break;
        case PuleGpuIr_ConstantType_int:
          data += std::to_string(op.constant.integer);
        break;
        case PuleGpuIr_ConstantType_float:
          data += std::to_string(op.constant.floating);
        break;
      }
    } break;
    case priv::Opcode::logicalNot: {
      auto & op = instr.opLogicalNot;
      data += toStr(op.returnValue) + " = OpLogicalNot " + toStr(op.value);
    } break;
    case priv::Opcode::logicalAnd: {
      auto & op = instr.opLogicalAnd;
      data += (
          toStr(op.returnValue)
        + " = OpLogicalAnd "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::logicalOr: {
      auto & op = instr.opLogicalOr;
      data += (
          toStr(op.returnValue)
        + " = OpLogicalOr "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::logicalEq: {
      auto & op = instr.opLogicalEq;
      data += (
          toStr(op.returnValue)
        + " = OpLogicalEq "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::logicalNotEq: {
      auto & op = instr.opLogicalNotEq;
      data += (
          toStr(op.returnValue)
        + " = OpLogicalNotEq "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::select: {
      auto & op = instr.opSelect;
      data += (
          toStr(op.returnValue)
        + " = OpSelect "
        + toStr(op.condition)
        + " " + toStr(op.valueTrue)
        + " " + toStr(op.valueFalse)
      );
    } break;
    case priv::Opcode::intEq: {
      auto & op = instr.opIntEq;
      data += (
          toStr(op.returnValue)
        + " = OpIEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intNotEq: {
      auto & op = instr.opIntNotEq;
      data += (
          toStr(op.returnValue)
        + " = OpINotEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intGreaterThan: {
      auto & op = instr.opIntGreaterThan;
      data += (
          toStr(op.returnValue)
        + " = OpSGreaterThan "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intGreaterThanEq: {
      auto & op = instr.opIntGreaterThanEq;
      data += (
          toStr(op.returnValue)
        + " = OpSGreaterThanEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intLessThan: {
      auto & op = instr.opIntLessThan;
      data += (
          toStr(op.returnValue)
        + " = OpSLessThan "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intLessThanEq: {
      auto & op = instr.opIntLessThanEq;
      data += (
          toStr(op.returnValue)
        + " = OpSLessThanEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatEq: {
      auto & op = instr.opFloatEq;
      data += (
          toStr(op.returnValue)
        + " = OpFOrdEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatNotEq: {
      auto & op = instr.opFloatNotEq;
      data += (
          toStr(op.returnValue)
        + " = OpFOrdNotEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatGreaterThan: {
      auto & op = instr.opFloatGreaterThan;
      data += (
          toStr(op.returnValue)
        + " = OpFOrdGreaterThan "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatGreaterThanEq: {
      auto & op = instr.opFloatGreaterThanEq;
      data += (
          toStr(op.returnValue)
        + " = OpFOrdGreaterThanEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatLessThan: {
      auto & op = instr.opFloatLessThan;
      data += (
          toStr(op.returnValue)
        + " = OpFOrdLessThan "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatLessThanEq: {
      auto & op = instr.opFloatLessThanEq;
      data += (
          toStr(op.returnValue)
        + " = OpFOrdLessThanEqual "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::transpose: {
      auto & op = instr.opTranspose;
      data += toStr(op.returnValue) + " = OpTranspose " + toStr(op.value);
    } break;
    case priv::Opcode::intNegate: {
      auto & op = instr.opIntNegate;
      data += toStr(op.returnValue) + " = OpSNegate " + toStr(op.value);
    } break;
    case priv::Opcode::floatNegate: {
      auto & op = instr.opFloatNegate;
      data += toStr(op.returnValue) + " = OpFNegate " + toStr(op.value);
    } break;
    case priv::Opcode::intAdd: {
      auto & op = instr.opIntAdd;
      data += (
          toStr(op.returnValue)
        + " = OpIAdd "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intSubtract: {
      auto & op = instr.opIntSubtract;
      data += (
          toStr(op.returnValue)
        + " = OpISub "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intMultiply: {
      auto & op = instr.opIntMultiply;
      data += (
          toStr(op.returnValue)
        + " = OpIMul "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intDivide: {
      auto & op = instr.opIntDivide;
      data += (
          toStr(op.returnValue)
        + " = OpSDiv "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::intModulo: {
      auto & op = instr.opIntModulo;
      data += (
          toStr(op.returnValue)
        + " = OpSMod "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatAdd: {
      auto & op = instr.opFloatAdd;
      data += (
          toStr(op.returnValue)
        + " = OpFAdd "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatSubtract: {
      auto & op = instr.opFloatSubtract;
      data += (
          toStr(op.returnValue)
        + " = OpFSub "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatMultiply: {
      auto & op = instr.opFloatMultiply;
      data += (
          toStr(op.returnValue)
        + " = OpFMul "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatDivide: {
      auto & op = instr.opFloatDivide;
      data += (
          toStr(op.returnValue)
        + " = OpFDiv "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::floatRem: {
      auto & op = instr.opFloatRem;
      data += (
          toStr(op.returnValue)
        + " = OpFRem "
        + toStr(op.value1)
        + " " + toStr(op.value2)
      );
    } break;
    case priv::Opcode::vectorMulScalar: {
      auto & op = instr.opVectorMulScalar;
      data += (
          toStr(op.returnValue)
        + " = OpVectorTimesScalar "
        + toStr(op.vector)
        + " " + toStr(op.scalar)
      );
    } break;
    case priv::Opcode::matrixMulScalar: {
      auto & op = instr.opMatrixMulScalar;
      data += (
          toStr(op.returnValue)
        + " = OpMatrixTimesScalar "
        + toStr(op.matrix)
        + " " + toStr(op.scalar)
      );
    } break;
    case priv::Opcode::vectorMulMatrix: {
      auto & op = instr.opVectorMulMatrix;
      data += (
          toStr(op.returnValue)
        + " = OpVectorTimesMatrix "
        + toStr(op.vector)
        + " " + toStr(op.matrix)
      );
    } break;
    case priv::Opcode::matrixMulVector: {
      auto & op = instr.opMatrixMulVector;
      data += (
          toStr(op.returnValue)
        + " = OpMatrixTimesVector "
        + toStr(op.matrix)
        + " " + toStr(op.vector)
      );
    } break;
    case priv::Opcode::matrixMulMatrix: {
      auto & op = instr.opMatrixMulMatrix;
      data += (
          toStr(op.returnValue)
        + " = OpMatrixTimesMatrix "
        + toStr(op.matrix1)
        + " " + toStr(op.matrix2)
      );
    } break;
    case priv::Opcode::vectorOuterProduct: {
      auto & op = instr.opVectorOuterProduct;
      data += (
          toStr(op.returnValue)
        + " = OpOuterProduct "
        + toStr(op.vector1)
        + " " + toStr(op.vector2)
      );
    } break;
    case priv::Opcode::vectorDotProduct: {
      auto & op = instr.opVectorDotProduct;
      data += (
          toStr(op.returnValue)
        + " = OpDot "
        + toStr(op.vector1)
        + " " + toStr(op.vector2)
      );
    } break;
    case priv::Opcode::branchJmp: {
      auto & op = instr.opBranchJmp;
      data += "OpBranch " + toStr(op.target);
    } break;
    case priv::Opcode::branchCond: {
      auto & op = instr.opBranchCond;
      data += (
          "OpBranchConditional "
        + toStr(op.condition)
        + " " + toStr(op.targetTrue)
        + " " + toStr(op.targetFalse)
      );
    } break;
    case priv::Opcode::returnVoid: {
      data += "OpReturn";
    } break;
    case priv::Opcode::returnValue: {
      auto & op = instr.opReturnValue;
      data += "OpReturnValue " + toStr(op.value);
    } break;
    case priv::Opcode::accessChain: {
      auto & op = instr.opAccessChain;
      data += toStr(op.returnValue) + " = OpAccessChain " + toStr(op.base);
    } break;
    case priv::Opcode::label: {
      auto & op = instr.opLabel;
      data += toStr(op.target) + " OpLabel " + op.label;
    } break;
  }
}

std::vector<uint32_t> compilePipeline(
  priv::Shader const & shaderStage
) {
  std::string dataSpirvPlaintext;
  for (auto & instr : shaderStage.instructions) {
    compilePipelineInstruction(instr, dataSpirvPlaintext);
  }
  spvtools::SpirvTools tools(SPV_ENV_UNIVERSAL_1_6);
  spvtools::Optimizer optimizer(SPV_ENV_UNIVERSAL_1_6);

  puleLogDev("generated SPIRV:```\n%s\n```", dataSpirvPlaintext.c_str());

  std::vector<uint32_t> spirv;
  if (!tools.Assemble(dataSpirvPlaintext, &spirv)) {
    PULE_assert(false && "failed to assemble SPIR-V");
  }
  if (!tools.Validate(spirv)) {
    PULE_assert(false && "failed to validate SPIR-V");
  }

  return spirv;
}

} // namespace priv

extern "C" {

PuleGpuIr_Pipeline puleGpuIrPipeline(
  PuleStringView label,
  PuleGpuIr_PipelineType type
) {
  auto pipeline = priv::Pipeline {
    .label = std::string(label.contents, label.len),
    .type = type,
  };
  return priv::pipelines.create(pipeline);
}

void puleGpuIrPipelineDestroy(PuleGpuIr_Pipeline const pipeline) {
  priv::pipelines.destroy(pipeline);
}

PuleGpuPipeline puleGpuIr_pipelineCompile(PuleGpuIr_Pipeline const pipeline) {
  auto & privPipeline = *priv::pipelines.at(pipeline);
  PuleGpuPipeline gpuPipeline;
  switch (privPipeline.type) {
    default: PULE_assert(false && "unsupported pipeline type");
    case PuleGpuIr_PipelineType_renderVertexFragment: {
      std::vector<uint32_t> vertexFnSpirv;
      std::vector<uint32_t> fragmentFnSpirv;
      for (auto & fn : privPipeline.entryPoints) {
        auto const & shader = *priv::shaders.at(fn);
        auto const stageType = shader.stage.stageType;
        switch (stageType) {
          default: PULE_assert(false && "unsupported shader stage");
          case PuleGpuIr_ShaderStageType_vertex: {
            vertexFnSpirv = priv::compilePipeline(shader);
          } break;
          case PuleGpuIr_ShaderStageType_fragment: {
            fragmentFnSpirv = priv::compilePipeline(shader);
          } break;
        }
      }
    } break;
  }
  return gpuPipeline;
}

} // extern "C"

// -- misc ---------------------------------------------------------------------
extern "C" {

PuleGpuIr_Shader puleGpuIr_pipelineAddShader(
  PuleGpuIr_Pipeline pipeline,
  PuleGpuIr_ShaderDescriptor descriptor
) {
  auto & privPipeline = *priv::pipelines.at(pipeline);
  PuleGpuIr_Shader const fn = (
    priv::shaders.create(priv::Shader {
      .label = std::string(descriptor.label.contents, descriptor.label.len),
      .stage = descriptor.stage,
      .parameters = std::vector<PuleGpuIr_Parameter>(
        descriptor.params,
        descriptor.params + descriptor.paramLen
      ),
    })
  );
  privPipeline.entryPoints.push_back(fn);
}

} // extern "C"

// -- instructions -------------------------------------------------------------

#define PRIV_SHADER(opcode_) \
  auto & privShader = *priv::shaders.at(shader); \
  priv::Instruction instr; \
  instr.opcode = opcode_

#define PRIV_SHADER_VALUE(opcode_) \
  auto & privShader = *priv::shaders.at(shader); \
  auto returnValue = privShader.newValue(); \
  priv::Instruction instr; \
  instr.opcode = opcode_
#define PRIV_SHADER_TYPE(opcode_) \
  auto & privShader = *priv::shaders.at(shader); \
  auto returnType = privShader.newType(); \
  priv::Instruction instr; \
  instr.opcode = opcode_

extern "C" {

PuleGpuIr_Type puleGpuIr_opTypeFloat(PuleGpuIr_Shader shader, size_t bits) {
  PRIV_SHADER_TYPE(priv::Opcode::typeFloat);
  instr.opTypeFloat = {
    .bits = bits,
    .returnType = returnType,
  };
  privShader.instructions.push_back(instr);
  return instr.opTypeFloat.returnType;
}

PuleGpuIr_Type puleGpuIr_opTypeVector(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Type elementType, size_t elementSize
) {
  PRIV_SHADER_TYPE(priv::Opcode::typeVector);
  instr.opTypeVector = {
    .elementType = elementType,
    .elementSize = elementSize,
    .returnType = returnType,
  };
  privShader.instructions.push_back(instr);
  return instr.opTypeVector.returnType;
}

PuleGpuIr_Type puleGpuIr_opTypePointer(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Type underlyingType, PuleGpuIr_StorageClass storageClass
) {
  PRIV_SHADER_TYPE(priv::Opcode::typePointer);
  instr.opTypePointer = {
    .underlyingType = underlyingType,
    .storageClass = storageClass,
    .returnType = returnType,
  };
  privShader.instructions.push_back(instr);
  return instr.opTypePointer.returnType;
}

PuleGpuIr_Value puleGpuIr_opVariable(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Type type,
  PuleGpuIr_StorageClass storageClass
) {
  PRIV_SHADER_VALUE(priv::Opcode::variable);
  instr.opVariable = {
    .type = type,
    .storageClass = storageClass,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opVariable.returnValue;
};

PuleGpuIr_Value puleGpuIr_opLoad(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Type resultType,
  PuleGpuIr_Value pointer
) {
  PRIV_SHADER_VALUE(priv::Opcode::load);
  instr.opLoad = {
    .resultType = resultType,
    .pointer = pointer,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opLoad.returnValue;
}

void puleGpuIr_opStore(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value pointer,
  PuleGpuIr_Value value
) {
  PRIV_SHADER(priv::Opcode::store);
  instr.opStore = {
    .pointer = pointer,
    .value = value,
  };
  privShader.instructions.push_back(instr);
}

PuleGpuIr_Value puleGpuIr_opConstant(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Type type,
  PuleGpuIr_ConstantType constantType,
  PuleGpuIr_Constant constant
) {
  PRIV_SHADER_VALUE(priv::Opcode::constant);
  instr.opConstant = {
    .type = type,
    .constantType = constantType,
    .constant = constant,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opConstant.returnValue;
}

PuleGpuIr_Value puleGpuIr_opLogicalNot(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value
) {
  PRIV_SHADER_VALUE(priv::Opcode::logicalNot);
  instr.opLogicalNot = {
    .value = value,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opLogicalNot.returnValue;
}

PuleGpuIr_Value puleGpuIr_opLogicalAnd(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::logicalAnd);
  instr.opLogicalAnd = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opLogicalAnd.returnValue;
}

PuleGpuIr_Value puleGpuIr_opLogicalOr(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::logicalOr);
  instr.opLogicalOr = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opLogicalOr.returnValue;
}

PuleGpuIr_Value puleGpuIr_opLogicalEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::logicalEq);
  instr.opLogicalEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opLogicalEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opLogicalNotEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::logicalNotEq);
  instr.opLogicalNotEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opLogicalNotEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opSelect(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value condition,
  PuleGpuIr_Value valueTrue,
  PuleGpuIr_Value valueFalse
) {
  PRIV_SHADER_VALUE(priv::Opcode::select);
  instr.opSelect = {
    .condition = condition,
    .valueTrue = valueTrue,
    .valueFalse = valueFalse,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opSelect.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intEq);
  instr.opIntEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntNotEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intNotEq);
  instr.opIntNotEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntNotEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntGreaterThan(
  PuleGpuIr_Shader shader,
  bool isSigned,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intGreaterThan);
  instr.opIntGreaterThan = {
    .isSigned = isSigned,
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntGreaterThan.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntGreaterThanEq(
  PuleGpuIr_Shader shader,
  bool isSigned,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intGreaterThanEq);
  instr.opIntGreaterThanEq = {
    .isSigned = isSigned,
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntGreaterThanEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntLessThan(
  PuleGpuIr_Shader shader,
  bool isSigned,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intLessThan);
  instr.opIntLessThan = {
    .isSigned = isSigned,
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntLessThan.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntLessThanEq(
  PuleGpuIr_Shader shader,
  bool isSigned,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intLessThanEq);
  instr.opIntLessThanEq = {
    .isSigned = isSigned,
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntLessThanEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatEq);
  instr.opFloatEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatNotEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatNotEq);
  instr.opFloatNotEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatNotEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatGreaterThan(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatGreaterThan);
  instr.opFloatGreaterThan = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatGreaterThan.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatGreaterThanEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatGreaterThanEq);
  instr.opFloatGreaterThanEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatGreaterThanEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatLessThan(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatLessThan);
  instr.opFloatLessThan = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatLessThan.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatLessThanEq(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatLessThanEq);
  instr.opFloatLessThanEq = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatLessThanEq.returnValue;
}

PuleGpuIr_Value puleGpuIr_opTranspose(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value
) {
  PRIV_SHADER_VALUE(priv::Opcode::transpose);
  instr.opTranspose = {
    .value = value,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opTranspose.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntNegate(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value
) {
  PRIV_SHADER_VALUE(priv::Opcode::intNegate);
  instr.opIntNegate = {
    .value = value,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntNegate.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatNegate(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatNegate);
  instr.opFloatNegate = {
    .value = value,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatNegate.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntAdd(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intAdd);
  instr.opIntAdd = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntAdd.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntSubtract(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intSubtract);
  instr.opIntSubtract = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntSubtract.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntMultiply(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intMultiply);
  instr.opIntMultiply = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntMultiply.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntDivide(
  PuleGpuIr_Shader shader,
  bool isSigned,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intDivide);
  instr.opIntDivide = {
    .isSigned = isSigned,
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntDivide.returnValue;
}

PuleGpuIr_Value puleGpuIr_opIntModulo(
  PuleGpuIr_Shader shader,
  bool isSigned,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::intModulo);
  instr.opIntModulo = {
    .isSigned = isSigned,
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opIntModulo.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatAdd(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatAdd);
  instr.opFloatAdd = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatAdd.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatSubtract(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatSubtract);
  instr.opFloatSubtract = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatSubtract.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatMultiply(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatMultiply);
  instr.opFloatMultiply = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatMultiply.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatDivide(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatDivide);
  instr.opFloatDivide = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatDivide.returnValue;
}

PuleGpuIr_Value puleGpuIr_opFloatRem(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value1,
  PuleGpuIr_Value value2
) {
  PRIV_SHADER_VALUE(priv::Opcode::floatRem);
  instr.opFloatRem = {
    .value1 = value1,
    .value2 = value2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opFloatRem.returnValue;
}

PuleGpuIr_Value puleGpuIr_opVectorMulScalar(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value vector,
  PuleGpuIr_Value scalar
) {
  PRIV_SHADER_VALUE(priv::Opcode::vectorMulScalar);
  instr.opVectorMulScalar = {
    .vector = vector,
    .scalar = scalar,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opVectorMulScalar.returnValue;
}

PuleGpuIr_Value puleGpuIr_opMatrixMulScalar(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value matrix,
  PuleGpuIr_Value scalar
) {
  PRIV_SHADER_VALUE(priv::Opcode::matrixMulScalar);
  instr.opMatrixMulScalar = {
    .matrix = matrix,
    .scalar = scalar,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opMatrixMulScalar.returnValue;
}

PuleGpuIr_Value puleGpuIr_opVectorMulMatrix(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value vector,
  PuleGpuIr_Value matrix
) {
  PRIV_SHADER_VALUE(priv::Opcode::vectorMulMatrix);
  instr.opVectorMulMatrix = {
    .vector = vector,
    .matrix = matrix,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opVectorMulMatrix.returnValue;
}

PuleGpuIr_Value puleGpuIr_opMatrixMulVector(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value matrix,
  PuleGpuIr_Value vector
) {
  PRIV_SHADER_VALUE(priv::Opcode::matrixMulVector);
  instr.opMatrixMulVector = {
    .matrix = matrix,
    .vector = vector,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opMatrixMulVector.returnValue;
}

PuleGpuIr_Value puleGpuIr_opMatrixMulMatrix(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value matrix1,
  PuleGpuIr_Value matrix2
) {
  PRIV_SHADER_VALUE(priv::Opcode::matrixMulMatrix);
  instr.opMatrixMulMatrix = {
    .matrix1 = matrix1,
    .matrix2 = matrix2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opMatrixMulMatrix.returnValue;
}

PuleGpuIr_Value puleGpuIr_opVectorOuterProduct(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value vector1,
  PuleGpuIr_Value vector2
) {
  PRIV_SHADER_VALUE(priv::Opcode::vectorOuterProduct);
  instr.opVectorOuterProduct = {
    .vector1 = vector1,
    .vector2 = vector2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opVectorOuterProduct.returnValue;
}

PuleGpuIr_Value puleGpuIr_opVectorDotProduct(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value vector1,
  PuleGpuIr_Value vector2
) {
  PRIV_SHADER_VALUE(priv::Opcode::vectorDotProduct);
  instr.opVectorDotProduct = {
    .vector1 = vector1,
    .vector2 = vector2,
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opVectorDotProduct.returnValue;
}

void puleGpuIr_opBranchJmp(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value target
) {
  PRIV_SHADER(priv::Opcode::branchJmp);
  instr.opBranchJmp = {
    .target = target,
  };
  privShader.instructions.push_back(instr);
}

void puleGpuIr_opBranchCond(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value condition,
  PuleGpuIr_Value targetTrue,
  PuleGpuIr_Value targetFalse
) {
  PRIV_SHADER(priv::Opcode::branchCond);
  instr.opBranchCond = {
    .condition = condition,
    .targetTrue = targetTrue,
    .targetFalse = targetFalse,
  };
  privShader.instructions.push_back(instr);
}

void puleGpuIr_opReturn(
  PuleGpuIr_Shader shader
) {
  PRIV_SHADER(priv::Opcode::returnVoid);
  instr.opReturnVoid = { };
  privShader.instructions.push_back(instr);
}

void puleGpuIr_opReturnValue(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value value
) {
  PRIV_SHADER(priv::Opcode::returnValue);
  instr.opReturnValue = {
    .value = value,
  };
  privShader.instructions.push_back(instr);
}

PuleGpuIr_Value puleGpuIr_opAccessChain(
  PuleGpuIr_Shader shader,
  PuleGpuIr_Value base,
  PuleGpuIr_Value * indices,
  size_t indexLen
) {
  PRIV_SHADER_VALUE(priv::Opcode::accessChain);
  instr.opAccessChain = {
    .base = base,
    .indices = std::vector<PuleGpuIr_Value>(indices, indices + indexLen),
    .returnValue = returnValue,
  };
  privShader.instructions.push_back(instr);
  return instr.opAccessChain.returnValue;
}

PuleGpuIr_Value puleGpuIr_opLabel(
  PuleGpuIr_Shader shader,
  PuleStringView label
) {
  PRIV_SHADER_VALUE(priv::Opcode::label);
  instr.opLabel = {
    .target = returnValue,
    .label = std::string(label.contents, label.len),
  };
  privShader.instructions.push_back(instr);
  return instr.opLabel.target;
}

} // extern "C"

#undef PRIV_SHADER_VALUE
#undef PRIV_SHADER
