#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-string/string.h>
#include <pulchritude-file/file.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorParser_none,
  PuleErrorParser_astPoisoned,
} PuleErrorParser;

typedef struct { uint64_t id; } PuleParser;
PULE_exportFn PuleParser puleParserCreate(
  PuleStringView const name,
  PuleStringView const commentStart, // e.g. "//"
  PuleStringView const commentEnd // e.g. "\n"
);
PULE_exportFn void puleParserDestroy(PuleParser const parser);

PULE_exportFn PuleParser puleParserCreateFromString(
  PuleStringView const sv,
  PuleStringView const name,
  PuleStringView const commentStart, // e.g. "//"
  PuleStringView const commentEnd, // e.g. "\n"
  PuleError * const error
);

PULE_exportFn PuleParser puleParserCreateForDefaultExpressionGrammar();

typedef struct { uint64_t id; } PuleParserRegexToken;
PULE_exportFn PuleParserRegexToken puleParserRegexTokenCreate(
  PuleParser const parser,
  PuleStringView const regex
);

typedef enum {
  PuleParserNodeType_regex,
  PuleParserNodeType_rule,
  PuleParserNodeType_group,
  PuleParserNodeType_sequence,
} PuleParserNodeType;

typedef enum {
  PuleParserNodeRepeat_zeroOrMore,
  PuleParserNodeRepeat_oneOrMore,
  PuleParserNodeRepeat_optional,
  PuleParserNodeRepeat_once,
} PuleParserNodeRepeat;

typedef struct {
  PuleParserNodeType type;
  PuleParserNodeRepeat repeat;
  uint64_t id;
} PuleParserRuleNode;

typedef struct { uint64_t id; } PuleParserGroup;
PULE_exportFn PuleParserGroup puleParserGroupCreate(
  PuleParser const parser,
  PuleParserRuleNode * const nodes,
  size_t const nodeCount
);

typedef struct { uint64_t id; } PuleParserRule;
typedef struct {
  PuleParser const parser;
  PuleStringView const label;
  PuleParserRuleNode * const nodes;
  size_t const nodeCount;
  bool const ignoreWhitespace;
} PuleParserRuleCreateInfo;
PULE_exportFn PuleParserRule puleParserRuleCreate(
  PuleParserRuleCreateInfo const createInfo
);
PULE_exportFn void puleParserRuleSetNodes(
  PuleParser const parser,
  PuleParserRule const rule,
  PuleParserRuleNode * const nodes,
  size_t const nodeCount
);
PULE_exportFn PuleParserRule puleParserRule(
  PuleParser const parser,
  PuleStringView const label
);

PULE_exportFn void puleParserDump(PuleParser const parser);

// parse string with the rules, returning the ast
// TODO rename this from PuleParserAst to PuleParserTree as this is not an AST
//      e.g. there is no precedence, reordering of nodes, stripping of
//      unnecessary nodes, etc.
typedef struct { uint64_t id; } PuleParserAst;
PULE_exportFn PuleParserAst puleParserAstCreate(
  PuleParser const parser,
  PuleStringView const string,
  PuleParserRule const headRule,
  PuleError * const error
);
PULE_exportFn void puleParserAstDestroy(PuleParserAst const ast);

// ast traversal
typedef struct {
  uint64_t id;
  PuleStringView match;
  PuleParserNodeType type;
  uint64_t parserNodeId;
  size_t childCount;
} PuleParserAstNode;
PULE_exportFn PuleParserAstNode puleParserAstRoot(PuleParserAst const ast);
PULE_exportFn PuleParserAstNode puleParserAstNodeChild(
  PuleParserAstNode const node,
  size_t const index
);

PULE_exportFn void puleParserAstNodeDump(PuleParserAstNode const node);

PULE_exportFn void puleParserAstNodeDumpShallow(PuleParserAstNode const node);

#ifdef __cplusplus
}
#endif
