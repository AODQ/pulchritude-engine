/* auto generated file parser */
#pragma once
#include "core.h"

#include "string.h"
#include "file.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorParser_none = 0,
  PuleErrorParser_astPoisoned = 1,
} PuleErrorParser;
const uint32_t PuleErrorParserSize = 2;
typedef enum {
  PuleParserNodeType_regex = 0,
  PuleParserNodeType_rule = 1,
  PuleParserNodeType_group = 2,
  PuleParserNodeType_sequence = 3,
} PuleParserNodeType;
const uint32_t PuleParserNodeTypeSize = 4;
typedef enum {
  PuleParserNodeRepeat_zeroOrMore = 0,
  PuleParserNodeRepeat_oneOrMore = 1,
  PuleParserNodeRepeat_optional = 2,
  PuleParserNodeRepeat_once = 3,
} PuleParserNodeRepeat;
const uint32_t PuleParserNodeRepeatSize = 4;

// entities
typedef struct PuleParser { uint64_t id; } PuleParser;
typedef struct PuleParserRegexToken { uint64_t id; } PuleParserRegexToken;
typedef struct PuleParserGroup { uint64_t id; } PuleParserGroup;
typedef struct PuleParserRule { uint64_t id; } PuleParserRule;
/* 
 parse string with the rules, returning the ast
 TODO rename this from PuleParserAst to PuleParserTree as this is not an AST
      e.g. there is no precedence, reordering of nodes, stripping of
      unnecessary nodes, etc.
 */
typedef struct PuleParserAst { uint64_t id; } PuleParserAst;

// structs
struct PuleParserRuleNode;
struct PuleParserRuleCreateInfo;
struct PuleParserAstNode;

typedef struct PuleParserRuleNode {
  PuleParserNodeType type;
  PuleParserNodeRepeat repeat;
  uint64_t id;
} PuleParserRuleNode;
typedef struct PuleParserRuleCreateInfo {
  PuleParser parser;
  PuleStringView label;
  PuleParserRuleNode * nodes;
  size_t nodeCount;
  bool ignoreWhitespace;
} PuleParserRuleCreateInfo;
typedef struct PuleParserAstNode {
  uint64_t id;
  PuleStringView match;
  PuleParserNodeType type;
  uint64_t parserNodeId;
  size_t childCount;
} PuleParserAstNode;

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
