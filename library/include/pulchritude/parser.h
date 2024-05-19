/* auto generated file parser */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/string.h>
#include <pulchritude/file.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleParserNodeType type;
  PuleParserNodeRepeat repeat;
  uint64_t id;
} PuleParserRuleNode;
typedef struct {
  PuleParser parser;
  PuleStringView label;
  PuleParserRuleNode * nodes;
  size_t nodeCount;
  bool ignoreWhitespace;
} PuleParserRuleCreateInfo;
typedef struct {
  uint64_t id;
  PuleStringView match;
  PuleParserNodeType type;
  uint64_t parserNodeId;
  size_t childCount;
} PuleParserAstNode;

// enum
typedef enum {
  PuleErrorParser_none,
  PuleErrorParser_astPoisoned,
} PuleErrorParser;
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

// entities
typedef struct { uint64_t id; } PuleParser;
typedef struct { uint64_t id; } PuleParserRegexToken;
typedef struct { uint64_t id; } PuleParserGroup;
typedef struct { uint64_t id; } PuleParserRule;
/* 
 parse string with the rules, returning the ast
 TODO rename this from PuleParserAst to PuleParserTree as this is not an AST
      e.g. there is no precedence, reordering of nodes, stripping of
      unnecessary nodes, etc.
 */
typedef struct { uint64_t id; } PuleParserAst;

// functions
PULE_exportFn PuleParser puleParserCreate(PuleStringView name, PuleStringView commentStart, PuleStringView commentEnd);
PULE_exportFn void puleParserDestroy(PuleParser parser);
PULE_exportFn PuleParser puleParserCreateFromString(PuleStringView sv, PuleStringView name, PuleStringView commentStart, PuleStringView commentEnd, PuleError * error);
PULE_exportFn PuleParser puleParserCreateForDefaultExpressionGrammar();
PULE_exportFn PuleParserRegexToken puleParserRegexTokenCreate(PuleParser parser, PuleStringView regex);
PULE_exportFn PuleParserGroup puleParserGroupCreate(PuleParser parser, PuleParserRuleNode * nodes, size_t nodeCount);
PULE_exportFn PuleParserRule puleParserRuleCreate(PuleParserRuleCreateInfo createInfo);
PULE_exportFn void puleParserRuleSetNodes(PuleParser parser, PuleParserRule rule, PuleParserRuleNode * nodes, size_t nodeCount);
PULE_exportFn PuleParserRule puleParserRule(PuleParser parser, PuleStringView label);
PULE_exportFn void puleParserDump(PuleParser parser);
PULE_exportFn PuleParserAst puleParserAstCreate(PuleParser parser, PuleStringView string, PuleParserRule headRule, PuleError * error);
PULE_exportFn void puleParserAstDestroy(PuleParserAst ast);
PULE_exportFn PuleParserAstNode puleParserAstRoot(PuleParserAst ast);
PULE_exportFn PuleParserAstNode puleParserAstNodeChild(PuleParserAstNode node, size_t index);
/* 
this is a convenience function that returns all matches in the node; it
recurses in-order until it finds a leaf node, then appends all leaf node
matches together as the return value
 */
PULE_exportFn PuleString puleParserAstNodeMatch(PuleParserAstNode node);
PULE_exportFn void puleParserAstNodeDump(PuleParserAstNode node);
PULE_exportFn void puleParserAstNodeDumpShallow(PuleParserAstNode node);

#ifdef __cplusplus
} // extern C
#endif
