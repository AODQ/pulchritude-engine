#include <pulchritude-parser/parser.h>

#include <string>
#include <vector>

extern "C" {

// the following is an approximate representation of the language in PEG/BNF:
//
// rules := ruledef+;
// ruledef := %ruleref ":[^=]*=" %ruleset ';';
// ruleset := %ruletoken+;
// ruletoken := ( %ruleref | %regex | %group | ) %repeat;
// group := '\(' %grouptoken+ '\)';
// grouptoken := %ruletoken '\|';
// ruleref :ws= '%' %identifier;
// regex := '\'(?:[^'\\\\]|\\\\.)*\'';
// identifier := '[a-zA-Z_][a-zA-Z0-9_]*';
// repeat := ('\*' | '\+' | '\?' |)?;

PuleParser puleParserCreateForDefaultExpressionGrammar() {
  PuleParser parser = puleParserCreate();

  #define token(name, regex) \
    PuleParserRegexToken const rt##name = ( \
      puleParserRegexTokenCreate(parser, puleCStr(regex)) \
    )

  token(LParen, "\\(");
  token(RParen, "\\)");
  token(RBar, "\\|");
  token(Star, "\\*");
  token(Semicolon, ";");
  token(Plus, "\\+");
  token(Question, "\\?");
  token(ColonEq, ":[^=]*=");
  token(Percent, "%");
  token(Identifier, "[a-zA-Z_][a-zA-Z0-9_]*");
  token(Regex, "'(?:[^'\\\\]|\\\\.)*'");

  #undef token

  #define rulecreate(rulelabel, name, ignoreWs) \
    PuleParserRule r##rulelabel = puleParserRuleCreate({ \
      .parser = parser, \
      .label = puleCStr(#name), \
      .nodes = nullptr, \
      .nodeCount = 0, \
      .ignoreWhitespace = ignoreWs, \
    })

  rulecreate(Ruletoken, ruletoken, true);
  rulecreate(Ruleset, ruleset, true);
  rulecreate(Grouptoken, grouptoken, true);
  rulecreate(Group, group, true);

  #undef rulecreate

  #define ruletoken(type, repeat, name) \
    { PuleParserNodeType_##type, PuleParserNodeRepeat_##repeat, name.id }

  // -- repeat --
  std::vector<PuleParserRuleNode> gRepeatRules = {
    ruletoken(regex, once, rtStar),
    ruletoken(regex, once, rtPlus),
    ruletoken(regex, once, rtQuestion),
  };
  PuleParserGroup gRepeat = (
    puleParserGroupCreate(parser, gRepeatRules.data(), gRepeatRules.size())
  );

  std::vector<PuleParserRuleNode> rRepeatRules = {
    ruletoken(group, optional, gRepeat),
  };

  PuleParserRule rRepeat = (
    puleParserRuleCreate({
      .parser = parser,
      .label = puleCStr("repeat"),
      .nodes = rRepeatRules.data(),
      .nodeCount = rRepeatRules.size(),
      .ignoreWhitespace = false,
    })
  );

  // -- identifier --
  PuleParserRuleNode rIdentifierRules = ruletoken(regex, once, rtIdentifier);
  PuleParserRule rIdentifier = (
    puleParserRuleCreate({
      .parser = parser,
      .label = puleCStr("identifier"),
      .nodes = &rIdentifierRules,
      .nodeCount = 1,
      .ignoreWhitespace = false,
    })
  );

  // -- regex --
  PuleParserRuleNode rRegexRules = ruletoken(regex, once, rtRegex);
  PuleParserRule rRegex = (
    puleParserRuleCreate({
      .parser = parser,
      .label = puleCStr("regex"),
      .nodes = &rRegexRules,
      .nodeCount = 1,
      .ignoreWhitespace = false,
    })
  );

  // -- ruleref    -- :nows= "%" %identifier
  std::vector<PuleParserRuleNode> rRulerefRules = {
    ruletoken(regex, once, rtPercent),
    ruletoken(rule, once, rIdentifier),
  };
  PuleParserRule rRuleref = (
    puleParserRuleCreate({
      .parser = parser,
      .label = puleCStr("ruleref"),
      .nodes = rRulerefRules.data(),
      .nodeCount = rRulerefRules.size(),
      .ignoreWhitespace = true,
    })
  );
  // -- ruletoken -- := ( %ruleref | %regex | %group | ) %repeat;
  std::vector<PuleParserRuleNode> gRuletokenGroupRules = {
    ruletoken(rule, once, rRuleref),
    ruletoken(rule, once, rRegex),
    ruletoken(rule, once, rGroup),
  };
  PuleParserGroup rRuletokenGroup = (
    puleParserGroupCreate(
      parser,
      gRuletokenGroupRules.data(),
      gRuletokenGroupRules.size()
    )
  );
  std::vector<PuleParserRuleNode> rRuletokenRules = {
    ruletoken(group, once, rRuletokenGroup),
    ruletoken(rule, once, rRepeat),
  };
  puleParserRuleSetNodes(
    parser, rRuletoken, rRuletokenRules.data(), rRuletokenRules.size()
  );

  // -- ruleset -- := %ruletoken+
  std::vector<PuleParserRuleNode> rRulesetRules = {
    ruletoken(rule, oneOrMore, rRuletoken),
  };
  puleParserRuleSetNodes(
    parser, rRuleset, rRulesetRules.data(), rRulesetRules.size()
  );

  // -- grouptoken -- := %ruletoken '\|';
  std::vector<PuleParserRuleNode> gGrouptokenGroupRules = {
    ruletoken(rule, once, rRuleref),
    ruletoken(rule, once, rRegex),
    ruletoken(rule, once, rGroup),
  };
  std::vector<PuleParserRuleNode> rGrouptokenRules = {
    ruletoken(rule, once, rRuletoken),
    ruletoken(regex, once, rtRBar),
  };
  puleParserRuleSetNodes(
    parser, rGrouptoken, rGrouptokenRules.data(), rGrouptokenRules.size()
  );

  // -- group -- := "(" %grouptoken+ ")"
  std::vector<PuleParserRuleNode> rGroupRules = {
    ruletoken(regex, once, rtLParen),
    ruletoken(rule, oneOrMore, rGrouptoken),
    ruletoken(regex, once, rtRParen),
  };
  puleParserRuleSetNodes(
    parser, rGroup, rGroupRules.data(), rGroupRules.size()
  );
  // -- ruledef -- := %ruleref ":[^=]*=" %ruleset ';'
  std::vector<PuleParserRuleNode> rRuledefRules = {
    ruletoken(rule, once, rRuleref),
    ruletoken(regex, once, rtColonEq),
    ruletoken(rule, once, rRuleset),
    ruletoken(regex, once, rtSemicolon),
  };
  PuleParserRule rRuledef = puleParserRuleCreate({
    .parser = parser,
    .label = puleCStr("ruledef"),
    .nodes = rRuledefRules.data(),
    .nodeCount = rRuledefRules.size(),
    .ignoreWhitespace = true,
  });

  // -- rules -- := ruledef+
  std::vector<PuleParserRuleNode> rRulesRules = {
    ruletoken(rule, oneOrMore, rRuledef),
  };
  puleParserRuleCreate({
    .parser = parser,
    .label = puleCStr("rules"),
    .nodes = rRulesRules.data(),
    .nodeCount = rRulesRules.size(),
    .ignoreWhitespace = true,
  });

  #undef ruletoken

  return parser;
}

} // extern C

namespace pint {

struct Ctx {
  std::unordered_map<std::string, PuleParserRule> rules;
  PuleParser parser;
  bool findRules;
};

std::string traverseAstRuleref(PuleParserAstNode const node) {
  // in this case, create rules to be referenced later
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "ruleref"));
  PULE_assert(node.childCount == 2);
  PuleParserAstNode const identifierNode = puleParserAstNodeChild(node, 1);
  PULE_assert(identifierNode.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(identifierNode.match, "identifier"));
  PULE_assert(identifierNode.childCount == 1);
  PuleParserAstNode const identifierRegexNode = puleParserAstNodeChild(
    identifierNode, 0
  );
  PULE_assert(identifierRegexNode.type == PuleParserNodeType_regex);
  return std::string(identifierRegexNode.match.contents);
}

PuleParserRuleNode traverseAstRuleToken(
  PuleParserAstNode const node,
  Ctx & ctx
);

// grouptoken := ( %ruleref | %regex | %group ) '\|';
PuleParserRuleNode traverseAstGroupToken(
  PuleParserAstNode const node,
  Ctx & ctx
) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(node.childCount == 2);
  PuleParserAstNode const tokenNode = puleParserAstNodeChild(node, 0);
  return traverseAstRuleToken(tokenNode, ctx);
}

// group := '\(' %grouptoken+ '\)';
PuleParserRuleNode traverseAstGroup(PuleParserAstNode const node, Ctx & ctx) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(node.childCount == 3);
  auto const seqNode = puleParserAstNodeChild(node, 1);
  PULE_assert(seqNode.type == PuleParserNodeType_sequence);
  std::vector<PuleParserRuleNode> groupTokens;
  for (size_t it = 0; it < seqNode.childCount; ++ it) {
    PuleParserAstNode const tokenNode = puleParserAstNodeChild(seqNode, it);
    groupTokens.emplace_back(traverseAstGroupToken(tokenNode, ctx));
  }
  return {
    .type = PuleParserNodeType_group,
    .id = (
      puleParserGroupCreate(
        ctx.parser,
        groupTokens.data(),
        groupTokens.size()
      )
    ).id,
  };
}

// ruletoken := ( %ruleref | %regex | %group | ) %repeat;
PuleParserRuleNode traverseAstRuleToken(
  PuleParserAstNode const node,
  Ctx & ctx
) {
  size_t const childCount = node.childCount;
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "ruletoken"));
  PULE_assert(childCount == 2);
  // unwrap repeat
  PuleParserNodeRepeat repeat;
  PuleParserAstNode const repeatNode = puleParserAstNodeChild(node, 1);
  PULE_assert(repeatNode.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(repeatNode.match, "repeat"));
  PULE_assert(repeatNode.childCount <= 1);
  PuleParserAstNode const rnode = puleParserAstNodeChild(repeatNode, 0);
  if (puleStringViewEqCStr(rnode.match, "*")) {
    repeat = PuleParserNodeRepeat_zeroOrMore;
  } else if (puleStringViewEqCStr(rnode.match, "+")) {
    repeat = PuleParserNodeRepeat_oneOrMore;
  } else if (puleStringViewEqCStr(rnode.match, "?")) {
    repeat = PuleParserNodeRepeat_optional;
  } else if (puleStringViewEqCStr(rnode.match, "")) {
    repeat = PuleParserNodeRepeat_once;
  } else {
    puleLog("Unknown repeat: '%s", rnode.match.contents);
    PULE_assert(false);
  }

  // unwrap group (note groups never show up in the AST)
  PuleParserAstNode const groupNode = puleParserAstNodeChild(node, 0);
  PULE_assert(groupNode.type == PuleParserNodeType_rule);
  PuleParserRuleNode rulenode;
  rulenode.repeat = repeat;
  if (puleStringViewEqCStr(groupNode.match, "ruleref")) {
    rulenode.type = PuleParserNodeType_rule;
    rulenode.id = (
      puleParserRule(
        ctx.parser,
        puleCStr(traverseAstRuleref(groupNode).c_str())
      ).id
    );
  } else if (puleStringViewEqCStr(groupNode.match, "regex")) {
    rulenode.type = PuleParserNodeType_regex;
    PuleParserAstNode const regexNode = puleParserAstNodeChild(groupNode, 0);
    PULE_assert(regexNode.type == PuleParserNodeType_regex);
    // strip off the quotes
    std::string match = std::string(regexNode.match.contents);
    match = match.substr(1, match.length()-2);
    rulenode.id = (
      puleParserRegexTokenCreate(ctx.parser, puleCStr(match.c_str())).id
    );
  } else if (puleStringViewEqCStr(groupNode.match, "group")) {
    rulenode.type = PuleParserNodeType_group;
    rulenode.id = traverseAstGroup(groupNode, ctx).id;
  } else {
    PULE_assert(false);
  }
  return rulenode;
}

// ruleset := %ruletoken+;
std::vector<PuleParserRuleNode> traverseAstRuleSet(
  PuleParserAstNode const node,
  Ctx & ctx
) {
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "ruleset"));
  // unwrap sequence
  PuleParserAstNode const seqNode = puleParserAstNodeChild(node, 0);
  PULE_assert(seqNode.type == PuleParserNodeType_sequence);
  std::vector<PuleParserRuleNode> nodes;
  for (size_t it = 0; it < seqNode.childCount; ++ it) {
    nodes.emplace_back(
      traverseAstRuleToken(puleParserAstNodeChild(seqNode, it), ctx)
    );
  }
  return nodes;
}

// ruledef := %ruleref ":[^=]*=" %ruleset ';';
void traverseAstRuleDefs(PuleParserAstNode const node, Ctx & ctx) {
  size_t const childCount = node.childCount;
  PULE_assert(node.type == PuleParserNodeType_rule);
  PULE_assert(puleStringViewEqCStr(node.match, "ruledef"));
  PULE_assert(childCount == 4);
  std::string ruleName = traverseAstRuleref(puleParserAstNodeChild(node, 0));
  if (ctx.findRules) {
    ctx.rules.emplace(
      ruleName,
      puleParserRuleCreate({
        .parser = ctx.parser,
        .label = puleCStr(ruleName.c_str()),
        .nodes = nullptr,
        .nodeCount = 0,
        .ignoreWhitespace = true, // TODO can check :=/:ws= in ruledef
      })
    );
    return;
  }

  // can create everything else as we know all rule IDs
  std::vector<PuleParserRuleNode> ruleNodes = (
    traverseAstRuleSet(puleParserAstNodeChild(node, 2), ctx)
  );
  puleParserRuleSetNodes(
    ctx.parser,
    ctx.rules.at(ruleName),
    ruleNodes.data(),
    ruleNodes.size()
  );
}

void traverseAstRules(
  PuleParserAstNode const node,
  Ctx & ctx
) {
  // unwrap the rule
  size_t const childCount = node.childCount;
  PULE_assert(node.type == PuleParserNodeType_rule);
  puleLog("Rule: %s", node.match.contents);
  PULE_assert(puleStringViewEqCStr(node.match, "rules"));

  // unwrap rule's sequence of ruledefs
  PULE_assert(childCount == 1); // sequence
  PuleParserAstNode const seqNode = puleParserAstNodeChild(node, 0);
  size_t const seqChildCount = seqNode.childCount;
  PULE_assert(seqNode.type == PuleParserNodeType_sequence);
  for (size_t it = 0; it < seqChildCount; ++it) {
    traverseAstRuleDefs(puleParserAstNodeChild(seqNode, it), ctx);
  }
}

} // namespace pint


extern "C" {

PULE_exportFn PuleParser puleParserCreateFromString(
  PuleStringView const sv,
  PuleError * const error
) {
  PuleParser egParser = puleParserCreateForDefaultExpressionGrammar();
  puleParserDump(egParser);

  auto ast = (
    puleParserAstCreate(
      egParser,
      sv,
      puleParserRule(egParser, puleCStr("rules")),
      error
    )
  );
  if (puleErrorConsume(error)) { return {}; }

  // first iterate and find all rules, then generate
  pint::Ctx ctx = { .parser = puleParserCreate(), .findRules = true, };
  pint::traverseAstRules(puleParserAstRoot(ast), ctx);
  ctx.findRules = false;
  pint::traverseAstRules(puleParserAstRoot(ast), ctx);

  return ctx.parser;
}

} // extern "C"
