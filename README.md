# Pulchritude

### summary
Pulchritude is a [lingua franca](https://en.wikipedia.org/wiki/Lingua_franca)
type of game engine that ties together a lot of libraries and exposes them to
the user through a unified C ABI. The engine is written in C++ with an exposing
C ABI to allow as many libraries to be imported as possible. The application
software for the game can be written in any desired language; the bulk of an
application exists as a collection of dynamic libraries that can each expose more
functions and rely on each other, allowing for multiple languages to be used in
a simple and sane manner. The engine can be extended easily by adding new
dynamic libraries that expose a C ABI.

What this might end up looking like is:

```txt
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
               │   │Foundation│◄───────│Pulchritude's │───┘
               │   │(Zig, C)  │◄┐      │Exposing C ABI│
               │   └──────────┘ │      └──────────────┘
               │         │   └──┘            │
               │         ▼                   │
               │  ┌────────────────┐         │
               │  │Gameplay Logic  │◄────────┘
               └► │  (lua, D, Haxe)│◄┐
                  └────────────────┘ │
                                  └──┘
```

The engine would be the application entry point, and every other bit will exist
as either a static library, for math, networking, physics, gui, parsing,
logging, etc, or as a dynamic library, which would implement the application.
For clarity on wording, the static libraries will be known as library, and the
dynamic libraries as components. To clarify, most static libraries will be
called indirectly through dynamic libraries that expose a C ABI.

Every plugin should be replaceable with others that expose the same C ABI, it's
even a simple manner to replace the engine plugins; each plugin has a
corresponding CMake variable that will disable the build+install of that
plugin. This allows for even the most central part of the engine, such as the
logger or default memory allocator, to be replaced. In other words, the only
irreplacable part of Pulchritude is the cohesive interfaces between plugins.

---

### current considerations

 - Right now bindings are done by converting C header to JSON script and then
   converting JSON script to desired language. However it might be better to
   write the headers in JSON script to begin with, and then automatically
   translate them to C headers; the main benefit to this is that bindings can
   have additional information attached that wouldn't be present in the C
   header; such as field initializers. As well new libraries would be forced to
   be coherent to a standard that can be portable among all languages.

### usage

The directory layout for the engine is:

  - engine/
    - application/source.cpp ; application & entry point
  - library/ ; static libraries to provide minimal engine support
    - include ; directory for all include files for the ABI (add this
                to include path for your builder)
      - pulchritude-plugin/plugin.h, pulchritude-log/log.h, etc
    - log/ ; each plugin
      - CMakeLists.txt
      - src/plugin.c
  - third-party/ ; optional, location of external libraries
    - plugin/src/pulchritude-interop-plugin/plugin.c

When compiled, the application will install to bin/, and each each plugin will
install to bin/plugins

A C++ CMakeLists is provided, which can be used to compile the engine. This is
all you need for a simple 'hello world'. The engine provides some minimal
libraries, which can be disabled if wanted (you'd have to provide your own
replacement);

  - allocator
  - asset-font
  - asset-image
  - gfx
  - json
  - logging
  - math
  - plugin
  - string
  - window

Note that if you disable an engine-level library, you will have to provide your
own definitions for the C ABI to ensure other components function correctly.

So as an example to a simple Zig project,

- project/
  - build/ ; location to put build files
  - install/ ; location to install binaries, libraries/plugins, symbols, etc
  - repo/ ; project source code
    - build.zig ; build files, like CMakeLists.txt or equivalent
    - pulchritude-engine/ ; clone/fork of the engine
    - components/ ; plugins that form the primary logic/implementation
      - core/ ; primary implementation
        - src/core.zig
      - ai/ai.lua
      - bootstrap/main.c
      - ...
    - libraries/ ; optional, list of pulchritude libraries to use
      - pulchritude-math/
      - pulchritude-assets/
      - ...
    - third-party/ ; optional, location of external libraries
    - application/ ; optional, if you want to override the default application
      - source.zig ; entry point

---

#### hello world example

There is a script that can help you set a project up, located at the examples
repo https://github.com/aodq/pulchritude-examples . The script can also be used
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

----

- example-project/repo/CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
cmake_policy(VERSION 3.0)
project(
  example-project
  VERSION 0.0
  LANGUAGES CXX
  DESCRIPTION ""
)

add_subdirectory(pulchritude-engine)
add_subdirectory(components/core)
```

- example-project/repo/core/CMakeLists.txt
```cmake
add_library(example-project-core SHARED)
target_include_directories(example-project-core PUBLIC "include/")
target_sources(example-project-core PRIVATE src/core.cpp)
set_target_properties(example-project-core POSITION_INDEPENDENT_CODE ON)
target_link_libraries(example-project-core pulchritude-plugin pulchritude-log)

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

- example-project/repo/core/core.cpp
```cpp
#include <pulchritude-log/log.h>

extern "C" {

PulePluginType pulcPluginType() {
  return PulePluginType_component;
}

void pulcComponentLoad() {
  puleLog("hello from test application");
}

} // extern C
```

---

### dynamic binding

As many interactions between the engine, libraries and components is through
indirect pointers loaded by functions like linux's `dlopen`, part of writing
the bindings is to provide an interface through a struct, namespace, or
equivalent. There are some exceptions, like some `pule` functions are provided
statically, mostly to handle dynamic library loading.

```cpp
// C++
namespace pulc {
  void (* const players)();
};

// loaded by
pulc::players = pulePluginLoadFn("pulcPlayers");
```

this will require some level of maintenance between each language.

I might consider having these structs be part of the C ABI

```c
// C ABI
typedef struct {
  void (* const initializeBindings)();
  void (* const players)();
} PulcBindings;

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

---

### writing bindings

Writing bindings for scripts and compiled languages will most likely take
similar approaches. At some point there will be a script that will take the C
headers and translate them to JSON, and with that file you can automatically
generate bindings to your preferred language. This will require a conversion
script for each scripting/programming language. Of course some languages can
already generate bindings from C headers so this might be an unnecessary step.

Another approach is to writing the bindings manually; the maintenance for this
might not be worthwhile compared to just writing a JSON generator. Another issue
is that calling C functions directly from your language might not fit into that
language's workflow. Thus there ought to be a hand-crafted layer on top of the
bindings that transforms the functions to something more comfortable, for
example:

```c
PuleString puleString(
  PuleAllocator const, char const * const, PuleError * const error
);
// allocator is simply puleAllocatorDefault
PuleString puleStringDefault(char const * const, PuleError * const);
```

in a different, pseudo - language might be handled as
```
Errorable(PuleString) puleString(
  char const * const format, PuleAllocator alloc = default
) {
  PuleError e;
  str = puleString(alloc, format, &e);
  if (e != PuleError_none) {
    return e;
  }
  return str;
}
```

### error handling

Error handling from C-ABIs is a bit of a mess. Preferrably we'd want to have
the error returned along with the value, and some sort of pattern matching to
address the error. Given the C-ABI, instead the error will be apart of the
parameters. The idea is that the bindings to your language will help here as
well.

```c
typedef enum {
  PulcInitializeError_acquiredNoPlayers = 1,
} PulcInitializeError;
PulcGameState pulcInitialize() {
  // silly example
  state->players = pulcAcquirePlayers();
  if (state->players == 0) {
    pulePushError(
      PulcInitializeError_acquiredNoPlayers,
      puleString("failed to acquire players")
    );
    return PulcGameState {};
  }
  return state;
}
```

to consume an error is simple
```c
// maybe(?) best to put it in thread local storage, if language supports this,
static thread_local PuleError error;

PulcGameState gameState = pulcInitialize(&error);
switch (puleErrorConsume(&error)) { // checks & logs error
  default: break;
  case PulcInitializeError_acquiredNoPlayers:
    exit(-1);
}
```

---

### style

I don't think code-style is much important to follow, I usually only tend to
care to emphasize verticallity in code rather than long unbreaking horizontal
lines. However, since all components need to interact through a C ABI, they
all need to follow the same conventions. This extends past style, to things
like error handling, parameter typing, memory allocation, and more.

This is extremely important in the headers so that simple automated binding
generators can be build, including `scripts/json-binding-generator.py` which
generates json bindings for this script.

Outside of the header/exported C-ABI symbols, style isn't relevant.

Firstly, functions are camelCased and must start with either 'puli' if they
are from a lIbrary, 'pule' if from the Engine, and 'pulc' if from a Component.
The component part is optional, as if these functions only serve a single
application, then there's no need to follow the engine naming convention.
However the convention should be followed when functions the component exposes
is called 'upstream' to the engine or libraries, such as 'pulcComponentLoad',
'pulcComponentUnload', 'pulcPluginType', and more.

**No non-exported/local functions/variables should start with `pul` prefix**.

```c
PULE_exportFn void pulePrintHelloWorld();
```

As shown above, every exported function must have `PULE_exportFn`, as while
there needs to be a specific signature on Windows builds, this also helps
bindings know when to start parsing a function.

When naming things, try to categorize things linearly, similar to namespacing.
The names should be more specific as they are read left-right. For example, the
source of the symbol (application/engine/library :: pulc/pule/puli), the
associated library/namespace, and so on:

```c
PULE_exportFn void pulcPlayerCreate();
PULE_exportFn void pulcPlayerInventory();
PULE_exportFn void pulcNetPlayerSync();
```

Structs are PascalCased, and they follow the same `Pulc`/`Puli`/`Pulc`
semantics. Structs are heavily encouraged to be used to group function
parameters together.

```c
typedef struct {
  char const * label;
  uint32_t majorVersion;
  uint32_t minorVersion;
  uint32_t hotfixVersion;
} PuleApplicationInfo;
PULE_exportFn void puleRegisterApplication(PuleApplicationInfo const);
```

each member of a struct must be seperate, that is, `int32_t x, y, z` is not
allowed. This just makes parsing the syntax in scripts simpler. As well, if
you intend to have some sort of default value, then it would be useful for this
to be put in generated bindings for languages that support field initializers.
This is done with empty macro `puleFieldInitialize` which can give a useful
hint.

```c
typedef struct {
  uint32_t defaultWeapon puleFieldInitialize(0);
} PulcPlayer;
```

Each parameter must be constant. If you need to modify a parameter, copy it
locally in the function. Every type must be explicitly sized. If the size of
the parameter is larger than a POD, consider using a const-pointer. Pointers
and const are both type-modifiers, thus they are read from right-to-left (`int
const` rather than `const int`). Pointers must be spaced out the same as any
other named type modifier (`int * foo`). Every parameter must have a label,
even if redundant, to ease binding creation

```c
int32_t puleVersion();
PULE_exportFn void puleIncrementValue(int * const value);

typedef struct {
  // ... lots of variables
} PulcGameState;
PULE_exportFn void pulcGameStateLog(PulcGameState const * const state);
```

There is no 'get' or 'set', instead it's preferred to simply return a value, a
pointer-to-a-const, or a pointer-to-a-non-const. There may be exceptions to
this, like if you need additional functionality besides just get/set (like
ref-counts or something).

```c
typedef struct {
  ...
} PulcEntityPlayer;
PULE_exportFn PulcEntityPlayer * pulcPlayerFromId(int32_t const id);
```

Consts follow the same `pule/a/c` convention. Macros start with `PULE/A/C_`, but
the rest is camel-case. Provide both a constant and macro if reasonable.

```c
#define PULE_maxEnumU32 0x7FFFFFFF
const int32_t puleMaxEnumU32 = PULE_maxEnumU32;
```

Enums introduce a `_` between the enum type and the enum value. They must have
a `_Begin` value if(f) non-zero, and must have an `_End` value. They as well
must have a `_MaxEnum` that will specify the size of the enum.
```c
typedef enum {
  PulcTeamColor_red,
  PulcTeamColor_green,
  PulcTeamColor_End,
  PulcTeamColor_MaxEnum = puleMaxEnumU32,
} PulcTeamColor;
```

Parameters and struct members that are arrays end with `s`/`es` suffix.
```c
typedef struct {
  int32_t * playerIds;
} PulcTeam;
PULE_exportFn void pulcTeamLog(PulcTeam const * const teams);
```

*It's always* preferrable to pass arrays of items rather than a single item
when interacting with arrays and dynamic libraries, as is already the
convention for data-oriented approaches to programming.

```c
for (size_t it = 0; it < pulcPlayersLength(); ++ it) {
  PulcPlayer * player = pulcPlayer(it);
}
```

is less performant than

```c
size_t playerLen = pulcPlayersLength();
PulcPlayer * players = pulcPlayers();
```

Any passed function (such as a callback) should have a userdata parameter, to
allow user's to pass their own data to the callback. This is done only to make
the user's life a little easier, if they choose to use it.

```c
void (*n)(PuliIntersectionInfo, void * const userdata)
PULE_exportFn puliIntersectionCallback();
```

If a parameter is nullable (it can either point to a valid value, or be null),
  the name should end with 'Nullable'.

```c
PULE_exportFn void pulcSetUserData(void * const userDataNullable);
```

---

### contribution

There are a few methods to contribute;

#####  - finding and/or fixing bugs

I recommend to create new issues for every bug you encounter, especially
crashes. If an issue for an existing bug or crash already exists, feel free to
bump it, post relevant information, and of course fix it.

#####  - writing documentation

This can be done as a PR

#####  - suggesting and/or adding functionality to the engine

Even if you don't plan to implement the feature yourself, an issue here will be
appreciated. If an issue already exists, feel free to bump, post relevant
information, and of course submit a PR.

#####  - writing your own binding front-ends / writing replacements to existing libraries

Although this can be done it's own repository, and would not change the engine
at a fundamental level, I would recommend creating an issue to document its'
existence. 


#####  - creating new libraries, or extending existing libraries
If you plan to write a new library, or extend the functionality of an existing
library (in a way that modifies the exposing C ABI), I humbly request you
create an issue and propose the modifications to be made to the exposing C ABI.
We should strive to unify the ABI to be consistenty pulchritude.

  ---


---
###### *when tomorrow comes you'll wish you had today*
