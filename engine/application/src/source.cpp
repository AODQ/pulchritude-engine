/* pulzoad | puzzled engine | github.com/aodq/pulzoad-engine | aodq.net */

// whfg pybfr lbhe rlrf naq gura erzrzore
// gur gubhtugf lbh'ir ybpxrq njnl
// jura gbzbeebj pbzrf lbh'yy jvfu
// lbh unq gbqnl

#include <cstdint>
#include <cstdio>

#include <pulzoad-plugin/plugin.h>

namespace {
  void (*puleLog)(char const * const formatCStr, ...);

  template <typename T>
  void loadFn(T & fn, size_t const pluginId, char const * const label) {
    fn = reinterpret_cast<T>(pulePluginLoadFn(pluginId, label));
  }
}

int32_t main(
  [[maybe_unused]] int32_t const argumentLength,
  [[maybe_unused]] char const * const * const arguments
) {
  printf("hello. loading plugins now\n");
  pulePluginsLoad();

  printf("loading log\n");
  {
    size_t const logPluginId = pulePluginIdFromName("pulzoad-log");
    if (logPluginId == -1ull) { return 1; }
    loadFn(::puleLog, logPluginId, "puleLog");
    if (::puleLog == nullptr) { return 1; }
  }

  ::puleLog("hello from %s", "pulzoad-logger");

  ::puleLog("unloading plugins now. bye");
  pulePluginsFree();
  return 0;
}

// jryy znlor guvf pbhyq or gur raqvat
// jvgu abguvat yrsg bs lbh
// n uhaqerq jvfurf pbhyqa'g fnl
// v qba'g jnag gb
