#pragma once

#include "pulchritude-core/core.h"
#include "pulchritude-string/string.h"
#include "pulchritude-error/error.h"
#include "pulchritude-array/array.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorScript_none,
  PuleErrorScript_executeRuntimeError,
  PuleErrorScript_internalError,
  PuleErrorScript_memoryError,
  PuleErrorScript_syntaxError,
} PuleErrorScript;

typedef enum {
  PuleScriptDebugSymbols_enable,
  PuleScriptDebugSymbols_disable,
} PuleScriptDebugSymbols;

// -- script context
typedef struct { uint64_t id; } PuleScriptContext;
PULE_exportFn PuleScriptContext puleScriptContextCreate();
PULE_exportFn void puleScriptContextDestroy(PuleScriptContext const context);

// -- script module
typedef struct { uint64_t id; } PuleScriptModule;

// %PuleErrorScript { memoryError, syntaxError }
typedef struct {
  PuleStringView scriptSource;
  PuleStringView scriptLabel;
  PuleScriptDebugSymbols debug;
} PuleScriptModuleFromSourceCreateInfo;
PULE_exportFn PuleScriptModule puleScriptModuleCreateFromSource(
  PuleScriptContext const context,
  PuleScriptModuleFromSourceCreateInfo const createInfo,
  PuleError * const error
);
PULE_exportFn void puleScriptModuleUpdateFromSource(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule,
  PuleStringView scriptSource,
  PuleError * const error
);
// %PuleErrorScript { memoryError, syntaxError }
// TODO not written yet, probably only when LuaJIT is integrated
PULE_exportFn PuleScriptModule puleScriptModuleCreateFromBinary(
  PuleScriptContext const context,
  PuleArrayView const scriptBinary,
  PuleError * const error
);
PULE_exportFn void puleScriptModuleDestroy(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule
);
// %PuleErrorScript { executeRuntimeError, internalError, memoryError }
PULE_exportFn void puleScriptModuleExecute(
  PuleScriptContext const context,
  PuleScriptModule const scriptModule,
  PuleStringView const functionLabel,
  PuleError * const error
);

// this is a global table that script can use to store variables, especially
//   useful in between different script calls or environments
// As a simple example, one function/script might create a window, but with no
//   way to access it in another function/script context/environment, you can
//   at least store the pointer on pulchritude's side
PULE_exportFn void * puleScriptGlobal(PuleStringView const globalLabel);
PULE_exportFn void puleScriptGlobalSet(
  PuleStringView const globalLabel,
  void * const data
);

// this is an array to allow scripts or different contexts to allocate memory
//   managed by Pulchritude. This is mostly for languages like Lua that don't
//   have built-in arrays
typedef struct { uint64_t id; } PuleScriptArrayF32;
PULE_exportFn PuleScriptArrayF32 puleScriptArrayF32Create(
  PuleAllocator const allocator
);
PULE_exportFn void puleScriptArrayF32Destroy(PuleScriptArrayF32 const arr);
PULE_exportFn void * puleScriptArrayF32Ptr(PuleScriptArrayF32 const arr);
PULE_exportFn void puleScriptArrayF32Append(
  PuleScriptArrayF32 const arr,
  float const value
);
PULE_exportFn void puleScriptArrayF32Remove(
  PuleScriptArrayF32 const arr,
  size_t const index
);
PULE_exportFn float puleScriptArrayF32At(
  PuleScriptArrayF32 const arr,
  size_t const index
);
PULE_exportFn size_t puleScriptArrayF32Length(
  PuleScriptArrayF32 const arr
);
PULE_exportFn size_t puleScriptArrayF32ElementByteSize(
  PuleScriptArrayF32 const arr
);

// script module file-watching
// TODO - most likely move this to asset/script?
#if defined(PULCHRITUDE_SCRIPT_BIND_FILE)
#include "pulchritude-file/file.h"
typedef struct {
  PuleScriptModule scriptModule;
  PuleFileWatcher watcher;
} PuleScriptModuleFileWatchReturn;
PULE_exportFn PuleScriptModuleFileWatchReturn puleScriptModuleFileWatch(
  PuleScriptContext const context,
  PuleAllocator const allocator,
  PuleStringView const filename,
  PuleScriptDebugSymbols debug,
  PuleError * const error
);
#endif

#ifdef __cplusplus
}
#endif
