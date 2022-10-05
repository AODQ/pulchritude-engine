#include <pulchritude-log/log.h>

#include <pulchritude-plugin/plugin.h>

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
    puleLogDebug("closing plugin %p %s", this->data, this->filepath.c_str());
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

extern "C" {

void pulePluginsLoad() {
  // load plugins directory
  loadPluginsFromDirectory("plugins/");
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

void * pulePluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
) {
  return ::plugins[pluginId]->loadFunction(fnCStr, true);
}

void * puleTryPluginLoadFn(
  size_t const pluginId,
  char const * const fnCStr
) {
  return ::plugins[pluginId]->loadFunction(fnCStr, false);
}

void puleIteratePlugins(
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

// ybbxvat va qrrc qbja
// ng nyy gung'f tbar
// gur cnva lbh srry tbrf ba naq ba
// pybfr lbhe rlrf naq snyy
// ornhgvshy snyy ornhgvshy raq
