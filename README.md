# Summary

This is a game engine that ties together a lot of libraries and exposes them to
the user through a unified C ABI. The engine is written in C++ with an exposing
C ABI to allow as many libraries to be imported as possible. The application
software for the game can be written in any desired language; the bulk of the
game exists as a collection of dynamic libraries that can each expose more
functions and rely on each other, allowing for multiple languages to be used in
a simple and sane manner. The engine can be extended easily by adding new
dynamic libraries that expose a C ABI.

What this might end up looking like is:

                       ┌────────────┐
               ┌───────│Engine (C++)│◄────────────────────┐
               │       │Application │                     │
               │       └────────────┘                     │
               │         │                                │
               │         │                                │
               │         │     ┌───────────────────────┐  │
               │         │     │Libraries (C, C++, etc)│  │
               │         ▼     └─────────────┬─────────┘  │
               │   ┌──────────┐              ▼            │
               │   │Core Game │        ┌──────────────┐   │
               │   │Foundation│◄───────│Exposing C ABI│───┘
               │   │(Zig, C)  │◄┐      └──────────────┘
               │   └──────────┘ │            │
               │         │   └──┘            │
               │         ▼                   │
               │  ┌────────────────┐         │
               │  │Gameplay Logic  │◄────────┘
               └► │  (Nim, D, Haxe)│◄┐
                  └────────────────┘ │
                                  └──┘

The engine would be the application entry point, and every other bit will exist
as either a static library, for math, networking, physics, gui, parsing,
logging, etc, or as a dynamic library, which would implement the application.
For clarity on wording, the static libraries will be known as library, and the
dynamic libraries as components. To clarify, most static libraries will be
called indirectly through dynamic libraries that expose a C ABI.

# Usage

The directory layout for the engine is:

  - engine/
    - application/source.cpp ; application & entry point
  - libraries/ ; static libraries to provide minimal engine support
    - plugin/include/bulkher-plugin/plugin.hpp
    - plugin/src/bulkher-plugin/plugin.cpp
    - plugin/CMakeLists.txt
    - ...
  - interop/ ;
    - plugin/include/bulkher-interop-plugin/plugin.h
    - plugin/src/bulkher-interop-plugin/plugin.c

A C++ CMakeLists is provided, which can be used to compile the engine. This is
all you need for a simple 'hello world'. The engine provides some minimal
libraries, which can be disabled if wanted;
  - logging

Note that if you disable an engine-level library, you will have to provide your
own definitions for the C ABI to ensure other components function correctly.

So as an example to a simple Zig project,

- project/
  - build/ ; location to put build files
  - install/ ; location to install binaries, libraries/plugins, symbols, etc
  - repo/ ; project source code
    - build.zig ; build files, like CMakeLists.txt or equivalent
    - bulkher-engine/ ; clone/fork of the engine
    - components/ ; plugins that form the primary logic/implementation
      - core/ ; primary implementation
        - src/core.zig
      - ai/ai.lua
      - bootstrap/main.c
      - ...
    - libraries/ ; optional, list of bulkher libraries to use
      - bulkher-math/
      - bulkher-assets/
      - ...
    - application/ ; optional, if you want to override the default application
      - source.zig ; entry point

# Hello World example

There is a script that can help you set a project up, located at the examples
repo https://github.com/aodq/bulkher-examples . The script can also be used
as reference.

As well I'll list a simple C++ & CMake example

directories
- example-project
  - build
  - install
  - repo
    - CMakeLists.txt
    - components/core
      - src/core.cpp
      - CMakeLists.txt


example-project/repo/CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
cmake_policy(VERSION 3.0)
project(
  example-project
  VERSION 0.0
  LANGUAGES CXX
  DESCRIPTION ""
)

add_subdirectory(bulkher-engine)
add_subdirectory(components/core)
```

example-project/repo/core/CMakeLists.txt
```cmake
add_library(example-project-core SHARED)
target_include_directories(example-project-core PUBLIC "include/")
target_sources(example-project-core PRIVATE src/core.cpp)
set_target_properties(example-project-core POSITION_INDEPENDENT_CODE ON)
target_link_libraries(example-project-core bulkher-plugin bulkher-log)

install(
  TARGETS example-project-core
  LIBRARY NAMELINK_SKIP
  LIBRARY
    DESTINATION bin/plugins/
    COMPONENT plugin
  RUNTIME
    DESTINATION bin/plugins/
    COMPONENT plugin
)
```

example-project/repo/core/core.cpp
```cpp
#include <bulkher-log/log.h>

extern "C" {

PulePluginType pulcPluginType() {
  return PulePluginType_component;
}

void pulcComponentLoad() {
  puleLog("hello from test application");
}

} // extern C
```

# dynamic binding

As many interactions between the engine, libraries and components is through
indirect pointers loaded by functions like linux's `dlopen`, part of writing
the bindings is to provide an interface through a struct, namespace, or
equivalent. There are some exceptions, like some `pule` functions are provided
statically, mostly to handle dynamic library loading.

```
// C++
namespace pulc {
  void (*players)();
};

// loaded by
pulc::players = pulePluginLoadFn("pulcPlayers");
```

this will require some level of maintenance between each language.

I might consider having these structs be part of the C ABI

```
// C ABI
struct PulcBindings {
  void (*initializeBindings)();
  void (*players)();
};

// .. in some other language
PulcBindings pulc;
pulc.initializeBindings = pulePluginLoadFn("pulcInitializeBindings");
pulcDynamicBindingsLoad(&pulc);
```

The thing to note is that most likely *you don't* need to do this for most of
the libraries you are linking against. This is recommended to be done when
interfacing from the foundation of your application, to the internal logic
parts; the functions that you want to be hot-reloaded. Otherwise there is
no need to manually load & reload these indirect function pointers.

# Error-handling

Error handling from C-ABIs is a bit of a mess. Preferrably we'd want to have
the error returned along with the value, and some sort of pattern matching to
address the error. This is tedious for us as each function would need its own
custom return type, so some sort of middle ground is provided to you. Possibly
the bindings to your language can help here as well.

```
enum PulcInitializeError {
  PulcInitializeError_acquiredNoPlayers = 1,
};
PulError pulcInitialize(PulcGameState * const state) {
  // silly example
  state->players = pulcAcquirePlayers();
  if (state->players == 0) {
    return (
      puleRegisterError(
        PulcInitializeError_acquiredNoPlayers,
        "failed to acquire players"
      )
    );
  }
  return state;
}
```

to consume an error is simple
```
PulError error = pulcInitialize();
if (puleConsumeError(error)) { // checks & logs error
  return;
}
```

# Style

I don't think code-style is much important to follow, I usually only tend to
care to emphasize verticallity in code rather than long unbreaking horizontal
lines. However, since all components need to interact through a C ABI, they
all need to follow the same conventions. This extends past style, to things
like error handling, parameter typing, memory allocation, and more.

Firstly, functions are camelCased and must start with either 'puli' if they
are from a lIbrary, 'pule' if from the Engine, and 'pulc' if from a Component.
The component part is optional, as if these functions only serve a single
application, then there's no need to follow the engine naming convention.
However the convention should be followed when functions the component exposes
is called 'upstream' to the engine or libraries, such as 'pulcComponentLoad' or
'pulcComponentUnload'.

**No non-exported/local functions/variables should start with `pul` prefix**.

```
void pulePrintHelloWorld();
```

Structs are PascalCased, and they follow the same `Pulc`/`Puli`/`Pulc`
semantics. Structs are heavily encouraged to be used to group function
parameters together.

```
struct PuleApplicationInfo {
  char const * label;
  uint32_t majorVersion, minorVersion, hotfixVersion = 0;
};
void puleRegisterApplication(PuleApplicationInfo const);
```

Each parameter must be constant. If you need to modify a parameter, copy it
locally in the function. Every type must be explicitly sized. If the size of
the parameter is larger than a POD, consider using a const-pointer. Pointers
and const are both type-modifiers, thus they are read from right-to-left (`int
const` rather than `const int`). Pointers must be spaced out the same as any
other named type modifier (`int * foo`).

```
int32_t puleVersion();
void puleIncrementValue(int * const value);

struct PulcGameState {
  // ... lots of variables
};
void pulcGameStateLog(PulcGameState const * const state);
```

There is no 'get' or 'set', instead it's preferred to simply return a value, a
pointer-to-a-const, or a pointer-to-a-non-const. There may be exceptions to
this, like if you need additional functionality besides just get/set (like
ref-counts or something).

```
struct PulcEntityPlayer {
  ...
};
PulcEntityPlayer * pulcPlayerFromId(int32_t const id);
```

If a function can error, then only the error may be returned. Any mutable ref
values must be the first set of parameters, and variables marked as 'out'
(value written) or `inout` (value read and written).

```
PuleError pulcPlayerFromId(PulcEntityPlayer * const playerOut);
```

Consts follow the same `pule/a/c` convention. Macros start with `PULE/A/C_`, but
the rest is camel-case. Provide both a constant and macro if reasonable.

```
#define PULE_maxEnumU32 0x7FFFFFFF
const int32_t puleMaxEnumU32 = PULE_maxEnumU32;
```

Enums introduce a `_` between the enum type and the enum value. They must have
a `Begin` value if(f) non-zero, and must have an `End` value. They as well
must have a `MaxEnum` that will specify the size of the enum.
```
enum PulcTeamColor {
  PulcTeamColor_red,
  PulcTeamColor_green,
  PulcTeamColorEnd,
  PulcTeamColorMaxEnum = puleMaxEnumU32,
};
```

Parameters and struct members that are arrays end with `s`/`es` suffix.
```
struct PulcTeam {
  int32_t * playerIds;
};
void pulcTeamLog(PulcTeam const * const teams);
```

*It's always* preferrable to pass arrays of items rather than a single item
when interacting with arrays and dynamic libraries, as is already the
convention for data-oriented approaches to programming.

```
for (size_t it = 0; it < pulcPlayersLength(); ++ it) {
  PulcPlayer * player = pulcPlayer(it);
}
```

is less performant than

```
size_t playerLen = pulcPlayersLength();
PulcPlayer * players = pulcPlayers();
```

Any passed function (such as a callback) should have a userdata parameter, to
allow user's to pass their own data to the callback. This is done only to make
the user's life a little easier, if they choose to use it.

```
void puleSetErrorCallback(
  void (*fn)(PuleError const error, void * const userdata)
);
```

# Positives / Negatives

Every component could theoretically be hot-reloadable, though most likely the
'gameplay logic' will be hot-reloaded to allow fast prototyping development.
This allows for fast game development.

While you could also add a scripting engine to this workflow, what we have here
might be preferrable as exposing a C ABI is simple, there is improved
compile-time safety and error-handling, multiple languages can be used, and of
course, performance will be a bit faster. That said, there could still be some
uses for adding a scripting engine, and one would only have to expose the
existing C ABI to that language to make use of it.

Another core benefit of splitting it up like this is to hopefully improve
compile times, as the prototyping hot-reload compilations should be blazingly
fast without having to rely on entire headers introduced by engines and
libraries, which in the case of C++ can bloat otherwise simple object files.

Additionally, by having everything interface to a C ABI, every component can
interopate at the same level, allowing for multiple modules to interact and
depend on each other.

A major downside to this approach is that bindings will potentially have to
be rewritten for each language used, depending on the quality of it's FFI. Some
languages can import C headers without requiring to manually write bindings,
and many tools exist to generate bindings, so hopefully this won't be an issue
in general workflow.

One other downside to this approach is that each language used might come with
its own runtime overhead, so the languages used should be minimized, and if
possibly stripping out unnecessary runtimes that could bloat the resulting
binaries.

