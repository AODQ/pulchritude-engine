# error

## structs
### PuleError
```c
struct {
  description : PuleString;
  id : uint32_t;
  sourceLocationNullable : char const ptr;
  lineNumber : size_t;
  child : PuleError ptr;
};
```

## functions
### puleError
```c
puleError() PuleError;
```
### puleErrorDestroy

    deallocates memory used by error, does not log

```c
puleErrorDestroy(
  error : PuleError ptr
) void;
```
### puleErrorConsume

    consumes error , logging the error appropiately with puleLog, returning the
    ID of the error, and deallocating the description

```c
puleErrorConsume(
  error : PuleError ptr
) uint32_t;
```
### puleErrorExists
```c
puleErrorExists(
  error : PuleError ptr
) bool;
```
### puleErrorPropagate
```c
puleErrorPropagate(
  source : PuleError ptr,
  newError : PuleError
) void;
```
