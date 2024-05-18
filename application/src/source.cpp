/* pulchritude engine | github.com/aodq/pulchritude-engine | aodq.net */

// pybfr lbhe rlrf naq erzrzore jung'f ybpxrq njnl

#include <pulchritude-asset/pds.h>
#include <pulchritude-log/log.h>
#include <pulchritude-plugin/engine.h>
#include <pulchritude-plugin/plugin.h>
#include <pulchritude-task-graph/task-graph.h>
#include <pulchritude-camera/camera.h>

// need struct definitions, but don't call anything from here directly
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-imgui-engine/imgui-engine.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-platform/platform.h>

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
      plugin.sourceFiles.emplace_back(
        puleDsAsString(pluginSourceFiles.values[it]).contents
      );
    }
  }
  iteratePluginInfo.plugins.emplace_back(plugin);
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
  PuleEngineLayer * pul;
  PuleScriptContext scriptContext;
};
void scriptTaskGraphNodeUpdate(
  PuleTaskGraphNode const node,
  void * const userdata
) {
  auto info = *static_cast<ScriptTaskGraphNodeUpdateInfo *>(userdata);
  auto & pul = *info.pul;
  // fetch script module
  auto const scriptModule = PuleScriptModule {
    pul.taskGraphNodeAttributeFetchU64(node, pul.cStr("script-module"))
  };
  PuleError err = pul.error();
  pul.scriptModuleExecute(
    info.scriptContext,
    scriptModule,
    pul.taskGraphNodeLabel(node),
    &err
  );
  pul.errorConsume(&err);
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
  PuleAllocator const, PulePlatform const, PuleEngineLayer const
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
    plugins.emplace_back(guiEditorFn);
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
  PuleEngineLayer & pul,
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
    pul.assetShaderModuleCreateFromPaths(
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
      userVarargs.emplace_back(label);
      continue;
    }
    // this breaks into application parameters
    if (label == "--") {
      puleLog("breaking out into application parameters");
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
    programParameters.emplace_back(ParameterInfo {
      .label = label,
      .content = content,
    });
  }
  userVarargs.emplace_back(""); // null terminator for array
  puleLog("user varargs:");
  for (auto & uv : userVarargs) {
    puleLog("\t>%s", uv.c_str());
  }

  bool isGuiEditor = false;
  bool isEarlyExit = false;
  bool allowPluginReload = false;

  std::vector<PuleStringView> pluginPaths;
  std::vector<PuleString> pluginLayers;
  PuleString assetPath;
  for (auto const & param : programParameters) {
    if (param.label == "--plugin-path") {
      pluginPaths.emplace_back(puleCStr(param.content.c_str()));
    } else if (param.label == "--asset-path") {
      assetPath = puleString(puleAllocateDefault(), param.content.c_str());
    } else if (param.label == "--debug") {
      *puleLogDebugEnabled() = true;
      puleLogDebug("[PuleApplication] Debug enabled");
    } else if (param.label == "--error-segfaults") {
      *puleLogErrorSegfaultsEnabled() = true;
      puleLogDebug("[PuleApplication] Segfault on error enabled");
    } else if (param.label == "--plugin-layer") {
      pluginLayers.emplace_back(
        puleString(puleAllocateDefault(), param.content.c_str())
      );
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

  // ensure default is picked
  if (
    pluginLayers.size() == 0
    || !puleStringViewEqCStr(puleStringView(pluginLayers.back()), "default")
  ) {
    pluginLayers.emplace_back(puleString(puleAllocateDefault(), "default"));
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

  // load layers, in reverse order (default->debug->etc)
  std::vector<PuleEngineLayer> layers;
  for (size_t layerIt = 0; layerIt < pluginLayers.size(); ++ layerIt) {
    auto pluginLayerName = pluginLayers[pluginLayers.size()-layerIt-1];
    puleLogDebug(
      "[PuleApplication] Loading plugin layer '%s'",
      pluginLayerName.contents
    );
    PuleEngineLayer engineLayer;
    memset(&engineLayer, 0, sizeof(PuleEngineLayer));
    pulePluginLoadEngineLayer(
      &engineLayer,
      puleStringView(pluginLayerName),
      layers.size() > 0 ? &layers.back() : nullptr
    );
    puleLogDebug("[PuleApplication] Finished loading plugin layers");
    layers.insert(layers.begin(), engineLayer);
  }
  assert(layers.size() > 0);
  PulePluginPayload const payload = (
    pulePluginPayloadCreate(puleAllocateDefault())
  );
  // store engine layer into payload
  pulePluginPayloadStore(payload, puleCStr("pule-engine-layer"), &layers[0]);
  PuleEngineLayer & pulBase = layers[0];

  // store asset path and set it to engine's filesystem
  pulePluginPayloadStore(payload, puleCStr("pule-fs-asset-path"), &assetPath);
  puleFilesystemAssetPathSet(puleStringView(assetPath));

  // store application varargs
  pulePluginPayloadStore(
    payload, puleCStr("pule-application-varargs"), userVarargs.data()
  );

  // load up project configuration
  PuleError err = puleError();
  PuleDsValue const projectValue = (
    layers[0].assetPdsLoadFromFile(
      layers[0].allocateDefault(),
      layers[0].cStr("/assets/project.pds"),
      &err
    )
  );
  if (layers[0].errorConsume(&err)) { return -1; }

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
    puleLog("found component plugin '%s'", componentPlugins[it].name.c_str());
    if (componentPlugins[it].updateFn) {
      componentUpdateablePlugins.emplace_back(componentPlugins[it]);
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
        "plugins/lib" + plugin.name + ".so;"
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

  // store layer
  puleLogDebug(
    "[PuleApplication] initializing from assets/project.pds with layer '%s'",
    pulBase.layerName.contents
  );

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
      pulBase.dsObjectMember(projectValue, "entry-payload")
    );
    if (!pulBase.dsIsNull(entryPayload)) {
      // -- platform create
      PuleDsValue const payloadPlatform = (
        pulBase.dsObjectMember(entryPayload, "platform")
      );
      if (!pulBase.dsIsNull(payloadPlatform)) {
        puleLogDebug("[PuleApplication] creating platform");
        size_t const defaultWidth = 800; // TODO get from window?
        size_t const defaultHeight = 600;
        PuleStringView const defaultName = pulBase.cStr("pulchritude app");
        size_t const width = (
          pulBase.dsMemberAsI64(payloadPlatform, "width") ?: defaultWidth
        );
        size_t const height = (
          pulBase.dsMemberAsI64(payloadPlatform, "height") ?: defaultHeight
        );
        PuleStringView name = (
          pulBase.dsMemberAsString(payloadPlatform, "name")
        );
        name = (name.len == 0 ? name : defaultName);
        PulePlatformVsyncMode vsyncMode = (
          PulePlatformVsyncMode_double
          // TODO... puleDsMemberAsString */
        );
        pulBase.platformInitialize(&err);
        if (pulBase.errorConsume(&err)) { return -1; }
        platform = (
          pulBase.platformCreate(
            PulePlatformCreateInfo {
              .name = name,
              .width = width, .height = height,
              .vsyncMode = vsyncMode,
            },
            &err
          )
        );
        if (pulBase.errorConsume(&err)) { return -1; }
      }
      // -- graphics create
      PuleDsValue const payloadGfx = (
        pulBase.dsObjectMember(entryPayload, "gfx")
      );
      if (!pulBase.dsIsNull(payloadGfx)) {
        puleLogDebug("[PuleApplication] creating graphics context");
        if (pulBase.dsMemberAsBool(payloadGfx, "initialize")) {
          if (platform.id == 0) {
            pulBase.logError(
              "Trying to initialize gfx context without platform"
            );
            return -1;
          }
          pulBase.gpuInitialize(platform, &err);
          if (pulBase.errorConsume(&err)) { return -1; }
        }
        auto renderGraphPath = (
          pulBase.dsObjectMember(payloadGfx, "render-graph-path")
        );
        if (!pulBase.dsIsNull(renderGraphPath)) {
          puleLogDebug("[PuleApplication] creating render graph");
          auto renderGraphValue = (
            pulBase.assetPdsLoadFromFile(
              pulBase.allocateDefault(),
              pulBase.cStr(
                (
                  std::string(assetPath.contents)
                  + pulBase.dsAsString(renderGraphPath).contents
                ).c_str()
              ),
              &err
            )
          );
          if (pulBase.errorConsume(&err)) { return -1; }
          renderGraph = (
            puleAssetRenderGraphFromPds(
              puleAllocateDefault(),
              platform,
              renderGraphValue
            )
          );
          pulBase.dsDestroy(renderGraphValue);
        }
        auto generateWindowDepthFramebuffer = (
          pulBase.dsObjectMember(
            payloadGfx,
            "generate-window-depth-framebuffer"
          )
        );
        (void)generateWindowDepthFramebuffer;
        // load shader modules
        PuleDsValueArray const dsShaderModules = (
          pulBase.dsMemberAsArray(payloadGfx, "shader-modules")
        );
        for (size_t it = 0; it < dsShaderModules.length; ++ it) {
          PuleDsValue const dsShaderModule = dsShaderModules.values[it];
          shaderModules.emplace_back(
            loadShaderModule(pulBase, dsShaderModule, puleStringView(assetPath))
          );
        }
      }
      // -- ecs create
      PuleDsValue const payloadEcs = (
        pulBase.dsObjectMember(entryPayload, "ecs")
      );
      if (
        !pulBase.dsIsNull(payloadEcs)
        && pulBase.dsMemberAsBool(payloadEcs, "create-world")
      ) {
        puleLogDebug("[PuleApplication] creating ECS");
        ecsWorld = pulBase.ecsWorldCreate();
        ecsWorldAdvance = pulBase.dsMemberAsBool(payloadEcs, "world-advance");
        // -- load in ECS components & systems from plugins
        bool const loadComponent = (
          pulBase.dsMemberAsBool(payloadEcs, "register-components")
        );
        bool const loadSystem = (
          pulBase.dsMemberAsBool(payloadEcs, "register-systems")
        );
        for (auto const & componentPlugin : componentPlugins) {
          void (*registerComponentsFn)(
            PuleEngineLayer *,
            PuleEcsWorld const
          ) = nullptr;
          void (*registerSystemsFn)(
            PuleEngineLayer *,
            PuleEcsWorld const
          ) = nullptr;
          if (loadComponent) {
            ::tryLoadFn(
              registerComponentsFn,
              componentPlugin.id,
              "pulcRegisterComponents"
            );
            if (registerComponentsFn) {
              registerComponentsFn(&pulBase, ecsWorld);
            }
          }
          if (loadSystem) {
            ::tryLoadFn(
              registerSystemsFn,
              componentPlugin.id,
              "pulcRegisterSystems"
            );
            if (registerComponentsFn) {
              registerSystemsFn(&pulBase, ecsWorld);
            }
          }
        }
      }
      // -- file watcher
      PuleDsValue const payloadFile = (
        pulBase.dsObjectMember(entryPayload, "file")
      );
      if (
        !pulBase.dsIsNull(payloadFile)
        && pulBase.dsMemberAsBool(payloadFile, "watcher-check-all")
      ) {
        fileWatcherCheckAll = true;
      }
      // -- script create
      PuleDsValue const payloadScript = (
        pulBase.dsObjectMember(entryPayload, "script")
      );
      puleLogDebug("[PuleApplication] initializing script");
      if (
        !pulBase.dsIsNull(payloadScript)
        && pulBase.dsMemberAsBool(payloadScript, "initialize")
      ) {
        scriptContext = pulBase.scriptContextCreate();
        // script graph path
        auto scriptGraphPath = (
          pulBase.dsObjectMember(payloadScript, "script-graph-path")
        );
        if (!pulBase.dsIsNull(scriptGraphPath)) {
          puleLogDebug("[PuleApplication] creating script graph");
          auto scriptGraphValue = (
            pulBase.assetPdsLoadFromFile(
              pulBase.allocateDefault(),
              pulBase.cStr(
                (
                  std::string(assetPath.contents)
                  + pulBase.dsAsString(scriptGraphPath).contents
                ).c_str()
              ),
              &err
            )
          );
          if (pulBase.errorConsume(&err)) { return -1; }
          scriptTaskGraph = (
            pulBase.assetScriptTaskGraphFromPds(
              puleAllocateDefault(),
              scriptContext,
              scriptGraphValue,
              pulBase.stringView(assetPath)
            )
          );
          pulBase.dsDestroy(scriptGraphValue);
        }
      }
      // -- camera create
      PuleDsValue const payloadCamera = (
        pulBase.dsObjectMember(entryPayload, "camera-set")
      );
      if (!pulBase.dsIsNull(payloadCamera)) {
        puleLogDebug("[PuleApplication] creating camera");
        cameraSet = pulBase.cameraSetCreate(puleCStr("pule-primary"));
      }
    }
  }

  // create defaults
  //if (renderGraph.id == 0 && platform.id != 0) {
  //  renderGraph = puleRenderGraphCreate(puleAllocateDefault());
  //}

  // insert into payload as necessary
  if (platform.id != 0) {
    pulBase.pluginPayloadStoreU64(
      payload,
      pulBase.cStr("pule-platform"),
      platform.id
    );
  }
  if (ecsWorld.id != 0) {
    pulBase.pluginPayloadStoreU64(
      payload,
      pulBase.cStr("pule-ecs-world"),
      ecsWorld.id
    );
  }
  if (renderGraph.id != 0) {
    pulBase.pluginPayloadStoreU64(
      payload,
      pulBase.cStr("pule-render-graph"),
      renderGraph.id
    );
  }
  if (scriptTaskGraph.id != 0) {
    pulBase.pluginPayloadStoreU64(
      payload,
      pulBase.cStr("pule-script-task-graph"),
      scriptTaskGraph.id
    );
  }
  if (cameraSet.id != 0) {
    pulBase.pluginPayloadStoreU64(
      payload,
      pulBase.cStr("pule-camera-set"),
      cameraSet.id
    );
  }
  for (auto shaderModule : shaderModules) {
    std::string const shaderModuleLabel = (
      std::string("pule-asset-shader-module-")
      + pulBase.assetShaderModuleLabel(shaderModule).contents
    );
    puleLogDebug(
      "[PuleApplication] storing shader module %d at payload label '%s'",
      pulBase.assetShaderModuleHandle(shaderModule).id,
      shaderModuleLabel.c_str()
    );
    pulBase.pluginPayloadStoreU64(
      payload,
      pulBase.cStr(shaderModuleLabel.c_str()),
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
  assert(isGuiEditor ? platform.id : true);
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
    pulBase.imguiInitialize(platform);
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
      pulBase.renderGraphFrameStart(renderGraph);
    }
    if (isGuiEditor) {
      pulBase.imguiNewFrame();
    }
    if (cameraSet.id != 0) {
      pulBase.cameraControllerPollEvents();
      pulBase.cameraSetRefresh(cameraSet);
    }
    if (scriptTaskGraph.id != 0) {
      auto info = ScriptTaskGraphNodeUpdateInfo {
        .pul = &pulBase,
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
      pulBase.ecsWorldAdvance(ecsWorld, 16.0f);
    }
    if (isGuiEditor) {
      // pulchritude engine display
      puleImguiEngineDisplay(PuleImguiEngineDisplayInfo {
        .world = ecsWorld,
        .platform = platform,
      });
      // plugin provided functions
      for (auto const guiFn : guiEditorFns) {
        guiFn(puleAllocateDefault(), platform, pulBase);
      }
      // render out
      pulBase.imguiRender(renderGraph);
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
      pulBase.renderGraphFrameSubmit(
        swapchainAvailableSemaphore,
        renderGraph
      );
    }
    if (platform.id != 0) {
      pulePlatformSwapFramebuffer(platform);
    }
    if (fileWatcherCheckAll) {
      pulBase.fileWatchCheckAll();
    }
    pulBase.fileWatchCheckAll();
    if (isEarlyExit) {
      puleLog("<--> frame end <-->");
    }
    if (isEarlyExit) { // debug early exit
      static size_t frameCount = 0;
      if (++frameCount > 2) { break; }
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

  for (auto const pluginLayerName : pluginLayers) {
    puleStringDestroy(pluginLayerName);
  }

  pulBase.stringDestroy(assetPath);
  pulBase.pluginPayloadDestroy(payload);
  pulBase.scriptContextDestroy(scriptContext);

  if (platform.id == 0) {
    pulBase.platformDestroy(platform);
    pulBase.ecsWorldDestroy(ecsWorld);
  }

  pulePluginsFree();
  puleLog("[PuleApplication] clean exit");
  return 0;
}

// jryy znlor guvf pbhyq or gur raqvat jvgu abguvat yrsg bs lbh
