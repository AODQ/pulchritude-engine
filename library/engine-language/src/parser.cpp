#include "parser.hpp"

#include <pulchritude-parser/parser.h>

#include <string>
#include <vector>

/*

parser for the engine's language, known as 'PEL' (Pulchritude Engine Language)

It's meant to be a extremely simple way to interact with the engine at run-time
for development with JIT (with support for compiling for release). It's aware
of all the engine symbols, and you can inject your own third-party symbols.

It's a C-like language sans-syntax, with only few actual changes:
  - no arrays, only pointers (you can define an array to a pointer)
  - no declarable enums, structs or unions (they can still come from the engine)
  - many other features cut out (macros, type aliases, etc)

The use-case is to replace the need for a scripting language, as while you can
still use Lua & friends with the engine, this is meant to be a more friendly
approach:
  - you can access memory directly (pointers, memory allocations, etc)
  - support for 'struct methods' (they convert to C-like functions)
  - indices are 0-based (as opposed to 1-based in Lua)
  - no garbage collection

While you can write an entire application/game in PEL, it's designed to be
used for writing logic and possibly in the future to REPL debug 

Here are some examples

---
@fn main() -> void {
  puleLog(`hello world`,);
};


@fn add(a : i32, b : i32,) -> i32 {
  @let chaos : i32 := puleRand(0, 100,);
  return a + b + chaos;
}

@fn fib(n : i32,) -> i32 {
  if (n <= 1) {
    return n;
  }
  return fib(n - 1,) + fib(n - 2,);
}

@fn printElements(a : i32 @ptr, count : usize,) -> void {
  # for loop
  for @(++ it : usize; 0 .. count) {
    puleLog("%d", a[it],);
  }
  # while loop
  @mut it : usize := 0;
  while (it < count) {
    puleLog("%d", a[it],);
    ++ it;
  }
}

@fn printRange(buf : PuleBuffer,) -> void {
  for @(++ it : u8 *; buf.contents .. buf.contents + buf.len) {
    puleLog("%c", it.*,);
  }
}

@fn randArray() -> i32 {
  @let arr : i32 * = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
  @let idx : usize := puleRand(0, 9,);
  return (arr + idx).*;
}

@fn randArrayAlternative() -> i32 {
  @let arr : i32 * = alloc(10 * $sizeof(i32,),);
  for @(++ it : i32 *; arr .. arr + 10) {
    it.* = $cast(i32, it - arr,);
  }
  return arr[puleRand(0, 9,)];
}

@fn handleAiLogic(ai : PulAiController,) -> void {
  switch (ai.state) {
    ^ PulAiState_idle {
      ai.state := PulAiState_walking;
    },
    ^ PulAiState_walking {
      // Or alternatively, with the actual non-translated C ABI
      pulAiStateSet(ai, PulAiState_walking,);
    },
  }
}
---

*/

namespace pint {

std::string const expressionGrammar = R"(
%globals := %global+;
%global := (%function_definition | %function_declaration |);
%function_declaration :=
  %function_tag %identifier '\(' %parameter* '\)' '->' %type ';'
;
%function_definition :=
  '@fn' %identifier '\(' %parameter* '\)' '->' %type %block;
%function_tag := ('@fn' | '@externfn' |);
%parameter := %identifier ':' %type ',';
%block := '\{' %body '\}';
%body := %statement*;
%statement := (%statement_block | %statement_instruction |);
%statement_block :=
  (%statement_if | %statement_while | %statement_switch | %block |);
%statement_instruction :=
  (%call | %return | %declaration | %assignment |) ';';

%statement_switch := 'switch' '\(' %expression '\)' %block_switch;

%block_switch := '\{' %block_switch_case* '\}';

%block_switch_case := ('default' | %block_switch_case_caret |) %block ',';
%block_switch_case_caret := '\^' %expression;

%statement_if := 'if' '\(' %expression '\)' %block %statement_else?;
%statement_else := 'else' %statement_block;

%statement_while := 'while' '\(' %expression '\)' %block;

%declaration := ('@let' | '@mut' |) %identifier ':' %type '=' %expression;

%assignment := %identifier '=' %expression;

%return := 'return' %expression?;
%call := %identifier '\(' %argument* '\)';
%argument := %expression ',';

%expression := %expression_logic;
%expression_logic := %expression_additive %expression_logic_tail*;
%expression_logic_tail := %operator_logic %expression_additive;
%expression_additive := %expression_multiplicative %expression_additive_tail*;
%expression_additive_tail := %operator_additive %expression_multiplicative;
%expression_multiplicative := %expression_unary %expression_multiplicative_tail*;
%expression_multiplicative_tail := %operator_multiplicative %expression_unary;
%expression_unary :=
  '[-!]'? (%expression_parens | %literal | %identifier | %type |);
%expression_parens := '\(' %expression '\)';

%operator_logic :=
  (
      '==' | '!=' | '<' | '>' | '<='
    | '>=' | '&&' | '||' | '!' | '&' | '\|'
    | '^'
    |
  )
;
%operator_additive := ('\+' | '\-' |);
%operator_multiplicative := ('\*' | '%' | '\/' |);

%literal := (%string | %integer | %float | %boolean | %metacall | %call |);

%metacall := '@' %identifier '\(' %argument* '\)';

%integer := '[0-9]+';
%float := '[0-9]+\.([0-9]+)?f';
%boolean := ('true' | 'false' |);
%string := '`[^`]*`';
%type := %identifier %typemodifier*;
%typemodifier := 'ptr';
%identifier := '[a-zA-Z_][a-zA-Z0-9_]*';
)";

} // namespace pint

PuleParser pule::puleParserCreateForDefaultEngine() {
  PuleError error = puleError();
  PuleParser parser = (
    puleParserCreateFromString(
      puleCStr(pint::expressionGrammar.c_str()),
      puleCStr("pulchritude-engine-language"),
      puleCStr("#"),
      puleCStr("\n"),
      &error
    )
  );
  if (puleErrorConsume(&error)) { return {}; }
  return parser;
}
