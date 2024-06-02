# allocator

## structs
### PuleAllocateInfo
a request for an allocation of memory
```c
struct {
  /* minimum number of bytes to be allocated */
  numBytes : size_t;
  /* alignment of pointer, 0 is allowed */
  alignment : size_t = 0;
  /* if memory should be zeroed out */
  zeroOut : bool = false;
};
```
### PuleReallocateInfo
a request for reallocation of existing memory
```c
struct {
  /* address to reallocate memory for */
  allocation : void ptr;
  /* minimum number of bytes to reallocate */
  numBytes : size_t;
  /* alignment of pointer, 0 is allowed */
  alignment : size_t = 0;
};
```
### PuleAllocator
interface to allocate memory
```c
struct {
  /* allocation internal details, can be null */
  implementation : void ptr;
  allocate : @fnptr(void ptr, void ptr, PuleAllocateInfo);
  reallocate : @fnptr(void ptr, void ptr, PuleReallocateInfo);
  deallocate : @fnptr(void, void ptr, void ptr);
};
```

## functions
### puleAllocateDefault

  default allocator provided by engine, probably wrapper around C stdlib,
   but could be wrapper to check for memory leaks

```c
puleAllocateDefault() PuleAllocator;
```
### puleAllocate
```c
puleAllocate(
  _ : PuleAllocator,
  _ : PuleAllocateInfo
) void ptr;
```
### puleMalloc
```c
puleMalloc(
  allocator : PuleAllocator,
  numBytes : size_t
) void ptr;
```
### puleReallocate
```c
puleReallocate(
  _ : PuleAllocator,
  _ : PuleReallocateInfo
) void ptr;
```
### puleDeallocate
```c
puleDeallocate(
  _ : PuleAllocator,
  allocAddress : void ptr
) void;
```
