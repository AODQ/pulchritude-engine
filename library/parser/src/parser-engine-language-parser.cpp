#include <pulchritude-parser/parser.h>

#include <vector>

#include <string>

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
}

@fn add(a : i32, b : i32,) -> i32 {
  @const chaos : i32 := puleRand(0, 100,);
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
  @var it : usize := 0;
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
  @const arr : i32 * = { 1, 2, 3, 4, 5, 6, 7, 8, 9, };
  @const idx : usize := puleRand(0, 9,);
  return (arr + idx).*;
}

@fn randArrayAlternative() -> i32 {
  @const arr : i32 * = alloc(10 * $sizeof(i32,),);
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
%function :=
  '@fn' %identifier '\(' %parameter* '\)' '->' %type %block ';';
%parameter := %identifier ':' %type ',';
%block := '\{' %body '\}';
%body := %statement*;
%statement := (%call |) ';';

%call := %identifier '\(' %argument* '\)';
%argument := %expression ',';
%expression := (%identifier | %literal |);

%literal := (%string|);
%string := '`[^`]*`';
%type := %identifier %typeModifier*;
%typemodifier := ('ptr' | 'const' |);
%identifier := '[a-zA-Z_][a-zA-Z0-9_]*';
)";

} // namespace pint

extern "C" {

PuleParser puleParserCreateForDefaultEngine() {
  PuleError error = puleError();
  PuleParser parser = (
    puleParserCreateFromString(
      puleCStr(pint::expressionGrammar.c_str()),
      &error
    )
  );
  if (puleErrorConsume(&error)) { return {}; }
  return parser;
}

}
