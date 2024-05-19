# log

## functions
### puleLogDebugEnabled
```c
puleLogDebugEnabled() bool ptr;
```
### puleLogErrorSegfaultsEnabled
```c
puleLogErrorSegfaultsEnabled() bool ptr;
```
### puleLog
```c
puleLog(
  formatCStr : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleLogDebug
```c
puleLogDebug(
  formatCStr : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleLogWarn
```c
puleLogWarn(
  formatCStr : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleLogError
```c
puleLogError(
  formatCStr : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleLogDev

  this is just for dev purposes in a branch, not to be committed to master

```c
puleLogDev(
  formatCStr : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleLogLn

  log without newline

```c
puleLogLn(
  formatCStr : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleLogRaw
```c
puleLogRaw(
  formatCStr : char const ptr,
  args : __VA_ARGS__
) void;
```
