# plugin

## structs
### PulePluginInfo
```c
struct {
  name : char const ptr;
  id : uint64_t;
};
```

## enums
### PulePluginType
```c
enum {
  PulePluginType_library,
  PulePluginType_component,
}
```

## entities
### PulePluginPayload

## functions
### pulePluginsLoad
```c
pulePluginsLoad(
  paths : PuleStringView const ptr,
  pathsLength : size_t
) void;
```
### pulePluginsFree
```c
pulePluginsFree() void;
```
### pulePluginsReload
```c
pulePluginsReload() void;
```
### pulePluginIdFromName
```c
pulePluginIdFromName(
  pluginNameCStr : char const ptr
) size_t;
```
### pulePluginName
```c
pulePluginName(
  pluginId : size_t
) char const ptr;
```
### pulePluginLoadFn
```c
pulePluginLoadFn(
  pluginId : size_t,
  fnCStr : char const ptr
) void ptr;
```
### pulePluginLoadFnTry
```c
pulePluginLoadFnTry(
  pluginId : size_t,
  fnCStr : char const ptr
) void ptr;
```
### pulePluginIterate
```c
pulePluginIterate(
  fn : @fnptr(void, PulePluginInfo, void ptr),
  userdata : void ptr
) void;
```
### pulePluginPayloadCreate
```c
pulePluginPayloadCreate(
  allocator : PuleAllocator
) PulePluginPayload;
```
### pulePluginPayloadDestroy
```c
pulePluginPayloadDestroy(
  payload : PulePluginPayload
) void;
```
### pulePluginPayloadFetch
```c
pulePluginPayloadFetch(
  payload : PulePluginPayload,
  handle : PuleStringView
) void ptr;
```
### pulePluginPayloadFetchU64
```c
pulePluginPayloadFetchU64(
  payload : PulePluginPayload,
  handle : PuleStringView
) uint64_t;
```
### pulePluginPayloadStore
```c
pulePluginPayloadStore(
  payload : PulePluginPayload,
  handle : PuleStringView,
  data : void ptr
) void;
```
### pulePluginPayloadStoreU64
```c
pulePluginPayloadStoreU64(
  payload : PulePluginPayload,
  handle : PuleStringView,
  data : uint64_t
) void;
```
### pulePluginPayloadRemove
```c
pulePluginPayloadRemove(
  payload : PulePluginPayload,
  handle : PuleStringView
) void;
```
