# stream

## structs
### PuleStreamRead
```c
struct {
  userdata : void ptr;
  readByte : @fnptr(uint8_t, void ptr);
  peekByte : @fnptr(uint8_t, void ptr);
  isDone : @fnptr(bool, void ptr);
  destroy : @fnptr(void, void ptr);
};
```
### PuleStreamWrite
```c
struct {
  userdata : void ptr;
  writeBytes : @fnptr(void, void ptr, uint8_t const ptr, size_t);
  flush : @fnptr(void, void ptr);
  destroy : @fnptr(void, void ptr);
};
```

## functions
### puleStreamReadByte
```c
puleStreamReadByte(
  stream : PuleStreamRead
) uint8_t;
```
### puleStreamReadLine
 TODO this can be done with ptr to PuleString maybe?
```c
puleStreamReadLine(
  stream : PuleStreamRead,
  alloc : PuleAllocator
) PuleString;
```
### puleStreamPeekByte
```c
puleStreamPeekByte(
  stream : PuleStreamRead
) uint8_t;
```
### puleStreamReadIsDone
```c
puleStreamReadIsDone(
  stream : PuleStreamRead
) bool;
```
### puleStreamReadDestroy
```c
puleStreamReadDestroy(
  stream : PuleStreamRead
) void;
```
### puleStreamDumpToBuffer
```c
puleStreamDumpToBuffer(
  stream : PuleStreamRead
) PuleBuffer;
```
### puleStreamReadFromString
```c
puleStreamReadFromString(
  stringView : PuleStringView
) PuleStreamRead;
```
### puleStreamReadFromBuffer
```c
puleStreamReadFromBuffer(
  bufferView : PuleBufferView
) PuleStreamRead;
```
### puleStreamWriteBytes
```c
puleStreamWriteBytes(
  stream : PuleStreamWrite,
  bytes : uint8_t const ptr,
  length : size_t
) void;
```
### puleStreamWriteStrFormat
```c
puleStreamWriteStrFormat(
  stream : PuleStreamWrite,
  format : char const ptr,
  args : __VA_ARGS__
) void;
```
### puleStreamWriteFlush
```c
puleStreamWriteFlush(
  stream : PuleStreamWrite
) void;
```
### puleStreamWriteDestroy
```c
puleStreamWriteDestroy(
  stream : PuleStreamWrite
) void;
```
### puleStreamStdoutWrite
```c
puleStreamStdoutWrite() PuleStreamWrite;
```
