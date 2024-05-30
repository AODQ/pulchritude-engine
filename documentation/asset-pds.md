# asset-pds

## structs
### PuleAssetPdsWriteInfo
```c
struct {
  head : PuleDsValue;
  prettyPrint : bool = false;
  /*  pretty-print only  */
  spacesPerTab : uint32_t = 2;
  initialTabLevel : uint32_t = 0;
};
```
### PuleAssetPdsCommandLineArgumentsInfo

  can parse command line arguments from the given PDS "help" will be inserted
  if the user requested help flag '--help'

```c
struct {
  allocator : PuleAllocator;
  layout : PuleDsValue;
  argumentLength : int32_t;
  arguments : char const ptr const ptr;
  userRequestedHelpOutNullable : bool ptr;
};
```
### PuleAssetPdsDescription
```c
struct {
  data : void ptr;
  label : char const ptr;
  format : char const ptr;
};
```
### PuleAssetPdsDeserializeInfo
```c
struct {
  value : PuleDsValue;
  descriptions : PuleAssetPdsDescription const ptr;
  descriptionCount : size_t;
};
```

## enums
### PuleErrorAssetPds
```c
enum {
  none,
  decode,
}
```

## functions
### puleAssetPdsLoadFromStream
```c
puleAssetPdsLoadFromStream(
  allocator : PuleAllocator,
  stream : PuleStreamRead,
  error : PuleError ptr
) PuleDsValue;
```
### puleAssetPdsLoadFromRvalStream

  convenience function, takes over management of the input stream

```c
puleAssetPdsLoadFromRvalStream(
  allocator : PuleAllocator,
  rvalStream : PuleStreamRead,
  error : PuleError ptr
) PuleDsValue;
```
### puleAssetPdsLoadFromFile

  convenience function, combined file+stream but disables PDS streaming

```c
puleAssetPdsLoadFromFile(
  allocator : PuleAllocator,
  filename : PuleStringView,
  error : PuleError ptr
) PuleDsValue;
```
### puleAssetPdsLoadFromString
```c
puleAssetPdsLoadFromString(
  allocator : PuleAllocator,
  contents : PuleStringView,
  error : PuleError ptr
) PuleDsValue;
```
### puleAssetPdsWriteToStream
```c
puleAssetPdsWriteToStream(
  stream : PuleStreamWrite,
  writeInfo : PuleAssetPdsWriteInfo
) void;
```
### puleAssetPdsWriteToFile
convenience function using file+stream but disables PDS streaming
```c
puleAssetPdsWriteToFile(
  head : PuleDsValue,
  filename : PuleStringView,
  error : PuleError ptr
) void;
```
### puleAssetPdsWriteToStdout

  convenience function using puleStreamStdoutWrite

```c
puleAssetPdsWriteToStdout(
  head : PuleDsValue
) void;
```
### puleAssetPdsLoadFromCommandLineArguments
```c
puleAssetPdsLoadFromCommandLineArguments(
  info : PuleAssetPdsCommandLineArgumentsInfo,
  error : PuleError ptr
) PuleDsValue;
```
### puleAssetPdsDeserialize
```c
puleAssetPdsDeserialize(
  info : PuleAssetPdsDeserializeInfo,
  error : PuleError ptr
) void;
```
