#pragma once

// this is by far the worst library in this engine, it's not really useful
// to anyone besides myself as a fun side-project. When the engine is capable
// of third party plugin integration, this will be the first library to be
// removed.

// This really exists because I want to use LLVM, however with introduction
// of opaque pointers and the large scope of project, I feel like the API can be
// simplified by quite a bit.

#include <pulchritude-core/core.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

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

void puleIRModuleDump(PuleIRModule const module);

// -- jit engine ---------------------------------------------------------------

typedef struct { size_t id; } PuleIRJitEngine;
typedef struct PuleIRJitEngineCreateInfo {
  bool optimize PULE_param(true);
  bool insertEngineSymbols PULE_param(true);
} PuleIRJitEngineCreateInfo;
PULE_exportFn PuleIRJitEngine puleIRJitEngineCreate(
  PuleIRJitEngineCreateInfo const ci
);
PULE_exportFn void puleIRJitEngineDestroy(PuleIRJitEngine const jitEngine);

PULE_exportFn void puleIRJitEngineAddModule(
  PuleIRJitEngine const jitEngine,
  PuleIRModule const module
);

PULE_exportFn void * puleIRJitEngineFunctionAddress(
  PuleIRJitEngine const jitEngine,
  PuleStringView const functionName
);

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

// -- types --------------------------------------------------------------------

typedef struct {
  PuleIRModule module;
  PuleIRDataType type;
  union {
    struct {
      PuleIRType const * memberTypes;
      size_t memberCount;
      bool packed;
      PuleStringView label;
    } structInfo;
    struct {
      PuleIRType returnType;
      PuleIRType const * parameterTypes;
      size_t parameterCount;
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
  PuleIRValueKind_constInteger,
  PuleIRValueKind_constFloat,
  PuleIRValueKind_constBuffer,
  PuleIRValueKind_constString,
  PuleIRValueKind_constArray,
  PuleIRValueKind_dynamic,
} PuleIRValueKind;

typedef struct { uint64_t id; } PuleIRValue;
PuleIRType puleIRValueType(PuleIRModule const m, PuleIRValue const value);
PuleIRValueKind puleIRValueKind(PuleIRModule const m, PuleIRValue const value);

PuleIRValue puleIRValueVoid();

// -- code blocks --------------------------------------------------------------

typedef struct { uint64_t id; } PuleIRCodeBlock;

typedef struct {
  PuleIRModule module;
  PuleIRValue context;
  PuleStringView label;
} PuleIRCodeBlockCreateInfo;
PuleIRCodeBlock puleIRCodeBlockCreate(
  PuleIRCodeBlockCreateInfo const createInfo
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
    } constInteger;
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
      PuleIRValue const * values;
      size_t valueCount;
    } constArray;
  };
} PuleIRValueConstCreateInfo;
PuleIRValue puleIRValueConstCreate(PuleIRValueConstCreateInfo const createInfo);

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
  PuleIRInstrType_allocate,
  PuleIRInstrType_free,
  PuleIRInstrType_load,
  PuleIRInstrType_store,
  PuleIRInstrType_operation,
} PuleIRInstrType;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_return);
  PuleIRValue value;
} PuleIRInstrReturn;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_call);
  PuleIRType functionType;
  PuleIRValue function;
  PuleIRValue const * arguments;
  size_t argumentCount;
  PuleStringView label;
} PuleIRInstrCall;

typedef struct {
  PuleIRInstrType instrType PULE_param(PuleIRInstrType_allocate);
  PuleIRType type;
  PuleIRValue valueSize;
  PuleStringView label;
} PuleIRInstrAllocate;

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

typedef struct PuleIRBuildCreateInfo {
  PuleIRModule module;
  PuleIRCodeBlock codeBlock;
  union {
    PuleIRInstrType instrType;
    PuleIRInstrReturn ret;
    PuleIRInstrCall call;
    PuleIRInstrAllocate allocate;
    PuleIRInstrFree free;
    PuleIRInstrLoad load;
    PuleIRInstrStore store;
    PuleIRInstrOp op;

    // getelementptr TODO what's the point of the type? Pointers are stored as
    // opaque values, so this would only work if if type is a struct or array,
    // or pointer to struct or array. If it's a pointer to a pointer to a struct
    // or array, then it would require two GEPs (one to get the pointer, and one
    // to get the struct or array). I think I'll just leave this out for now.
    // but isn't here buildStructGEP which handles one of the cases already?
  };
} PuleIRBuildCreateInfo;
PuleIRValue puleIRBuild(PuleIRBuildCreateInfo const createInfo);

#ifdef __cplusplus
} // extern "C"
#endif
