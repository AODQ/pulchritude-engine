/* pulchritude engine | github.com/aodq/pulchritude-engine | aodq.net */

// pybfr lbhe rlrf naq erzrzore jung'f ybpxrq njnl

#include <pulchritude/asset-pds.h>
#include <pulchritude/asset-render-graph.h>
#include <pulchritude/asset-script-task-graph.h>
#include <pulchritude/asset-shader-module.h>
#include <pulchritude/camera.h>
#include <pulchritude/gpu.h>
#include <pulchritude/imgui-engine.h>
#include <pulchritude/imgui.h>
#include <pulchritude/log.h>
#include <pulchritude/platform.h>
#include <pulchritude/plugin.h>
#include <pulchritude/script.h>
#include <pulchritude/string.hpp>
#include <pulchritude/task-graph.h>

#include <string>
#include <unordered_map>
#include <vector>

// loading plugins
namespace {

struct PluginInfo {
  size_t id;
  std::string name;
  std::vector<std::string> sourceFiles;
  bool (*updateFn)(PulePluginPayload const);
  bool allowLiveReload;
};

template <typename T>
void loadFn(T & fn, size_t const pluginId, char const * const label) {
  fn = reinterpret_cast<T>(pulePluginLoadFn(pluginId, label));
}

template <typename T>
void tryLoadFn(T & fn, size_t const pluginId, char const * const label) {
  fn = reinterpret_cast<T>(pulePluginLoadFnTry(pluginId, label));
}

struct IteratePluginsInfo {
  std::vector<PluginInfo> & plugins;
  PuleDsValue pluginArrayValue;
};
void iteratePlugins(PulePluginInfo const puPlugin, void * const userdata) {
  PulePluginType (*pluginTypeFn)() = nullptr;
  auto & iteratePluginInfo = (
    *reinterpret_cast<IteratePluginsInfo *>(userdata)
  );
  auto pluginArray = puleDsAsArray(iteratePluginInfo.pluginArrayValue);
  tryLoadFn(pluginTypeFn, puPlugin.id, "pulcPluginType");
  if (!pluginTypeFn) { return; }
  // store plugin
  PluginInfo plugin = {};
  plugin.id = puPlugin.id;
  // check if updatable
  ::tryLoadFn(plugin.updateFn, plugin.id, "pulcComponentUpdate");
  // check if live reloadable
  bool (*componentAllowLiveReloadFn)() = nullptr;
  ::tryLoadFn(
    componentAllowLiveReloadFn, plugin.id, "pulcPluginAllowLiveReload"
  );
  plugin.allowLiveReload = (
       (componentAllowLiveReloadFn && componentAllowLiveReloadFn())
    || plugin.updateFn
  );
  // need to find all source files that can be watched
  PuleDsValue pluginSourceFilesValue = { .id = 0, };
  char const * pluginName = pulePluginName(plugin.id);
  for (size_t it = 0; it < pluginArray.length; ++ it) {
    PuleStringView const pluginItName = (
      puleDsMemberAsString(pluginArray.values[it], "name")
    );
    if (puleStringViewEqCStr(pluginItName, pluginName)) {
      pluginSourceFilesValue = (
        puleDsObjectMember(pluginArray.values[it], "known-files")
      );
      break;
    }
  }
  // store name
  plugin.name = pluginName;
  // store source files
  if (pluginSourceFilesValue.id != 0) {
    PuleDsValueArray const pluginSourceFiles = (
      puleDsAsArray(pluginSourceFilesValue)
    );
    for (size_t it = 0; it < pluginSourceFiles.length; ++ it) {
      plugin.sourceFiles.push_back(
        puleDsAsString(pluginSourceFiles.values[it]).contents
      );
    }
  }
  iteratePluginInfo.plugins.push_back(plugin);
}

struct PluginReloadInfo {
  PulePluginPayload payload;
  std::vector<PluginInfo> * plugins;
};
void componentPluginFileWatchUpdate(PuleStringView const, void * const ud) {
  auto & info = *reinterpret_cast<PluginReloadInfo *>(ud);
  pulePluginsReload();
  for (auto & plugin : *info.plugins) {
    puleLog("updating plugin %s", plugin.name.c_str());
    plugin.updateFn = nullptr;
    ::tryLoadFn(plugin.updateFn, plugin.id, "pulcComponentUpdate");
    void (*componentReloadFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentReloadFn, plugin.id, "pulcComponentReload");
    if (componentReloadFn) {
      componentReloadFn(info.payload);
    }
  }
  puleLog("reload finished");
}

//struct PluginsCollectRenderGraphInfo {
//  PuleRenderGraph renderGraph;
//  PulePlatform platform;
//};
//void iteratePluginsCollectRenderGraphs(
//  PulePluginInfo const plugin,
//  void * const userdata
//) {
//  auto const info = (
//    *reinterpret_cast<PluginsCollectRenderGraphInfo *>(userdata)
//  );
//  PuleRenderGraph (*renderGraphFn)(PulePlatform const plaftform) = nullptr;
//  tryLoadFn(renderGraphFn, plugin.id, "pulcRenderGraph");
//  if (renderGraphFn) {
//    puleLogDebug("found pulcRenderGraph in plugin %s", plugin.name);
//    puleRenderGraphMerge(info.renderGraph, renderGraphFn(info.platform));
//  }
//}

}

namespace { // -- script task graph callbacks ----------------------------------
struct ScriptTaskGraphNodeUpdateInfo {
  PuleScriptContext scriptContext;
};
void scriptTaskGraphNodeUpdate(
  PuleTaskGraphNode const node,
  void * const userdata
) {
  auto info = *static_cast<ScriptTaskGraphNodeUpdateInfo *>(userdata);
  // fetch script module
  auto const scriptModule = PuleScriptModule {
    puleTaskGraphNodeAttributeFetchU64(node, "script-module"_psv)
  };
  PuleError err = puleError();
  puleScriptModuleExecute(
    info.scriptContext,
    scriptModule,
    puleTaskGraphNodeLabel(node),
    &err
  );
  puleErrorConsume(&err);
}
} // -- script task graph callbacks --------------------------------------------

namespace { // -- render task graph callbacks ----------------------------------

} // -- render task graph callbacks --------------------------------------------

// update components
namespace {
}

struct ParameterInfo {
  std::string label;
  std::string content;
};

struct KnownParameterInfo {
  bool hasParameter = false;
};

namespace { // editor
using GuiEditorFn = void (*)(
  PuleAllocator const, PulePlatform const
);

void guiPluginLoad(PulePluginInfo const plugin, void * const userdata) {
  auto & plugins = (
    *reinterpret_cast<std::vector<GuiEditorFn> *>(userdata)
  );
  GuiEditorFn const guiEditorFn = (
    reinterpret_cast<GuiEditorFn>(
      pulePluginLoadFnTry(plugin.id, "puldGuiEditor")
    )
  );
  if (guiEditorFn) {
    plugins.push_back(guiEditorFn);
  }
}
} // namespace editor

std::unordered_map<std::string, KnownParameterInfo> knownProgramParameters = {
  { "--asset-path",          {.hasParameter = true,  } },
  { "--gui-editor",          {.hasParameter = false, } },
  { "--debug",               {.hasParameter = false, } },
  { "--error-segfaults",     {.hasParameter = false, } },
  { "--plugin-layer",        {.hasParameter = true,  } },
  { "--plugin-path",         {.hasParameter = true,  } },
  { "--early-exit",          {.hasParameter = false, } },
  { "--allow-plugin-reload", {.hasParameter = false, } },
};
std::vector<ParameterInfo> programParameters;

PuleAssetShaderModule loadShaderModule(
  PuleDsValue const dsShaderModule,
  PuleStringView const assetPath
) {
  PuleStringView const shaderModuleLabel = (
    puleDsMemberAsString(dsShaderModule, "label")
  );
  std::string const vertexPath = (
    std::string(assetPath.contents)
    + (
      puleDsMemberAsString(
        dsShaderModule, "vertex"
      ).contents
    )
  );
  std::string const fragmentPath = (
    std::string(assetPath.contents)
    + (
      puleDsMemberAsString(
        dsShaderModule, "fragment"
      ).contents
    )
  );

  return (
    puleAssetShaderModuleCreateFromPaths(
      shaderModuleLabel,
      puleCStr(vertexPath.c_str()),
      puleCStr(fragmentPath.c_str())
    )
  );
}

int32_t main(
  [[maybe_unused]] int32_t const argumentLength,
  [[maybe_unused]] char const * const * const arguments
) {
  //-* parse parameters/pds files -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  // parse into parameters
  bool parseVarargs = false;
  std::vector<std::string> userVarargs;
  for (int32_t argumentIt = 1; argumentIt < argumentLength; ++ argumentIt) {
    auto const label = std::string(arguments[argumentIt]);
    std::string content = "";
    if (parseVarargs) {
      userVarargs.push_back(label);
      continue;
    }
    // this breaks into application parameters
    if (label == "--") {
      parseVarargs = true;
      continue;
    }
    auto knownInfo = knownProgramParameters.find(label);
    if (knownInfo == knownProgramParameters.end()) {
      puleLogError("[PuleApplication] unknown parameter '%s'", label.c_str());
      return 1;
    }
    if (knownInfo->second.hasParameter) {
      if (argumentIt+1 >= argumentLength) {
        puleLogError(
          "[PuleApplication] Expecting parameter for '%s",
          label.c_str()
        );
        return 1;
      }
      content = std::string(arguments[argumentIt+1]);
      argumentIt += 1;
    }
    programParameters.push_back(ParameterInfo {
      .label = label,
      .content = content,
    });
  }
  userVarargs.push_back(""); // null terminator for array

  bool isGuiEditor = false;
  bool isEarlyExit = false;
  bool allowPluginReload = false;

  std::vector<PuleStringView> pluginPaths;
  PuleString assetPath;
  for (auto const & param : programParameters) {
    if (param.label == "--plugin-path") {
      pluginPaths.push_back(puleCStr(param.content.c_str()));
    } else if (param.label == "--asset-path") {
      assetPath = (
        puleStringCopy(puleAllocateDefault(), puleCStr(param.content.c_str()))
      );
    } else if (param.label == "--debug") {
      *puleLogDebugEnabled() = true;
      puleLogDebug("[PuleApplication] Debug enabled");
    } else if (param.label == "--error-segfaults") {
      *puleLogErrorSegfaultsEnabled() = true;
      puleLogDebug("[PuleApplication] Segfault on error enabled");
    } else if (param.label == "--gui-editor") {
      isGuiEditor = true;
    } else if (param.label == "--early-exit") {
      isEarlyExit = true;
    } else if (param.label == "--allow-plugin-reload") {
      allowPluginReload = true;
    }
  }

  if (assetPath.len == 0) {
    puleLogError("[PuleApplication] No asset path specified");
    return -1;
  }

  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //              _____  _____   ___  ______  _____  _   _ ______              *
  //             /  ___||_   _| / _ \ | ___ \|_   _|| | | || ___ \             *
  //             \ `--.   | |  / /_\ \| |_/ /  | |  | | | || |_/ /             *
  //              `--. \  | |  |  _  ||    /   | |  | | | ||  __/              *
  //             /\__/ /  | |  | | | || |\ \   | |  | |_| || |                 *
  //             \____/   \_/  \_| |_/\_| \_|  \_/   \___/ \_|                 *
  //                                                                           *
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

  // load plugins
  pulePluginsLoad(pluginPaths.data(), pluginPaths.size());

  PulePluginPayload const payload = (
    pulePluginPayloadCreate(puleAllocateDefault())
  );
  // store asset path and set it to engine's filesystem
  pulePluginPayloadStore(payload, "pule-fs-asset-path"_psv, &assetPath);
  puleFilesystemAssetPathSet(puleStringView(assetPath));

  // store application varargs
  pulePluginPayloadStore(
    payload, "pule-application-varargs"_psv, userVarargs.data()
  );

  // load up project configuration
  PuleError err = puleError();
  PuleDsValue const projectValue = (
    puleAssetPdsLoadFromFile(
      puleAllocateDefault(),
      "/assets/project.pds"_psv,
      &err
    )
  );
  if (puleErrorConsume(&err)) { return -1; }

  // get the component plugins, also getting the slice that can be updated
  std::vector<PluginInfo> componentPlugins;
  auto iteratePluginInfo = IteratePluginsInfo {
    .plugins = componentPlugins,
    .pluginArrayValue = (
      puleDsObjectMember(
        puleDsObjectMember(projectValue, "build-info"),
        "plugins"
      )
    )
  };
  pulePluginIterate(
    ::iteratePlugins,
    reinterpret_cast<void *>(&iteratePluginInfo)
  );
  std::vector<PluginInfo> componentUpdateablePlugins;
  for (size_t it = 0; it < componentPlugins.size(); ++ it) {
    if (componentPlugins[it].updateFn) {
      componentUpdateablePlugins.push_back(componentPlugins[it]);
    }
  }


  // setup plugin reload (just for any updateable), by watching files
  PluginReloadInfo stackAllocPluginReloadInfo;
  if (allowPluginReload) {
    stackAllocPluginReloadInfo = PluginReloadInfo {
      .payload = payload,
      .plugins = &componentPlugins,
    };
    std::string semicolonSeparatedPluginPaths = "";
    for (auto & plugin : componentPlugins) {
      if (!plugin.allowLiveReload) { continue; }
      semicolonSeparatedPluginPaths += (
      #if defined(__UNIX__)
        "plugins/lib" + plugin.name + ".so;"
      #else
        "plugins/lib" + plugin.name + "lib;"
      #endif
      );
    }
    puleFileWatch({
      .fileUpdatedCallback = ::componentPluginFileWatchUpdate,
      .filename = puleCStr(semicolonSeparatedPluginPaths.c_str()),
      .waitTime = { .valueMilli = 250, },
      .checkMd5Sum = true,
      .userdata = (void *)&stackAllocPluginReloadInfo,
    });
  }

  PulePlatform platform = { .id = 0, };
  PuleScriptContext scriptContext = { .id = 0, };
  bool fileWatcherCheckAll = false;
  PuleEcsWorld ecsWorld = { .id = 0, };
  PuleRenderGraph renderGraph = { .id = 0, };
  PuleTaskGraph scriptTaskGraph = { .id = 0, };
  PuleCameraSet cameraSet = { .id = 0, };
  std::vector<PuleAssetShaderModule> shaderModules = {};
  bool ecsWorldAdvance = false;

  // TODO break these out into functions
  { // -- load project.pds, use base layer to allow them to capture startup
    // -- entry payload
    PuleDsValue const entryPayload = (
      puleDsObjectMember(projectValue, "entry-payload")
    );
    if (!puleDsIsNull(entryPayload)) {
      // -- platform create
      PuleDsValue const payloadPlatform = (
        puleDsObjectMember(entryPayload, "platform")
      );
      if (!puleDsIsNull(payloadPlatform)) {
        puleLogDebug("[PuleApplication] creating platform");
        size_t const defaultWidth = 800; // TODO get from window?
        size_t const defaultHeight = 600;
        PuleStringView const defaultName = "pulchritude app"_psv;
        size_t const width = (
          puleDsMemberAsI64(payloadPlatform, "width") ?: defaultWidth
        );
        size_t const height = (
          puleDsMemberAsI64(payloadPlatform, "height") ?: defaultHeight
        );
        PuleStringView name = (
          puleDsMemberAsString(payloadPlatform, "name")
        );
        name = (name.len == 0 ? name : defaultName);
        PulePlatformVsyncMode vsyncMode = (
          PulePlatformVsyncMode_double
          // TODO... puleDsMemberAsString */
        );
        pulePlatformInitialize(&err);
        if (puleErrorConsume(&err)) { return -1; }
        platform = (
          pulePlatformCreate(
            PulePlatformCreateInfo {
              .name = name,
              .width = width, .height = height,
              .vsyncMode = vsyncMode,
            },
            &err
          )
        );
        if (puleErrorConsume(&err)) { return -1; }
      }
      // -- graphics create
      PuleDsValue const payloadGfx = (
        puleDsObjectMember(entryPayload, "gfx")
      );
      if (!puleDsIsNull(payloadGfx)) {
        puleLogDebug("[PuleApplication] creating graphics context");
        if (puleDsMemberAsBool(payloadGfx, "initialize")) {
          if (platform.id == 0) {
            puleLogError(
              "Trying to initialize gfx context without platform"
            );
            return -1;
          }
          puleGpuInitialize(platform, &err);
          if (puleErrorConsume(&err)) { return -1; }
        }
        auto renderGraphPath = (
          puleDsObjectMember(payloadGfx, "render-graph-path")
        );
        if (!puleDsIsNull(renderGraphPath)) {
          puleLogDebug("[PuleApplication] creating render graph");
          auto renderGraphValue = (
            puleAssetPdsLoadFromFile(
              puleAllocateDefault(),
              puleCStr(
                (
                  std::string(assetPath.contents)
                  + puleDsAsString(renderGraphPath).contents
                ).c_str()
              ),
              &err
            )
          );
          if (puleErrorConsume(&err)) { return -1; }
          renderGraph = (
            puleAssetRenderGraphFromPds(
              puleAllocateDefault(),
              platform,
              renderGraphValue
            )
          );
          puleDsDestroy(renderGraphValue);
        }
        auto generateWindowDepthFramebuffer = (
          puleDsObjectMember(
            payloadGfx,
            "generate-window-depth-framebuffer"
          )
        );
        (void)generateWindowDepthFramebuffer;
        // load shader modules
        PuleDsValueArray const dsShaderModules = (
          puleDsMemberAsArray(payloadGfx, "shader-modules")
        );
        for (size_t it = 0; it < dsShaderModules.length; ++ it) {
          PuleDsValue const dsShaderModule = dsShaderModules.values[it];
          shaderModules.push_back(
            loadShaderModule(dsShaderModule, puleStringView(assetPath))
          );
        }
      }
      // -- ecs create
      PuleDsValue const payloadEcs = (
        puleDsObjectMember(entryPayload, "ecs")
      );
      if (
        !puleDsIsNull(payloadEcs)
        && puleDsMemberAsBool(payloadEcs, "create-world")
      ) {
        puleLogDebug("[PuleApplication] creating ECS");
        ecsWorld = puleEcsWorldCreate();
        ecsWorldAdvance = puleDsMemberAsBool(payloadEcs, "world-advance");
        // -- load in ECS components & systems from plugins
        bool const loadComponent = (
          puleDsMemberAsBool(payloadEcs, "register-components")
        );
        bool const loadSystem = (
          puleDsMemberAsBool(payloadEcs, "register-systems")
        );
        for (auto const & componentPlugin : componentPlugins) {
          void (*registerComponentsFn)(PuleEcsWorld const) = nullptr;
          void (*registerSystemsFn)(PuleEcsWorld const) = nullptr;
          if (loadComponent) {
            ::tryLoadFn(
              registerComponentsFn,
              componentPlugin.id,
              "pulcRegisterComponents"
            );
            if (registerComponentsFn) {
              registerComponentsFn(ecsWorld);
            }
          }
          if (loadSystem) {
            ::tryLoadFn(
              registerSystemsFn,
              componentPlugin.id,
              "pulcRegisterSystems"
            );
            if (registerComponentsFn) {
              registerSystemsFn(ecsWorld);
            }
          }
        }
      }
      // -- file watcher
      PuleDsValue const payloadFile = (
        puleDsObjectMember(entryPayload, "file")
      );
      if (
        !puleDsIsNull(payloadFile)
        && puleDsMemberAsBool(payloadFile, "watcher-check-all")
      ) {
        fileWatcherCheckAll = true;
      }
      // -- script create
      PuleDsValue const payloadScript = (
        puleDsObjectMember(entryPayload, "script")
      );
      puleLogDebug("[PuleApplication] initializing script");
      if (
        !puleDsIsNull(payloadScript)
        && puleDsMemberAsBool(payloadScript, "initialize")
      ) {
        scriptContext = puleScriptContextCreate();
        // script graph path
        auto scriptGraphPath = (
          puleDsObjectMember(payloadScript, "script-graph-path")
        );
        if (!puleDsIsNull(scriptGraphPath)) {
          puleLogDebug("[PuleApplication] creating script graph");
          auto scriptGraphValue = (
            puleAssetPdsLoadFromFile(
              puleAllocateDefault(),
              puleCStr(
                (
                  std::string(assetPath.contents)
                  + puleDsAsString(scriptGraphPath).contents
                ).c_str()
              ),
              &err
            )
          );
          if (puleErrorConsume(&err)) { return -1; }
          scriptTaskGraph = (
            puleAssetScriptTaskGraphFromPds(
              puleAllocateDefault(),
              scriptContext,
              scriptGraphValue,
              puleStringView(assetPath)
            )
          );
          puleDsDestroy(scriptGraphValue);
        }
      }
      // -- camera create
      PuleDsValue const payloadCamera = (
        puleDsObjectMember(entryPayload, "camera-set")
      );
      if (!puleDsIsNull(payloadCamera)) {
        puleLogDebug("[PuleApplication] creating camera");
        cameraSet = puleCameraSetCreate("pule-primary"_psv);
      }
    }
  }

  // create defaults
  //if (renderGraph.id == 0 && platform.id != 0) {
  //  renderGraph = puleRenderGraphCreate(puleAllocateDefault());
  //}

  // insert into payload as necessary
  if (platform.id != 0) {
    pulePluginPayloadStoreU64(
      payload,
      "pule-platform"_psv,
      platform.id
    );
  }
  if (ecsWorld.id != 0) {
    pulePluginPayloadStoreU64(
      payload,
      "pule-ecs-world"_psv,
      ecsWorld.id
    );
  }
  if (renderGraph.id != 0) {
    pulePluginPayloadStoreU64(
      payload,
      "pule-render-graph"_psv,
      renderGraph.id
    );
  }
  if (scriptTaskGraph.id != 0) {
    pulePluginPayloadStoreU64(
      payload,
      "pule-script-task-graph"_psv,
      scriptTaskGraph.id
    );
  }
  if (cameraSet.id != 0) {
    pulePluginPayloadStoreU64(
      payload,
      "pule-camera-set"_psv,
      cameraSet.id
    );
  }
  for (auto shaderModule : shaderModules) {
    std::string const shaderModuleLabel = (
      std::string("pule-asset-shader-module-")
      + puleAssetShaderModuleLabel(shaderModule).contents
    );
    puleLogDebug(
      "[PuleApplication] storing shader module %d at payload label '%s'",
      puleAssetShaderModuleHandle(shaderModule).id,
      shaderModuleLabel.c_str()
    );
    pulePluginPayloadStoreU64(
      payload,
      puleCStr(shaderModuleLabel.c_str()),
      shaderModule.id
    );
  }

  // initiate all plugins with pulcComponentLoad
  puleLogDebug("[PuleApplication] initializing plugins");
  for (auto const & componentPlugin : componentPlugins) {
    //if (isGuiEditor) { break; } is this necessaary?
    // try to load component
    void (*componentLoadFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentLoadFn, componentPlugin.id, "pulcComponentLoad");
    puleLogDebug(
      "[PuleApplication] Loading plugin %s",
      pulePluginName(componentPlugin.id)
    );
    if (componentLoadFn) {
      puleLogDebug(
        "[PuleApplication] Loading function for plugin %d",
        componentPlugin.id
      );;
      componentLoadFn(payload);
    }
  }

  //{ // check if any plugin contributes to render task graph
  //  auto renderGraphInfo = PluginsCollectRenderGraphInfo {
  //    .renderGraph = renderGraph,
  //    .platform = platform,
  //  };
  //  pulePluginIterate(
  //    ::iteratePluginsCollectRenderGraphs, &renderGraphInfo
  //  );
  //}

  // load gui editor if requested
  PULE_assert(isGuiEditor ? platform.id : true);
  std::vector<GuiEditorFn> guiEditorFns;
  if (isGuiEditor) {
    pulePluginIterate(
      ::guiPluginLoad,
      reinterpret_cast<void *>(&guiEditorFns)
    );
    puleLog(
      "[PuleApplication] Loaded %u gui editor functions",
      guiEditorFns.size()
    );
    puleLog("Initializing imgui...");
    puleImguiInitialize(platform);
  }

  // get the render task graphs from plugins

  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //                         _      _____  _____ ______                        *
  //                        | |    |  _  ||  _  || ___ \                       *
  //                        | |    | | | || | | || |_/ /                       *
  //                        | |    | | | || | | ||  __/                        *
  //                        | |____\ \_/ /\ \_/ /| |                           *
  //                        \_____/ \___/  \___/ \_|                           *
  //                                                                           *
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  bool hasUpdate = componentUpdateablePlugins.size() > 0 || isGuiEditor;
  PuleGpuSemaphore swapchainAvailableSemaphore = { .id = 0, };
  while (hasUpdate) {
    if (isEarlyExit) {
      puleLog("<--> frame start <-->");
    }
    if (platform.id) {
      swapchainAvailableSemaphore = puleGpuFrameStart();
      pulePlatformPollEvents(platform);
    }
    if (renderGraph.id != 0) {
      puleRenderGraphFrameStart(renderGraph);
    }
    if (isGuiEditor) {
      puleImguiNewFrame();
    }
    if (cameraSet.id != 0) {
      puleCameraControllerPollEvents();
      puleCameraSetRefresh(cameraSet);
    }
    if (scriptTaskGraph.id != 0) {
      auto info = ScriptTaskGraphNodeUpdateInfo {
        .scriptContext = scriptContext,
      };
      puleTaskGraphExecuteInOrder(PuleTaskGraphExecuteInfo {
        .graph = scriptTaskGraph,
        .callback = &scriptTaskGraphNodeUpdate,
        .userdata = static_cast<void *>(&info),
        .multithreaded = false,
      });
    }
    if (!isGuiEditor && ecsWorldAdvance) {
      puleEcsWorldAdvance(ecsWorld, 16.0f);
    }
    if (isGuiEditor) {
      // pulchritude engine display
      puleImguiEngineDisplay(PuleImguiEngineDisplayInfo {
        .world = ecsWorld,
        .platform = platform,
      });
      // plugin provided functions
      for (auto const guiFn : guiEditorFns) {
        guiFn(puleAllocateDefault(), platform);
      }
      // render out
      puleImguiRender(renderGraph);
    } else {
      // update components
      for (size_t it = 0; it < componentUpdateablePlugins.size(); ++ it) {
        componentUpdateablePlugins.at(it).updateFn(payload);
      }
    }
    if (renderGraph.id != 0) {
      // TODO how to handle multiple render graphs? Should I just limit
      // to one per scene? And only one scene is active at a time?
      // You can still have recursive render graphs since they can
      // be merged together.
      puleRenderGraphFrameSubmit(
        swapchainAvailableSemaphore,
        renderGraph
      );
    }
    if (platform.id != 0) {
      pulePlatformSwapFramebuffer(platform);
    }
    if (fileWatcherCheckAll) {
      puleFileWatchCheckAll();
    }
    puleFileWatchCheckAll();
    if (isEarlyExit) {
      puleLog("<--> frame end <-->");
    }
    if (isEarlyExit) { // debug early exit
      static size_t frameCount = 0;
      if (++frameCount > 0) { break; }
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

  puleLogDebug(
    "[PuleApplication] Shutting down...",
    guiEditorFns.size()
  );

  // try to unload components
  for (auto const & componentPlugin : componentPlugins) {
    void (*componentUnloadFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentUnloadFn, componentPlugin.id, "pulcComponentUnload");
    if (componentUnloadFn) {
      componentUnloadFn(payload);
    }
  }

  puleStringDestroy(assetPath);
  pulePluginPayloadDestroy(payload);
  puleScriptContextDestroy(scriptContext);

  if (platform.id == 0) {
    pulePlatformDestroy(platform);
    puleEcsWorldDestroy(ecsWorld);
  }

  pulePluginsFree();
  puleLog("[PuleApplication] clean exit");
  puleLog("TODO PROPER SHUT DOWN IM JUST FORCE QUITTING");
  exit(0);
  return 0;
}

// jryy znlor guvf pbhyq or gur raqvat jvgu abguvat yrsg bs lbh
