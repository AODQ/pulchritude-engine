#pragma once

// it's just a wrapper around LLVM

#include <pulchritude-core/core.h>
#include <pulchritude-string/string.h>

// -- modules ------------------------------------------------------------------

typedef struct { size_t id; } PuleIRModule;

typedef struct PuleIRModuleCreateInfo {
  PuleStringView label;
  PuleStringView ir PULE_param({}); // will load the IR into module if not null
} PuleIRModuleCreateInfo;
PuleIRModule puleIRModuleCreate(PuleIRModuleCreateInfo const createInfo);
void puleIRModuleDestroy(PuleIRModule const module);

void * puleIRModuleJITFunctionAddress(
  PuleIRModule const module,
  PuleStringView const functionName
);

void puleIRModuleVerify(PuleIRModule const module);
void puleIRModuleOptimize(PuleIRModule const module);

void puleIRModuleDump(PuleIRModule const module);

// -- data types ---------------------------------------------------------------

typedef enum {
  PuleIRDataType_u8,
  PuleIRDataType_u16,
  PuleIRDataType_u32,
  PuleIRDataType_u64,
  PuleIRDataType_i8,
  PuleIRDataType_i16,
  PuleIRDataType_i32,
  PuleIRDataType_i64,
  PuleIRDataType_f16,
  PuleIRDataType_f32,
  PuleIRDataType_f64,
  PuleIRDataType_bool,
  PuleIRDataType_void,
  PuleIRDataType_ptr,
  PuleIRDataType_array,
  PuleIRDataType_struct,
  PuleIRDataType_function,
} PuleIRDataType;

typedef struct { uint64_t id; } PuleIRType;

bool puleIRDataTypeIsInt(PuleIRDataType const type);
bool puleIRDataTypeIsFloat(PuleIRDataType const type);
bool puleIRDataTypeIsSigned(PuleIRDataType const type);
uint32_t puleIRDataTypeByteCount(PuleIRDataType const type);

// -- types --------------------------------------------------------------------

typedef struct {
  PuleIRModule module;
  PuleIRDataType type;
  size_t byteCount;
  union {
    struct {
      PuleIRType const * memberTypes;
      size_t memberCount;
      bool packed;
      PuleStringView label;
    } structInfo;
    struct {
      PuleIRType returnType;
      size_t parameterCount;
      PuleIRType const * parameterTypes;
      bool variadic;
    } fnInfo;
    struct {
      PuleIRType elementType;
      size_t elementCount;
    } arrayInfo;
    struct { uint8_t unused; } genericInfo;
  };
} PuleIRDataTypeInfo;
PuleIRType puleIRTypeCreate(PuleIRDataTypeInfo const createInfo);

PuleIRDataTypeInfo puleIRTypeInfo(PuleIRModule const m, PuleIRType const type);

// -- values -------------------------------------------------------------------

typedef enum {
  PuleIRValueKind_function,
  PuleIRValueKind_constInt,
  PuleIRValueKind_constFloat,
  PuleIRValueKind_constBuffer,
  PuleIRValueKind_constString,
  PuleIRValueKind_constArray,
  PuleIRValueKind_dynamic,
} PuleIRValueKind;

typedef struct { uint64_t id; } PuleIRValue;
PuleIRType puleIRValueType(PuleIRModule const m, PuleIRValue const value);
PuleIRValueKind puleIRValueKind(PuleIRModule const m, PuleIRValue const value);
void puleIRValueDump(PuleIRModule const module, PuleIRValue const value);

PuleIRValue puleIRValueVoid();

// -- code blocks --------------------------------------------------------------

typedef struct { uint64_t id; } PuleIRCodeBlock;

typedef struct {
  PuleIRModule module;
  PuleIRValue context;
  bool positionBuilderAtEnd;
  PuleStringView label;
} PuleIRCodeBlockCreateInfo;
PULE_exportFn PuleIRCodeBlock puleIRCodeBlockCreate(
  PuleIRCodeBlockCreateInfo const createInfo
);

PULE_exportFn PuleIRCodeBlock puleIRCodeBlockLast(
  PuleIRModule const module,
  PuleIRValue const fnValue
);

PULE_exportFn void puleIRCodeBlockMoveAfter(
  PuleIRModule const module,
  PuleIRCodeBlock const codeBlock,
  PuleIRCodeBlock const moveTarget
);
typedef enum {
  PuleIRCodeBlockTerminateType_ret,
} PuleIRCodeBlockTerminateType;

typedef struct {
  PuleIRModule module;
  PuleIRCodeBlock codeBlock;
  PuleIRCodeBlockTerminateType type;
  union {
    struct {
      PuleIRValue value; // if id is 0, that is equivalent to void
    } ret;
  };
} PuleIRCodeBlockTerminateCreateInfo;

void puleIRCodeBlockPositionBuilderAtEnd(
  PuleIRModule const module,
  PuleIRCodeBlock const codeBlock
);

// TODO ensure code block is always terminated
void puleIRCodeBlockTerminate(
  PuleIRCodeBlockTerminateCreateInfo const createInfo
);

// -- const value creation -----------------------------------------------------

typedef enum {
  PuleIRLinkage_none,
  PuleIRLinkage_external,
} PuleIRLinkage;

typedef struct {
  PuleIRModule module;
  PuleIRType type;
  PuleIRValueKind kind;
  PuleStringView label;
  union {
    struct {
      PuleIRLinkage linkage PULE_param(PuleIRLinkage_none);
    } function;
    struct {
      PuleIRCodeBlock codeBlock;
      uint64_t u64;
      bool isSigned;
    } constInt;
    struct {
      PuleIRCodeBlock codeBlock;
      double f64;
    } constFloat;
    struct {
      PuleIRCodeBlock codeBlock;
      uint8_t const * data;
      size_t size;
    } constBuffer;
    struct {
      PuleIRCodeBlock codeBlock;
      PuleStringView string;
    } constString;
    struct {
      PuleIRCodeBlock codeBlock;
      size_t valueCount;
      PuleIRValue const * values;
    } constArray;
  };
} PuleIRValueConstCreateInfo;
PuleIRValue puleIRValueConstCreate(PuleIRValueConstCreateInfo const createInfo);

// function variables
size_t puleIRValueFunctionParameterCount(
  PuleIRModule const m,
  PuleIRValue const fnValue
);
PuleIRValue puleIRValueFunctionParameter(
  PuleIRModule const m,
  PuleIRValue const fnValue,
  size_t const index
);

// convenience value functions
PuleIRValue puleIRValueConstI64(PuleIRModule const m, int64_t const i64);

// -- code block build ---------------------------------------------------------

typedef struct {
  PuleIRModule module;
  PuleIRCodeBlock codeBlock;
  PuleIRValue value;
} PuleIRBuildReturnCreateInfo;
PuleIRValue puleIRBuildReturn(PuleIRBuildReturnCreateInfo const createInfo);

typedef enum {
  PuleIROperator_add,
  PuleIROperator_sub,
  PuleIROperator_mul,
  PuleIROperator_div,
  PuleIROperator_mod,
  PuleIROperator_and,
  PuleIROperator_or,
  PuleIROperator_xor,
  PuleIROperator_bit_and,
  PuleIROperator_bit_or,
  PuleIROperator_bit_xor,
  PuleIROperator_shl,
  PuleIROperator_shr,
  PuleIROperator_eq,
  PuleIROperator_ne,
  PuleIROperator_lt,
  PuleIROperator_le,
  PuleIROperator_gt,
  PuleIROperator_ge,
  PuleIROperator_neg,
  PuleIROperator_not,
} PuleIROperator;

typedef enum {
  PuleIRInstrType_return,
  PuleIRInstrType_call,
  PuleIRInstrType_stackAlloc,
  PuleIRInstrType_free,
  PuleIRInstrType_load,
  PuleIRInstrType_store,
  PuleIRInstrType_operation,
  PuleIRInstrType_branch,
  PuleIRInstrType_trunc,
  PuleIRInstrType_cast,
  PuleIRInstrType_switch,
} PuleIRInstrType;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_return);
  PuleIRValue value;
} PuleIRInstrReturn;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_call);
  PuleIRType functionType;
  PuleIRValue function;
  size_t argumentCount;
  PuleIRValue const * arguments;
  PuleStringView label;
} PuleIRInstrCall;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_stackAlloc);
  PuleIRType type;
  PuleIRValue valueSize;
  PuleStringView label;
} PuleIRInstrStackAlloc;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_free);
  PuleIRValue value;
} PuleIRInstrFree;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_load);
  PuleIRType type;
  PuleIRValue value;
  PuleStringView label;
} PuleIRInstrLoad;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_store);
  PuleIRValue value;
  PuleIRValue dst;
} PuleIRInstrStore;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_operation);
  PuleIROperator type;
  union {
    struct {
      PuleIRValue lhs;
    } unary;
    struct {
      PuleIRValue lhs;
      PuleIRValue rhs;
    } binary;
  };
  PuleStringView label;
  bool floatingPoint PULE_param(false);
} PuleIRInstrOp;

typedef struct PuleIRInstrBranch {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_branch);
  bool isConditional;
  union {
    struct {
      PuleIRValue condition;
      PuleIRCodeBlock codeBlockTrue;
      PuleIRCodeBlock codeBlockFalse;
    } conditional;
    struct {
      PuleIRCodeBlock codeBlock;
    } unconditional;
  };
} PuleIRInstrBranch;

typedef struct PuleIRInstrTrunc {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_trunc);
  PuleIRValue value;
  PuleIRType type;
  PuleStringView label;
} PuleIRInstrTrunc;

typedef enum {
  PuleIRCast_intTrunc,
  PuleIRCast_uintExtend,
  PuleIRCast_intExtend,
  PuleIRCast_floatToUint,
  PuleIRCast_floatToInt,
  PuleIRCast_uintToFloat,
  PuleIRCast_intToFloat,
  PuleIRCast_floatTrunc,
  PuleIRCast_floatExtend,
} PuleIRCast;

PuleIRCast puleIRCastTypeFromDataTypes(
  PuleIRDataType const from,
  PuleIRDataType const to
);

typedef struct PuleIRInstrCast {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_cast);
  PuleIRCast castType;
  PuleIRValue value;
  PuleIRType type;
  PuleStringView label;
} PuleIRInstrCast;

typedef struct PuleIrInstrSwitchCase {
  PuleIRValue value;
  PuleIRCodeBlock codeBlock;
} PuleIrInstrSwitchCase;

typedef struct PuleIRInstrSwitch {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_switch);
  PuleIRValue value;
  PuleIRCodeBlock codeBlockDefault;
  size_t caseCount;
  PuleIrInstrSwitchCase const * cases;
} PuleIRInstrSwitch;

typedef union PuleIRBuildInstrUnion {
  PuleIRInstrType instrType;
  PuleIRInstrReturn ret;
  PuleIRInstrCall call;
  PuleIRInstrStackAlloc stackAlloc;
  PuleIRInstrFree free;
  PuleIRInstrLoad load;
  PuleIRInstrStore store;
  PuleIRInstrOp op;
  PuleIRInstrBranch branch;
  PuleIRInstrTrunc trunc;
  PuleIRInstrCast cast;
  PuleIRInstrSwitch switch_;

  // getelementptr TODO what's the point of the type? Pointers are stored as
  // opaque values, so this would only work if if type is a struct or array,
  // or pointer to struct or array. If it's a pointer to a pointer to a struct
  // or array, then it would require two GEPs (one to get the pointer, and one
  // to get the struct or array). I think I'll just leave this out for now.
  // but isn't here buildStructGEP which handles one of the cases already?
} PuleIRBuildInstrUnion;

typedef struct PuleIRBuildCreateInfo {
  PuleIRModule module;
  PuleIRCodeBlock codeBlock;
  PuleIRBuildInstrUnion instr;
} PuleIRBuildCreateInfo;
PuleIRValue puleIRBuild(PuleIRBuildCreateInfo const createInfo);
