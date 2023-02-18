# Pulchritude

<p align="center"> <img src="documents/pulchritude-logo.png"> </p>
<p align="center"> <img src="documents/pulchritude-logo.gif"> </p>

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

Some plugins in some cases are harder to replace than others and might require
  changing underlying assets. For example, the default scripting in Pulchritude
  is Lua, and to change that mid-project would obviously require porting a lot
  of Lua code. As well, shading languages might need to be ported if switching
  the underlying graphics API. The same might apply to image assets, font
  assets, etc. Some assets have custom backend implementations, like the data
  serializer and 3D model representation, which should make writing these plugin
  replacements a bit simpler.

---

### requirements

  - CMake
  - GCC or Clang
  - works only on Linux currently
  - ZLIB

### current considerations

 - Right now bindings are done by converting C header to JSON script and then
   converting JSON script to desired language. However it might be better to
   write the headers in JSON script to begin with, and then automatically
   translate them to C headers; the main benefit to this is that bindings can
   have additional information attached that wouldn't be present in the C
   header; such as field initializers. As well new libraries would be forced to
   be coherent to a standard that can be portable among all languages.

 - headers should have `package` such as `pulchritude-gfx/package.h`

 - do not use any C++ STL in the library; basically don't want to impose the
   STL onto applications. This isn't really a priority because there are
   libraries that depend on the C++ STL already anyway

### usage

The directory layout for the engine is:

  - engine/
    - application/source.cpp ; application & entry point
  - library/ ; static libraries to provide minimal engine support
    - include ; directory for all include files for the ABI (add this
                to include path for your builder)
      - pulchritude-plugin/plugin.h, pulchritude-log/log.h, etc
    - log/ ; each plugin
      - include/ ; private includes
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

void pulcComponentLoad(PulePluginPayload const payload) {
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

Pulchritude can provide interfaces to many languages, but the ones targetted are
primarily those that have the best FFI interop.

Current candidates are, in order of priority

     - C/C++, obviously, but they also share headers
     - zig
     - lua
     - V
     - C#
     - rust
     - D

    effectively, making most applications look like

                     .-----------.
    .----------.     | 3rd party |    .-------------.     .-------.
    |  ENGINE  |     | LIBRARIES |    | APPLICATION |     | LOGIC |
    '----------'     '-----------'    '-------------'     '-------'
     C, C++,         C, C++            C, C++, zig,        lua, D,
     Zig, Rust                         C#, rust, D,        V
                                       lua, V

  | C header | -> | JSON | -> | language bindings | -> | first class support |

From C header, translate to a JSON file. The JSON file contains all metadata
necessary to generate bindings for any language that supports C-ABI function
calls. Each language needs an appropiate script written perform the
transformation, but once that's done then the language can be used with the
engine. Some things might require some extra work, such as error handling; for
example a language might have errors that can be combined along with the return
value, and thus
    `PuleObj puleCreate(char const * const label, PuleError * const error)`
would become something like
    `PuleObj~(PuleErrorObjCreate) puleCreate(char const * const label)

A language is considered under first class support if it has an additional
layer that facilitates the bindings to be comfortable with the language's
ecosystem. This would most likely need to be some set of manually written
functions, but care needs to be taken such the engine & libraries can be updated
without breaking these functions. These should be limited in scope and contain
only auxillary functions, preferrably that can be paired with many functions

Some examples of what this would look like are;

ex 1:
    logging
-> C
  void puleLog(char const * const format, ...)
-> JSON
    # this json layout is only examplatory
    {
        "type": "function",
        "return-type": "void",
        "parameters": [
            { "type": {"const", "*", "const"}, "label": "format"},
            { "type": "variadic", "label": "..."}
        ]
    }
-> Zig binding
    pub extern fn puleLog(format: [*c] const u8, ...) extern(.C);
-> Zig first class support
    pub const log = struct {
        pub fn info(format: [:0] const u8, formatter:anytype) {
            // apply formatting to a buffer...
            puleLog("%s", formattedBuffer);
        }
        pub fn error(format: [:0] const u8, formatter:anytype) {
            // apply formatting to a buffer...
            puleLog("%s", formattedBuffer);
        }
        pub fn debug(format: [:0] const u8, formatter:anytype) {
            // apply formatting to a buffer...
            puleLog("%s", formattedBuffer);
        }
    }
    // ei pul.log.info("numobjects {} | name {s}", .{numObjs, name});
    //    matches 1:1 to zig's std.log.info

ex 2:
    allocator

 provide example how to pass a zig allocator to a pul allocator

### external libraries

external libraries need to be easy to find, easy to integrate and follow same
  consistency as engine libraries

list of things that makes a library a good candidate to be part of core
  engine and not an external library:

  - will be used by many applications (or libraries);
          examples: logging, plugin management
  - the libraries' dependencies are minimal, not bloated; in other
      words it wouldn't disproportionally impact the size of the
      repository. This excludes dependencies on other engine libraries.
          examples: GLFW w.r.t windowing, OpenGL w.r.t graphics
  - allows new users to write 'hello world' type applications; it's
      a detrimental first time experience to require managing external
      libraries for a user's first test application, while remaining or
      increasing in useful for all future uses.
          examples: imgui, resource loading
  - covers a broad spectrum of usecases in a simple ABI, such that it can
      be easily extended for
          examples: math, networking

core libraries should check ALL of these requirements and there needs to be a
strong argument for their inclusion. We want to avoid bloating the engine but
at the same time allow it to be flexible for most 'example'/test projects in a
broad range of usecases

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

The 'value' label of an enum can not start with an integer, for example,
`PuleGfxImageTarget_2D` is not valid. This is so that binding generators can do
`PuleGfxImageTarget.2D` for languages that support typed enums, and most
languages cannot have the first character of a variable label be numeric. Instead in these cases use `i2D`

```
typedef enum {
  PulcArrayDimension_i1D,
  PulcArrayDimension_i2D,
  PulcArrayDimension_i3D,
} PulcArrayDimension;
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

### rendering

Right now the renderer focuses on a 2D renderer, but it should be performant
  enough to use on 3D renderers as well. Currently it targets OpenGL3.3, with
  as 'modern' as a feature-set as that allows, but the API is framed in a way
  that it can be used in other graphics APIs like Vulkan, Metal, DX12, and of
  course can be upgraded to GL4.6.

Of course not everyone using pulchritude engine is expected to be a graphics
  programmer, and so the 'lower level' approach pulchritude takes to rendering
  can seem a bit steep at first. If you are writing a 3D renderer, this is just
  part of the experience for now, however, for 2D renderers, there is a higher
  level 2D renderer library that will do a lot of 2D work on your behalf.

To do most work consists of two phases. Either creating/uploading data to be
  used, or submitting commands to the GPU. The command submission is not
  immediate-mode like OpenGL is. Instead commands are recorded and then can be
  submitted to the GPU to be processed. This allows multi-threaded work to be a
  bit easier, as well caters to more modern graphics apis. This also has the
  benefit that users have to think of an appropiate workflow to their
  rendering, which should hopefully push to have larger draw calls that are
  batched together.

OpenGL 'uniforms' for all intents don't exist, instead consider push constants
  or uniform buffers. Push constants are great if the uniform data varies
  between each draw call, and not per-frame, and uniform buffers are great for
  when uniform data varies per-frame, and not per draw call. Try to work your
  renderer to not rely on requiring variation between each draw-call and
  per-frame, but if not possible then you will simply have to rerecord command
  buffers on-the-fly. The overhead for this might seem silly, for OpenGL, but
  the overhead is completely negligible compared to submitting too many draw
  calls.

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


##### long term plans


  - allocations:
    - before 0.1, allocations can be made without a PuleAllocator
    - before 1.0, allocations can be made without a PuleAllocator if the
        corresponding API function accepts a PuleAllocator
    - by version 1.0, every allocation must be backed by a PuleAllocator
  - hints:
    - before 0.1, ABI signatures don't need hints
    - 0.1 and on, ABI signatures must have the correct hints
  - documentation:
    - before 0.1, no documentation is necessary
    - before 1.0, documentation on ABI functions, and a simple setup guide
    - by version 1.0, must have tutorials, setup guides, and documentation on
        examples
  - community
    - 0.0 to 1.0, I'd like to get some people at some point to help aid in
        development of the engine
    - by version 1.0, I'd want some users
  - rendering
    - before 0.1, only a 2D renderer is fine
    - before 1.0, need to expand the 3D renderer
    - possibly also at some point Vulkan support
    - possibly at some point OpenGL3.2 support
  - errors
    - before 0.1, all errors can just be logged out rather than set Error
    - by 0.1, all errors must either assert (internal failure) or set Error
  - platforms
    - before 0.1, only Linux builds are fine
    - before 1.0, Linux and Windows builds
  - build system & language support
    - before 0.1, polish C/C++/cmake/xmake, zig and a scripting language
    - before 1.0, support some set of core languages
    - by version 1.0, support wide array of languages, with good polished
        bindings for core languages
    - at some point we also want to target VSCode and other IDEs
  - third party modules
    - before/by version 1.0, we want some set of third party modules that can
        integrate easily into new projects
  - binding generator/parser
    - before 0.1, using hacked up python scripts to parse & generate bindings
    - before or by 1.0, want more polished scripts or alternative
