#pragma once

#include <pulchritude-core/core.h>

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

typedef enum {
  PulePluginType_library,
  PulePluginType_component,
  PulePluginType_End,
  PulePluginType_MaxEnum = 0x7FFFFFFF,
} PulePluginType;

typedef struct {
  char const * name;
  uint64_t id;
} PulePluginInfo;

#ifdef __cplusplus
extern "C" {
#endif
PULE_exportFn void pulePluginsLoad();
PULE_exportFn void pulePluginsFree();
PULE_exportFn void pulePluginsReload();
PULE_exportFn size_t pulePluginIdFromName(char const * const pluginNameCStr);
PULE_exportFn void * pulePluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
);
PULE_exportFn void * puleTryPluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
);
PULE_exportFn void puleIteratePlugins(
  void (* const fn)(PulePluginInfo const, void * const userdata),
  void * const userdata
);

#ifdef __cplusplus
} // extern c
#endif
