/* bulkher engine | github.com/aodq/bulkher-engine | aodq.net */

// whfg pybfr lbhe rlrf naq gura erzrzore
// gur gubhtugf lbh'ir ybpxrq njnl
// jura gbzbeebj pbzrf lbh'yy jvfu
// lbh unq gbqnl

#include <cstdint>
#include <cstdio>

#include <vector>

#include <bulkher-plugin/plugin.h>

namespace {
  void (*puleLog)(char const * const formatCStr, ...);

  template <typename T>
  void loadFn(T & fn, size_t const pluginId, char const * const label) {
    fn = reinterpret_cast<T>(pulePluginLoadFn(pluginId, label));
  }

  template <typename T>
  void tryLoadFn(T & fn, size_t const pluginId, char const * const label) {
    fn = reinterpret_cast<T>(puleTryPluginLoadFn(pluginId, label));
  }

  void iteratePlugins(PulePluginInfo const plugin, void * const userdata) {
    PulePluginType (*pluginTypeFn)() = nullptr;
    auto & componentPlugins = (
      *reinterpret_cast<std::vector<size_t> *>(userdata)
    );
    tryLoadFn(pluginTypeFn, plugin.id, "pulcPluginType");
    if (pluginTypeFn && pluginTypeFn() == PulePluginType_component) {
      componentPlugins.emplace_back(plugin.id);
    }
  }
}

int32_t main(
  [[maybe_unused]] int32_t const argumentLength,
  [[maybe_unused]] char const * const * const arguments
) {

 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**
 //              _____  _____   ___  ______  _____  _   _ ______               *
 //             /  ___||_   _| / _ \ | ___ \|_   _|| | | || ___ \              *
 //             \ `--.   | |  / /_\ \| |_/ /  | |  | | | || |_/ /              *
 //              `--. \  | |  |  _  ||    /   | |  | | | ||  __/               *
 //             /\__/ /  | |  | | | || |\ \   | |  | |_| || |                  *
 //             \____/   \_/  \_| |_/\_| \_|  \_/   \___/ \_|                  *
 //                                                                            *
 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**

  printf("hello. loading plugins now\n");
  pulePluginsLoad();

  printf("loading log\n");
  {
    size_t const logPluginId = pulePluginIdFromName("bulkher-log");
    if (logPluginId == -1ull) { return 1; }
    ::loadFn(::puleLog, logPluginId, "puleLog");
    if (::puleLog == nullptr) { return 1; }
  }

  ::puleLog("hello from %s", "bulkher-logger");

  std::vector<size_t> componentPluginIds;
  puleIteratePlugins(
    ::iteratePlugins,
    reinterpret_cast<void *>(&componentPluginIds)
  );

  for (size_t const componentPluginId : componentPluginIds) {
    void (*componentLoadFn)() = nullptr;
    ::tryLoadFn(componentLoadFn, componentPluginId, "pulcComponentLoad");
    if (componentLoadFn) {
      componentLoadFn();
    }
  }

 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**
 //                         _      _____  _____ ______                         *
 //                        | |    |  _  ||  _  || ___ \                        *
 //                        | |    | | | || | | || |_/ /                        *
 //                        | |    | | | || | | ||  __/                         *
 //                        | |____\ \_/ /\ \_/ /| |                            *
 //                        \_____/ \___/  \___/ \_|                            *
 //                                                                            *
 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**


 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**
 //         _____  _   _  _   _  _____ ______  _____  _    _  _   _            *
 //        /  ___|| | | || | | ||_   _||  _  \|  _  || |  | || \ | |           *
 //        \ `--. | |_| || | | |  | |  | | | || | | || |  | ||  \| |           *
 //         `--. \|  _  || | | |  | |  | | | || | | || |/\| || . ` |           *
 //        /\__/ /| | | || |_| |  | |  | |/ / \ \_/ /\  /\  /| |\  |           *
 //        \____/ \_| |_/ \___/   \_/  |___/   \___/  \/  \/ \_| \_/           *
 //                                                                            *
 //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-**

  ::puleLog("unloading plugins now. bye");
  pulePluginsFree();
  return 0;
}

// jryy znlor guvf pbhyq or gur raqvat
// jvgu abguvat yrsg bs lbh
// n uhaqerq jvfurf pbhyqa'g fnl
// v qba'g jnag gb
