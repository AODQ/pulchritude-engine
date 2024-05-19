/* auto generated file script */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/string.h>
#include <pulchritude/error.h>
#include <pulchritude/array.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleStringView scriptSource;
  PuleStringView scriptLabel;
  PuleScriptDebugSymbols debug;
} PuleScriptModuleFromSourceCreateInfo;
typedef struct {
  PuleScriptModule scriptModule;
  PuleFileWatcher watcher;
} PuleScriptModuleFileWatchReturn;

// enum
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

// entities
typedef struct { uint64_t id; } PuleScriptContext;
typedef struct { uint64_t id; } PuleScriptModule;
/* 
  this is an array to allow scripts or different contexts to allocate memory
  managed by Pulchritude. This is mostly for languages like Lua that don't have
  built-in arrays
 */
typedef struct { uint64_t id; } PuleScriptArrayF32;

// functions
PULE_exportFn PuleScriptContext puleScriptContextCreate();
PULE_exportFn void puleScriptContextDestroy(PuleScriptContext _);
PULE_exportFn PuleScriptModule puleScriptModuleCreateFromSource(PuleScriptContext context, PuleScriptModuleFromSourceCreateInfo createInfo, PuleError * error);
PULE_exportFn void puleScriptModuleUpdateFromSource(PuleScriptContext context, PuleScriptModule scriptModule, PuleStringView scriptSource, PuleError * error);
PULE_exportFn PuleScriptModule puleScriptModuleCreateFromBinary(PuleScriptContext context, PuleArrayView scriptBinary, PuleError * error);
PULE_exportFn void puleScriptModuleDestroy(PuleScriptContext context, PuleScriptModule scriptModule);
PULE_exportFn void puleScriptModuleExecute(PuleScriptContext context, PuleScriptModule scriptModule, PuleStringView functionLabel, PuleError * error);
/* 
  this is a global table that script can use to store variables, especially
  useful in between different script calls or environments As a simple example,
  one function/script might create a window, but with no way to access it in
  another function/script context/environment, you can at least store the
  pointer on pulchritude's side
 */
PULE_exportFn void * puleScriptGlobal(PuleStringView globalLabel);
PULE_exportFn void puleScriptGlobalSet(PuleStringView globalLabel, void * data);
PULE_exportFn PuleScriptArrayF32 puleScriptArrayF32Create(PuleAllocator allocator);
PULE_exportFn void puleScriptArrayF32Destroy(PuleScriptArrayF32 arr);
PULE_exportFn void * puleScriptArrayF32Ptr(PuleScriptArrayF32 arr);
PULE_exportFn void puleScriptArrayF32Append(PuleScriptArrayF32 arr, float value);
PULE_exportFn void puleScriptArrayF32Remove(PuleScriptArrayF32 arr, size_t index);
PULE_exportFn float puleScriptArrayF32At(PuleScriptArrayF32 arr, size_t index);
PULE_exportFn size_t puleScriptArrayF32Length(PuleScriptArrayF32 arr);
PULE_exportFn size_t puleScriptArrayF32ElementByteSize(PuleScriptArrayF32 arr);
PULE_exportFn PuleScriptModuleFileWatchReturn puleScriptModuleFileWatch(PuleScriptContext context, PuleAllocator allocator, PuleStringView filename, PuleScriptDebugSymbols debug, PuleError * error);

#ifdef __cplusplus
} // extern C
#endif
