# engine-language

## structs
### PuleELModuleCreateInfo
```c
struct {
  moduleIR : PuleStringView;
  name : PuleStringView;
  optimize : bool = false;
  debug : bool = false;
  error : PuleError ptr;
};
```
### PuleELEngineCreateInfo
```c
struct {
  insertEngineSymbols : bool = false;
};
```

## enums
### PuleErrorEL
```c
enum {
  none,
  compileError,
}
```

## entities
### PuleELModule
### PuleELEngine
### PuleELFence
### PuleELQueue

## functions
### puleELModuleCreate
```c
puleELModuleCreate(
  ci : PuleELModuleCreateInfo
) PuleELModule;
```
### puleELModuleDestroy
```c
puleELModuleDestroy(
  module : PuleELModule
) void;
```
### puleELEngineCreate
```c
puleELEngineCreate(
  ci : PuleELEngineCreateInfo
) PuleELEngine;
```
### puleELEngineDestroy
```c
puleELEngineDestroy(
  engine : PuleELEngine
) void;
```
### puleELEngineAddModule
```c
puleELEngineAddModule(
  engine : PuleELEngine,
  module : PuleELModule
) void;
```
### puleELFenceWait
```c
puleELFenceWait(
  fence : PuleELFence,
  timeout : PuleMicrosecond
) bool;
```
### puleELFenceDestroy
```c
puleELFenceDestroy(
  fence : PuleELFence
) void;
```
### puleELQueueCreate
```c
puleELQueueCreate(
  engine : PuleELEngine
) PuleELQueue;
```
### puleELQueueDestroy
```c
puleELQueueDestroy(
  queue : PuleELQueue
) void;
```
### puleELQueueStackLength
```c
puleELQueueStackLength(
  queue : PuleELQueue
) size_t;
```
### puleELQueueStackPush
```c
puleELQueueStackPush(
  queue : PuleELQueue,
  value : uint64_t
) void;
```
### puleELQueueStackPop
```c
puleELQueueStackPop(
  queue : PuleELQueue
) uint64_t;
```
### puleELQueueSubmit
```c
puleELQueueSubmit(
  queue : PuleELQueue,
  functionName : PuleStringView
) void;
```
