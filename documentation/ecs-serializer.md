# ecs-serializer

## structs
### PuleEcsSerializeWorldInfo
```c
struct {
  world : PuleEcsWorld;
  allocator : PuleAllocator;
};
```
### PuleEcsDeserializeWorldInfo
```c
struct {
  world : PuleEcsWorld;
  readObjectPds : PuleDsValue;
  allocator : PuleAllocator;
};
```

## functions
### puleEcsSerializeWorld
```c
puleEcsSerializeWorld(
  info : PuleEcsSerializeWorldInfo
) PuleDsValue;
```
### puleEcsDeserializeWorld
```c
puleEcsDeserializeWorld(
  info : PuleEcsDeserializeWorldInfo
) void;
```
