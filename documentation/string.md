# string

## structs
### PuleString
ASCII/UTF8 encoded string that must end with null-terminating character
```c
struct {
  contents : char ptr;
  /* number of bytes excluding null-terminator */
  len : size_t = 0;
  allocator : PuleAllocator;
};
```
### PuleStringView
light-weight view into a string, with no knowledge of its underlying
    allocation. Unable to modify the contents of the string. The convention
    is to pass immutable data between interfaces with the string-view, and
    make copies if the local system needs to retain the information.
```c
struct {
  contents : char const ptr = null;
  len : size_t = 0;
};
```

## functions
### puleStringDefault

  create string with default allocator

```c
puleStringDefault(
  baseContents : char const ptr
) void;
```
### puleString
```c
puleString(
  _ : PuleAllocator,
  contents : char const ptr
) PuleString;
```
### puleStringCopy
```c
puleStringCopy(
  _ : PuleAllocator,
  base : PuleStringView
) PuleString;
```
### puleStringDestroy
```c
puleStringDestroy(
  _ : PuleString
) void;
```
### puleStringAppend
```c
puleStringAppend(
  str : PuleString ref,
  append : PuleStringView
) void;
```
### puleStringFormat
```c
puleStringFormat(
  _ : PuleAllocator,
  format : char const ptr,
  _ : vaArgs
) PuleString;
```
### puleStringFormatDefault
```c
puleStringFormatDefault(
  format : char const ptr,
  _ : vaArgs
) PuleString;
```
### puleStringFormatVargs
```c
puleStringFormatVargs(
  _ : PuleAllocator,
  format : char const ptr,
  _ : vaList
) PuleString;
```
### puleStringView
```c
puleStringView(
  string : PuleString
) PuleStringView;
```
### puleCStr
```c
puleCStr(
  cstr : char const ptr
) PuleStringView;
```
### puleStringViewEq
```c
puleStringViewEq(
  v0 : PuleStringView,
  v1 : PuleStringView
) bool;
```
### puleStringViewContains
```c
puleStringViewContains(
  base : PuleStringView,
  seg : PuleStringView
) bool;
```
### puleStringViewEqCstr
```c
puleStringViewEqCstr(
  v0 : PuleStringView,
  v1 : char const ptr
) bool;
```
### puleStringViewHash
```c
puleStringViewHash(
  view : PuleStringView
) size_t;
```
