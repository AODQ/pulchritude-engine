#include <pulchritude-plugin/plugin.h>

#include <pulchritude-file/file.h>
#include <pulchritude-log/log.h>
#include <pulchritude-plugin/engine.h>
#include <pulchritude-string/string.h>

#include <filesystem>
#include <string>
#include <vector>

#if defined(__unix__) || defined(__APPLE__)
  #include <dlfcn.h>
#elif defined(_WIN32) || defined(_WIN64)
  #include <errhandlingapi.h>
  #include <libloaderapi.h>
#else
  #error "Unsupported operating system"
#endif

namespace {

#if defined(__unix__) || defined(__APPLE__)
  using PluginHandle = void *;
#elif defined(_WIN64) || defined(_WIN32)
  using PluginHandle = ::HMODULE;
#endif

struct Plugin {
  Plugin(std::filesystem::path filepath);
  ~Plugin();

  std::filesystem::path filepath;
  PluginHandle data = nullptr;

  std::string pluginName = "";

  void * loadFunction(char const * label, bool shouldError);
  void reload();
  void close();
  void open();
};

// list of unique plugins currently loaded by process
//  operations like `dlopen` gauruntee to only open one dylib per-process
std::vector<std::unique_ptr<Plugin>> plugins; // <<<<< important <<<<<

Plugin::Plugin(std::filesystem::path filepath_)
  : filepath(filepath_)
{
  this->open();
}

Plugin::~Plugin() {
  this->close();
}

void * Plugin::loadFunction(char const * label, bool shouldError) {
  if (label == 0) {
    puleLogError("trying to load function with null label");
    return nullptr;
  }
  void * fn = nullptr;
  #if defined(__unix__) || defined(__APPLE__)
    fn = ::dlsym(this->data, label);
    if (!fn && shouldError) {
      puleLogError(
        "failed to load function '%s' for plugin '%s'", label, ::dlerror()
      );
    }
  #elif defined(_WIN32) || defined(_WIN64)
    fn = reinterpret_cast<void *>(::GetProcAddress(this->data, label));
    if (!fn && shouldError) {
      puleLogError(
        "failed to load function '%s' for plugin '%s'",
        label, ::GetLastError()
      );
    }
  #endif
  return fn;
}

void Plugin::reload() {
  this->close();
  this->open();
}

void Plugin::close() {
  if (!this->data) { return; }
  #if defined(__unix__) || defined(__APPLE__)
    //puleLogDebug("closing plugin %p %s", this->data, this->filepath.c_str());
    if (::dlclose(this->data)) {
      puleLogError(
        "failed to close plugin '%s': %s",
        this->filepath.c_str(),
        ::dlerror()
      );
    }
  #elif defined(_WIN32) || defined(_WIN64)
    if (::FreeLibrary(this->data)) {
      puleLogError(
        "failed to load plugin '%s'; %s"
      , this->filepath.c_str(), ::GetLastError()
      );
    }
  #endif
  this->data = nullptr;
}

void Plugin::open() {
  #if defined(__unix__) || defined(__APPLE__)
    this->data = ::dlopen(this->filepath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!this->data) {
      puleLogError(
        "failed to load plugin '%s'; %s",
        this->filepath.c_str(),
        ::dlerror()
      );
    }
  #elif defined(_WIN32) || defined(_WIN64)
    this->data = ::LoadLibraryW(this->filepath.c_str());
    if (!this->data) {
      puleLogError(
        "failed to load plugin '%s'; %s"
      , this->filepath.c_str(), ::GetLastError()
      );
    }
  #endif

  // grab name
  {
    for (
      char const * pluginNameIt = this->filepath.c_str();
      pluginNameIt[0] != '\0';
      ++ pluginNameIt
    ) {
      if (
            (pluginNameIt[0] == '/' || pluginNameIt[0] == '\\')
         && pluginNameIt[1] == 'l'
         && pluginNameIt[2] == 'i'
         && pluginNameIt[3] == 'b'
      ) {
        this->pluginName = std::string{pluginNameIt+4};
        this->pluginName = (
          this->pluginName.substr(
            0,
            this->pluginName.size()-3 // .so
          )
        );
        // don't break at end in cases like 'lib/libplugin-foo.so'
      }
    }
    if (this->pluginName == "") {
      puleLogError(
        "failed to get the name of plugin from '%s'",
        this->filepath.c_str()
      );
    }
  }

  /* puleLogDebug( */
  /*   "opened '%s' @ %p | path %s", */
  /*   this->pluginName.c_str(), */
  /*   this->data, this->filepath.c_str() */
  /* ); */

  // check if handle already exists in plugins, as each plugin must be unique
  for (auto & plugin : ::plugins) {
    if (&*plugin == this) { continue; }
    if (plugin->data == this->data) {
      /* puleLogDebug("plugin %s already loaded", this->filepath.c_str()); */
      break;
    }
  }
}

void loadPluginFromFile(std::filesystem::path path) {
  puleLogDebug("Loading plugin %s", path.c_str());
  ::plugins.emplace_back(std::make_unique<::Plugin>(path));
  auto & pluginEnd = ::plugins.back();

  // check plugin loaded
  if (!pluginEnd->data) {
    ::plugins.pop_back();
    puleLogError(
      "shared object file %s could not load correctly",
      path.c_str()
    );
  }
}

void loadPluginsFromDirectory(char const * const label) {
  for (
    auto const & directoryEntry
    : std::filesystem::recursive_directory_iterator(label)
  ) {
    if (!directoryEntry.is_regular_file()) {
      continue;
    }

    loadPluginFromFile(directoryEntry.path());
  }
}

} // -- anon namespace

#include <filesystem>

extern "C" {

void pulePluginsLoad(
  PuleStringView const * const paths,
  size_t const pathsLength
) {
  for (size_t it = 0; it < pathsLength; ++ it) {
    loadPluginsFromDirectory(paths[it].contents);
  }
}

void pulePluginsFree() {
  ::plugins.clear();
}

void pulePluginsReload() {
  for (auto & pluginIt : ::plugins) {
    pluginIt->reload();
  }
}

size_t pulePluginIdFromName(char const * const pluginNameCStr) {
  size_t it = 0;
  for (auto & pluginIt : ::plugins) {
    if (strcmp(pluginIt->pluginName.c_str(), pluginNameCStr) != 0){
      ++ it;
      continue;
    }

    return it;
  }

  ::loadPluginFromFile(
    std::filesystem::path(
      "plugins/lib" + std::string{pluginNameCStr} + ".so"
    )
  );
  return ::plugins.size()-1;
}

char const * pulePluginName(size_t const pluginId) {
  return ::plugins[pluginId]->pluginName.c_str();
}

void * pulePluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
) {
  return ::plugins[pluginId]->loadFunction(fnCStr, true);
}

void * pulePluginLoadFnTry(
  size_t const pluginId,
  char const * const fnCStr
) {
  return ::plugins[pluginId]->loadFunction(fnCStr, false);
}

void pulePluginIterate(
  void (*fn)(PulePluginInfo const, void * const userdata),
  void * userdata
) {
  size_t pluginIt = 0;
  for (auto const & plugin : ::plugins) {
    fn(
      PulePluginInfo {
        .name = plugin->pluginName.c_str(),
        .id = pluginIt,
      },
      userdata
    );
    ++ pluginIt;
  }
}

} // extern c

namespace {

void * loadSymbol(
  char const * name, PuleStringView const layer,
  bool canLoadDefault = true
) {
  if (!canLoadDefault && puleStringViewEqCStr(layer, "default")) {
    return nullptr;
  }
  // transform layer to plugin name relative to layer
  // ex, 'pulchritude-debug'
  // ex, 'log' -> 'pulchritude-log' or 'pulchritude-log-debug'
  auto const layerStr = std::string(layer.contents);
  auto const correctModuleLabel = std::string("pulchritude-layer-") + layerStr;

  // fetch , TODO use unordered map or something to do direct lookup
  if (!puleStringViewEqCStr(layer, "default")) {
    for (auto & plugin : plugins) {
      if (plugin->pluginName == correctModuleLabel) {
        auto fn = plugin->loadFunction(name, false);
        if (fn) { return fn; }
        break;
      }
    }

    puleLogDebug(
      "symbol '%s' does not exist in module (%s)%s",
      name, correctModuleLabel.c_str(),
      canLoadDefault ? ", loading from default" : ""
    );
  }

  if (!canLoadDefault) {
    return nullptr;
  }

  // fetch from default libraries (ignore layers)
  for (auto & plugin : plugins) {
    if (
      puleStringViewContains(
        puleCStr(plugin->pluginName.c_str()),
        puleCStr("pulchritude-layer")
      )
    ) {
      continue;
    }
    auto fn = plugin->loadFunction(name, false);
    if (fn) { return fn; }
  }

  puleLogDebug(
    "could not load symbol '%s' from layer %s",
    name, layer.contents
  );
  return nullptr;
}

} // namespace

extern "C" {

void pulePluginLoadEngineLayer(
  PuleEngineLayer * layer,
  PuleStringView const layerName,
  PuleEngineLayer * layerParentNullable
) {
  layer->parent = layerParentNullable;
  layer->layerName = puleString(puleAllocateDefault(), layerName.contents);
#include "engine-loader-mixin.inl"
  auto layerEntryFn = (
    reinterpret_cast<void (*)(PuleEngineLayer * const layer)>(
      ::loadSymbol("puleEngineLayerEntry", layerName) // TODO chop last
    )
  );
  PULE_assert(
    (!layerParentNullable || layerEntryFn) && "missing layer entry function"
  );
  if (layerEntryFn) {
    puleLogDebug("Loading layer entry function");
    assert(layerParentNullable); // should have parent (as this is not default)
    layerEntryFn(layerParentNullable);
  }
}

} // extern c

// ybbxvat va qrrc qbja
// ng nyy gung'f tbar
// gur cnva lbh srry tbrf ba naq ba
// pybfr lbhe rlrf naq snyy
// ornhgvshy snyy ornhgvshy raq
