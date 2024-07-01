#include "binding-generator.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <pulchritude/core.hpp>
#include <pulchritude/file.hpp>
#include <pulchritude/plugin.hpp>
#include <pulchritude/parser.hpp>
#include <pulchritude/string.hpp>

std::string const expressionGrammar = R"(
%globals := %global+;
%global := (%struct_decl | %entity_decl | %enum_decl | %func_decl | %include | %alias |);
%alias := '@alias' %identifier ':' %identifier %comment? ';';
%func_decl := '@fn' %identifier '\(' %parameter* '\)' %type? %comment? ';';
%struct_decl := %struct_union %identifier '\{' %comment? %field* '\}' ';';
%struct_union := ('@struct' | '@union' |);
%entity_decl := '@entity' %identifier %comment? ';';
%enum_decl := %enum_bitfield %identifier '\{' %comment? %enum_field* '\}' ';';
%enum_bitfield := ('@enum' | '@bitfield' |);
%include := '@include' '\`[^\`]*\`' ';';

%parameter := %identifier ':' %type ','?;
%field := '@field' %identifier ':' %type %assignment? %comment? ';';
%enum_field := %identifier %assignment? %comment? ';';

%assignment := '=' %expression;

%expression := (
  %hexint | %boolint | %float | %int | %char | %bool| %identifier |
);

%int := %digit+;
%hexint := '0x[0-9a-fA-F]+';
%boolint := '0b[01]+';
%float := %digit+ '.' %digit+ 'f'?;
%char := '\'' %char '\'';
%bool := ('true' | 'false' |);

%digit := '[0-9]';

%fnptr_param := %type ','?;

%type := (%identifier | %fnptr |) %typemodifier*;
%typemodifier := ('ptr' | 'ref' | 'const' | %array |);
%array := '@arr' '\[' ( %int | %identifier | ) '\]';

%identifier := '[a-zA-Z_][a-zA-Z0-9_]*';
%fnptr := '@fnptr' '\(' %fnptr_param* '\)';

%comment := '#\`[^\`]*\`';
)";

PuleParser pelParser() {
  static PuleParser parser = { .id = 0, };
  if (parser.id != 0) { return parser; }

  PuleError error = puleError();
  parser = (
    puleParserCreateFromString(
      puleCStr(expressionGrammar.c_str()),
      puleCStr("pulchritude-engine-language"),
      puleCStr("/*"),
      puleCStr("*/"),
      &error
    )
  );
  if (puleErrorConsume(&error)) { return {}; }
  return parser;
}

// %type := (%identifier | %fnptr |) %typemodifier*;
// %typemodifier := ('ptr' | 'ref' | 'const' |);
// %fnptr := '@fnptr' '\(' %fnptr_param* '\)';
// %fnptr_param := %type ','?;
BindingType parseType(PuleParserAstNode const node) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "type"));
  auto typeIdentifier = puleParserAstNodeChild(node, 0);
  auto typeModifiers = puleParserAstNodeChild(node, 1);
  // fetch modifiers
  std::vector<BindingTypeModifier> outMods;
  for (size_t it = 0; it < typeModifiers.childCount; ++ it) {
    auto mod = (
      puleParserAstNodeChild(puleParserAstNodeChild(typeModifiers, it), 0).match
    );
    if (puleStringViewEqCStr(mod, "ptr")) {
      outMods.push_back({BindingTypeModifierType::tPtr, "",});
    } else if (puleStringViewEqCStr(mod, "ref")) {
      outMods.push_back({BindingTypeModifierType::tRef, "",});
    } else if (puleStringViewEqCStr(mod, "const")) {
      outMods.push_back({BindingTypeModifierType::tConst, ""});
    } else if (puleStringViewEqCStr(mod, "array")) {
      // %array := @arr '[' (%int | %identifier) ']';
      auto arrSize = (
        puleParserAstNodeChild(
          puleParserAstNodeChild(
            puleParserAstNodeChild(typeModifiers, it),
            0
          ),
          2
        )
      );
      PULE_assert(arrSize.type == PuleParserNodeType_rule);
      auto sizeStr = puleParserAstNodeMatch(arrSize);
      puleScopeExit { puleStringDestroy(sizeStr); };
      outMods.push_back({
        .type = BindingTypeModifierType::tArray,
        .size = std::string(sizeStr.contents, sizeStr.len),
      });
    } else {
      puleLog("for node: ");
      puleParserAstNodeDump(node);
      puleLogError("unknown type modifier '%s'", mod.contents);
      PULE_assert(false);
    }
  }
  // fetch type
  if (puleStringViewEqCStr(typeIdentifier.match, "identifier")) {
    auto identifier = puleParserAstNodeChild(typeIdentifier, 0).match;
    PULE_assert(identifier.contents != nullptr);
    return {
      .name = std::string(identifier.contents),
      .fnptrParams = {},
      .modifiers = outMods,
      .type = BindingTypeType::identifier,
    };
  }
  if (puleStringViewEqCStr(typeIdentifier.match, "fnptr")) {
    auto fnptrParamSeq = puleParserAstNodeChild(typeIdentifier, 2);
    std::vector<BindingType> params;
    for (size_t it = 0; it < fnptrParamSeq.childCount; ++ it) {
      auto paramType = (
        puleParserAstNodeChild(puleParserAstNodeChild(fnptrParamSeq, it), 0)
      );
      params.push_back(parseType(paramType));
    }
    return {
      .name = "",
      .fnptrParams = params,
      .modifiers = outMods,
      .type = BindingTypeType::fnPtr,
    };
  }
  PULE_assert(false);
}

// %assignment := '=' %expression;
// %expression := (
//    %int | %hexint | %boolint | %float | %char | %bool| %identifier |);
// %int := %digit+;
// %hexint := '0x[0-9a-fA-F]+';
// %boolint := '0b[01]+';
void parseAssignment(
  PuleParserAstNode fieldAssignment,
  BindingAssignment & assignField,
  BindingAssignmentType & assignFieldType
) {
  PULE_assert(fieldAssignment.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(fieldAssignment.match, "assignment"));
  // dig, %assignment? -> %assignment -> %expression -> (matches)
  auto assignment = puleParserAstNodeChild(fieldAssignment, 1);
  auto matches = puleParserAstNodeChild(assignment, 0);
  std::string matchStr;
  PuleString matchPstr = puleParserAstNodeMatch(matches);
  matchStr = std::string(matchPstr.contents, matchPstr.len);
  puleStringDestroy(matchPstr);

  if (puleStringViewEq(matches.match, "identifier"_psv)) {
    assignField.valIdentifier = (
      puleStringCopy(puleAllocateDefault(), puleCStr(matchStr.c_str()))
    );
    assignFieldType = BindingAssignmentType::identifier;
  } else if (puleStringViewEq(matches.match, "int"_psv)) {
    assignField.valInt = std::stoll(matchStr);
    assignFieldType = BindingAssignmentType::tInt;
  } else if (puleStringViewEq(matches.match, "hexint"_psv)) {
    assignField.valInt = std::stoll(matchStr, nullptr, 16);
    assignFieldType = BindingAssignmentType::tInt;
  } else if (puleStringViewEq(matches.match, "boolint"_psv)) {
    assignField.valInt = std::stoll(matchStr, nullptr, 2);
    assignFieldType = BindingAssignmentType::tInt;
  } else if (puleStringViewEq(matches.match, "float"_psv)) {
    assignField.valFloat = std::stof(matchStr);
    assignFieldType = BindingAssignmentType::tFloat;
  } else if (puleStringViewEq(matches.match, "char"_psv)) {
    assignField.valChar = matchStr[1];
    assignFieldType = BindingAssignmentType::tChar;
  } else if (puleStringViewEq(matches.match, "bool"_psv)) {
    assignField.valBool = (
      puleStringViewEq("true"_psv, matches.match)
    );
    assignFieldType = BindingAssignmentType::tBool;
  } else {
    PULE_assert(false);
  }
}


std::string formatComment(std::string const & comment) {
  if (comment.size() == 0) { return ""; }
  PULE_assert(comment.size() > 3); // at least #``
  return comment.substr(2, comment.size() - 3);
}

// %alias := '@alias' %identifier ':' %identifier %comment? ';';
BindingAlias parseAlias(PuleParserAstNode const node) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "alias"));
  auto aliasIdentifier = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 1), 0).match
  );
  auto aliasType = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 3), 0).match
  );
  auto aliasCommentChild = puleParserAstNodeChild(node, 4);
  std::string aliasComment = "";
  if (aliasCommentChild.childCount > 0) {
    aliasComment = (
      std::string(puleParserAstNodeChild(aliasCommentChild, 0).match.contents)
    );
  }
  return {
    .name = std::string(aliasIdentifier.contents, aliasIdentifier.len),
    .comment = formatComment(aliasComment),
    .alias = std::string(aliasType.contents, aliasType.len),
  };
}

// %func_decl := '@fn' %identifier '\(' %parameter* '\)' %type? %comment? ';';
BindingFunc parseFunc(PuleParserAstNode const node) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "func_decl"));
  auto fnIdentifier = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 1), 0).match
  );
  auto fnCommentChild = (
    puleParserAstNodeChild(node, 6)
  );
  std::string fnComment = "";
  if (fnCommentChild.childCount > 0) {
    fnComment = (
      std::string(puleParserAstNodeChild(fnCommentChild, 0).match.contents)
    );
  }
  auto returnTypeChild = (
    puleParserAstNodeChild(node, 5)
  );
  static auto const defaultReturnType = BindingType {
    .name = "void",
    .fnptrParams = {},
    .modifiers = {},
    .type = BindingTypeType::identifier,
  };
  auto parameters = puleParserAstNodeChild(node, 3);
  std::vector<BindingFuncParameter> params;
  for (size_t it = 0; it < parameters.childCount; ++ it) {
    auto param = puleParserAstNodeChild(parameters, it);
    PULE_assert(param.type == PuleParserNodeType_rule);
    PULE_assert(puleStringViewEqCStr(param.match, "parameter"));
    auto paramIdentifier = (
      puleParserAstNodeChild(puleParserAstNodeChild(param, 0), 0).match
    );
    auto paramType = puleParserAstNodeChild(param, 2);
    std::string paramTypeIdentifier;
    params.push_back({
      .name = std::string(paramIdentifier.contents, paramIdentifier.len),
      .type = parseType(paramType),
    });
  }
  return {
    .name = std::string(fnIdentifier.contents, fnIdentifier.len),
    .comment = formatComment(fnComment),
    .parameters = params,
    .returnType = (
        returnTypeChild.childCount > 0
      ? parseType(returnTypeChild) : defaultReturnType
    ),
  };
}

// enum_decl := %enum_bitfield %identifier '{' %comment? %enum_field* '}';
// %enum_bitfield := ('@enum' | '@bitfield' |);
BindingEnum parseEnum(PuleParserAstNode const node) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "enum_decl"));
  bool isBitfield = (
    puleStringViewEq(
      puleParserAstNodeChild(puleParserAstNodeChild(node, 0), 0).match,
      "@bitfield"_psv
    )
  );
  auto enumIdentifier = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 1), 0).match
  );
  auto enumCommentChild = puleParserAstNodeChild(node, 3);
  std::string enumComment = "";
  if (enumCommentChild.childCount > 0) {
    enumComment = (
      std::string(puleParserAstNodeChild(enumCommentChild, 0).match.contents)
    );
  }
  auto enumFields = puleParserAstNodeChild(node, 4);
  std::vector<BindingEnumField> fields;
  // %enum_field := %identifier %assignment? %comment? ';';
  int64_t enumCounter = 0;
  for (size_t it = 0; it < enumFields.childCount; ++ it) {
    auto field = puleParserAstNodeChild(enumFields, it);
    PULE_assert(field.type == PuleParserNodeType_rule);
    PULE_assert(puleStringViewEqCStr(field.match, "enum_field"));
    auto fieldIdentifier = (
      puleParserAstNodeMatch(puleParserAstNodeChild(field, 0))
    );
    puleScopeExit { puleStringDestroy(fieldIdentifier); };
    auto fieldAssignment = puleParserAstNodeChild(field, 1);
    auto fieldCommentChild = puleParserAstNodeChild(field, 2);
    std::string fieldComment = "";
    if (fieldCommentChild.childCount > 0) {
      fieldComment = (
        std::string(puleParserAstNodeChild(fieldCommentChild, 0).match.contents)
      );
    }
    BindingAssignment assignField = {};
    BindingAssignmentType assignFieldType = BindingAssignmentType::none;
    int64_t value = enumCounter;
    if (fieldAssignment.childCount > 0) {
      parseAssignment(fieldAssignment, assignField, assignFieldType);
      PULE_assert(assignFieldType == BindingAssignmentType::tInt);
      value = assignField.valInt;
      enumCounter = value;
    }
    fields.push_back({
      .name = std::string(fieldIdentifier.contents, fieldIdentifier.len),
      .comment = formatComment(fieldComment),
      .value = value,
    });
    ++ enumCounter;
  }
  return {
    .name = std::string(enumIdentifier.contents, enumIdentifier.len),
    .comment = formatComment(enumComment),
    .fields = fields,
    .isBitfield = isBitfield,
  };
}

// %struct_decl := %struct_union %identifier '{' %comment? %field* '}';
// %struct_union := ('@struct' | '@union');
BindingStruct parseStruct(PuleParserAstNode const node) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "struct_decl"));
  auto structUnion = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 0), 0).match
  );
  PULE_assert(
       puleStringViewEqCStr(structUnion, "@struct")
    || puleStringViewEqCStr(structUnion, "@union")
  );
  auto structIdentifier = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 1), 0).match
  );
  auto structCommentChild = puleParserAstNodeChild(node, 3);
  std::string structComment = "";
  if (structCommentChild.childCount > 0) {
    structComment = (
      std::string(puleParserAstNodeChild(structCommentChild, 0).match.contents)
    );
  }

  // %field := '@field' %identifier ':' %type %assignment? %comment? ';';
  auto structFields = puleParserAstNodeChild(node, 4);
  std::vector<BindingStructField> fields;
  for (size_t it = 0; it < structFields.childCount; ++ it) {
    auto field = puleParserAstNodeChild(structFields, it);
    PULE_assert(field.type == PuleParserNodeType_rule);
    PULE_assert(puleStringViewEqCStr(field.match, "field"));
    auto fieldIdentifier = (
      puleParserAstNodeChild(puleParserAstNodeChild(field, 1), 0).match
    );
    auto fieldType = puleParserAstNodeChild(field, 3);
    auto fieldAssignment = puleParserAstNodeChild(field, 4);
    auto fieldCommentChild = puleParserAstNodeChild(field, 5);
    std::string fieldComment = "";
    if (fieldCommentChild.childCount > 0) {
      fieldComment = (
        std::string(puleParserAstNodeChild(fieldCommentChild, 0).match.contents)
      );
    }
    BindingAssignment assignField = {};
    BindingAssignmentType assignFieldType = BindingAssignmentType::none;
    if (fieldAssignment.childCount > 0) {
      parseAssignment(fieldAssignment, assignField, assignFieldType);
    }
    fields.push_back({
      .name = std::string(fieldIdentifier.contents, fieldIdentifier.len),
      .type = parseType(fieldType),
      .assignment = assignField,
      .assignmentType = assignFieldType,
      .comment = formatComment(fieldComment),
    });
  }
  return {
    .name = std::string(structIdentifier.contents, structIdentifier.len),
    .comment = formatComment(structComment),
    .fields = fields,
    .isUnion = puleStringViewEqCStr(structUnion, "@union"),
  };
}

// %entity_decl := '@entity' %identifier %comment? ';';
BindingEntity parseEntity(PuleParserAstNode const node) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "entity_decl"));
  auto entityIdentifier = (
    puleParserAstNodeChild(puleParserAstNodeChild(node, 1), 0).match
  );
  auto entityCommentChild = puleParserAstNodeChild(node, 2);
  std::string entityComment = "";
  if (entityCommentChild.childCount > 0) {
    entityComment = (
      std::string(puleParserAstNodeChild(entityCommentChild, 0).match.contents)
    );
  }
  return {
    .name = std::string(entityIdentifier.contents, entityIdentifier.len),
    .comment = formatComment(entityComment),
  };
}

// %include := '@include '`[^\`]*`' ';';
std::string parseInclude(PuleParserAstNode const node) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "include"));
  auto includePath = (
    puleParserAstNodeMatch(puleParserAstNodeChild(node, 1))
  );
  puleScopeExit { puleStringDestroy(includePath); };
  return std::string(includePath.contents+1, includePath.len-2);
}

void applyPelParser(void *, PuleStringView path, bool isFile) {
  if (!isFile) { return; }
  if (!puleStringViewEnds(path, ".pel"_psv)) { return; }
  PuleError err = puleError();
  PuleString contents = puleFileDump(path, PuleFileDataMode_text, &err);
  if (puleErrorConsume(&err) > 0) { return; }
  puleScopeExit { puleStringDestroy(contents); };

  auto parser = pelParser();

  PuleParserRule const globalRule = (
    puleParserRule(parser, puleCStr("globals"))
  );

  auto ast = (
    puleParserAstCreate(
      parser,
      puleStringView(contents),
      globalRule,
      &err
    )
  );
  if (puleErrorConsume(&err) > 0) { return; }
  puleScopeExit { puleParserAstDestroy(ast); };

  BindingFile file;

  auto globals = puleParserAstNodeChild(puleParserAstRoot(ast), 0);
  PULE_assert(globals.type == PuleParserNodeType_sequence);
  for (size_t it = 0; it < globals.childCount; ++ it) {
    auto global = puleParserAstNodeChild(globals, it);
    // dig, sequence -> rule -> (matches)
    // %globals := %global+;
    // %global := (%struct_decl | %entity_decl | %enum_decl | %func_decl |);
    global = puleParserAstNodeChild(global, 0);
    if (puleStringViewEq("func_decl"_psv, global.match)) {
      file.funcs.push_back(parseFunc(global));
    } else if (puleStringViewEq("alias"_psv, global.match)) {
      file.aliases.push_back(parseAlias(global));
    } else if (puleStringViewEq("struct_decl"_psv, global.match)) {
      file.structs.push_back(parseStruct(global));
    } else if (puleStringViewEq("enum_decl"_psv, global.match)) {
      file.enums.push_back(parseEnum(global));
    } else if (puleStringViewEq("entity_decl"_psv, global.match)) {
      file.entities.push_back(parseEntity(global));
    } else if (puleStringViewEq("include"_psv, global.match)) {
      file.includes.push_back(parseInclude(global));
    } else {
      PULE_assert(false);
    }
  }

  // generate bindings
  GenerateBindingInfo info;
  info.file = file;

  // TODO this needs to move into the binding generator

  std::string newpath = std::string(path.contents, path.len);
  // remove .pel
  newpath = newpath.substr(0, newpath.size() - 4);
  // remove ../../../../../skeleton/pulchritude-
  newpath = newpath.substr(36);
  info.path = puleCStr(newpath.c_str());

  generateBindingFileC(info);
  generateBindingFileCpp(info);
  generateBindingFileMd(info);
}

extern "C" {

PulePluginType pulcPluginType() {
  return PulePluginType_component;
}

void pulcComponentLoad([[maybe_unused]] PulePluginPayload const payload) {
  // assert we are running in the right directory
  PULE_assert(puleFilesystemPathExists("../../../../../skeleton"_psv));
  PULE_assert(puleFilesystemPathExists("../../../../../library"_psv));
  PULE_assert(puleFilesystemPathExists("../../../../../library/include"_psv));

  // walk through skeleton directory and find all .pel
  puleFilesystemDirWalk(
    "../../../../../skeleton"_psv,
    applyPelParser,
    nullptr
  );
}

void pulcComponentUnload([[maybe_unused]] PulePluginPayload const payload) {
}

}
