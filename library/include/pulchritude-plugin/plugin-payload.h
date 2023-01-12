#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-string/string.h>

typedef struct { uint64_t id; } PulePluginPayload;

#ifdef __cplusplus
extern "C" {
#endif

PulePluginPayload pulePluginPayloadCreate(PuleAllocator const allocator);
void pulePluginPayloadDestroy(PulePluginPayload const payload);

// fetches data stores
PULE_exportFn void * pulePluginPayloadFetch(
  PulePluginPayload const payload,
  PuleStringView const handle
);

PULE_exportFn void * pulePluginPayloadFetch(
  PulePluginPayload const payload,
  PuleStringView const handle
);

PULE_exportFn uint64_t pulePluginPayloadFetchU64(
  PulePluginPayload const payload,
  PuleStringView const handle
);

PULE_exportFn void pulePluginPayloadStore(
  PulePluginPayload const payload,
  PuleStringView const handle,
  void * const data
);

PULE_exportFn void pulePluginPayloadStoreU64(
  PulePluginPayload const payload,
  PuleStringView const handle,
  uint64_t const data
);

PULE_exportFn void pulePluginPayloadRemove(
  PulePluginPayload const payload,
  PuleStringView const handle
);

#ifdef __cplusplus
} // extern c
#endif
