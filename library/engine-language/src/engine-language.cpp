#include <pulchritude-engine-language/engine-language.h>

#include "parser.hpp"

#include <pulchritude-parser/parser.h>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct CodeBlock {
  PuleIRCodeBlock block;
  bool hasTerminated;
};

struct Ctx {
  PuleIRModule module;
  PuleIRType typeVoid;
  PuleIRType typeU64;
  PuleIRType typeI64;
  PuleIRValue currentFunction;
  PuleIRValue currentFunctionReturnValue;
  std::vector<CodeBlock> codeBlockStack;
  PuleIRCodeBlock currentFunctionReturnBlock;
  // TODO probably use a vector of maps to handle scopes
  std::unordered_map<std::string, PuleIRType> globalTypes;
  // TODO again, use a vector of maps to handle scopes
  // TODO this is temp but should be a vector of maps, the first being global
  //      (at least to scope of a module), second being function,
  //      then the rest nested blocks
  std::unordered_map<std::string, PuleIRValue> gVariables; // TODO const&group
  std::unordered_map<std::string, PuleIRValue> variables; // TODO const&group
  std::unordered_map<std::string, PuleIRType> variableTypes;
};

};

namespace pint {

struct TypeValue {
  bool isType;
  union {
    PuleIRType t;
    PuleIRValue v;
  };
};

PuleIRValue irBuild(Ctx & ctx, PuleIRBuildInstrUnion const & ci) {
  if (ctx.codeBlockStack.back().hasTerminated) {
    return { .id = 0, };
  }
  bool const hasUnconditionalBranch = (
       (ci.instrType == PuleIRInstrType_return)
    || (
         ci.instrType == PuleIRInstrType_branch
      && ci.branch.isConditional == false
    )
  );
  if (hasUnconditionalBranch) {
    ctx.codeBlockStack.back().hasTerminated = true;
  }
  return (
    puleIRBuild({
      .module = ctx.module,
      .codeBlock = ctx.codeBlockStack.back().block,
      .instr = ci,
    })
  );
}

TypeValue traverseAstExpression(
  Ctx & ctx,
  PuleParserAstNode const expr,
  PuleIRDataType const implicitType
);

PuleIRType findType(Ctx const & ctx, std::string const & label) {
  if (ctx.globalTypes.find(label) != ctx.globalTypes.end()) {
    return ctx.globalTypes.at(label);
  } else {
    return { .id = 0, };
  }
}

PuleIRValue findValue(Ctx const & ctx, std::string const & label) {
  if (ctx.variables.find(label) != ctx.variables.end()) {
    return ctx.variables.at(label);
  } else {
    if (ctx.gVariables.find(label) != ctx.gVariables.end()) {
      return ctx.gVariables.at(label);
    }
    puleLogError("could not find value: %s", label.c_str());
    PULE_assert(false);
  }
}

PuleIRType findValueType(Ctx const & ctx, std::string const & label) {
  if (ctx.variables.find(label) != ctx.variables.end()) {
    return ctx.variableTypes.at(label);
  } else {
    if (ctx.gVariables.find(label) != ctx.gVariables.end()) {
      return ctx.variableTypes.at(label);
    }
    puleLogError("could not find value: %s", label.c_str());
    PULE_assert(false);
  }
}

PuleIRValue loadValue(Ctx & ctx, std::string const & label) {
  if (ctx.variables.find(label) != ctx.variables.end()) {
    return (
      irBuild(ctx, {
        .load = {
          .type = findValueType(ctx, label), // TODO underlying type
          .value = ctx.variables.at(label),
          .label = puleCStr(""),
        }
      })
    );
  } else {
    if (ctx.gVariables.find(label) != ctx.gVariables.end()) {
      return (
        irBuild(ctx, {
          .load = {
            .type = findValueType(ctx, label), // TODO underlying type
            .value = ctx.gVariables.at(label),
            .label = puleCStr(""),
          }
        })
      );
    }
    puleLogError("could not find value: %s", label.c_str());
    PULE_assert(false);
  }
}

void storeValue(Ctx & ctx, std::string const & label, PuleIRValue value) {
  if (ctx.variables.find(label) != ctx.variables.end()) {
    irBuild(ctx, {
      .store = {
        .value = value,
        .dst = ctx.variables.at(label),
      },
    });
  } else {
    // can't store to global
    puleLogError("could not find value: %s", label.c_str());
    PULE_assert(false);
  }
}

struct Type {
  // for function: return type, parameter types
  // for struct: member types
  // for variable: type modifiers
  std::vector<PuleIRType> types;

  PuleIRType irType() const {
    PULE_assert(types.size() > 0);
    return types.back();
  }

  PuleIRType underlyingIrType() const {
    PULE_assert(types.size() > 0);
    if (types.size() == 1) {
      return types.back();
    }
    return types[types.size() - 2];
  }
};

bool ruleTypeIsVariadic(PuleParserAstNode const ruleType) {
  // %type := %identifier %typeModifier*;
  PULE_assert(ruleType.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(ruleType.match, "type"));
  PuleParserAstNode const typeIdentifier = puleParserAstNodeChild(ruleType, 0);
  PULE_assert(typeIdentifier.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(typeIdentifier.match, "identifier"));
  PuleStringView const identifierLabel = (
    puleParserAstNodeChild(typeIdentifier, 0).match
  );
  PuleParserAstNode const typeModifier = puleParserAstNodeChild(ruleType, 1);
  if (typeModifier.childCount > 0) {
    // variadic can never have type modifiers
    PULE_assert(!puleStringViewEqCStr(identifierLabel, "varargs"));
    return false;
  }
  return puleStringViewEqCStr(identifierLabel, "varargs");
}

pint::Type createTypeFromRuleType(
  Ctx & ctx, PuleParserAstNode const ruleType
) {
  // %type := %identifier %typeModifier*;
  PuleStringView const identifierSv = (
    puleParserAstNodeChild(puleParserAstNodeChild(ruleType, 0), 0).match
  );

  std::string identifierStr = std::string(identifierSv.contents);
  pint::Type type;
  type.types.emplace_back(findType(ctx, identifierSv.contents));

  PuleParserAstNode const typeModifier = puleParserAstNodeChild(ruleType, 1);
  PULE_assert(typeModifier.type == PuleParserNodeType_sequence);
  for (size_t it = 0; it < typeModifier.childCount; ++ it) {
    PuleParserAstNode const modifier = puleParserAstNodeChild(typeModifier, it);
    PULE_assert(modifier.type == PuleParserNodeType_rule);
    PuleStringView const modifierLabel = (
      puleParserAstNodeChild(modifier, 0).match
    );
    if (puleStringViewEqCStr(modifierLabel, "ptr")) {
      type.types.emplace_back(
        puleIRTypeCreate({
          .module = ctx.module,
          .type = PuleIRDataType_ptr,
        })
      );
      // TODO handle const
    } else {
      puleLogError("unknown type modifier: %s", modifierLabel.contents);
      PULE_assert(false);
    }
  }

  return type;
}

PuleIRType fetchPrimitiveTypeFromRuleIdentifier(
  Ctx & ctx, PuleParserAstNode const identifier
) {
  // %identifier := '[a-zA-Z_][a-zA-Z0-9_]*';
  PuleStringView const identifierSv = (
    puleParserAstNodeChild(identifier, 0).match
  );

  return findType(ctx, identifierSv.contents);
}

void traverseAstStatementBlock(Ctx & ctx, PuleParserAstNode const statement);

// %metacall := '@' %identifier '\(' %argument* '\)';
PuleIRValue traverseAstMetacall(Ctx & ctx, PuleParserAstNode const metacall) {
  PULE_assert(metacall.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(metacall.match, "metacall"));
  PuleParserAstNode const metaFn = puleParserAstNodeChild(metacall, 1);
  PuleParserAstNode const metaArgs = puleParserAstNodeChild(metacall, 3);

  PuleStringView const metaFnLabel = (
    puleParserAstNodeChild(metaFn, 0).match
  );

  // these can do all sorts of things, so pass in the nodes before traversing
  if (puleStringViewEqCStr(metaFnLabel, "cast")) {
    PULE_assert(metaArgs.childCount == 2);
    PuleParserAstNode const metaArgType = puleParserAstNodeChild(metaArgs, 0);
    PuleParserAstNode const metaArgValue = puleParserAstNodeChild(metaArgs, 1);

    // get type/value
    TypeValue const type = (
      traverseAstExpression(
        ctx, puleParserAstNodeChild(metaArgType, 0), PuleIRDataType_i64
      )
    );
    PULE_assert(type.isType == true);
    TypeValue value = (
      traverseAstExpression(
        ctx, puleParserAstNodeChild(metaArgValue, 0), PuleIRDataType_i64
      )
    );
    PULE_assert(value.isType == false);

    PuleIRDataType const srcDatatype = (
      puleIRTypeInfo(ctx.module, puleIRValueType(ctx.module, value.v)).type
    );
    PuleIRDataType const dstDatatype = (
      puleIRTypeInfo(ctx.module, type.t).type
    );

    if (srcDatatype == dstDatatype) {
      return value.v;
    }
    puleLog("cast from %d to %d", srcDatatype, dstDatatype);
    if (
      !puleIRDataTypeIsInt(srcDatatype) && !puleIRDataTypeIsFloat(srcDatatype)
    ) {
      PULE_assert(false && "cannot cast to/from non-int/float");
    }
    if (
      !puleIRDataTypeIsInt(dstDatatype) && !puleIRDataTypeIsFloat(dstDatatype)
    ) {
      PULE_assert(false && "cannot cast to/from non-int/float");
    }

    // cast to type if appropiate
    return (
      irBuild(ctx, {
        .cast = {
          .castType = puleIRCastTypeFromDataTypes(srcDatatype, dstDatatype),
          .value = value.v,
          .type = type.t,
          .label = puleCStr(""),
        },
      })
    );
  }
  return { .id = 0, };
}

void traverseAstCodeblock(Ctx & ctx, PuleParserAstNode const block);

// %literal := (%string | %integer | %float | %boolean | %metacall |)
PuleIRValue traverseAstLiteral(
  Ctx & ctx, PuleParserAstNode const literal, PuleIRDataType const implicitType
) {
  PULE_assert(literal.type == PuleParserNodeType_rule);
  PULE_assert(literal.childCount == 1);
  PULE_assert(puleStringViewEqCStr(literal.match, "literal"));
  PuleParserAstNode const literalValue = puleParserAstNodeChild(literal, 0);
  PULE_assert(literalValue.type == PuleParserNodeType_rule);
  if (puleStringViewEqCStr(literalValue.match, "integer")) {
    PuleParserAstNode const integer = puleParserAstNodeChild(literalValue, 0);
    PULE_assert(integer.type == PuleParserNodeType_regex);
    std::string integerValue = std::string(integer.match.contents);
    // if implicit type is void (e.g. varargs) then assume i64
    PuleIRDataType const implicitIntType = (
      implicitType == PuleIRDataType_void ? PuleIRDataType_i64 : implicitType
    );
    return (
      puleIRValueConstCreate({
        .module = ctx.module,
        .type = puleIRTypeCreate({
          .module = ctx.module,
          .type = implicitIntType,
        }),
        .kind = PuleIRValueKind_constInt,
        .label = puleCStr(""),
        .constInt = {
          .codeBlock = ctx.codeBlockStack.back().block,
          .u64 = (uint64_t)std::stoi(integerValue),
          .isSigned = true,
        },
      })
    );
  } else if (puleStringViewEqCStr(literalValue.match, "string")) {
    PuleParserAstNode const str = puleParserAstNodeChild(literalValue, 0);
    PULE_assert(str.type == PuleParserNodeType_regex);
    std::string strValue = std::string(str.match.contents);
    // cut off quotes
    strValue = strValue.substr(1, strValue.size() - 2);
    return (
      puleIRValueConstCreate({
        .module = ctx.module,
        .type = puleIRTypeCreate({
          .module = ctx.module,
          .type = PuleIRDataType_ptr,
        }),
        .kind = PuleIRValueKind_constString,
        .label = puleCStr(""),
        .constString = {
          .codeBlock = ctx.codeBlockStack.back().block,
          .string = puleCStr(strValue.c_str()),
        },
      })
    );
  } else if (puleStringViewEqCStr(literalValue.match, "float")) {
    PULE_assert(false && "TODO");
  } else if (puleStringViewEqCStr(literalValue.match, "boolean")) {
    PuleParserAstNode const boolean = puleParserAstNodeChild(literalValue, 0);
    PULE_assert(boolean.type == PuleParserNodeType_regex);
    return (
      puleIRValueConstCreate({
        .module = ctx.module,
        .type = puleIRTypeCreate({
          .module = ctx.module,
          .type = PuleIRDataType_bool,
        }),
        .kind = PuleIRValueKind_constInt,
        .label = puleCStr(""),
        .constInt = {
          .codeBlock = ctx.codeBlockStack.back().block,
          .u64 = puleStringViewEqCStr(boolean.match, "true"),
          .isSigned = false,
        },
      })
    );
  } else if (puleStringViewEqCStr(literalValue.match, "metacall")) {
    return traverseAstMetacall(ctx, literalValue);
  } else if (puleStringViewEqCStr(literalValue.match, "call")) {
    PULE_assert(false && "TODO");
  } else {
    puleLogError("unknown literal: %s", literalValue.match.contents);
    PULE_assert(false);
  }
}

// %expression_parens := '\(' %expression '\)';
TypeValue traverseAstExpressionParens(
  Ctx & ctx, PuleParserAstNode const expr, PuleIRDataType const implicitType
) {
  PULE_assert(expr.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(expr.match, "expression_parens"));
  PULE_assert(expr.childCount == 3);
  PuleParserAstNode const exprChild = puleParserAstNodeChild(expr, 1);
  return traverseAstExpression(ctx, exprChild, implicitType);
}

// %expression_unary :=
//   '-'? (%expression_parens | %literal | %identifier | %type |);
TypeValue traverseAstExpressionUnary(
  Ctx & ctx, PuleParserAstNode const expr, PuleIRDataType const implicitType
) {
  PULE_assert(expr.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(expr.match, "expression_unary"));
  PuleParserAstNode const exprNeg = puleParserAstNodeChild(expr, 0);
  PULE_assert(exprNeg.type == PuleParserNodeType_regex);
  PuleParserAstNode const exprChild = puleParserAstNodeChild(expr, 1);
  TypeValue value;
  value.isType = false;
  if (puleStringViewEqCStr(exprChild.match, "identifier")) {
    PuleParserAstNode const identifier = (
      puleParserAstNodeChild(exprChild, 0)
    );
    // this could be a type or value (if it didn't have type modifiers)
    PuleIRType type = findType(ctx, identifier.match.contents);
    if (type.id == 0) {
      value.v = loadValue(ctx, identifier.match.contents);
    } else {
      value.isType = true;
      value.t = type;
    }
  } else if (puleStringViewEqCStr(exprChild.match, "literal")) {
    value.v = traverseAstLiteral(ctx, exprChild, implicitType);
  } else if (puleStringViewEqCStr(exprChild.match, "expression_parens")) {
    value = traverseAstExpressionParens(ctx, exprChild, implicitType);
  } else if (puleStringViewEqCStr(exprChild.match, "type")) {
    value.isType = true;
    value.t = createTypeFromRuleType(ctx, exprChild).irType();
  } else {
    puleLogError("unknown unary value: %s", exprChild.match.contents);
    PULE_assert(false);
  }

  // apply negation
  bool const isNegated = puleStringViewEqCStr(exprNeg.match, "-");
  PULE_assert(value.isType ? !isNegated : true);
  if (!value.isType && isNegated) {
    PuleIRDataTypeInfo typeInfo = (
      puleIRTypeInfo(ctx.module, puleIRValueType(ctx.module, value.v))
    );
    PULE_assert(
         typeInfo.type != PuleIRDataType_void
      && typeInfo.type != PuleIRDataType_function
      && typeInfo.type != PuleIRDataType_ptr
      && typeInfo.type != PuleIRDataType_bool
      && typeInfo.type != PuleIRDataType_array
      && typeInfo.type != PuleIRDataType_struct
      && "cannot negate this type"
    );
    bool const isFloating = puleIRDataTypeIsFloat(typeInfo.type);
    PuleIRValueConstCreateInfo constCi = {
      .module = ctx.module,
      .type = puleIRValueType(ctx.module, value.v),
      .kind = (
        isFloating ? PuleIRValueKind_constFloat : PuleIRValueKind_constInt
      ),
      .label = puleCStr(""),
      .constInt = {
        .codeBlock = ctx.codeBlockStack.back().block,
        .u64 = 0,
        .isSigned = true,
      },
    };
    if (isFloating) {
      constCi.constFloat.f64 = 0.0;
    } else {
      constCi.constInt.u64 = 0;
    }
    PuleIRValue const zeroValue = puleIRValueConstCreate(constCi);
    value.v = (
      irBuild(ctx, {
        .op = {
          .type = PuleIROperator_sub,
          .binary = {
            .lhs = zeroValue,
            .rhs = value.v,
          },
          .label = puleCStr(""),
          .floatingPoint = isFloating,
        },
      })
    );
  }

  return value;
}

struct TraverseTreeExpressionOperatorInfo {
  std::string expressionName;
  std::string tailName;
  TraverseTreeExpressionOperatorInfo * next;
};
TypeValue traverseAstExpressionOperator(
  Ctx & ctx,
  PuleParserAstNode const expr,
  TraverseTreeExpressionOperatorInfo const & info,
  PuleIRDataType implicitType
);

bool isSameSign(PuleIRDataType const lhs, PuleIRDataType const rhs) {
  switch (lhs) {
    case PuleIRDataType_u8:
    case PuleIRDataType_u16:
    case PuleIRDataType_u32:
    case PuleIRDataType_u64:
      return (
           rhs == PuleIRDataType_u8
        || rhs == PuleIRDataType_u16
        || rhs == PuleIRDataType_u32
        || rhs == PuleIRDataType_u64
      );
    case PuleIRDataType_i8:
    case PuleIRDataType_i16:
    case PuleIRDataType_i32:
    case PuleIRDataType_i64:
      return (
           rhs == PuleIRDataType_i8
        || rhs == PuleIRDataType_i16
        || rhs == PuleIRDataType_i32
        || rhs == PuleIRDataType_i64
      );
    default:
      return true;
  }
}

bool canUpcast(PuleIRDataType const src, PuleIRDataType const dst) {
  if (src == dst) {
    return true;
  }
  if (puleIRDataTypeIsFloat(src) && puleIRDataTypeIsFloat(dst)) {
    switch (src) {
      default: PULE_assert(false);
      case PuleIRDataType_f16: return true; // can always upcast
      case PuleIRDataType_f32: return dst == PuleIRDataType_f64;
      case PuleIRDataType_f64: return false; // can never upcast
    }
  }
  if (puleIRDataTypeIsFloat(src) && puleIRDataTypeIsInt(dst)) {
    return false;
  }
  if (puleIRDataTypeIsInt(src) && puleIRDataTypeIsFloat(dst)) {
    // TODO can't do for now
    return false;
  }
  if (!puleIRDataTypeIsInt(src) || !puleIRDataTypeIsInt(dst)) {
    return false;
  }
  switch (src) {
    default: PULE_assert(false);
    case PuleIRDataType_u8: return dst != PuleIRDataType_i8;
    case PuleIRDataType_u16:
      return (
           dst != PuleIRDataType_i8 && dst != PuleIRDataType_u8
        && dst != PuleIRDataType_i16
      );
    case PuleIRDataType_u32:
      return (
           dst != PuleIRDataType_i8 && dst != PuleIRDataType_u8
        && dst != PuleIRDataType_i16 && dst != PuleIRDataType_u16
        && dst != PuleIRDataType_i32
      );
    case PuleIRDataType_u64:
      return false; // can never upcast
    case PuleIRDataType_i8: return dst != PuleIRDataType_u8;
    case PuleIRDataType_i16:
      return (
           dst != PuleIRDataType_i8 && dst != PuleIRDataType_u8
        && dst != PuleIRDataType_u16
      );
    case PuleIRDataType_i32:
      return (
           dst != PuleIRDataType_i8 && dst != PuleIRDataType_u8
        && dst != PuleIRDataType_i16 && dst != PuleIRDataType_u16
        && dst != PuleIRDataType_u32
      );
    case PuleIRDataType_i64:
      return false; // can never upcast
  }
}

bool canCast(PuleIRDataType const src, PuleIRDataType const dst) {
  if (src == dst) {
    return true;
  }
  if (
       (puleIRDataTypeIsFloat(src) || puleIRDataTypeIsInt(src))
    && (puleIRDataTypeIsFloat(dst) || puleIRDataTypeIsInt(dst))
  ) {
    return true;
  }
  // if not integer or float, probably can't cast
  return false;
}

PuleIRValue explicitCast(
  Ctx & ctx,
  PuleIRValue const srcValue, PuleIRType const dstType
) {
  PuleIRType srcType = puleIRValueType(ctx.module, srcValue);
  PuleIRDataType const srcDatatype = puleIRTypeInfo(ctx.module, srcType).type;
  PuleIRDataType const dstDatatype = puleIRTypeInfo(ctx.module, dstType).type;
  PULE_assert(canCast(srcDatatype, dstDatatype));
  return (
    irBuild(ctx, {
      .cast = {
        .castType = puleIRCastTypeFromDataTypes(srcDatatype, dstDatatype),
        .value = srcValue,
        .type = dstType,
        .label = puleCStr(""),
      }
    })
  );
}

PuleIRValue implicitCast(
  Ctx & ctx,
  PuleIRValue const srcValue, PuleIRType const dstType
) {
  PuleIRType srcType = puleIRValueType(ctx.module, srcValue);
  if (srcType.id == dstType.id) {
    return srcValue;
  }
  PuleIRDataType const srcDatatype = puleIRTypeInfo(ctx.module, srcType).type;
  PuleIRDataType const dstDatatype = puleIRTypeInfo(ctx.module, dstType).type;
  if (dstDatatype == PuleIRDataType_void) {
    // this means no implicit cast
    return srcValue;
  }
  puleLog("implicit cast from %d to %d", srcDatatype, dstDatatype);
  // can only try to even implicit cast with integral types
  PULE_assert(
       (puleIRDataTypeIsFloat(srcDatatype) || puleIRDataTypeIsInt(srcDatatype))
    && (puleIRDataTypeIsFloat(dstDatatype) || puleIRDataTypeIsInt(dstDatatype))
  );
  PULE_assert(canUpcast(srcDatatype, dstDatatype));
  PuleIRBuildCreateInfo buildCi = {
    .module = ctx.module,
    .codeBlock = ctx.codeBlockStack.back().block,
  };
  if (puleIRDataTypeIsInt(dstDatatype)) {
    buildCi.instr.cast = {
      .castType = (
          puleIRDataTypeIsSigned(dstDatatype)
        ? PuleIRCast_intExtend
        : PuleIRCast_uintExtend
      ),
      .value = srcValue,
      .type = dstType,
      .label = puleCStr(""),
    };
  } else {
    // TODO
  }
  return irBuild(ctx, buildCi.instr);
}

// %expression_X_tail := %operator_X %expression_Y;
PuleIRValue traverseAstExpressionOperatorTail(
  Ctx & ctx,
  PuleParserAstNode const expr,
  PuleIRValue const lhs,
  TraverseTreeExpressionOperatorInfo const & info,
  PuleIRDataType const implicitType
) {
  PULE_assert(expr.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(expr.match, info.tailName.c_str()));
  PuleParserAstNode const exprOperator = (
    puleParserAstNodeChild(expr, 0)
  );
  PULE_assert(exprOperator.type == PuleParserNodeType_rule);
  PuleStringView operatorLabel = puleParserAstNodeChild(exprOperator, 0).match;
  PuleParserAstNode const exprChild = (
    puleParserAstNodeChild(expr, 1)
  );
  PULE_assert(exprChild.type == PuleParserNodeType_rule);
  PULE_assert(
    puleStringViewEqCStr(exprChild.match, info.next->expressionName.c_str())
  );
  TypeValue rhs;
  if (info.next->next == nullptr) {
    // we hit the bottom of our precedence, so we are dealing with unary
    rhs = traverseAstExpressionUnary(ctx, exprChild, implicitType);
  } else {
    // go up the tree
    rhs = (
      traverseAstExpressionOperator(ctx, exprChild, *info.next, implicitType)
    );
  }
  // we should never hit a type here
  PULE_assert(rhs.isType == false);

  // now go back down and join on operator
  std::unordered_map<std::string, PuleIROperator> const operators = {
    {"==", PuleIROperator_eq},
    {"!=", PuleIROperator_ne},
    {"+", PuleIROperator_add},
    {"-", PuleIROperator_sub},
    {"*", PuleIROperator_mul},
    {"/", PuleIROperator_div},
  };
  PuleIROperator op = operators.at(operatorLabel.contents);
  // TODO handle floating, implicit casts, etc
  PuleIRValue const result = (
    irBuild(ctx, {
      .op = {
        .type = op,
        .binary = {
          .lhs = lhs,
          .rhs = implicitCast(ctx, rhs.v, puleIRValueType(ctx.module, lhs)),
        },
        .label = puleCStr(""),
        .floatingPoint = false, // TODO
      },
    })
  );
  return result;
}

// %expression_X := %expression_Y %expression_X_tail*;
TypeValue traverseAstExpressionOperator(
  Ctx & ctx,
  PuleParserAstNode const expr,
  TraverseTreeExpressionOperatorInfo const & info,
  PuleIRDataType implicitType
) {
  PULE_assert(expr.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(expr.match, info.expressionName.c_str()));
  PuleParserAstNode const exprChild = puleParserAstNodeChild(expr, 0);
  PuleParserAstNode const exprTail = puleParserAstNodeChild(expr, 1);

  TypeValue lhsIt;
  if (info.next->next == nullptr) {
    // hit bottom of precedence, dealing with unary
    lhsIt = traverseAstExpressionUnary(ctx, exprChild, implicitType);
  } else {
    // go up the tree
    lhsIt = (
      traverseAstExpressionOperator(ctx, exprChild, *info.next, implicitType)
    );
  }
  // go sideways, joining on operator
  for (size_t it = 0; it < exprTail.childCount; ++ it) {
    // we should never hit a type here
    PULE_assert(lhsIt.isType == false);
    lhsIt.v = (
      traverseAstExpressionOperatorTail(
        ctx, puleParserAstNodeChild(exprTail, it), lhsIt.v, info, 
        puleIRTypeInfo(ctx.module, puleIRValueType(ctx.module, lhsIt.v)).type
      )
    );
  }
  return lhsIt;
}

// this handles expression with precedence
TypeValue traverseAstExpression(
  Ctx & ctx,
  PuleParserAstNode const expr,
  PuleIRDataType const implicitType
) {
  PULE_assert(expr.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(expr.match, "expression"));
  PuleParserAstNode const exprChild = puleParserAstNodeChild(expr, 0);

  // build operator tree
  static TraverseTreeExpressionOperatorInfo infoUnary = {
    .expressionName = "expression_unary",
    .tailName = "",
    .next = nullptr,
  };
  static TraverseTreeExpressionOperatorInfo infoMultiplicative = {
    .expressionName = "expression_multiplicative",
    .tailName = "expression_multiplicative_tail",
    .next = &infoUnary,
  };
  static TraverseTreeExpressionOperatorInfo infoAdditive = {
    .expressionName = "expression_additive",
    .tailName = "expression_additive_tail",
    .next = &infoMultiplicative,
  };
  static TraverseTreeExpressionOperatorInfo infoEquality = {
    .expressionName = "expression_equality",
    .tailName = "expression_equality_tail",
    .next = &infoAdditive,
  };

  return (
    traverseAstExpressionOperator(ctx, exprChild, infoEquality, implicitType)
  );
}

void traverseAstStatementIf(Ctx & ctx, PuleParserAstNode const statementIf) {
  // %statement_if := 'if' '\(' %expression '\)' %block %statement_else?;
  auto const conditionNode = puleParserAstNodeChild(statementIf, 2);
  auto const blockNode = puleParserAstNodeChild(statementIf, 4);
  auto const elseBlock = puleParserAstNodeChild(statementIf, 5);
  TypeValue condition = (
    traverseAstExpression(ctx, conditionNode, PuleIRDataType_i64)
  );
  PULE_assert(condition.isType == false);
  // convert condition to bool
  condition.v = (
    irBuild(ctx, {
      .trunc = {
        .value = condition.v,
        .type = puleIRTypeCreate({
          .module = ctx.module,
          .type = PuleIRDataType_bool,
        }),
        .label = puleCStr("conditional_trunc"),
      }
    })
  );

  // create code blocks for if/else, then build branch
  PuleIRCodeBlock const codeBlockTrue = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = ctx.currentFunction,
      .positionBuilderAtEnd = true,
      .label = puleCStr("if_true"),
    })
  );
  PuleIRCodeBlock const codeBlockFalse = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = ctx.currentFunction,
      .positionBuilderAtEnd = true,
      .label = puleCStr("if_false"),
    })
  );
  PuleIRCodeBlock const codeBlockEnd = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = ctx.currentFunction,
      .positionBuilderAtEnd = true,
      .label = puleCStr("if_end"),
    })
  );
  irBuild(ctx, {
    .branch = PuleIRInstrBranch {
      .isConditional = true,
      .conditional = {
        .condition = condition.v,
        .codeBlockTrue = codeBlockTrue,
        .codeBlockFalse = codeBlockFalse,
      },
    },
  });
  // traverse our true-block, jumping to end-block at the end
  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = codeBlockTrue,
    .hasTerminated = false,
  });
  traverseAstCodeblock(ctx, blockNode);
  irBuild(ctx, {
    .branch = PuleIRInstrBranch {
      .isConditional = false,
      .unconditional = {
        .codeBlock = codeBlockEnd,
      },
    },
  });
  // even if there's no else, put the false-block here to make it simpler
  ctx.codeBlockStack.pop_back();
  // now handle else, where we either have a block or another if chain
  // %statement_else := 'else' %statement_block
  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = codeBlockFalse,
    .hasTerminated = false,
  });
  if (elseBlock.childCount > 0) {
    PULE_assert(elseBlock.childCount == 2);
    PuleParserAstNode const elseBlockNode = puleParserAstNodeChild(
      elseBlock, 1
    );
    // traverse false-block, jumping to end-block at end
    traverseAstStatementBlock(ctx, elseBlockNode);
  }
  irBuild(ctx, {
    .branch = PuleIRInstrBranch {
      .isConditional = false,
      .unconditional = {
        .codeBlock = codeBlockEnd,
      },
    },
  });
  ctx.codeBlockStack.pop_back();

  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = codeBlockEnd,
    .hasTerminated = false,
  });
}

void traverseAstStatementWhile(
  Ctx & ctx,
  PuleParserAstNode const statementWhile
) {
  auto const conditionNode = puleParserAstNodeChild(statementWhile, 2);
  auto const blockNode = puleParserAstNodeChild(statementWhile, 4);

  // create code blocks while/while_end, then build branch
  PuleIRCodeBlock const codeBlockWhileCheck = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = ctx.currentFunction,
      .positionBuilderAtEnd = true,
      .label = puleCStr("while_check"),
    })
  );
  PuleIRCodeBlock const codeBlockWhileStart = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = ctx.currentFunction,
      .positionBuilderAtEnd = true,
      .label = puleCStr("while_start"),
    })
  );
  PuleIRCodeBlock const codeBlockWhileEnd = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = ctx.currentFunction,
      .positionBuilderAtEnd = true,
      .label = puleCStr("while_end"),
    })
  );
  // traverse through the while-block condition check
  irBuild(ctx, {
    .branch = PuleIRInstrBranch {
      .isConditional = false,
      .unconditional = { .codeBlock = codeBlockWhileCheck },
    },
  });

  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = codeBlockWhileCheck,
    .hasTerminated = false,
  });
  TypeValue condition = (
    traverseAstExpression(ctx, conditionNode, PuleIRDataType_i64)
  );
  PULE_assert(condition.isType == false);
  // convert condition to bool
  condition.v = (
    irBuild(ctx, {
      .trunc = {
        .value = condition.v,
        .type = puleIRTypeCreate({
          .module = ctx.module,
          .type = PuleIRDataType_bool,
        }),
        .label = puleCStr("conditional_trunc"),
      }
    })
  );
  irBuild(ctx, {
    .branch = PuleIRInstrBranch {
      .isConditional = true,
      .conditional = {
        .condition = condition.v,
        .codeBlockTrue = codeBlockWhileStart,
        .codeBlockFalse = codeBlockWhileEnd,
      },
    },
  });
  ctx.codeBlockStack.pop_back();
  // traverse through the while-block
  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = codeBlockWhileStart,
    .hasTerminated = false,
  });
  traverseAstCodeblock(ctx, blockNode);
  irBuild(ctx, {
    .branch = PuleIRInstrBranch {
      .isConditional = false,
      .unconditional = {
        .codeBlock = codeBlockWhileCheck,
      },
    },
  });
  ctx.codeBlockStack.pop_back();
  // end of while
  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = codeBlockWhileEnd,
    .hasTerminated = false,
  });
}

void traverseAstStatementBlock(Ctx & ctx, PuleParserAstNode const statement) {
  // %statement_block := (%statement_if | %statement_while | %block |);
  PULE_assert(statement.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(statement.match, "statement_block"));
  PULE_assert(statement.childCount == 1);
  PuleParserAstNode const statementChild = puleParserAstNodeChild(statement, 0);
  if (puleStringViewEqCStr(statementChild.match, "statement_if")) {
    traverseAstStatementIf(ctx, statementChild);
  } else if (puleStringViewEqCStr(statementChild.match, "statement_while")) {
    traverseAstStatementWhile(ctx, statementChild);
  } else if (puleStringViewEqCStr(statementChild.match, "block")) {
    traverseAstCodeblock(ctx, statementChild);
  } else {
    puleLogError("unknown statement block: %s", statement.match.contents);
    PULE_assert(false);
  }
}

// %assignment := %identifier '=' %expression;
void traverseAstAssignment(
  Ctx & ctx,
  PuleParserAstNode const assignment
) {
  PULE_assert(assignment.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(assignment.match, "assignment"));
  PULE_assert(assignment.childCount == 3);
  PuleParserAstNode const identifier = puleParserAstNodeChild(assignment, 0);
  PuleParserAstNode const expression = puleParserAstNodeChild(assignment, 2);

  PuleStringView const identifierMatch = (
    puleParserAstNodeChild(identifier, 0).match
  );

  PuleIRType const identifierType = (
    findValueType(ctx, identifierMatch.contents)
  );
  PuleIRDataType identifierDataType = (
    puleIRTypeInfo(ctx.module, identifierType).type
  );

  TypeValue const assignmentValue = (
    traverseAstExpression(ctx, expression, identifierDataType)
  );
  PULE_assert(assignmentValue.isType == false);
  storeValue(
    ctx,
    identifierMatch.contents,
    implicitCast(ctx, assignmentValue.v, identifierType)
  );
}

// %declaration := ('@let' | '@mut' |) %identifier ':' %type '=' %expression;
void traverseAstDeclaration(
  Ctx & ctx,
  PuleParserAstNode const declaration
) {
  PULE_assert(declaration.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(declaration.match, "declaration"));
  PuleParserAstNode const
    constOrVar = puleParserAstNodeChild(declaration, 0),
    identifier = puleParserAstNodeChild(declaration, 1),
    type       = puleParserAstNodeChild(declaration, 3),
    expression = puleParserAstNodeChild(declaration, 5)
  ;
  PULE_assert(constOrVar.type == PuleParserNodeType_regex);
  PULE_assert(identifier.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(identifier.match, "identifier"));
  PULE_assert(type.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(type.match, "type"));
  PULE_assert(expression.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(expression.match, "expression"));

  PuleParserAstNode const identifierNode = (
    puleParserAstNodeChild(identifier, 0)
  );

  // create/allocate variable
  bool isConst = puleStringViewEqCStr(constOrVar.match, "@let");
  std::string const label = identifierNode.match.contents;
  pint::Type const typeInfo = createTypeFromRuleType(ctx, type);
  PuleIRType const identifierType = typeInfo.irType();
  PuleIRDataType const identifierDataType = (
    puleIRTypeInfo(ctx.module, identifierType).type
  );
  TypeValue const value = (
    traverseAstExpression(
      ctx,
      expression,
      identifierDataType
    )
  );
  PULE_assert(value.isType == false);

  // get the byte length of the variable to allocate
  bool isPointer = typeInfo.types.size() > 1;
  size_t const variableByteLen = (
      isPointer
    ? sizeof(void *) // TODO maybe it's ok to rely on byteCount for ptrs?
    : puleIRTypeInfo(ctx.module, typeInfo.types[0]).byteCount
  );
  PuleIRValue const variable = (
    irBuild(ctx, {
      .stackAlloc = {
        .type = typeInfo.irType(),
        .valueSize = puleIRValueConstI64(ctx.module, variableByteLen),
        //.label = puleCStr(label.c_str()),
        .label = puleCStr(""),
      },
    })
  );
  ctx.variables.emplace(label, variable);
  ctx.variableTypes.emplace(label, typeInfo.irType());
  // assign value to allocated variable
  storeValue(ctx, label, implicitCast(ctx, value.v, identifierType));
}

void traverseAstCall(Ctx & ctx, PuleParserAstNode const statement) {
  // %identifier '\(' %argument* '\)'
  PuleParserAstNode const identifier = puleParserAstNodeChild(statement, 0);
  PuleParserAstNode const argumentSeq = ( // %argument := %expression ','
    puleParserAstNodeChild(statement, 2)
  );
  PULE_assert(identifier.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(identifier.match, "identifier"));
  std::string label = puleParserAstNodeChild(identifier, 0).match.contents;
  PULE_assert(argumentSeq.type == PuleParserNodeType_sequence);
  std::vector<PuleIRValue> arguments;
  auto const fnType = ctx.globalTypes.at(label);
  puleLog("call fn type %d for label %s", fnType.id, label.c_str());
  auto const fnInfo = (
    puleIRTypeInfo(ctx.module, fnType).fnInfo
  );
  auto const fnValue = pint::findValue(ctx, label);
  for (size_t argIt = 0; argIt < argumentSeq.childCount; ++ argIt) {
    // get implicit type to cast to, for variadic arguments default to void
    PuleIRType implicitType = ctx.typeVoid;
    PuleIRDataType implicitDatatype = PuleIRDataType_void;
    if (!fnInfo.variadic || argIt < fnInfo.parameterCount) {
      implicitType = fnInfo.parameterTypes[argIt];
      implicitDatatype = puleIRTypeInfo(ctx.module, implicitType).type;
      puleLog("implicit type %d", implicitType);
    }
    // traverse argument expression; %argument := %expression ','
    TypeValue const argument = (
      traverseAstExpression(
        ctx,
        puleParserAstNodeChild(
          puleParserAstNodeChild(argumentSeq, argIt),
          0
        ),
        implicitDatatype
      )
    );
    PULE_assert(argument.isType == false);
    // cast argument to implicit type
    arguments.emplace_back(implicitCast(ctx, argument.v, implicitType));
  }
  irBuild(ctx, {
    .call = {
      .functionType = fnType,
      .function = fnValue,
      .argumentCount = arguments.size(),
      .arguments = arguments.data(),
      .label = puleCStr(""),
    },
  });
}

void traverseAstStatementInstruction(
  Ctx & ctx,
  PuleParserAstNode const statement
) {
  if (puleStringViewEqCStr(statement.match, "call")) {
    traverseAstCall(ctx, statement);
  } else if (puleStringViewEqCStr(statement.match, "return")) {
    // 'return' %expression
    PuleParserAstNode const expression = (
      puleParserAstNodeChild(statement, 1)
    );
    PULE_assert(expression.type == PuleParserNodeType_rule);
    PULE_assert(puleStringViewEqCStr(expression.match, "expression"));
    // traverse return expression, then clear implicit type and build return
    auto const fnInfo = (
      puleIRTypeInfo(
        ctx.module,
        puleIRValueType(ctx.module, ctx.currentFunction)
      ).fnInfo
    );
    PuleIRType returnType = fnInfo.returnType;
    PuleIRDataType const returnDatatype = (
      puleIRTypeInfo(ctx.module, returnType).type
    );
    TypeValue const value = (
      traverseAstExpression(ctx, expression, returnDatatype)
    );
    PULE_assert(value.isType == false);
    // store return value
    irBuild(ctx, {
      .store = {
        .value = implicitCast(ctx, value.v, returnType),
        .dst = ctx.currentFunctionReturnValue,
      },
    });
    // jump to return block
    irBuild(ctx, {
      .branch = PuleIRInstrBranch {
        .isConditional = false,
        .unconditional = {
          .codeBlock = ctx.currentFunctionReturnBlock,
        },
      },
    });
  } else if (puleStringViewEqCStr(statement.match, "declaration")) {
    traverseAstDeclaration(ctx, statement);
  } else if (puleStringViewEqCStr(statement.match, "assignment")) {
    traverseAstAssignment(ctx, statement);
  } else {
    puleLogError("unknown statement: %s", statement.match.contents);
    PULE_assert(false && "unknown operation");
  }
}

void traverseAstCodeblock(Ctx & ctx, PuleParserAstNode const block) {
  PULE_assert(block.type == PuleParserNodeType_rule);
  PULE_assert(block.childCount == 3); //  '\{' %body '\}'
  PuleParserAstNode const body = ( // %body := %statement*
    puleParserAstNodeChild(puleParserAstNodeChild(block, 1), 0)
  );
  PULE_assert(body.type == PuleParserNodeType_sequence);
  for (size_t it = 0; it < body.childCount; ++ it) {
    PuleParserAstNode statement = puleParserAstNodeChild(body, it);
    PULE_assert(statement.type == PuleParserNodeType_rule);
    // %statement := (%statement_block | %statement_instruction);
    statement = puleParserAstNodeChild(statement, 0);
    PULE_assert(statement.type == PuleParserNodeType_rule);
    PuleParserAstNode const statementChild = (
      puleParserAstNodeChild(statement, 0)
    );
    if (puleStringViewEqCStr(statement.match, "statement_block")) {
      traverseAstStatementBlock(ctx, statement);
    } else if (puleStringViewEqCStr(statement.match, "statement_instruction")) {
      traverseAstStatementInstruction(ctx, statementChild);
    } else {
      PULE_assert(false);
    }
  }
}

void addGlobalTypes(::Ctx & ctx) {
  #define typeDef(__name__, __type__) \
    { \
      std::string(#__name__), \
      puleIRTypeCreate( \
        {.module = ctx.module, .type = PuleIRDataType_##__type__,} \
      ), \
    }
  for (auto const & type : { std::pair<std::string, PuleIRType>
    typeDef(char, i8),
    typeDef(i8, i8),
    typeDef(i16, i16),
    typeDef(i32, i32),
    typeDef(i64, i64),
    typeDef(u8, u8),
    typeDef(u16, u16),
    typeDef(u32, u32),
    typeDef(u64, u64),
    typeDef(isize, i64),
    typeDef(usize, u64),
    typeDef(f32, f32),
    typeDef(f64, f64),
    typeDef(ptr, ptr),
    typeDef(bool, bool),
    typeDef(void, void),
  }) {
    ctx.globalTypes.emplace(type.first, type.second);
  }
}

void addEngineSymbols(::Ctx & ctx, PuleIRModule const module) {
  //PuleIRType const ptrType = (
  //  puleIRTypeCreate({
  //    .module = module,
  //    .type = PuleIRDataType_ptr,
  //  })
  //);
  //PuleIRType const fnTypePuleLog = (
  //  puleIRTypeCreate({
  //    .module = module,
  //    .type = PuleIRDataType_function,
  //    .fnInfo = {
  //      .returnType = puleIRTypeCreate({
  //        .module = module,
  //        .type = PuleIRDataType_void,
  //      }),
  //      .parameterCount = 1,
  //      .parameterTypes = &ptrType,
  //      .variadic = true,
  //    },
  //  })
  //);
  //ctx.globalTypes.emplace("puleLog", fnTypePuleLog);
  //ctx.variables.emplace(
  //  "puleLog",
  //  puleIRValueConstCreate({
  //    .module = module,
  //    .type = fnTypePuleLog,
  //    .kind = PuleIRValueKind_function,
  //    .label = puleCStr("puleLog"),
  //    .function = { .linkage = PuleIRLinkage_external, },
  //  })
  //);
}

PuleIRType createFunctionTypeFromAst(
  Ctx & ctx,
  PuleParserAstNode const identifier,
  PuleParserAstNode const parameterSeq,
  PuleParserAstNode const returnType
) {
  PULE_assert(identifier.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(identifier.match, "identifier"));
  PULE_assert(parameterSeq.type == PuleParserNodeType_sequence);
  PULE_assert(returnType.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(returnType.match, "type"));

  std::string const label = (
    puleParserAstNodeChild(identifier, 0).match.contents
  );
  std::vector<pint::Type> parameterTypes;
  std::vector<PuleIRType> parameterIRTypes;
  bool variadic = false;
  puleLog("parameter seq child count: %zu", parameterSeq.childCount);
  for (size_t it = 0; it < parameterSeq.childCount; ++ it) {
    PuleParserAstNode const parameter = (
      puleParserAstNodeChild(parameterSeq, it)
    );
    PULE_assert(parameter.type == PuleParserNodeType_rule);
    // %parameter := %identifier ':' %type;
    PULE_assert(puleStringViewEqCStr(parameter.match, "parameter"));
    PuleParserAstNode const parameterType = (
      puleParserAstNodeChild(parameter, 2)
    );
    PULE_assert(parameterType.type == PuleParserNodeType_rule);
    PULE_assert(puleStringViewEqCStr(parameterType.match, "type"));
    if (ruleTypeIsVariadic(parameterType)) {
      variadic = true;
      PULE_assert(it == parameterSeq.childCount - 1);
      break;
    }
    parameterTypes.emplace_back(createTypeFromRuleType(ctx, parameterType));
    puleLog("parameter type: %d", parameterTypes.back().irType());
    parameterIRTypes.emplace_back(parameterTypes.back().irType());
  }
  pint::Type const returnTypeType = createTypeFromRuleType(ctx, returnType);

  return (
    puleIRTypeCreate({
      .module = ctx.module,
      .type = PuleIRDataType_function,
      .fnInfo = {
        .returnType = returnTypeType.irType(),
        .parameterCount = parameterIRTypes.size(),
        .parameterTypes = parameterIRTypes.data(),
        .variadic = variadic,
      },
    })
  );
}

void traverseAstFunctionDefinition(Ctx & ctx, PuleParserAstNode const node) {
  // '@fn' %identifier '\(' %parameter* '\)' '->' %identifier %block ';';
  PuleParserAstNode const identifier = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 1), 0)
  );

  // clear context
  ctx.currentFunction = { .id = 0, };
  ctx.variables.clear();
  ctx.variableTypes.clear();
  ctx.codeBlockStack.clear();

  // create function (first type, then the value, then code block)
  std::string functionLabel = identifier.match.contents;
  pint::Type const returnType = (
    createTypeFromRuleType(ctx, puleParserAstNodeChild(node, 6))
  );
  PuleIRType const functionType = (
    createFunctionTypeFromAst(
      ctx,
      puleParserAstNodeChild(node, 1),
      puleParserAstNodeChild(node, 3),
      puleParserAstNodeChild(node, 6)
    )
  );
  PuleIRValue const fn = (
    puleIRValueConstCreate({
      .module = ctx.module,
      .type = functionType,
      .kind = PuleIRValueKind_function,
      .label = puleCStr(functionLabel.c_str()),
      .function = { .linkage = PuleIRLinkage_external, },
    })
  );
  ctx.currentFunction = fn;
  bool isPointer = returnType.types.size() > 1;
  size_t const variableByteLen = (
      isPointer
    ? sizeof(void *) // TODO maybe it's ok to rely on byteCount for ptrs?
    : puleIRTypeInfo(ctx.module, returnType.types[0]).byteCount
  );
  PuleIRCodeBlock puleIRCodeBlock = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = fn,
      .positionBuilderAtEnd = true,
      .label = puleCStr("entry"),
    })
  );
  PuleIRCodeBlock puleIRCodeBlockReturn = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = fn,
      .positionBuilderAtEnd = true,
      .label = puleCStr("entry_return"),
    })
  );
  ctx.currentFunctionReturnBlock = puleIRCodeBlockReturn;
  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = puleIRCodeBlock,
    .hasTerminated = false,
  });
  PuleIRDataTypeInfo const returnTypeInfo = (
    puleIRTypeInfo(ctx.module, returnType.irType())
  );
  if (returnTypeInfo.type == PuleIRDataType_void) {
    ctx.currentFunctionReturnValue = { .id = 0, };
  } else {
    ctx.currentFunctionReturnValue = (
      irBuild(ctx, {
        .stackAlloc = {
          .type = returnType.irType(),
          .valueSize = puleIRValueConstI64(ctx.module, variableByteLen),
          .label = puleCStr("return_value"),
        },
      })
    );
  }
  // create the body to attach to code block
  traverseAstCodeblock(ctx, puleParserAstNodeChild(node, 7));
  irBuild(ctx, {
    .branch = PuleIRInstrBranch {
      .isConditional = false,
      .unconditional = {
        .codeBlock = ctx.currentFunctionReturnBlock,
      },
    },
  });
  ctx.codeBlockStack.pop_back();
  ctx.codeBlockStack.emplace_back(CodeBlock {
    .block = puleIRCodeBlockReturn,
    .hasTerminated = false,
  });
  // if void, then add return void at end to make legal llvm IR
  if (
       returnType.types.size() == 1
    && returnType.types[0].id == ctx.typeVoid.id
  ) {
    puleIRCodeBlockTerminate({
      .module = ctx.module,
      .codeBlock = ctx.codeBlockStack.back().block,
      .type = PuleIRCodeBlockTerminateType_ret,
      .ret = { .value = { .id = 0, }, },
    });
  } else {
    PuleIRValue returnValue = (
      irBuild(ctx, {
        .load = {
          .type = returnType.irType(),
          .value = ctx.currentFunctionReturnValue,
          .label = puleCStr("return_load_value"),
        },
      })
    );
    puleIRCodeBlockTerminate({
      .module = ctx.module,
      .codeBlock = ctx.codeBlockStack.back().block,
      .type = PuleIRCodeBlockTerminateType_ret,
      .ret = { .value = returnValue, },
    });
  }
  // move return code block to end of function
  PuleIRCodeBlock const codeBlockEnd = (
    puleIRCodeBlockLast(ctx.module, ctx.currentFunction)
  );
  puleIRCodeBlockMoveAfter(
    ctx.module, ctx.currentFunctionReturnBlock, codeBlockEnd
  );
  ctx.codeBlockStack.pop_back();
}

// %function_declaration :=
//  %function_tag %identifier '\(' %parameter* '\)' '->' %identifier ';';
void traverseAstFunctionDeclaration(Ctx & ctx, PuleParserAstNode const child) {
  PULE_assert(child.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(child.match, "function_declaration"));
  PULE_assert(child.childCount == 8);
  PuleStringView const functionTag = (
    puleParserAstNodeChild(puleParserAstNodeChild(child, 0), 0).match
  );
  PuleStringView const functionLabel = (
    puleParserAstNodeChild(puleParserAstNodeChild(child, 1), 0).match
  );
  PuleIRType const functionType = (
    createFunctionTypeFromAst(
      ctx,
      puleParserAstNodeChild(child, 1),
      puleParserAstNodeChild(child, 3),
      puleParserAstNodeChild(child, 6)
    )
  );

  PuleIRLinkage linkage = PuleIRLinkage_none;
  if (puleStringViewEqCStr(functionTag, "@fn")) {
    // TODO do nothing for now
    return;
  } else if (puleStringViewEqCStr(functionTag, "@externfn")) {
    linkage = PuleIRLinkage_external;
  } else {
    puleLogError("unknown function tag: %s", functionTag.contents);
    PULE_assert(false);
  }

  // create function declaration
  // TODO when creating functions I need to check for duplicates, and
  //      ensure they have the same signature and no duplicate definitions
  ctx.gVariables.emplace(
    functionLabel.contents,
    puleIRValueConstCreate({
      .module = ctx.module,
      .type = functionType,
      .kind = PuleIRValueKind_function,
      .label = functionLabel,
      .function = { .linkage = linkage, },
    })
  );
  puleLog("adding function type: %s", functionLabel.contents);
  ctx.globalTypes.emplace(functionLabel.contents, functionType);
}

void traverseAstGlobals(Ctx & ctx, PuleParserAstNode const globals) {
  // %globals := %global*;
  PULE_assert(globals.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(globals.match, "globals"));
  PULE_assert(globals.childCount == 1);
  // %global := (%function | %function_declaration |);
  PuleParserAstNode const globalSeq = (
    puleParserAstNodeChild(globals, 0)
  );
  PULE_assert(globalSeq.type == PuleParserNodeType_sequence);
  for (size_t it = 0; it < globalSeq.childCount; ++ it) {
    PuleParserAstNode const global = puleParserAstNodeChild(globalSeq, it);
    PULE_assert(global.type == PuleParserNodeType_rule);
    PULE_assert(puleStringViewEqCStr(global.match, "global"));
    PuleParserAstNode const globalChild = (
      puleParserAstNodeChild(global, 0)
    );
    if (puleStringViewEqCStr(globalChild.match, "function_definition")) {
      traverseAstFunctionDefinition(ctx, globalChild);
    } else if (puleStringViewEqCStr(globalChild.match, "function_declaration")){
      traverseAstFunctionDeclaration(ctx, globalChild);
    } else {
      puleLogError("unknown global: %s", globalChild.match.contents);
      PULE_assert(false);
    }
  }
}

} // namespace pint

extern "C" {

PuleIRModule puleELModuleCreate(
  PuleStreamRead const stream,
  PuleError * const error
) {
  auto parser = pule::puleParserCreateForDefaultEngine();
  PuleBuffer buffer = puleStreamDumpToBuffer(stream);
  puleScopeExit { puleBufferDestroy(buffer); };

  PuleParserRule const functionRule = (
    puleParserRule(parser, puleCStr("globals"))
  );

  PuleParserAst ast = (
    puleParserAstCreate(
      parser, puleCStr((char const *)buffer.data), functionRule, error
    )
  );
  if (puleErrorExists(error)) {
    PULE_error(PuleErrorEL_compileError, "failed to parse");
    return { .id = 0, };
  }
  puleParserAstNodeDump(puleParserAstRoot(ast));

  PuleIRModule module = puleIRModuleCreate({puleCStr("engine-language"),});
  ::Ctx ctx = {
    .module = module,
    .typeVoid = puleIRTypeCreate({
      .module = ctx.module,
      .type = PuleIRDataType_void,
    }),
    .typeU64 = puleIRTypeCreate({
      .module = ctx.module,
      .type = PuleIRDataType_u64,
    }),
    .typeI64 = puleIRTypeCreate({
      .module = ctx.module,
      .type = PuleIRDataType_i64,
    }),
  };
  ctx.typeVoid = (
    puleIRTypeCreate({
      .module = module,
      .type = PuleIRDataType_void,
    })
  );
  pint::addGlobalTypes(ctx);
  pint::addEngineSymbols(ctx, ctx.module);
  pint::traverseAstGlobals(ctx, puleParserAstRoot(ast));
  return module;
}

} // extern "C"
