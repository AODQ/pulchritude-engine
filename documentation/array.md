# array

## structs
### PuleArray
```c
struct {
  content : void ptr;
  elementByteLength : size_t;
  elementAlignmentByteLength : size_t;
  elementCount : size_t;
  allocator : PuleAllocator;
};
```
### PuleArrayCreateInfo
```c
struct {
  /* must be non-zero */
  elementByteLength : size_t;
  /* can be zero */
  elementAlignmentByteLength : size_t;
  allocator : PuleAllocator;
};
```
### PuleArrayView
view contents of an array without needing knowledge of its allocation
    detail.
```c
struct {
  data : uint8_t const ptr;
  elementStride : size_t;
  elementCount : size_t;
};
```
### PuleArrayViewMutable
view contents of an array without needing knowledge of its allocation
    detail.
```c
struct {
  data : uint8_t ptr;
  elementStride : size_t;
  elementCount : size_t;
};
```
### PuleBufferView
view contents of contiguous memory without needing knowledge of its
    allocation detail.
```c
struct {
  data : uint8_t const ptr;
  byteLength : size_t;
};
```
### PuleBufferViewMutable
```c
struct {
  data : uint8_t ptr;
  byteLength : size_t;
};
```
### PuleBuffer
```c
struct {
  data : uint8_t ptr;
  byteLength : size_t;
  allocator : PuleAllocator;
};
```

## enums
### PuleErrorArray
```c
enum {
  PuleErrorArray_none,
  PuleErrorArray_errorAllocation,
}
```

## functions
### puleArray
```c
puleArray(
  _ : PuleArrayCreateInfo
) PuleArray;
```
### puleArrayDestroy
```c
puleArrayDestroy(
  _ : PuleArray
) void;
```
### puleArrayAppend
```c
puleArrayAppend(
  _ : PuleArray ref
) void ptr;
```
### puleArrayElementAt
```c
puleArrayElementAt(
  _ : PuleArray,
  idx : size_t
) void ptr;
```
### puleBufferCreate
```c
puleBufferCreate(
  _ : PuleAllocator
) PuleBuffer;
```
### puleBufferCopyWithData
```c
puleBufferCopyWithData(
  _ : PuleAllocator,
  data : uint8_t const ptr,
  length : size_t
) PuleBuffer;
```
### puleBufferResize
```c
puleBufferResize(
  _ : PuleBuffer ref,
  length : size_t
) void;
```
### puleBufferAppend
```c
puleBufferAppend(
  _ : PuleBuffer ref,
  data : uint8_t const ptr,
  length : size_t
) void;
```
### puleBufferDestroy
```c
puleBufferDestroy(
  _ : PuleBuffer
) void;
```
