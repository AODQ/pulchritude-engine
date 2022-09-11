#pragma once

// data-serialization to allow entities to be serialized & deserialized. This
//   of course really be an automated task, given the nature of C, so it's up
//   to modules to assist serialization for any provided entity

#include <pulchritude-core/core.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-ecs/ecs.h>
#include <pulchritude-error/error.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  PuleEcsWorld world;
  PuleAllocator allocator;
} PuleEcsSerializeWorldInfo;

// serializes world
PULE_exportFn PuleDsValue puleEcsSerializeWorld(
  PuleEcsSerializeWorldInfo const info
);

typedef struct {
  PuleEcsWorld world;
  PuleDsValue readObjectPds;
  PuleAllocator allocator;
} PuleEcsDeserializeWorldInfo;

// deserializes world
PULE_exportFn void puleEcsDeserializeWorld(
  PuleEcsDeserializeWorldInfo const info
);

#ifdef __cplusplus
} // extern C
#endif
