/* pulzoad | puzzled engine | github.com/aodq/pulzoad-engine | aodq.net */
#include <pulzoad-plugin/plugin.h>

#include <cstdint>
#include <cstring>
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

  char const * (*pluginNameFn)();

  void * loadFunction(char const * label);
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

void * Plugin::loadFunction(char const * label) {
  void * fn = nullptr;
  #if defined(__unix__) || defined(__APPLE__)
    fn = ::dlsym(this->data, label);
    if (!fn) {
      printf(
        "Failed to load function '%s' for plugin '%s'\n", label, ::dlerror()
      );
    }
  #elif defined(_WIN32) || defined(_WIN64)
    fn = reinterpret_cast<void *>(::GetProcAddress(this->data, label));
    if (!fn) {
      printf(
        "Failed to load function '%s' for plugin '%s'\n",
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
    printf(
      "closing plugin %p %s\n",
      this->data, this->filepath.string().c_str()
    );
    if (::dlclose(this->data)) {
      printf(
        "Failed to close plugin '%s': %s\n",
        this->filepath.string().c_str(),
        ::dlerror()
      );
    }
  #elif defined(_WIN32) || defined(_WIN64)
    if (::FreeLibrary(this->data)) {
      printf(
        "Failed to load plugin '%s'; %s\n"
      , this->filepath.string(), ::GetLastError()
      );
    }
  #endif
  this->data = nullptr;
}

void Plugin::open() {
  #if defined(__unix__) || defined(__APPLE__)
    this->data = ::dlopen(this->filepath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    printf("opening %p : %s\n", this->data, this->filepath.string().c_str());
    if (!this->data) {
      printf(
        "Failed to load plugin '%s'; %s\n",
        this->filepath.string().c_str(),
        ::dlerror()
      );
    }
  #elif defined(_WIN32) || defined(_WIN64)
    this->data = ::LoadLibraryW(this->filepath.c_str());
    if (!this->data) {
      printf(
        "Failed to load plugin '%s'; %s\n"
      , this->filepath.string().c_str(), ::GetLastError()
      );
    }
  #endif

  // check if handle already exists in plugins, as each plugin must be unique
  for (auto & plugin : ::plugins) {
    if (&*plugin == this) { continue; }
    if (plugin->data == this->data) {
      printf("plugin %s already loaded\n", this->filepath.string().c_str());
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
    printf(
      "shared object file %s could not load correctly\n",
      path.string().c_str()
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
    if (!pluginIt->pluginNameFn) {
      pluginIt->pluginNameFn = (
        reinterpret_cast<char const *(*)()>(
          pluginIt->loadFunction("pluginLabel")
        )
      );
    }

    if (strcmp(pluginIt->pluginNameFn(), pluginNameCStr) != 0){
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
  return ::plugins[pluginId]->loadFunction(fnCStr);
}

} // extern c

// ybbxvat va qrrc qbja
// ng nyy gung'f tbar
// gur cnva lbh srry tbrf ba naq ba
// pybfr lbhe rlrf naq snyy
// ornhgvshy snyy ornhgvshy raq
