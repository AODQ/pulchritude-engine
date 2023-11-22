#include <pulchritude-engine-language/engine-language.h>

#include "parser.hpp"

#include <pulchritude-parser/parser.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace pint {

struct Ctx {
  PuleIRModule module;
  PuleIRType typeVoid;
  PuleIRCodeBlock currentCodeBlock;
  // TODO probably use a vector of maps to handle scopes
  std::unordered_map<std::string, PuleIRType> globalTypes;
  // TODO again, use a vector of maps to handle scopes
  std::unordered_map<std::string, PuleIRValue> values;
};

PuleIRType findType(Ctx const & ctx, std::string const & label) {
  if (ctx.globalTypes.find(label) != ctx.globalTypes.end()) {
    return ctx.globalTypes.at(label);
  } else {
    puleLogError("could not find type: %s", label.c_str());
    PULE_assert(false);
  }
}

PuleIRValue findValue(Ctx const & ctx, std::string const & label) {
  if (ctx.values.find(label) != ctx.values.end()) {
    return ctx.values.at(label);
  } else {
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
};

pint::Type fetchType(Ctx & ctx, PuleParserAstNode const identifier) {
  #define typeDef(__name__, __type__) \
    { \
      std::string(#__name__), \
      puleIRTypeCreate( \
        {.module = ctx.module, .type = PuleIRDataType_##__type__,} \
      ), \
    }
  static std::unordered_map<std::string, PuleIRType> types = {
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
    typeDef(bool, bool),
    typeDef(void, void),
  };
  // %type := %identifier %typeModifier*;
  PuleStringView const identifierSv = (
    puleParserAstNodeChild(puleParserAstNodeChild(identifier, 0), 0).match
  );

  std::string identifierStr = std::string(identifierSv.contents);
  pint::Type type;
  if (types.find(identifierStr) != types.end()) {
    type.types.emplace_back(types[identifierStr]);
  } else {
    puleLogError("could not find type: %s", identifierStr.c_str());
    PULE_assert(false);
  }

  PuleParserAstNode const typeModifier = puleParserAstNodeChild(identifier, 1);
  PULE_assert(typeModifier.type == PuleParserNodeType_sequence);
  for (size_t it = 0; it < typeModifier.childCount; ++ it) {
    PuleParserAstNode const modifier = puleParserAstNodeChild(typeModifier, it);
    PULE_assert(modifier.type == PuleParserNodeType_rule);
    PuleStringView const modifierLabel = (
      puleParserAstNodeChild(modifier, 0).match
    );
    if (puleStringViewEqCStr(modifierLabel, "pointer")) {
      type.types.emplace_back(
        puleIRTypeCreate({
          .module = ctx.module,
          .type = PuleIRDataType_ptr,
        })
      );
      // TODO handle const
    }
  }

  return type;
}

PuleIRValue traverseAstLiteral(Ctx & ctx, PuleParserAstNode const literal) {
  PULE_assert(literal.type == PuleParserNodeType_rule);
  PULE_assert(literal.childCount == 1);
  PULE_assert(puleStringViewEqCStr(literal.match, "literal"));
  PuleParserAstNode const literalValue = puleParserAstNodeChild(literal, 0);
  PULE_assert(literalValue.type == PuleParserNodeType_rule);
  if (puleStringViewEqCStr(literalValue.match, "integer")) {
    PULE_assert(false && "TODO");
  } else if (puleStringViewEqCStr(literalValue.match, "string")) {
    PuleParserAstNode const str = puleParserAstNodeChild(literalValue, 0);
    PULE_assert(str.type == PuleParserNodeType_regex);
    std::string strValue = std::string(str.match.contents);
    // cut off quotes
    strValue = strValue.substr(1, strValue.size() - 2);
    puleLog("string literal: `%s`", strValue.c_str());
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
          .codeBlock = ctx.currentCodeBlock,
          .string = puleCStr(strValue.c_str()),
        },
      })
    );
  } else if (puleStringViewEqCStr(literalValue.match, "float")) {
    PULE_assert(false && "TODO");
  } else if (puleStringViewEqCStr(literalValue.match, "boolean")) {
    PULE_assert(false && "TODO");
  } else {
    puleLogError("unknown literal: %s", literalValue.match.contents);
    PULE_assert(false);
  }
}

PuleIRValue traverseAstExpression(Ctx & ctx, PuleParserAstNode const expr) {
  PULE_assert(expr.type == PuleParserNodeType_rule);
  PULE_assert(expr.childCount == 1);
  PULE_assert(puleStringViewEqCStr(expr.match, "expression"));
  PuleParserAstNode const term = puleParserAstNodeChild(expr, 0);
  PULE_assert(term.type == PuleParserNodeType_rule);
  if (puleStringViewEqCStr(term.match, "literal")) {
    return traverseAstLiteral(ctx, term);
  } else {
    puleLogError("unknown term: %s", term.match.contents);
    PULE_assert(false);
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
    statement = puleParserAstNodeChild(statement, 0); // %statement := (..) ';'
    if (puleStringViewEqCStr(statement.match, "call")) {
      // %identifier '\(' %argument* '\)'
      PuleParserAstNode const identifier = puleParserAstNodeChild(statement, 0);
      PuleParserAstNode const argumentSeq = ( // %argument := %expression ','
        puleParserAstNodeChild(statement, 2)
      );
      PULE_assert(identifier.type == PuleParserNodeType_rule);
      PULE_assert(puleStringViewEqCStr(identifier.match, "identifier"));
      std::string label = puleParserAstNodeChild(identifier, 0).match.contents;
      puleLog("call: %s", label.c_str());
      PULE_assert(argumentSeq.type == PuleParserNodeType_sequence);
      std::vector<PuleIRValue> arguments;
      for (size_t argIt = 0; argIt < argumentSeq.childCount; ++ argIt) {
        // %argument := %expression ','
        arguments.emplace_back(
          traverseAstExpression(
            ctx,
            puleParserAstNodeChild(
              puleParserAstNodeChild(argumentSeq, argIt),
              0
            )
          )
        );
      }
      puleIRBuild({
        .module = ctx.module,
        .codeBlock = ctx.currentCodeBlock,
        .call = {
          .functionType = pint::findType(ctx, label),
          .function = pint::findValue(ctx, label),
          .arguments = arguments.data(),
          .argumentCount = arguments.size(),
          .label = "",
        },
      });
    } else {
      PULE_assert(false && "unknown operation");
    }
  }
}

void addEngineSymbols(pint::Ctx & ctx, PuleIRModule const module) {
  PuleIRType const ptrType = (
    puleIRTypeCreate({
      .module = module,
      .type = PuleIRDataType_ptr,
    })
  );
  PuleIRType const fnTypePuleLog = (
    puleIRTypeCreate({
      .module = module,
      .type = PuleIRDataType_function,
      .fnInfo = {
        .returnType = puleIRTypeCreate({
          .module = module,
          .type = PuleIRDataType_void,
        }),
        .parameterTypes = &ptrType,
        .parameterCount = 1,
        .variadic = true,
      },
    })
  );
  ctx.globalTypes.emplace("puleLog", fnTypePuleLog);
  ctx.values.emplace(
    "puleLog",
    puleIRValueConstCreate({
      .module = module,
      .type = fnTypePuleLog,
      .kind = PuleIRValueKind_function,
      .label = puleCStr("puleLog"),
      .function = { .linkage = PuleIRLinkage_external, },
    })
  );
}

void traverseAstFunctionDefinition(Ctx & ctx, PuleParserAstNode const node) {
  // '@fn' %identifier '\(' %parameter* '\)' '->' %identifier %block ';';
  PuleParserAstNode const identifier = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 1), 0)
  );

  std::string functionLabel = identifier.match.contents;
  puleLog("function name: %s", functionLabel.c_str());
  pint::Type const returnType = fetchType(ctx, puleParserAstNodeChild(node, 6));
  PuleIRType const functionType = (
    puleIRTypeCreate({
      .module = ctx.module,
      .type = PuleIRDataType_function,
      .fnInfo = {
        .returnType = returnType.irType(),
        .parameterTypes = nullptr,
        .parameterCount = 0,
        .variadic = false,
      },
    })
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
  PuleIRCodeBlock puleIRCodeBlock = (
    puleIRCodeBlockCreate({
      .module = ctx.module,
      .context = fn,
      .label = puleCStr("entry"),
    })
  );
  ctx.currentCodeBlock = puleIRCodeBlock;
  traverseAstCodeblock(ctx, puleParserAstNodeChild(node, 7));
  // if void, then add return void at end to make legal llvm IR
  if (
       returnType.types.size() == 1
    && returnType.types[0].id == ctx.typeVoid.id
  ) {
    puleIRCodeBlockTerminate({
      .module = ctx.module,
      .codeBlock = ctx.currentCodeBlock,
      .type = PuleIRCodeBlockTerminateType_ret,
      .ret = { .value = { .id = 0, }, },
    });
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
    puleParserRule(parser, puleCStr("function"))
  );

  PuleParserAst ast = (
    puleParserAstCreate(
      parser, puleCStr((char const *)buffer.data), functionRule, error
    )
  );
  if (puleErrorExists(error)) {
    PULE_error(PuleErrorEL_compileError, "failed to parser");
    return { .id = 0, };
  }

  PuleIRModule module = puleIRModuleCreate({puleCStr("engine-language"),});
  pint::Ctx ctx = {
    .module = module,
    .typeVoid = puleIRTypeCreate({
      .module = ctx.module,
      .type = PuleIRDataType_void,
    }),
  };
  ctx.typeVoid = (
    puleIRTypeCreate({
      .module = module,
      .type = PuleIRDataType_void,
    })
  );
  pint::addEngineSymbols(ctx, ctx.module);
  pint::traverseAstFunctionDefinition(ctx, puleParserAstRoot(ast));
  return module;
}

} // extern "C"
