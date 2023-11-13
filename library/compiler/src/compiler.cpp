#include <pulchritude-compiler/compiler.h>

// this is a simple LLVM IR wrapper, which is meant to simplify the API
//   to play with LLVM, and not to be used in any actual code.

#include <pulchritude-error/error.h>

#include <llvm-c/Core.h>

#include <unordered_map>
#include <utility>
#include <vector>

namespace pint {

struct Type {
  LLVMTypeRef type;
  PuleIRDataTypeInfo typeInfo;
};

struct Value {
  PuleIRType type;
  PuleIRValueKind kind;
  LLVMValueRef value;
};

struct ModuleInfo {
  LLVMContextRef context;
  LLVMModuleRef module;
  pule::ResourceContainer<LLVMBuilderRef> builders;
  pule::ResourceContainer<Type> types;
  pule::ResourceContainer<Value> values;
  std::unordered_map<PuleIRDataType, PuleIRType> genericTypes;
};


pule::ResourceContainer<pint::ModuleInfo> modules;
};

extern "C" {

// -- modules ------------------------------------------------------------------

PuleIRModule puleIRModuleCreate(PuleStringView const label) {
  pint::ModuleInfo moduleInfo;
  moduleInfo.context = LLVMContextCreate();
  moduleInfo.module = (
    LLVMModuleCreateWithNameInContext(label.contents, moduleInfo.context)
  );


  PuleIRModule puModule = { .id = pint::modules.create(moduleInfo), };
  auto & module = *pint::modules.at(puModule.id);

  auto & type = module.types;
  auto & genericType = module.genericTypes;
  auto & ctx = module.context;

  puleLog("creating ir module");

  // build all the generic data types
  for (auto & it : { std::pair<LLVMTypeRef (*)(LLVMContextRef), std::pair<PuleIRDataType, char const *>>
    { &LLVMInt8TypeInContext,     { PuleIRDataType_u8, "PuleIRDataType_u8", }, },
    { &LLVMInt16TypeInContext,    { PuleIRDataType_u16, "PuleIRDataType_u16 ", }, },
    { &LLVMInt32TypeInContext,    { PuleIRDataType_u32, "PuleIRDataType_u32 ", }, },
    { &LLVMInt64TypeInContext,    { PuleIRDataType_u64, "PuleIRDataType_u64 ", }, },
    { &LLVMFloatTypeInContext,    { PuleIRDataType_f32, "PuleIRDataType_f32 ", }, },
    { &LLVMDoubleTypeInContext,   { PuleIRDataType_f64, "PuleIRDataType_f64 ", }, },
    { &LLVMInt1TypeInContext,     { PuleIRDataType_bool, "PuleIRDataType_bool ", }, },
    { &LLVMVoidTypeInContext,     { PuleIRDataType_void, "PuleIRDataType_void ", }, },
  }) {
    genericType[it.second.first] = {
      type.create(
        pint::Type {
          .type = it.first(ctx),
          .typeInfo = PuleIRDataTypeInfo {
            .module = puModule, .type = it.second.first, .genericInfo = {},
          },
        }
      )
    };
  }

  genericType[PuleIRDataType_ptr] = {
    type.create(
      pint::Type {
        .type = LLVMPointerTypeInContext(ctx, 0),
        .typeInfo = {
          .module = puModule, .type = PuleIRDataType_ptr, .genericInfo = {},
        },
      }
    )
  };
  genericType[PuleIRDataType_i8]  = genericType[PuleIRDataType_u8];
  genericType[PuleIRDataType_i16] = genericType[PuleIRDataType_u16];
  genericType[PuleIRDataType_i32] = genericType[PuleIRDataType_u32];
  genericType[PuleIRDataType_i64] = genericType[PuleIRDataType_u64];

  return puModule;
}

void puleIRModuleDestroy(PuleIRModule const module) {
  auto moduleInfo = pint::modules.at(module.id);
  for (auto & builder : moduleInfo->builders) {
    LLVMDisposeBuilder(builder.second);
  }
  LLVMDisposeModule(moduleInfo->module);
  LLVMContextDispose(moduleInfo->context);
}

void puleIRModuleDump(PuleIRModule const module, PuleStringView const path) {
  auto moduleInfo = pint::modules.at(module.id);
  LLVMPrintModuleToFile(
    moduleInfo->module,
    path.contents,
    nullptr
  );
}

// -- types --------------------------------------------------------------------

PuleIRType puleIRTypeCreate(PuleIRDataTypeInfo const createInfo) {
  auto moduleInfo = pint::modules.at(createInfo.module.id);
  switch (createInfo.type) {
    default: PULE_assert(false && "TODO");
    case PuleIRDataType_u8:
    case PuleIRDataType_u16:
    case PuleIRDataType_u32:
    case PuleIRDataType_u64:
    case PuleIRDataType_i8:
    case PuleIRDataType_i16:
    case PuleIRDataType_i32:
    case PuleIRDataType_i64:
    case PuleIRDataType_f32:
    case PuleIRDataType_f64:
    case PuleIRDataType_bool:
    case PuleIRDataType_void:
    case PuleIRDataType_ptr:
      return moduleInfo->genericTypes[createInfo.type];
    case PuleIRDataType_function: {
      std::vector<LLVMTypeRef> paramTypes;
      for (size_t i = 0; i < createInfo.fnInfo.parameterCount; ++ i) {
        paramTypes.emplace_back(
          moduleInfo->types.at(createInfo.fnInfo.parameterTypes[i].id)->type
        );
      }
      return {
        moduleInfo->types.create({
          LLVMFunctionType(
            moduleInfo->types.at(createInfo.fnInfo.returnType)->type,
            paramTypes.data(),
            paramTypes.size(),
            createInfo.fnInfo.variadic
          ),
          createInfo,
        })
      };
    }
    case PuleIRDataType_struct: {
      LLVMTypeRef structType = (
        LLVMStructCreateNamed(
          moduleInfo->context,
          createInfo.structInfo.label.contents
        )
      );
      std::vector<LLVMTypeRef> memberTypes;
      for (size_t i = 0; i < createInfo.structInfo.memberCount; ++ i) {
        memberTypes.emplace_back(
          moduleInfo->types.at(createInfo.structInfo.memberTypes[i])->type
        );
      }
      LLVMStructSetBody(
        structType,
        memberTypes.data(),
        memberTypes.size(),
        createInfo.structInfo.packed
      );
      return { moduleInfo->types.create({structType, createInfo,}) };
    }
    case PuleIRDataType_array: {
      return {
        moduleInfo->types.create({
          LLVMArrayType(
            moduleInfo->types.at(createInfo.arrayInfo.elementType)->type,
            createInfo.arrayInfo.elementCount
          ),
          createInfo,
        })
      };
    }
  }
}

PuleIRDataTypeInfo puleIRTypeInfo(
  PuleIRModule const puModule,
  PuleIRType const type
) {
  return pint::modules.at(puModule.id)->types.at(type.id)->typeInfo;
}

// -- values -------------------------------------------------------------------

PuleIRType puleIRValueType(
  PuleIRModule const puModule,
  PuleIRValue const puValue
) {
  pint::Value const & value = (
    *pint::modules.at(puModule.id)->values.at(puValue.id)
  );
  return value.type;
}

PuleIRValueKind puleIRValueKind(
  PuleIRModule const puModule,
  PuleIRValue const puValue
) {
  pint::Value const & value = (
    *pint::modules.at(puModule.id)->values.at(puValue.id)
  );
  return value.kind;
}

PuleIRValue puleIRValueVoid() { return { .id = 0 }; }

// -- code blocks --------------------------------------------------------------

PuleIRCodeBlock puleIRCodeBlockCreate(
  PuleIRCodeBlockCreateInfo const createInfo
) {
  auto moduleInfo = pint::modules.at(createInfo.module.id);
  LLVMBuilderRef const builder = (
    LLVMCreateBuilderInContext(moduleInfo->context)
  );
  LLVMBasicBlockRef const block = (
    LLVMAppendBasicBlockInContext(
      moduleInfo->context,
      moduleInfo->values.at(createInfo.context)->value,
      createInfo.label.contents
    )
  );
  LLVMPositionBuilderAtEnd(builder, block);
  return {
    .id = reinterpret_cast<uint64_t>(moduleInfo->builders.create(builder)), };
}

// -- value creation -----------------------------------------------------------

PuleIRValue puleIRValueConstCreate(
  PuleIRValueConstCreateInfo const createInfo
) {
  auto & module = *pint::modules.at(createInfo.module.id);
  auto & type = *module.types.at(createInfo.type);
  PuleIRDataType puDataType = (
    puleIRTypeInfo(createInfo.module, createInfo.type).type
  );
  if (createInfo.kind == PuleIRValueKind_function) {
    PULE_assert(puDataType == PuleIRDataType_function);
    return {
      module.values.create({
        .type = createInfo.type,
        .kind = PuleIRValueKind_function,
        .value = (
          LLVMAddFunction(
            module.module,
            createInfo.label.contents,
            type.type
          )
        ),
      })
    };
  }
  switch (createInfo.kind) {
    default: PULE_assert(false && "TODO");
    case PuleIRValueKind_constString: {
      PULE_assert(puDataType == PuleIRDataType_ptr);
      return {
        module.values.create({
          .type = createInfo.type,
          .kind = PuleIRValueKind_constString,
          .value = (
            LLVMConstStringInContext(
              module.context,
              createInfo.constString.string.contents,
              createInfo.constString.string.len,
              false
            )
          )
        })
      };
    }
    case PuleIRValueKind_constInteger: {
      switch (puDataType) {
        default:
          PULE_assert(false && "can not create const value of this type");
        case PuleIRDataType_u8:
        case PuleIRDataType_u16:
        case PuleIRDataType_u32:
        case PuleIRDataType_u64:
        case PuleIRDataType_i8:
        case PuleIRDataType_i16:
        case PuleIRDataType_i32:
        case PuleIRDataType_i64: {
          return {
            module.values.create({
              .type = createInfo.type,
              .kind = PuleIRValueKind_dynamic,
              .value = (
                LLVMConstInt(
                  type.type,
                  createInfo.constInteger.u64,
                  createInfo.constInteger.isSigned
                )
              )
            })
          };
        }
      } // switch (createInfo.type)
    }
    case PuleIRValueKind_constFloat: {
      switch (puDataType) {
        default:
          PULE_assert(false && "can not create const value of this type");
        case PuleIRDataType_f16:
        case PuleIRDataType_f32:
        case PuleIRDataType_f64: {
          return {
            module.values.create({
              .type = createInfo.type,
              .kind = PuleIRValueKind_dynamic,
              .value = LLVMConstReal( type.type, createInfo.constFloat.f64)
            })
          };
        }
      } // switch (createInfo.type)
    }
  } // switch (createInfo.kind)

  PULE_assert(false);
  return { .id = 0 };
}

} // extern "C"

// -- code block build ---------------------------------------------------------

namespace pint {

PuleIRValue puleIRBuildReturn(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module.id);
  if (ci.ret.value.id == 0) {
    LLVMBuildRetVoid(module.builders.at(ci.codeBlock.id));
    return puleIRValueVoid();
  }
  return {
    module.values.create(pint::Value {
      .type = puleIRValueType(ci.module, ci.ret.value),
      .kind = puleIRValueKind(ci.module, ci.ret.value),
      .value = (
        LLVMBuildRet(
          module.builders.at(ci.codeBlock.id),
          module.values.at(ci.ret.value.id)->value
        )
      ),
    })
  };
}
PuleIRValue puleIRBuildCall(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module.id);
  // resulting type is the return type of the function
  PuleIRType const returnType = (
    puleIRTypeInfo(
      ci.module, ci.call.functionType
    ).fnInfo.returnType
  );
  std::vector<LLVMValueRef> arguments;
  for (size_t i = 0; i < ci.call.argumentCount; ++ i) {
    arguments.emplace_back(module.values.at(ci.call.arguments[i].id)->value);
  }
  return {
    module.values.create(pint::Value {
      .type = returnType,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildCall2(
          module.builders.at(ci.codeBlock.id),
          module.types.at(ci.call.functionType.id)->type,
          module.values.at(ci.call.function.id)->value,
          arguments.data(),
          arguments.size(),
          ci.call.label.contents
        )
      ),
    })
  };
}

PuleIRValue puleIRBuildAllocate(
  PuleIRBuildCreateInfo const ci
) {
  auto & module = *pint::modules.at(ci.module.id);
  auto & valueSizeInfo = *module.values.at(ci.allocate.valueSize.id);
  if (
    valueSizeInfo.kind != PuleIRValueKind_constInteger
    && valueSizeInfo.kind != PuleIRValueKind_dynamic
  ) {
    PULE_assert(false && "can not allocate with count of this type");
  }
  return {
    module.values.create(pint::Value {
      .type = puleIRTypeCreate({ci.module, PuleIRDataType_ptr, {}}),
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildAlloca(
          module.builders.at(ci.codeBlock.id),
          module.types.at(ci.allocate.type.id)->type,
          ci.allocate.label.contents
        )
      ),
    })
  };
}

void puleIRBuildFree(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module.id);
  LLVMBuildFree(
    module.builders.at(ci.codeBlock.id),
    module.values.at(ci.free.value.id)->value
  );
}

PuleIRValue puleIRBuildLoad(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module.id);
  return {
    module.values.create(pint::Value {
      .type = ci.load.type,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildLoad2(
          module.builders.at(ci.codeBlock.id),
          module.types.at(ci.load.type.id)->type,
          module.values.at(ci.load.value.id)->value,
          ci.load.label.contents
        )
      ),
    })
  };
}

void puleIRBuildStore(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module.id);
  LLVMBuildStore(
    module.builders.at(ci.codeBlock.id),
    module.values.at(ci.store.value.id)->value,
    module.values.at(ci.store.dst.id)->value
  );
}

PuleIRValue puleIRBuildOperation(PuleIRBuildCreateInfo const ci) {
  auto & module = *pint::modules.at(ci.module.id);

  bool const isBinary = (
       ci.op.type != PuleIROperator_neg
    && ci.op.type != PuleIROperator_not
  );

  pint::Value const & lhs = *module.values.at(ci.op.unary.lhs.id);
  pint::Type const & lhsType = *module.types.at(lhs.type.id);

  if (!isBinary) {
    // handle
  }

  bool const isLhsFloatingPoint = (
       lhsType.typeInfo.type == PuleIRDataType_f16
    || lhsType.typeInfo.type == PuleIRDataType_f32
    || lhsType.typeInfo.type == PuleIRDataType_f64
  );
  bool const isLhsSigned = (
       lhsType.typeInfo.type == PuleIRDataType_i8
    || lhsType.typeInfo.type == PuleIRDataType_i16
    || lhsType.typeInfo.type == PuleIRDataType_i32
    || lhsType.typeInfo.type == PuleIRDataType_i64
  );

  PULE_assert(isLhsFloatingPoint && ci.op.floatingPoint);

  pint::Value const & rhs = *module.values.at(ci.op.binary.lhs.id);

  static const std::unordered_map<
    PuleIROperator,
    std::vector<
      LLVMValueRef (*)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, char const *)
    >
  > operatorToBinaryIntFn = {
    { PuleIROperator_add, { LLVMBuildAdd,  LLVMBuildAdd},  },
    { PuleIROperator_sub, { LLVMBuildSub,  LLVMBuildSub},  },
    { PuleIROperator_mul, { LLVMBuildMul,  LLVMBuildMul},  },
    { PuleIROperator_div, { LLVMBuildUDiv, LLVMBuildSDiv}, },
    { PuleIROperator_mod, { LLVMBuildSRem, LLVMBuildSRem}, },
    { PuleIROperator_and, { LLVMBuildAnd,  LLVMBuildAnd},  },
    { PuleIROperator_or,  { LLVMBuildOr,   LLVMBuildOr},   },
    { PuleIROperator_xor, { LLVMBuildXor,  LLVMBuildXor},  },
    { PuleIROperator_shl, { LLVMBuildShl,  LLVMBuildShl},  },
    { PuleIROperator_shr, { LLVMBuildLShr, LLVMBuildLShr}, },
  };

  static const std::unordered_map<
    PuleIROperator,
    LLVMValueRef (*)(LLVMBuilderRef, LLVMValueRef, LLVMValueRef, char const *)
  > operatorToBinaryFloatFn = {
    { PuleIROperator_add, LLVMBuildFAdd, },
    { PuleIROperator_sub, LLVMBuildFSub, },
    { PuleIROperator_mul, LLVMBuildFMul, },
    { PuleIROperator_div, LLVMBuildFDiv, },
    { PuleIROperator_mod, LLVMBuildFRem, },
  };

  bool const isMathematic = (
       ci.op.type == PuleIROperator_add
    || ci.op.type == PuleIROperator_sub
    || ci.op.type == PuleIROperator_mul
    || ci.op.type == PuleIROperator_div
    || ci.op.type == PuleIROperator_mod
    || ci.op.type == PuleIROperator_and
    || ci.op.type == PuleIROperator_or
    || ci.op.type == PuleIROperator_xor
    || ci.op.type == PuleIROperator_shl
    || ci.op.type == PuleIROperator_shr
  );

  if (isMathematic && !ci.op.floatingPoint) {
    return {
      module.values.create(pint::Value {
        .type = lhs.type, // TODO this is wrong, needs to be max
        .kind = PuleIRValueKind_dynamic,
        .value = (
          operatorToBinaryIntFn.at(ci.op.type)[(int32_t)isLhsSigned](
            module.builders.at(ci.codeBlock.id),
            lhs.value,
            rhs.value,
            ci.op.label.contents
          )
        ),
      })
    };
  } else if (isMathematic && ci.op.floatingPoint) {
    PULE_assert(operatorToBinaryFloatFn.contains(ci.op.type));
    return {
      module.values.create(pint::Value {
        .type = lhs.type, // TODO this is wrong, needs to be max
        .kind = PuleIRValueKind_dynamic,
        .value = (
          operatorToBinaryFloatFn.at(ci.op.type)(
            module.builders.at(ci.codeBlock.id),
            lhs.value,
            rhs.value,
            ci.op.label.contents
          )
        ),
      })
    };
  }

  // logical
  LLVMIntPredicate predicateInt;
  LLVMRealPredicate predicateReal;
  switch (ci.op.type) {
    default: PULE_assert(false);
    case PuleIROperator_eq:
      predicateInt = LLVMIntEQ;
      predicateReal = LLVMRealOEQ;
    break;
    case PuleIROperator_ne:
      predicateInt = LLVMIntNE;
      predicateReal = LLVMRealONE;
    break;
    case PuleIROperator_lt:
      predicateInt = LLVMIntSLT;
      predicateReal = LLVMRealOLT;
    break;
    case PuleIROperator_le:
      predicateInt = LLVMIntSLE;
      predicateReal = LLVMRealOLE;
    break;
    case PuleIROperator_gt:
      predicateInt = LLVMIntSGT;
      predicateReal = LLVMRealOGT;
    break;
    case PuleIROperator_ge:
      predicateInt = LLVMIntSGE;
      predicateReal = LLVMRealOGE;
    break;
  }

  auto const returnTypeBool = (
    puleIRTypeCreate({.module = ci.module, .type = PuleIRDataType_bool,})
  );

  if (isLhsFloatingPoint) {
    return {
      module.values.create(pint::Value {
        .type = returnTypeBool,
        .kind = PuleIRValueKind_dynamic,
        .value = (
          LLVMBuildFCmp(
            module.builders.at(ci.codeBlock.id),
            predicateReal,
            lhs.value,
            rhs.value,
            ci.op.label.contents
          )
        ),
      })
    };
  }
  return {
    module.values.create(pint::Value {
      .type = returnTypeBool,
      .kind = PuleIRValueKind_dynamic,
      .value = (
        LLVMBuildICmp(
          module.builders.at(ci.codeBlock.id),
          predicateInt,
          lhs.value,
          rhs.value,
          ci.op.label.contents
        )
      ),
    })
  };
}

} // namespace pint

extern "C" {

PuleIRValue puleIRBuild(PuleIRBuildCreateInfo const createInfo) {
  switch (createInfo.instrType) {
    case PuleIRInstrType_return:
      return pint::puleIRBuildReturn(createInfo);
    case PuleIRInstrType_call:
      return pint::puleIRBuildCall(createInfo);
    case PuleIRInstrType_allocate:
      return pint::puleIRBuildAllocate(createInfo);
    case PuleIRInstrType_free:
      pint::puleIRBuildFree(createInfo);
      return { .id = 0 };
    case PuleIRInstrType_load:
      return pint::puleIRBuildLoad(createInfo);
    case PuleIRInstrType_store:
      pint::puleIRBuildStore(createInfo);
      return { .id = 0 };
    case PuleIRInstrType_operation:
      return pint::puleIRBuildOperation(createInfo);
  }
}

} // extern "C"
