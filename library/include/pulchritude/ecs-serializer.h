/* auto generated file ecs-serializer */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/data-serializer.h>
#include <pulchritude/ecs.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs
typedef struct {
  PuleEcsWorld world;
  PuleAllocator allocator;
} PuleEcsSerializeWorldInfo;
typedef struct {
  PuleEcsWorld world;
  PuleDsValue readObjectPds;
  PuleAllocator allocator;
} PuleEcsDeserializeWorldInfo;

// enum

// entities

// functions
PULE_exportFn PuleDsValue puleEcsSerializeWorld(PuleEcsSerializeWorldInfo info);
PULE_exportFn void puleEcsDeserializeWorld(PuleEcsDeserializeWorldInfo info);

#ifdef __cplusplus
} // extern C
#endif
