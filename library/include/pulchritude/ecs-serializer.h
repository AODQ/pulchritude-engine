/* auto generated file ecs-serializer */
#pragma once
#include "core.h"

#include "data-serializer.h"
#include "ecs.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs
struct PuleEcsSerializeWorldInfo;
struct PuleEcsDeserializeWorldInfo;

typedef struct PuleEcsSerializeWorldInfo {
  PuleEcsWorld world;
  PuleAllocator allocator;
} PuleEcsSerializeWorldInfo;
typedef struct PuleEcsDeserializeWorldInfo {
  PuleEcsWorld world;
  PuleDsValue readObjectPds;
  PuleAllocator allocator;
} PuleEcsDeserializeWorldInfo;

// functions
PULE_exportFn PuleDsValue puleEcsSerializeWorld(PuleEcsSerializeWorldInfo info);
PULE_exportFn void puleEcsDeserializeWorld(PuleEcsDeserializeWorldInfo info);

#ifdef __cplusplus
} // extern C
#endif
