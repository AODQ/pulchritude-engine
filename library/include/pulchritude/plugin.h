/* auto generated file plugin */
#pragma once
#include "core.h"

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PulePluginType_library = 0,
  PulePluginType_component = 1,
} PulePluginType;
const uint32_t PulePluginTypeSize = 2;

// entities
typedef struct PulePluginPayload { uint64_t id; } PulePluginPayload;

// structs
struct PulePluginInfo;

typedef struct PulePluginInfo {
  char const * name;
  uint64_t id;
} PulePluginInfo;

// functions
PULE_exportFn void pulePluginsLoad(PuleStringView const * paths, size_t pathsLength);
PULE_exportFn void pulePluginsFree();
PULE_exportFn void pulePluginsReload();
PULE_exportFn size_t pulePluginIdFromName(char const * pluginNameCStr);
PULE_exportFn char const * pulePluginName(size_t pluginId);
PULE_exportFn void * pulePluginLoadFn(size_t pluginId, char const * fnCStr);
PULE_exportFn void * pulePluginLoadFnTry(size_t pluginId, char const * fnCStr);
PULE_exportFn void pulePluginIterate(void(* fn)(PulePluginInfo, void *), void * userdata);
PULE_exportFn PulePluginPayload pulePluginPayloadCreate(PuleAllocator allocator);
PULE_exportFn void pulePluginPayloadDestroy(PulePluginPayload payload);
PULE_exportFn void * pulePluginPayloadFetch(PulePluginPayload payload, PuleStringView handle);
PULE_exportFn uint64_t pulePluginPayloadFetchU64(PulePluginPayload payload, PuleStringView handle);
PULE_exportFn void pulePluginPayloadStore(PulePluginPayload payload, PuleStringView handle, void * data);
PULE_exportFn void pulePluginPayloadStoreU64(PulePluginPayload payload, PuleStringView handle, uint64_t data);
PULE_exportFn void pulePluginPayloadRemove(PulePluginPayload payload, PuleStringView handle);

#ifdef __cplusplus
} // extern C
#endif
