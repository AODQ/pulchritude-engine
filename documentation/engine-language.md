# engine-language

## structs
### PuleELJitEngineCreateInfo
```c
struct {
  optimize : bool = false;
  insertEngineSymbols : bool = false;
};
```

## enums
### PuleErrorEL
```c
enum {
  PuleErrorEL_none,
  PuleErrorEL_compileError,
}
```

## entities
### PuleELModule
### PuleELJitEngine

## functions
### puleELModuleCreate
```c
puleELModuleCreate(
  stream : PuleStreamRead,
  name : PuleStringView,
  error : PuleError ptr
) PuleELModule;
```
### puleELModuleDestroy
```c
puleELModuleDestroy(
  module : PuleELModule
) void;
```
### puleELJitEngineCreate
```c
puleELJitEngineCreate(
  ci : PuleELJitEngineCreateInfo
) PuleELJitEngine;
```
### puleELJitEngineDestroy
```c
puleELJitEngineDestroy(
  jitEngine : PuleELJitEngine
) void;
```
### puleELJitEngineAddModule
```c
puleELJitEngineAddModule(
  jitEngine : PuleELJitEngine,
  module : PuleELModule
) void;
```
### puleELJitEngineFunctionAddress
```c
puleELJitEngineFunctionAddress(
  jitEngine : PuleELJitEngine,
  functionName : PuleStringView
) void ptr;
```
