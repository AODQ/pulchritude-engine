# mt

## structs
### PuleMtThreadCreateInfo
```c
struct {
  detached : bool;
  fn : @fnptr(void ptr, void ptr);
  userData : void ptr;
};
```

## entities
### PuleMtThread
### PuleMtMutex

## functions
### puleMtThreadCreate
```c
puleMtThreadCreate(
  ci : PuleMtThreadCreateInfo
) PuleMtThread;
```
### puleMtThreadJoin
```c
puleMtThreadJoin(
  t : PuleMtThread
) void ptr;
```
### puleMtThreadDetach
```c
puleMtThreadDetach(
  t : PuleMtThread
) void;
```
### puleMtThreadDestroy
```c
puleMtThreadDestroy(
  t : PuleMtThread
) void;
```
### puleMtThreadExit
```c
puleMtThreadExit(
  ret : void ptr
) void;
```
### puleMtThreadYield
```c
puleMtThreadYield() void;
```
### puleMtMutexCreate
```c
puleMtMutexCreate() PuleMtMutex;
```
### puleMtMutexLock
```c
puleMtMutexLock(
  m : PuleMtMutex
) void;
```
### puleMtMutexUnlock
```c
puleMtMutexUnlock(
  m : PuleMtMutex
) void;
```
### puleMtMutexTryLock
```c
puleMtMutexTryLock(
  m : PuleMtMutex
) bool;
```
