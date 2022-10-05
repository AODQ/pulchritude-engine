#include <pulchritude-script/script.h>

#include <pulchritude-time/time.h>

#include <pulchritude-log/log.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static int32_t getLuaLineNumber(lua_State * state) {
  lua_Debug debug;
  if (lua_getstack(state, 1, &debug) == 0) {
    return -1;
  }
  lua_getinfo(state, "n", &debug);
  return debug.currentline;
}

namespace { bool luaCheckError(
  lua_State * const state,
  int32_t const luaErrorCode,
  PuleError * const error,
  char const * const description
) {
  if (luaErrorCode == 0) {
    return false;
  }
  // fetch lua error as string
  char const * const errorstr = lua_tostring(state, -1);

  // transform error to something we can print out nicely
  PuleErrorScript errorPule;
  char const * errorstrPule = nullptr;
  switch (luaErrorCode) {
    case LUA_ERRSYNTAX:
      errorPule = PuleErrorScript_syntaxError;
      errorstrPule = "syntax";
    break;
    case LUA_ERRMEM:
      errorPule = PuleErrorScript_memoryError;
      errorstrPule = "memory";
    break;
    case LUA_ERRRUN:
      errorPule = PuleErrorScript_executeRuntimeError;
      errorstrPule = "runtime";
    break;
    case LUA_ERRFILE: case LUA_ERRERR:
      errorPule = PuleErrorScript_internalError;
      errorstrPule = "internal";
    break;
    default:
      errorPule = PuleErrorScript_internalError;
      errorstrPule = "internal";
    break;
  }

  PULE_error(
    errorPule,
    "lua %s error [%d]: '%s': line %d (%s)",
    errorstrPule, luaErrorCode, errorstr, getLuaLineNumber(state), description
  );
  return true;
}}

#include "setup.h"

// -- script context -----------------------------------------------------------
extern "C" {

PuleScriptContext puleScriptContextCreate() {
  lua_State * state = luaL_newstate();

  luaopen_base(state);
  luaopen_debug(state);
  luaopen_io(state);
  luaopen_math(state);
  luaopen_package(state);
  luaopen_string(state);
  luaopen_table(state);
  // no open for OS

  PuleScriptContext const ctx = {.id = reinterpret_cast<uint64_t>(state),};

  puleScriptContextSetup(ctx);
  return ctx;
}

void puleScriptContextDestroy(PuleScriptContext const context) {
  if (context.id == 0) return;
  lua_close(reinterpret_cast<lua_State *>(context.id));
}

} // C

// -- script module ------------------------------------------------------------
#include <string>
#include <unordered_map>
#include <vector>

namespace { struct LuaBinaryExtractor {
  std::vector<uint8_t> buffer;
};}

namespace { int32_t luaBinaryExtract(
  [[maybe_unused]] lua_State * state,
  void const * bufferIn,
  size_t bufferLen,
  void * userdata
) {
  LuaBinaryExtractor & extractor = *(LuaBinaryExtractor *)(userdata);
  for (size_t it = 0; it < bufferLen; ++ it) {
    extractor.buffer.emplace_back(
      reinterpret_cast<uint8_t const *>(bufferIn)[it]
    );
  }
  return 0;
}}

namespace { struct CompiledLuaModule {
  std::vector<uint8_t> buffer;
  std::string label;
  bool debug;
  bool hasExecutedYet;
};}

namespace {
  std::unordered_map<size_t, CompiledLuaModule> compiledModules;
  size_t compiledModulesHashIt = 1;
}

namespace { std::vector<uint8_t> compileLuaFromSource(
  lua_State * const state,
  PuleStringView const source,
  char const * const label,
  bool const debug,
  PuleError * const error
) {
  // in debug mode, don't compile so that source remains
  if (debug) {
    std::vector<uint8_t> data;
    data.resize(source.len);
    memcpy(data.data(), source.contents, source.len);
    return data;
  }

  // compile given lua source
  if (
    luaCheckError(
      state,
      luaL_loadbuffer(state, source.contents, source.len, label),
      error,
      "compiling lua source"
    )
  ) {
    return {};
  }

  // read back compiled bytecode to a buffer
  LuaBinaryExtractor binaryExtractor;
  if (
    luaCheckError(
      state,
      lua_dump(
        state,
        &::luaBinaryExtract,
        reinterpret_cast<void *>(&binaryExtractor),
        debug ? 0 : 1 // whether to strip out debug symbols
      ),
      error,
      "compiling lua source"
    )
  ) {
    return {};
  }

  lua_pop(state, lua_gettop(state)); // pop the compiled function

  return binaryExtractor.buffer;
}}

extern "C" {

// ---
PuleScriptModule puleScriptModuleCreateFromSource(
  PuleScriptContext const context,
  PuleScriptModuleFromSourceCreateInfo const ci,
  PuleError * const error
) {
  lua_State * state = reinterpret_cast<lua_State *>(context.id);
  auto const scriptLabelStr = (
    std::string(ci.scriptLabel.contents, ci.scriptLabel.len)
  );
  bool const debug = PuleScriptDebugSymbols_enable ? true : false;

  std::vector<uint8_t> const buffer = (
    compileLuaFromSource(
      state,
      ci.scriptSource,
      scriptLabelStr.c_str(),
      debug,
      error
    )
  );

  if (buffer.size() == 0) {
    return {.id=0,};
  }

  // store the bytecode+label to reference in future executes/destroys
  ::compiledModules.emplace(
    ::compiledModulesHashIt,
    ::CompiledLuaModule {
      .buffer = std::move(buffer),
      .label = std::move(scriptLabelStr),
      .debug = debug,
      .hasExecutedYet = false,
    }
  );

  return {.id = ::compiledModulesHashIt++};
}

// ---
void puleScriptModuleUpdateFromSource(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule,
  PuleStringView scriptSource,
  PuleError * const error
) {
  lua_State * state = reinterpret_cast<lua_State *>(context.id);
  CompiledLuaModule & compiledModule = ::compiledModules.at(scriptModule.id);

  compiledModule.hasExecutedYet = false;
  std::vector<uint8_t> const buffer = (
    compileLuaFromSource(
      state,
      scriptSource,
      compiledModule.label.c_str(),
      compiledModule.debug,
      error
    )
  );
  if (buffer.size() == 0) {
    return;
  }

  compiledModule.buffer = std::move(buffer);
}

// ---
void puleScriptModuleDestroy(
  [[maybe_unused]] PuleScriptContext const context,
  PuleScriptModule const scriptModule
) {
  if (scriptModule.id == 0) return;
  ::compiledModules.erase(scriptModule.id);
}

void puleScriptModuleExecute(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule,
  PuleStringView const functionLabel,
  PuleError * const error
) {
  lua_State * state = reinterpret_cast<lua_State *>(context.id);
  if (
    scriptModule.id == 0
    || ::compiledModules.find(scriptModule.id) == ::compiledModules.end()
  ) {
    puleLogError(
      "failed to execute script %s%s%s, could not find module '%d'",
      (functionLabel.len == 0 ? "" : "@'"),
      functionLabel.contents,
      (functionLabel.len == 0 ? "" : "'"),
      scriptModule.id
    );
  }
  CompiledLuaModule & compiledModule = ::compiledModules.at(scriptModule.id);

  // load compiled lua
  if (
    luaCheckError(
      state,
      luaL_loadbuffer(
        state,
        reinterpret_cast<char const *>(compiledModule.buffer.data()),
        compiledModule.buffer.size(),
        compiledModule.label.c_str()
      ),
      error,
      "executing lua"
    )
  ) {
    return;
  }

  // call script if not executed yet or requested by empty functionLabel
  if (!compiledModule.hasExecutedYet || functionLabel.len == 0) {
    luaCheckError(state, lua_pcall(state, 0, 0, 0), error, "pcall script");
    lua_pop(state, lua_gettop(state));
  }

  // call function
  if (functionLabel.len != 0) {
    int type = lua_getglobal(state, functionLabel.contents);
    /* switch (type) { */
    /*   case LUA_TNUMBER: puleLogDebug("NUMBER"); break; */
    /*   case LUA_TSTRING: puleLogDebug("LUA_TSTRING"); break; */
    /*   case LUA_TBOOLEAN: puleLogDebug("LUA_TBOOLEAN"); break; */
    /*   case LUA_TNIL: puleLogDebug("LUA_TNIL"); break; */
    /*   case LUA_TTABLE: puleLogDebug("LUA_TTABLE"); break; */
    /*   case LUA_TFUNCTION: puleLogDebug("LUA_TFUNCTION"); break; */
    /*   default: puleLogDebug("UNKNOWN!!"); */
    /* } */
    luaCheckError(state, lua_pcall(state, 0, 0, 0), error, "pcall functino");
    lua_pop(state, lua_gettop(state)); // pop the compiled function
  }
}

} // C

// ---- global table
namespace {
std::unordered_map<size_t, void *> globalTable;
}

extern "C" {

void * puleScriptGlobal(PuleStringView const globalLabel) {
  auto ptr = globalTable.find(puleStringViewHash(globalLabel));
  if (ptr == globalTable.end()) {
    return nullptr;
  }
  return ptr->second;
}

void puleScriptGlobalSet(PuleStringView const globalLabel, void * const data) {
  if (globalLabel.len == 0 || globalLabel.contents == nullptr) {
    puleLogError("trying to set global with null globalLabel");
  }
  globalTable[puleStringViewHash(globalLabel)] = data;
}

} // C

// ---- arrays

namespace {
struct ArrayF32 {
  PuleAllocator allocator;
  float * data;
  size_t allocatedLength;
  size_t actualLength;
};
} // namespace

extern "C" {
PuleScriptArrayF32 puleScriptArrayF32Create(PuleAllocator const allocator) {
  auto const array = (
    reinterpret_cast<ArrayF32 *>(
      puleAllocate(
        allocator,
        PuleAllocateInfo {
          .zeroOut = true,
          .numBytes = sizeof(ArrayF32),
          .alignment = 0,
        }
      )
    )
  );
  array->allocator = allocator;
  array->data = (
    reinterpret_cast<float *>(
      puleAllocate(
        allocator,
        PuleAllocateInfo {
          .zeroOut = true,
          .numBytes = sizeof(float) * 4,
          .alignment = 0,
        }
      )
    )
  );
  array->allocatedLength = 4;
  array->actualLength = 0;

  return {.id = reinterpret_cast<uint64_t>(array),};
}
void puleScriptArrayF32Destroy(PuleScriptArrayF32 const arr) {
  auto const array = reinterpret_cast<ArrayF32 *>(arr.id);
  puleDeallocate(array->allocator, array->data);
  puleDeallocate(array->allocator, array);
}
void * puleScriptArrayF32Ptr(PuleScriptArrayF32 const arr) {
  auto const array = reinterpret_cast<ArrayF32 *>(arr.id);
  return array->data;
}
void puleScriptArrayF32Append(
  PuleScriptArrayF32 const arr,
  float const value
) {
  auto const array = reinterpret_cast<ArrayF32 *>(arr.id);
  if (array->allocatedLength <= (array->actualLength+1)) {
    array->allocatedLength = (array->actualLength+1)*2;
    array->data = (
      reinterpret_cast<float *>(
        puleReallocate(
          array->allocator,
          PuleReallocateInfo {
            .allocation = array->data,
            .numBytes = array->allocatedLength*sizeof(float),
            .alignment = 0,
          }
        )
      )
    );
  }
  array->data[array->actualLength] = value;
  array->actualLength += 1;
}
void puleScriptArrayF32Remove(
  PuleScriptArrayF32 const arr,
  size_t const index
) {
  puleLogDebug("MEH,'puleScriptArrayF32Remove' not implemented yet"); 
}
float puleScriptArrayF32At(PuleScriptArrayF32 const arr, size_t const index) {
  auto const array = reinterpret_cast<ArrayF32 *>(arr.id);
  return array->data[index];
}
size_t puleScriptArrayF32Length(PuleScriptArrayF32 const arr) {
  auto const array = reinterpret_cast<ArrayF32 *>(arr.id);
  return array->actualLength;
}
size_t puleScriptArrayF32ElementByteSize(PuleScriptArrayF32 const arr) {
  return sizeof(float);
}

} // C

// ---- script module file watching

#if defined(PULCHRITUDE_SCRIPT_BIND_FILE)

// because of how lua_Reader works (it's pretty poor imo), just load the
// entire string. It's probably never going to be an issue anyway
namespace {
#include "unistd.h"
std::vector<uint8_t> readFileContents(PuleStringView const filename) {
  std::vector<uint8_t> filebuffer;
  PuleError err = puleError();
  puleSleepMicrosecond(puleMicrosecond(50'000));
  PuleFile const file = (
    puleFileOpen(filename, PuleFileDataMode_text, PuleFileOpenMode_read, &err)
  );
  if (puleErrorConsume(&err)) {
    return {};
  }
  uint64_t const filesize = puleFileSize(file);
  filebuffer.resize(filesize);
  puleFileReadBytes(
    file,
    PuleArrayViewMutable {
      .data = filebuffer.data(),
      .elementStride = 1,
      .elementCount = filebuffer.size(),
    }
  );
  puleFileClose(file);
  return filebuffer;
}}

namespace {
typedef struct {
  PuleAllocator allocator;
  PuleScriptContext context;
  PuleScriptModule scriptModule;
} ScriptWatchInfo;
void scriptWatchFileUpdatedCallback(
  PuleStringView const filename,
  void * const userdata
) {
  ScriptWatchInfo & info = *reinterpret_cast<ScriptWatchInfo *>(userdata);
  auto sourcebuffer = readFileContents(filename);
  PuleError err = puleError();
  puleScriptModuleUpdateFromSource(
    info.context,
    info.scriptModule,
    PuleStringView {
      .contents = reinterpret_cast<char const *>(sourcebuffer.data()),
      .len = sourcebuffer.size(),
    },
    &err
  );
  puleErrorConsume(&err);
}
void scriptWatchDeallocateUserdataCallback(
  void * const userdata
) {
  ScriptWatchInfo & info = *reinterpret_cast<ScriptWatchInfo *>(userdata);
  puleDeallocate(info.allocator, userdata);
}
}

extern "C" {
PuleScriptModuleFileWatchReturn puleScriptModuleFileWatch(
  PuleScriptContext const context,
  PuleAllocator const allocator,
  PuleStringView const filename,
  PuleScriptDebugSymbols debug,
  PuleError * const error
) {
  auto sourcebuffer = readFileContents(filename);
  // even if what we get back is empty, lua is fine with that
  PuleScriptModule const scriptModule = (
    puleScriptModuleCreateFromSource(
      context,
      PuleScriptModuleFromSourceCreateInfo {
        .scriptSource = PuleStringView {
          .contents = reinterpret_cast<char const *>(sourcebuffer.data()),
          .len = sourcebuffer.size(),
        },
        .scriptLabel = filename,
        .debug = debug,
      },
      error
    )
  );
  if (puleErrorExists(error)) {
    return {.scriptModule = {.id=0,}, .watcher = {.id=0,},};
  }
  ScriptWatchInfo * const watchinfo = (
    reinterpret_cast<ScriptWatchInfo *>(
      puleAllocate(
        allocator, {
          .zeroOut = true,
          .numBytes = sizeof(ScriptWatchInfo),
          .alignment = 0,
        }
      )
    )
  );
  *watchinfo = {
    .allocator = allocator,
    .context = context,
    .scriptModule = scriptModule,
  };
  auto const watcher = puleFileWatch({
    .fileUpdatedCallback = &::scriptWatchFileUpdatedCallback,
    .deallocateUserdataCallback = &::scriptWatchDeallocateUserdataCallback,
    .filename = filename,
    .userdata = watchinfo,
  });
  return {.scriptModule = scriptModule, .watcher = watcher,};
}
} // C
#endif
