#pragma once

#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

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
#ifdef __cplusplus
} // extern c
#endif
