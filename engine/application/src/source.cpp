/* pulchritude engine | github.com/aodq/pulchritude-engine | aodq.net */

// whfg pybfr lbhe rlrf naq gura erzrzore
// gur gubhtugf lbh'ir ybpxrq njnl
// jura gbzbeebj pbzrf lbh'yy jvfu
// lbh unq gbqnl

#include <cstdint>
#include <cstdio>

#include <vector>

#include <pulchritude-plugin/plugin.h>
#include <pulchritude-log/log.h>

// loading plugins
namespace {
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
    puleLog("name '%s' plugin type %p", plugin.name, pluginTypeFn);
    if (pluginTypeFn && pluginTypeFn() == PulePluginType_component) {
      puleLog("plugin registered as component");
      componentPlugins.emplace_back(plugin.id);
    }
  }
}

// update components
namespace {
  std::vector<void(*)()> updateableComponents;
}

int32_t main(
  [[maybe_unused]] int32_t const argumentLength,
  [[maybe_unused]] char const * const * const arguments
) {

  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //              _____  _____   ___  ______  _____  _   _ ______              *
  //             /  ___||_   _| / _ \ | ___ \|_   _|| | | || ___ \             *
  //             \ `--.   | |  / /_\ \| |_/ /  | |  | | | || |_/ /             *
  //              `--. \  | |  |  _  ||    /   | |  | | | ||  __/              *
  //             /\__/ /  | |  | | | || |\ \   | |  | |_| || |                 *
  //             \____/   \_/  \_| |_/\_| \_|  \_/   \___/ \_|                 *
  //                                                                           *
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

  puleLog("hello. loading plugins now\n");
  pulePluginsLoad();

  std::vector<size_t> componentPluginIds;
  puleIteratePlugins(
    ::iteratePlugins,
    reinterpret_cast<void *>(&componentPluginIds)
  );

  // initiate all plugins
  for (size_t const componentPluginId : componentPluginIds) {
    // try to load component
    void (*componentLoadFn)() = nullptr;
    ::tryLoadFn(componentLoadFn, componentPluginId, "pulcComponentLoad");
    puleLog("for plugin id %zu got loadfn %p", componentPluginId, componentLoadFn);
    if (componentLoadFn) {
      puleLog("loading component");
      componentLoadFn();
      puleLog("finished");
    }

    // check if they have an update function
    puleLog("loading component");
    void (*componentUpdateFn)() = nullptr;
    ::tryLoadFn(componentUpdateFn, componentPluginId, "pulcComponentUpdate");
    puleLog("loading component: %p", componentUpdateFn);
    if (componentUpdateFn) {
      updateableComponents.emplace_back(componentUpdateFn);
    }
  }

  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //                         _      _____  _____ ______                        *
  //                        | |    |  _  ||  _  || ___ \                       *
  //                        | |    | | | || | | || |_/ /                       *
  //                        | |    | | | || | | ||  __/                        *
  //                        | |____\ \_/ /\ \_/ /| |                           *
  //                        \_____/ \___/  \___/ \_|                           *
  //                                                                           *
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  puleLog("entering loop");
  while (true) {
    for (auto const componentUpdateFn : updateableComponents) {
      componentUpdateFn();
    }
  }

  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //         _____  _   _  _   _  _____ ______  _____  _    _  _   _           *
  //        /  ___|| | | || | | ||_   _||  _  \|  _  || |  | || \ | |          *
  //        \ `--. | |_| || | | |  | |  | | | || | | || |  | ||  \| |          *
  //         `--. \|  _  || | | |  | |  | | | || | | || |/\| || . ` |          *
  //        /\__/ /| | | || |_| |  | |  | |/ / \ \_/ /\  /\  /| |\  |          *
  //        \____/ \_| |_/ \___/   \_/  |___/   \___/  \/  \/ \_| \_/          *
  //                                                                           *
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

  // try to unload components
  for (size_t const componentPluginId : componentPluginIds) {
    void (*componentUnloadFn)() = nullptr;
    ::tryLoadFn(componentUnloadFn, componentPluginId, "pulcComponentUnload");
    if (componentUnloadFn) {
      componentUnloadFn();
    }
  }

  ::puleLog("unloading plugins now. bye");
  pulePluginsFree();
  return 0;
}

// jryy znlor guvf pbhyq or gur raqvat
// jvgu abguvat yrsg bs lbh
// n uhaqerq jvfurf pbhyqa'g fnl
// v qba'g jnag gb
