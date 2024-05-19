# parser

## structs
### PuleParserRuleNode
```c
struct {
  type : PuleParserNodeType;
  repeat : PuleParserNodeRepeat;
  id : uint64_t;
};
```
### PuleParserRuleCreateInfo
```c
struct {
  parser : PuleParser;
  label : PuleStringView;
  nodes : PuleParserRuleNode ptr;
  nodeCount : size_t;
  ignoreWhitespace : bool;
};
```
### PuleParserAstNode
```c
struct {
  id : uint64_t;
  match : PuleStringView;
  type : PuleParserNodeType;
  parserNodeId : uint64_t;
  childCount : size_t;
};
```

## enums
### PuleErrorParser
```c
enum {
  PuleErrorParser_none,
  PuleErrorParser_astPoisoned,
}
```
### PuleParserNodeType
```c
enum {
  PuleParserNodeType_regex,
  PuleParserNodeType_rule,
  PuleParserNodeType_group,
  PuleParserNodeType_sequence,
}
```
### PuleParserNodeRepeat
```c
enum {
  PuleParserNodeRepeat_zeroOrMore,
  PuleParserNodeRepeat_oneOrMore,
  PuleParserNodeRepeat_optional,
  PuleParserNodeRepeat_once,
}
```

## entities
### PuleParser
### PuleParserRegexToken
### PuleParserGroup
### PuleParserRule
### PuleParserAst

 parse string with the rules, returning the ast
 TODO rename this from PuleParserAst to PuleParserTree as this is not an AST
      e.g. there is no precedence, reordering of nodes, stripping of
      unnecessary nodes, etc.


## functions
### puleParserCreate
```c
puleParserCreate(
  name : PuleStringView,
  commentStart : PuleStringView,
  commentEnd : PuleStringView
) PuleParser;
```
### puleParserDestroy
```c
puleParserDestroy(
  parser : PuleParser
) void;
```
### puleParserCreateFromString
```c
puleParserCreateFromString(
  sv : PuleStringView,
  name : PuleStringView,
  commentStart : PuleStringView,
  commentEnd : PuleStringView,
  error : PuleError ptr
) PuleParser;
```
### puleParserCreateForDefaultExpressionGrammar
```c
puleParserCreateForDefaultExpressionGrammar() PuleParser;
```
### puleParserRegexTokenCreate
```c
puleParserRegexTokenCreate(
  parser : PuleParser,
  regex : PuleStringView
) PuleParserRegexToken;
```
### puleParserGroupCreate
```c
puleParserGroupCreate(
  parser : PuleParser,
  nodes : PuleParserRuleNode ptr,
  nodeCount : size_t
) PuleParserGroup;
```
### puleParserRuleCreate
```c
puleParserRuleCreate(
  createInfo : PuleParserRuleCreateInfo
) PuleParserRule;
```
### puleParserRuleSetNodes
```c
puleParserRuleSetNodes(
  parser : PuleParser,
  rule : PuleParserRule,
  nodes : PuleParserRuleNode ptr,
  nodeCount : size_t
) void;
```
### puleParserRule
```c
puleParserRule(
  parser : PuleParser,
  label : PuleStringView
) PuleParserRule;
```
### puleParserDump
```c
puleParserDump(
  parser : PuleParser
) void;
```
### puleParserAstCreate
```c
puleParserAstCreate(
  parser : PuleParser,
  string : PuleStringView,
  headRule : PuleParserRule,
  error : PuleError ptr
) PuleParserAst;
```
### puleParserAstDestroy
```c
puleParserAstDestroy(
  ast : PuleParserAst
) void;
```
### puleParserAstRoot
```c
puleParserAstRoot(
  ast : PuleParserAst
) PuleParserAstNode;
```
### puleParserAstNodeChild
```c
puleParserAstNodeChild(
  node : PuleParserAstNode,
  index : size_t
) PuleParserAstNode;
```
### puleParserAstNodeMatch

this is a convenience function that returns all matches in the node; it
recurses in-order until it finds a leaf node, then appends all leaf node
matches together as the return value

```c
puleParserAstNodeMatch(
  node : PuleParserAstNode
) PuleString;
```
### puleParserAstNodeDump
```c
puleParserAstNodeDump(
  node : PuleParserAstNode
) void;
```
### puleParserAstNodeDumpShallow
```c
puleParserAstNodeDumpShallow(
  node : PuleParserAstNode
) void;
```
