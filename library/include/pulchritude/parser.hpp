/* auto generated file parser */
#pragma once
#include "core.hpp"

#include "parser.h"
#include "string.hpp"
#include "file.hpp"

namespace pule {
struct Parser {
  PuleParser _handle;
  inline operator PuleParser() const {
    return _handle;
  }
  inline void destroy() {
    return puleParserDestroy(this->_handle);
  }
  inline PuleParserRegexToken regexTokenCreate(PuleStringView regex) {
    return puleParserRegexTokenCreate(this->_handle, regex);
  }
  inline PuleParserGroup groupCreate(PuleParserRuleNode * nodes, size_t nodeCount) {
    return puleParserGroupCreate(this->_handle, nodes, nodeCount);
  }
  inline void ruleSetNodes(PuleParserRule rule, PuleParserRuleNode * nodes, size_t nodeCount) {
    return puleParserRuleSetNodes(this->_handle, rule, nodes, nodeCount);
  }
  inline PuleParserRule rule(PuleStringView label) {
    return puleParserRule(this->_handle, label);
  }
  inline void dump() {
    return puleParserDump(this->_handle);
  }
  inline PuleParserAst astCreate(PuleStringView string, PuleParserRule headRule, PuleError * error) {
    return puleParserAstCreate(this->_handle, string, headRule, error);
  }
  static inline Parser create(PuleStringView name, PuleStringView commentStart, PuleStringView commentEnd) {
    return { ._handle = puleParserCreate(name, commentStart, commentEnd),};
  }
  static inline Parser createFromString(PuleStringView sv, PuleStringView name, PuleStringView commentStart, PuleStringView commentEnd, PuleError * error) {
    return { ._handle = puleParserCreateFromString(sv, name, commentStart, commentEnd, error),};
  }
  static inline Parser createForDefaultExpressionGrammar() {
    return { ._handle = puleParserCreateForDefaultExpressionGrammar(),};
  }
};
}
  inline void destroy(pule::Parser self) {
    return puleParserDestroy(self._handle);
  }
  inline PuleParserRegexToken regexTokenCreate(pule::Parser self, PuleStringView regex) {
    return puleParserRegexTokenCreate(self._handle, regex);
  }
  inline PuleParserGroup groupCreate(pule::Parser self, PuleParserRuleNode * nodes, size_t nodeCount) {
    return puleParserGroupCreate(self._handle, nodes, nodeCount);
  }
  inline void ruleSetNodes(pule::Parser self, PuleParserRule rule, PuleParserRuleNode * nodes, size_t nodeCount) {
    return puleParserRuleSetNodes(self._handle, rule, nodes, nodeCount);
  }
  inline PuleParserRule rule(pule::Parser self, PuleStringView label) {
    return puleParserRule(self._handle, label);
  }
  inline void dump(pule::Parser self) {
    return puleParserDump(self._handle);
  }
  inline PuleParserAst astCreate(pule::Parser self, PuleStringView string, PuleParserRule headRule, PuleError * error) {
    return puleParserAstCreate(self._handle, string, headRule, error);
  }
namespace pule {
struct ParserRegexToken {
  PuleParserRegexToken _handle;
  inline operator PuleParserRegexToken() const {
    return _handle;
  }
  static inline ParserRegexToken create(PuleParser parser, PuleStringView regex) {
    return { ._handle = puleParserRegexTokenCreate(parser, regex),};
  }
};
}
namespace pule {
struct ParserGroup {
  PuleParserGroup _handle;
  inline operator PuleParserGroup() const {
    return _handle;
  }
  static inline ParserGroup create(PuleParser parser, PuleParserRuleNode * nodes, size_t nodeCount) {
    return { ._handle = puleParserGroupCreate(parser, nodes, nodeCount),};
  }
};
}
namespace pule {
struct ParserRule {
  PuleParserRule _handle;
  inline operator PuleParserRule() const {
    return _handle;
  }
  static inline ParserRule create(PuleParserRuleCreateInfo createInfo) {
    return { ._handle = puleParserRuleCreate(createInfo),};
  }
};
}
namespace pule {
struct ParserAst {
  PuleParserAst _handle;
  inline operator PuleParserAst() const {
    return _handle;
  }
  inline void destroy() {
    return puleParserAstDestroy(this->_handle);
  }
  inline PuleParserAstNode root() {
    return puleParserAstRoot(this->_handle);
  }
  static inline ParserAst create(PuleParser parser, PuleStringView string, PuleParserRule headRule, PuleError * error) {
    return { ._handle = puleParserAstCreate(parser, string, headRule, error),};
  }
};
}
  inline void destroy(pule::ParserAst self) {
    return puleParserAstDestroy(self._handle);
  }
  inline PuleParserAstNode root(pule::ParserAst self) {
    return puleParserAstRoot(self._handle);
  }
namespace pule {
using ParserRuleNode = PuleParserRuleNode;
}
namespace pule {
using ParserRuleCreateInfo = PuleParserRuleCreateInfo;
}
namespace pule {
struct ParserAstNode {
  PuleParserAstNode _handle;
  inline operator PuleParserAstNode() const {
    return _handle;
  }
  inline PuleParserAstNode child(size_t index) {
    return puleParserAstNodeChild(this->_handle, index);
  }
  inline PuleString match() {
    return puleParserAstNodeMatch(this->_handle);
  }
  inline void dump() {
    return puleParserAstNodeDump(this->_handle);
  }
  inline void dumpShallow() {
    return puleParserAstNodeDumpShallow(this->_handle);
  }
};
}
  inline PuleParserAstNode child(pule::ParserAstNode self, size_t index) {
    return puleParserAstNodeChild(self._handle, index);
  }
  inline PuleString match(pule::ParserAstNode self) {
    return puleParserAstNodeMatch(self._handle);
  }
  inline void dump(pule::ParserAstNode self) {
    return puleParserAstNodeDump(self._handle);
  }
  inline void dumpShallow(pule::ParserAstNode self) {
    return puleParserAstNodeDumpShallow(self._handle);
  }
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorParser const e) {
  switch (e) {
    case PuleErrorParser_none: return puleString("none");
    case PuleErrorParser_astPoisoned: return puleString("astPoisoned");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleParserNodeType const e) {
  switch (e) {
    case PuleParserNodeType_regex: return puleString("regex");
    case PuleParserNodeType_rule: return puleString("rule");
    case PuleParserNodeType_group: return puleString("group");
    case PuleParserNodeType_sequence: return puleString("sequence");
    default: return puleString("N/A");
  }
}
inline pule::str toStr(PuleParserNodeRepeat const e) {
  switch (e) {
    case PuleParserNodeRepeat_zeroOrMore: return puleString("zeroOrMore");
    case PuleParserNodeRepeat_oneOrMore: return puleString("oneOrMore");
    case PuleParserNodeRepeat_optional: return puleString("optional");
    case PuleParserNodeRepeat_once: return puleString("once");
    default: return puleString("N/A");
  }
}
}
