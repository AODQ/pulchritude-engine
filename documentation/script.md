# script

## structs
### PuleScriptModuleFromSourceCreateInfo
```c
struct {
  scriptSource : PuleStringView;
  scriptLabel : PuleStringView;
  debug : PuleScriptDebugSymbols;
};
```
### PuleScriptModuleFileWatchReturn
```c
struct {
  scriptModule : PuleScriptModule;
  watcher : PuleFileWatcher;
};
```

## enums
### PuleErrorScript
```c
enum {
  PuleErrorScript_none,
  PuleErrorScript_executeRuntimeError,
  PuleErrorScript_internalError,
  PuleErrorScript_memoryError,
  PuleErrorScript_syntaxError,
}
```
### PuleScriptDebugSymbols
```c
enum {
  PuleScriptDebugSymbols_enable,
  PuleScriptDebugSymbols_disable,
}
```

## entities
### PuleScriptContext
### PuleScriptModule
### PuleScriptArrayF32

  this is an array to allow scripts or different contexts to allocate memory
  managed by Pulchritude. This is mostly for languages like Lua that don't have
  built-in arrays


## functions
### puleScriptContextCreate
```c
puleScriptContextCreate() PuleScriptContext;
```
### puleScriptContextDestroy
```c
puleScriptContextDestroy(
  _ : PuleScriptContext
) void;
```
### puleScriptModuleCreateFromSource
```c
puleScriptModuleCreateFromSource(
  context : PuleScriptContext,
  createInfo : PuleScriptModuleFromSourceCreateInfo,
  error : PuleError ptr
) PuleScriptModule;
```
### puleScriptModuleUpdateFromSource
```c
puleScriptModuleUpdateFromSource(
  context : PuleScriptContext,
  scriptModule : PuleScriptModule,
  scriptSource : PuleStringView,
  error : PuleError ptr
) void;
```
### puleScriptModuleCreateFromBinary
```c
puleScriptModuleCreateFromBinary(
  context : PuleScriptContext,
  scriptBinary : PuleArrayView,
  error : PuleError ptr
) PuleScriptModule;
```
### puleScriptModuleDestroy
```c
puleScriptModuleDestroy(
  context : PuleScriptContext,
  scriptModule : PuleScriptModule
) void;
```
### puleScriptModuleExecute
```c
puleScriptModuleExecute(
  context : PuleScriptContext,
  scriptModule : PuleScriptModule,
  functionLabel : PuleStringView,
  error : PuleError ptr
) void;
```
### puleScriptGlobal

  this is a global table that script can use to store variables, especially
  useful in between different script calls or environments As a simple example,
  one function/script might create a window, but with no way to access it in
  another function/script context/environment, you can at least store the
  pointer on pulchritude's side

```c
puleScriptGlobal(
  globalLabel : PuleStringView
) void ptr;
```
### puleScriptGlobalSet
```c
puleScriptGlobalSet(
  globalLabel : PuleStringView,
  data : void ptr
) void;
```
### puleScriptArrayF32Create
```c
puleScriptArrayF32Create(
  allocator : PuleAllocator
) PuleScriptArrayF32;
```
### puleScriptArrayF32Destroy
```c
puleScriptArrayF32Destroy(
  arr : PuleScriptArrayF32
) void;
```
### puleScriptArrayF32Ptr
```c
puleScriptArrayF32Ptr(
  arr : PuleScriptArrayF32
) void ptr;
```
### puleScriptArrayF32Append
```c
puleScriptArrayF32Append(
  arr : PuleScriptArrayF32,
  value : float
) void;
```
### puleScriptArrayF32Remove
```c
puleScriptArrayF32Remove(
  arr : PuleScriptArrayF32,
  index : size_t
) void;
```
### puleScriptArrayF32At
```c
puleScriptArrayF32At(
  arr : PuleScriptArrayF32,
  index : size_t
) float;
```
### puleScriptArrayF32Length
```c
puleScriptArrayF32Length(
  arr : PuleScriptArrayF32
) size_t;
```
### puleScriptArrayF32ElementByteSize
```c
puleScriptArrayF32ElementByteSize(
  arr : PuleScriptArrayF32
) size_t;
```
### puleScriptModuleFileWatch
```c
puleScriptModuleFileWatch(
  context : PuleScriptContext,
  allocator : PuleAllocator,
  filename : PuleStringView,
  debug : PuleScriptDebugSymbols,
  error : PuleError ptr
) PuleScriptModuleFileWatchReturn;
```
