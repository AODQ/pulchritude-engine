#pragma once

#include <pulchritude-core/core.h>
#include <pulchritude-plugin/plugin-payload.h>
#include <pulchritude-string/string.h>

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
PULE_exportFn void pulePluginsLoad(
  PuleStringView const * const paths,
  size_t const pathsLength
);
PULE_exportFn void pulePluginsFree();
PULE_exportFn void pulePluginsReload();
PULE_exportFn size_t pulePluginIdFromName(char const * const pluginNameCStr);
PULE_exportFn char const * pulePluginName(size_t const pluginId);
PULE_exportFn void * pulePluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
);
PULE_exportFn void * pulePluginLoadFnTry(
  size_t const pluginId,
  char const * const fnCStr
);
PULE_exportFn void pulePluginIterate(
  void (* const fn)(PulePluginInfo const plugin, void * const userdata),
  void * const userdata
);

#ifdef __cplusplus
} // extern c
#endif
