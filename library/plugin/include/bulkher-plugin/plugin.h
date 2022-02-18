#pragma once

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

enum PulePluginType {
  PulePluginType_library,
  PulePluginType_component,
  PulePluginTypeEnd,
  PulePluginTypeMaxEnum = 0x7FFFFFFF,
};

#ifdef __cplusplus
extern "C" {
#endif
void pulePluginsLoad();
void pulePluginsFree();
void pulePluginsReload();
size_t pulePluginIdFromName(char const * const pluginNameCStr);
void * pulePluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
);
void * puleTryPluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
);

struct PulePluginInfo {
  char const * name;
  size_t id;
};

void puleIteratePlugins(
  void (*fn)(PulePluginInfo const, void * const userdata),
  void * userdata
);

#ifdef __cplusplus
} // extern c
#endif
