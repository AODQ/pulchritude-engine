#include <pulchritude-parser/parser.h>

#include <regex>

namespace pint {

struct RegexToken {
  std::regex matcher;
  std::string label;
  uint64_t id = 0;
};

struct Rule {
  std::vector<PuleParserRuleNode> nodes;
  std::string label;
  uint64_t id = 0;
  bool ignoreWhitespace = false;
};

// group contains nodes, one of which must be matched
struct Group {
  std::vector<PuleParserRuleNode> nodes;
  uint64_t id = 0;
};

struct Parser {
  std::string name;
  std::string commentStart;
  std::string commentEnd;

  std::vector<RegexToken> regexTokens;
  std::vector<Rule> rules;
  std::vector<Group> groups;

  std::unordered_map<std::string, uint64_t> ruleLabelToId;
};

pule::ResourceContainer<Parser, PuleParser> parsermap;

std::string dumpRuleNode(
  pint::Parser const & parser,
  PuleParserRuleNode const & node
) {
  std::string str;
  switch (node.type) {
    default: PULE_assert(false && "sequence in rule node"); break;
    case PuleParserNodeType_regex:
      str += "'" + parser.regexTokens[node.id].label + "'";
    break;
    case PuleParserNodeType_rule:
      str += "%" + parser.rules[node.id].label;
    break;
    case PuleParserNodeType_group:
      str += "(";
      for (auto const & groupnode : parser.groups[node.id].nodes) {
        str += pint::dumpRuleNode(parser, groupnode);
        str += " | ";
      }
      str += ")";
    break;
  }
  switch (node.repeat) {
    case PuleParserNodeRepeat_zeroOrMore: str += ("*"); break;
    case PuleParserNodeRepeat_oneOrMore: str += ("+"); break;
    case PuleParserNodeRepeat_optional: str += ("?"); break;
    case PuleParserNodeRepeat_once: break;
  }
  return str + " ";
}

} // namespace pint

// -- creation -----------------------------------------------------------------

extern "C" {

PuleParser puleParserCreate(
  PuleStringView const name,
  PuleStringView const commentStart,
  PuleStringView const commentEnd
) {
  return (
    pint::parsermap.create({
      .name = name.contents,
      .commentStart = commentStart.contents,
      .commentEnd = commentEnd.contents,
    })
  );
}
void puleParserDestroy(PuleParser const parser) {
  pint::parsermap.destroy(parser);
}


PuleParserRegexToken puleParserRegexTokenCreate(
  PuleParser const puParser,
  PuleStringView const regex
) {
  pint::Parser & parser = *pint::parsermap.at(puParser);
  pint::RegexToken regexToken;
  std::string fixedRegex = std::string{ regex.contents, regex.len };
  fixedRegex = "^" + fixedRegex; // match from start of string
  regexToken.matcher = (
    std::regex(fixedRegex, std::regex::optimize | std::regex::ECMAScript)
  );
  regexToken.label = std::string(regex.contents, regex.len);
  regexToken.id = parser.regexTokens.size();
  parser.regexTokens.emplace_back(regexToken);
  return { regexToken.id, };
}

PuleParserGroup puleParserGroupCreate(
  PuleParser const puParser,
  PuleParserRuleNode * const nodes,
  size_t const nodeCount
) {
  pint::Parser & parser = *pint::parsermap.at(puParser);
  pint::Group group;
  group.id = parser.groups.size();
  for (size_t i = 0; i < nodeCount; ++i) {
    group.nodes.emplace_back(nodes[i]);
  }
  parser.groups.emplace_back(group);
  return { group.id, };
}

PuleParserRule puleParserRuleCreate(PuleParserRuleCreateInfo const ci) {
  pint::Parser & parser = *pint::parsermap.at(ci.parser);
  auto label = std::string{ ci.label.contents, ci.label.len };
  PULE_assert(parser.ruleLabelToId.find(label) == parser.ruleLabelToId.end());
  pint::Rule rule;
  rule.label = label;
  rule.id = parser.rules.size();
  rule.ignoreWhitespace = ci.ignoreWhitespace;
  for (size_t i = 0; i < ci.nodeCount; ++i) {
    rule.nodes.emplace_back(ci.nodes[i]);
  }
  parser.rules.emplace_back(rule);
  parser.ruleLabelToId[rule.label] = rule.id;
  return { rule.id, };
}

void puleParserRuleSetNodes(
  PuleParser const puParser,
  PuleParserRule const rule,
  PuleParserRuleNode * const nodes,
  size_t const nodeCount
) {
  pint::Parser & parser = *pint::parsermap.at(puParser);
  auto & ruleRef = parser.rules[rule.id];
  ruleRef.nodes.clear();
  for (size_t i = 0; i < nodeCount; ++i) {
    ruleRef.nodes.emplace_back(nodes[i]);
  }
}

PuleParserRule puleParserRule(
  PuleParser const puParser,
  PuleStringView const string
) {
  pint::Parser & parser = *pint::parsermap.at(puParser);
  auto label = std::string{ string.contents, string.len };
  return { parser.ruleLabelToId[label], };
}

void puleParserDump(PuleParser const puParser) {
  auto & parser = *pint::parsermap.at(puParser);
  puleLog("-- rules --");
  for (auto const & rule : parser.rules) {
    puleLogRaw("  [%d] %s := ", rule.id, rule.label.c_str());
    for (auto const & node : rule.nodes) {
      puleLogRaw("%s", pint::dumpRuleNode(parser, node).c_str());
    }
    puleLogRaw(";\n");
  }
}

}

// -- AST generation -----------------------------------------------------------

namespace pint {

struct AstNode {
  std::string match;
  PuleParserNodeType type;
  uint64_t parserNodeId;
  std::vector<AstNode> children;
  bool poison = false;
};

struct Ast {
  AstNode root;
};

struct ItTrackerSave {
  size_t itStackSize;
};

struct ItTracker {
  struct Iter {
    PuleParserRuleNode node;
    char const * it;
  };
  std::vector<Iter> its;

  // dereference to last element
  char const * operator*() const { return its.back().it; }

  void push(PuleParserRuleNode node, char const * const it) {
    its.emplace_back(node, it);
  }

  ItTrackerSave save(PuleParserRuleNode const node) {
    ItTrackerSave itSave = { .itStackSize = its.size(), };
    this->push(node, its.back().it);
    return itSave;
  }

  std::string dump(pint::Parser const & parser) {
    std::string str;
    // skip first one as it's just the start
    for (size_t it = 1; it < its.size(); ++ it) {
      str += "[" + pint::dumpRuleNode(parser, its[it].node) + "] -> ";
    }
    return str;
  }

  void pop(ItTrackerSave const save, bool poison) {
    if (its.size() == save.itStackSize) { return; }
    for (size_t it = 1; it < its.size(); ++ it) {
      // if the order is reversed, or left hand is null, implementation bug.
      if (its[it].it < its[it - 1].it) {
        puleLogError("it[%zu] < it[%zu]", it, it - 1);
        PULE_assert(false && "it < it-1");
      }
    }
    // if poisoned, then we remove the top of stack without 'saving' any of
    //   its changes (effectively reverting to the last known good it),
    // and if it's not poisoned, we can overwrite the second-to-last element
    //   with this element, since it's known to be good up-to-this-point.
    // this way we can eliminate a whole 'branch' of iterators if it ends up
    //   being poisoned, e.g. a complex rule with many branches is optional
    if (!poison) {
      its[save.itStackSize - 1].it = its.back().it;
    }
    its.resize(save.itStackSize);
  }
};

pule::ResourceContainer<Ast, PuleParserAst> astmap;

pint::AstNode parseAstNodeRuleNode(
  pint::Parser & parser,
  PuleParserRuleNode node,
  ItTracker & it,
  bool isOptional,
  std::vector<std::string> & errors
);

pint::AstNode parseAstNodeRuleNodeEntry(
  pint::Parser & parser,
  PuleParserRuleNode node,
  ItTracker & it,
  bool optional,
  std::vector<std::string> & errors
) {
  // TODO ignore whitespace from rule
  switch (node.type) {
    default: PULE_assert(false && "sequence in rule node"); break;
    case PuleParserNodeType_regex: {
      auto & regexToken = parser.regexTokens[node.id];
      std::match_results<char const *> match;
      if (std::regex_search(*it, match, regexToken.matcher)) {
        pint::AstNode astNode;
        astNode.match = std::string{ match[0].first, match[0].second };
        astNode.type = PuleParserNodeType_regex;
        astNode.parserNodeId = node.id;
        it.push(node, match[0].second);
        return astNode;
      } else {
        // does not increment it in case the regex is optional
        pint::AstNode astNode;
        astNode.type = PuleParserNodeType_regex;
        astNode.poison = true;
        std::string line;
        auto lineEnd = *it;
        while (*lineEnd != '\n' && *lineEnd != '\0') {
          line += *lineEnd++;
        }
        if (!optional) {
          errors.emplace_back(
              std::string{"expected \""} + regexToken.label + "\""
            + " at expression: " + line
            + "\n  [" + it.dump(parser) + "]"
          );
        }
        it.push(node, lineEnd);
        return astNode;
      }
    } break;
    case PuleParserNodeType_rule: {
      auto & rule = parser.rules[node.id];
      pint::AstNode astNode;
      astNode.match = rule.label;
      astNode.type = PuleParserNodeType_rule;
      astNode.parserNodeId = node.id;
      if (rule.nodes.size() == 0) {
        errors.emplace_back(
          std::string{"rule "} + rule.label + " is not defined"
        );
        astNode.poison = true;
        return astNode;
      }
      for (auto & rulenode : rule.nodes) {
        ItTrackerSave save = it.save(node);
        if (rule.ignoreWhitespace) {
          char const * zit = *it;
          while (std::isspace(*zit)) { ++zit; }
          it.push(node, zit);
        }
        astNode.children.emplace_back(
          pint::parseAstNodeRuleNode(parser, rulenode, it, optional, errors)
        );
        astNode.poison |= astNode.children.back().poison;
        it.pop(save, astNode.poison);
        if (astNode.poison) {
          break;
        }
      }
      return astNode;
    } break;
    case PuleParserNodeType_group: {
      pint::AstNode astNode;
      auto & group = parser.groups[node.id];
      bool matched = false;
      for (auto & groupnode : group.nodes) {
        ItTrackerSave save = it.save(node);
        pint::AstNode nodeCheck = (
          pint::parseAstNodeRuleNode(parser, groupnode, it, true, errors)
        );
        it.pop(save, nodeCheck.poison);
        if (nodeCheck.poison) { continue; }
        matched = true;
        astNode = nodeCheck;
        break;
      }
      if (!matched) { astNode.poison = true; }
      if (!optional) {
        std::string err = "  group: (";
        for (auto const & groupnode : group.nodes) {
          err += parser.rules[groupnode.id].label + " | ";
        }
        err += ") not matched to `";
        auto lineStart = *it;
        while (*lineStart != '\n' && *lineStart != '\0') {
          err += *lineStart++;
        }
        err += "`";
        errors.emplace_back(err + "\n  [" + it.dump(parser) + "]");
      }

      return astNode;
    }
  }
}

pint::AstNode parseAstNodeRuleNode(
  pint::Parser & parser,
  PuleParserRuleNode node,
  ItTracker & it,
  bool optional,
  std::vector<std::string> & errors
) {
  switch (node.repeat) {
    case PuleParserNodeRepeat_zeroOrMore: {
      pint::AstNode astNode;
      while (true) {
        ItTrackerSave save = it.save(node);
        auto nodeCheck = (
          pint::parseAstNodeRuleNodeEntry(parser, node, it, true, errors)
        );
        it.pop(save, nodeCheck.poison);
        if (nodeCheck.poison) { break; }
        astNode.children.emplace_back(nodeCheck);
      }
      astNode.type = PuleParserNodeType_sequence;
      astNode.parserNodeId = node.id;
      return astNode;
    } break;
    case PuleParserNodeRepeat_oneOrMore: {
      pint::AstNode astNode;
      ItTrackerSave save = it.save(node);
      auto nodeCheck = (
        pint::parseAstNodeRuleNodeEntry(parser, node, it, optional, errors)
      );
      it.pop(save, nodeCheck.poison);
      if (nodeCheck.poison) {
        errors.emplace_back("  (1+) expected at least one match");
        return nodeCheck;
      }
      astNode.children.emplace_back(nodeCheck);
      while (true) {
        save = it.save(node);
        nodeCheck = (
          pint::parseAstNodeRuleNodeEntry(parser, node, it, true, errors)
        );
        it.pop(save, nodeCheck.poison);
        if (nodeCheck.poison) { break; }
        astNode.children.emplace_back(nodeCheck);
      }
      astNode.type = PuleParserNodeType_sequence;
      astNode.parserNodeId = node.id;
      astNode.poison = astNode.children.size() == 0;
      return astNode;
    } break;
    case PuleParserNodeRepeat_optional: {
      ItTrackerSave save = it.save(node);
      auto nodeCheck = (
        pint::parseAstNodeRuleNodeEntry(parser, node, it, true, errors)
      );
      it.pop(save, nodeCheck.poison);
      if (nodeCheck.poison) {
        nodeCheck.poison = false;
        nodeCheck.type = node.type;
        nodeCheck.match = "";
        nodeCheck.parserNodeId = node.id;
        nodeCheck.children.clear();
      }
      return nodeCheck;
    }
    case PuleParserNodeRepeat_once: {
      ItTrackerSave save = it.save(node);
      pint::AstNode onceNode = (
        pint::parseAstNodeRuleNodeEntry(parser, node, it, optional, errors)
      );
      it.pop(save, onceNode.poison);
      return onceNode;
    }
  }
}

} // namespace pint

extern "C" {

PuleParserAst puleParserAstCreate(
  PuleParser const puParser,
  PuleStringView const string,
  PuleParserRule const puHeadRule,
  PuleError * const error
) {
  pint::Parser & parser = *pint::parsermap.at(puParser);
  pint::Ast ast;

  std::string stringCommentless = "";

  // remove comments if rule is non-empty
  // TODO have to make sure comment is not in a string
  if (parser.commentStart.size() > 0 && parser.commentEnd.size() > 0)
  {
    char const * it = string.contents;
    bool inComment = false;
    auto & cmtStart = parser.commentStart;
    auto & cmtEnd = parser.commentEnd;
    while (*it != '\0') {
      // check if in comment or not
      if (inComment) {
        if (std::strncmp(it, cmtEnd.c_str(), cmtEnd.size()) == 0) {
          inComment = false;
          it += parser.commentEnd.size();
        } else {
          ++ it;
        }
      } else {
        if (std::strncmp(it, cmtStart.c_str(), cmtStart.size()) == 0) {
          inComment = true;
          it += parser.commentStart.size();
        } else {
          stringCommentless += *it;
          ++ it;
        }
      }
    }
  } else {
    stringCommentless = std::string{ string.contents, string.len };
  }

  // parse commentless string
  pint::ItTracker itTracker = {
    .its = { { .node = {}, .it = stringCommentless.c_str(), }, },
  };

  std::vector<std::string> errors;

  ast.root = (
    pint::parseAstNodeRuleNode(
      parser,
      PuleParserRuleNode {
        .type = PuleParserNodeType_rule,
        .repeat = PuleParserNodeRepeat_once,
        .id = puHeadRule.id,
      },
      itTracker,
      false,
      errors
    )
  );
  // skip whitespace until EOF or non-whitespace
  char const * it = *itTracker;
  while (*it != '\0' && (*it == '\n' || std::isspace(*it))) { ++it; }
  // if not EOF, then we haven't parsed everything
  if (*it != '\0') {
    std::string err = "expected end of string, got '";
    auto lineStart = it;
    while (*lineStart != '\0' && *lineStart != '\n') {
      err += *lineStart;
      ++ lineStart;
    }
    err += "'";
    errors.emplace_back(err);
    ast.root.poison = true;
  }
  // print out all parsing errors
  // TODO if it's not poisoned, we should assert there are no errors, but as
  //      of now there can be errors because we don't track errors with the
  //      same care as the iterator tracker
  if (ast.root.poison) {
    PuleError err = puleError();
    err.lineNumber = __LINE__;
    err.sourceLocationNullable = __FILE__;
    err.id = PuleErrorParser_astPoisoned;
    std::string errorstr = "\n";
    for (auto const & e : errors) {
      errorstr += "  " + e + "\n";
    }
    err.description = (
      puleStringFormatDefault("ast root poison, errors: %s", errorstr.c_str())
    );
    puleErrorPropagate(error, err);
    return { .id = 0, };
  }

  return pint::astmap.create(ast);
}

void puleParserAstDestroy(PuleParserAst const ast) {
  if (ast.id == 0) { return; }
  pint::astmap.destroy(ast);
}

} // extern "C"

namespace pint {

void astNodeDump(PuleParserAstNode const puNode, size_t const depth);

void astNodeDumpChildren(
  PuleParserAstNode const puNode,
  size_t const depth
) {
  // inline children
  if (puNode.childCount == 1) {
    puleLogRaw(" | ");
    astNodeDump(puleParserAstNodeChild(puNode, 0), depth);
    return;
  }
  // multiline children
  for (size_t i = 0; i < puNode.childCount; ++i) {
    puleLogRaw("\n%*s", depth * 2, " ");
    astNodeDump(puleParserAstNodeChild(puNode, i), depth + 1);
  }
}

void astNodeDump(PuleParserAstNode const puNode, size_t const depth = 0) {
  if (puNode.id == 0) { return; }
  auto const & node = *(pint::AstNode *)puNode.id;
  switch (node.type) {
    case PuleParserNodeType_sequence:
      puleLogRaw("sequence");
      astNodeDumpChildren(puNode, depth);
    break;
    case PuleParserNodeType_regex:
      puleLogRaw("regex '%s'", node.match.c_str());
    break;
    case PuleParserNodeType_rule: {
      std::string info;
      puleLogRaw("%%%s", node.match.c_str());
      astNodeDumpChildren(puNode, depth);
    } break;
    case PuleParserNodeType_group:
      puleLogRaw("group");
      astNodeDumpChildren(puNode, depth);
    break;
  }
}

} // namespace pint

// -- AST traversal ------------------------------------------------------------
extern "C" {

PuleParserAstNode puleParserAstRoot(PuleParserAst const ast) {
  pint::Ast & pintAst = *pint::astmap.at(ast);
  return {
    .id = (uint64_t)&pintAst.root,
    .match = puleCStr(pintAst.root.match.c_str()),
    .type = pintAst.root.type,
    .parserNodeId = pintAst.root.parserNodeId,
    .childCount = pintAst.root.children.size(),
  };
}

PuleParserAstNode puleParserAstNodeChild(
  PuleParserAstNode const node,
  size_t const index
) {
  pint::AstNode * astNode = (pint::AstNode *)node.id;
  if (index < 0 || index >= astNode->children.size()) {
    puleLogError(
      "index %zu out of bounds [0, %zu)", index, astNode->children.size()
    );
    puleParserAstNodeDump(node);
    return { .id = 0, };
  }
  PULE_assert(index < astNode->children.size());
  astNode = &astNode->children[index];
  return {
    .id = (uint64_t)astNode,
    .match = puleCStr(astNode->match.c_str()),
    .type = astNode->type,
    .parserNodeId = astNode->parserNodeId,
    .childCount = astNode->children.size(),
  };
}

// -- dump ---------------------------------------------------------------------

void puleParserAstNodeDump(PuleParserAstNode const node) {
  pint::astNodeDump(node);
  puleLogRaw("\n");
}

void puleParserAstNodeDumpShallow(PuleParserAstNode const puNode) {
  if (puNode.id == 0) { return; }
  auto const & node = *(pint::AstNode *)puNode.id;
  auto const childrenCount = node.children.size();
  std::string type;
  switch (node.type) {
    case PuleParserNodeType_sequence: type = "sequence"; break;
    case PuleParserNodeType_regex:    type = "regex";    break;
    case PuleParserNodeType_rule:     type = "rule";     break;
    case PuleParserNodeType_group:    type = "group";    break;
  }
  puleLog(
    "%s [%d children] [match '%s']",
    type.c_str(), childrenCount, node.match.c_str()
  );
}

} // extern "C"
