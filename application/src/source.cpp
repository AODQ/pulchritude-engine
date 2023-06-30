/* pulchritude engine | github.com/aodq/pulchritude-engine | aodq.net */

// pybfr lbhe rlrf naq erzrzore jung'f ybpxrq njnl sbe v ungr gur ynzre naq here

#include <pulchritude-asset/pds.h>
#include <pulchritude-log/log.h>
#include <pulchritude-plugin/engine.h>
#include <pulchritude-plugin/plugin.h>
#include <pulchritude-task-graph/task-graph.h>
#include <pulchritude-camera/camera.h>

// need struct definitions, but don't call anything from here directly
#include <pulchritude-gfx/gfx.h>
#include <pulchritude-imgui-engine/imgui-engine.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-platform/platform.h>

#include <string>
#include <unordered_map>
#include <vector>

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
    if (pluginTypeFn && pluginTypeFn() == PulePluginType_component) {
      componentPlugins.emplace_back(plugin.id);
    }
  }

  struct PluginsCollectRenderGraphInfo {
    PuleRenderGraph renderGraph;
    PulePlatform platform;
  };
  void iteratePluginsCollectRenderGraphs(
    PulePluginInfo const plugin,
    void * const userdata
  ) {
    auto const info = (
      *reinterpret_cast<PluginsCollectRenderGraphInfo *>(userdata)
    );
    PuleRenderGraph (*renderGraphFn)(PulePlatform const plaftform) = nullptr;
    tryLoadFn(renderGraphFn, plugin.id, "pulcRenderGraph");
    if (renderGraphFn) {
      puleRenderGraphMerge(info.renderGraph, renderGraphFn(info.platform));
    }
  }
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

uint64_t fetchResourceHandle(
  PuleStringView const label,
  void * const pluginPayload
) {
  if (puleStringViewEqCStr(label, "window-swapchain-image")) {
    return puleGfxWindowImage().id;
  }
  return (
    pulePluginPayloadFetchU64(
      PulePluginPayload { .id = reinterpret_cast<uint64_t>(pluginPayload), },
      label
    )
  );
}

} // -- render task graph callbacks --------------------------------------------

// update components
namespace {
  std::vector<void(*)(PulePluginPayload const)> updateableComponents;
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
      puleTryPluginLoadFn(plugin.id, "puldGuiEditor")
    )
  );
  if (guiEditorFn) {
    plugins.emplace_back(guiEditorFn);
  }
}
} // namespace editor

std::unordered_map<std::string, KnownParameterInfo> knownProgramParameters = {
  { "--asset-path",      {.hasParameter = true,  } },
  { "--gui-editor",      {.hasParameter = false, } },
  { "--debug",           {.hasParameter = false, } },
  { "--error-segfaults", {.hasParameter = false, } },
  { "--plugin-layer",    {.hasParameter = true,  } },
  { "--plugin-path",     {.hasParameter = true,  } },
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
  for (int32_t argumentIt = 1; argumentIt < argumentLength; ++ argumentIt) {
    auto const label = std::string(arguments[argumentIt]);
    std::string content = "";
    // this breaks into application parameters
    if (label == "--") {
      break;
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

  bool isGuiEditor = false;

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
    puleLog(
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
    puleLog("[PuleApplication] Finished loading plugin layers");
    layers.insert(layers.begin(), engineLayer);
  }
  assert(layers.size() > 0);

  std::vector<size_t> componentPluginIds;
  puleIteratePlugins(
    ::iteratePlugins,
    reinterpret_cast<void *>(&componentPluginIds)
  );

  PulePluginPayload const payload = (
    pulePluginPayloadCreate(puleAllocateDefault())
  );

  // store layer
  pulePluginPayloadStore(payload, puleCStr("pule-engine-layer"), &layers[0]);
  PuleEngineLayer & pulBase = layers[0];
  puleLogDebug(
    "[PuleApplication] initializing from assets/project.pds with layer '%s'",
    pulBase.layerName.contents
  );

  // store asset path
  pulePluginPayloadStore(payload, puleCStr("pule-fs-asset-path"), &assetPath);
  puleFilesystemAssetPathSet(puleStringView(assetPath));

  PulePlatform platform = { .id = 0, };
  PuleScriptContext scriptContext = { .id = 0, };
  bool fileWatcherCheckAll = true;
  PuleEcsWorld ecsWorld = { .id = 0, };
  PuleRenderGraph renderGraph = { .id = 0, };
  PuleTaskGraph scriptTaskGraph = { .id = 0, };
  PuleCameraSet cameraSet = { .id = 0, };
  std::vector<PuleAssetShaderModule> shaderModules = {};
  bool ecsWorldAdvance = false;

  // TODO break these out into functions
  { // -- load project.pds, use base layer to allow them to capture startup
    PuleError err = puleError();
    PuleDsValue const projectValue = (
      pulBase.assetPdsLoadFromFile(
        pulBase.allocateDefault(),
        pulBase.cStr("/assets/project.pds"),
        &err
      )
    );
    if (pulBase.errorConsume(&err)) { return -1; }


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
        puleLog("[PuleApplication] creating platform");
        size_t const defaultWidth = 800; // TODO get from window?
        size_t const defaultHeight = 800;
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
        puleLog("[PuleApplication] creating graphics context");
        if (pulBase.dsMemberAsBool(payloadGfx, "initialize")) {
          if (platform.id == 0) {
            pulBase.logError(
              "Trying to initialize gfx context without platform"
            );
            return -1;
          }
          pulBase.gfxInitialize(platform, &err);
          if (pulBase.errorConsume(&err)) { return -1; }
        }
        auto renderGraphPath = (
          pulBase.dsObjectMember(payloadGfx, "render-graph-path")
        );
        if (!pulBase.dsIsNull(renderGraphPath)) {
          puleLog("[PuleApplication] creating render graph");
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
        puleLog("[PuleApplication] creating ECS");
        ecsWorld = pulBase.ecsWorldCreate();
        ecsWorldAdvance = pulBase.dsMemberAsBool(payloadEcs, "world-advance");
        // -- load in ECS components & systems from plugins
        bool const loadComponent = (
          pulBase.dsMemberAsBool(payloadEcs, "register-components")
        );
        bool const loadSystem = (
          pulBase.dsMemberAsBool(payloadEcs, "register-systems")
        );
        for (size_t const componentPluginId : componentPluginIds) {
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
              componentPluginId,
              "pulcRegisterComponents"
            );
            if (registerComponentsFn) {
              registerComponentsFn(&pulBase, ecsWorld);
            }
          }
          if (loadSystem) {
            ::tryLoadFn(
              registerSystemsFn,
              componentPluginId,
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
      puleLog("[PuleApplication] initializing script");
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
          puleLog("[PuleApplication] creating script graph");
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
  if (renderGraph.id == 0 && platform.id != 0) {
    renderGraph = puleRenderGraphCreate(puleAllocateDefault());
  }

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
      pulBase.assetShaderModuleGfxHandle(shaderModule).id,
      shaderModuleLabel.c_str()
    );
    pulBase.pluginPayloadStoreU64(
      payload,
      pulBase.cStr(shaderModuleLabel.c_str()),
      shaderModule.id
    );
  }

  // initiate all plugins
  puleLogDebug("[PuleApplication] initializing plugins");
  for (size_t const componentPluginId : componentPluginIds) {
    if (isGuiEditor) { break; }
    // try to load component
    void (*componentLoadFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentLoadFn, componentPluginId, "pulcComponentLoad");
    if (componentLoadFn) {
      puleLogDebug("[PuleApplication] Loading function");
      componentLoadFn(payload);
    }

    // check if they have an update function
    void (*componentUpdateFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentUpdateFn, componentPluginId, "pulcComponentUpdate");
    if (componentUpdateFn) {
      updateableComponents.emplace_back(componentUpdateFn);
    }
  }

  { // check if any plugin contributes to render task graph
    auto renderGraphInfo = PluginsCollectRenderGraphInfo {
      .renderGraph = renderGraph,
      .platform = platform,
    };
    puleIteratePlugins(
      ::iteratePluginsCollectRenderGraphs, &renderGraphInfo
    );
  }

  // load gui editor if requested
  assert(isGuiEditor ? platform.id : true);
  std::vector<GuiEditorFn> guiEditorFns;
  if (isGuiEditor) {
    puleIteratePlugins(
      ::guiPluginLoad,
      reinterpret_cast<void *>(&guiEditorFns)
    );
    puleLog(
      "[PuleApplication] Loaded %u gui editor functions",
      guiEditorFns.size()
    );
    pulBase.imguiInitialize(platform);
    // guiPrepareRenderCommandList = (
    //   pulBase.gfxCommandListCreate(
    //     puleAllocateDefault(), puleCStr("pule-gui-prepare-render")
    //   )
    // );
    // PuleGfxCommandListRecorder const recorder = (
    //   pulBase.gfxCommandListRecorder(guiPrepareRenderCommandList)
    // );
    // // TODO::CRIT
    // // pulBase.gfxCommandListAppendAction(
    // //   recorder,
    // //   PuleGfxCommand {
    // //     .clearImageColor = {
    // //       .action = PuleGfxAction_clearImageColor,
    // //       .framebuffer = pulBase.gfxFramebufferWindow(),
    // //       .color = PuleF32v4{0.2f, 0.2f, 0.3f, 1.0f},
    // //     },
    // //   }
    // // );
    // // pulBase.gfxCommandListAppendAction(
    // //   recorder,
    // //   PuleGfxCommand {
    // //     .clearImageDepth = {
    // //       .action = PuleGfxAction_clearImageDepth,
    // //       .framebuffer = pulBase.gfxFramebufferWindow(),
    // //       .depth = 1.0f,
    // //     },
    // //   }
    // // );
    // pulBase.gfxCommandListRecorderFinish(recorder);
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
  bool hasUpdate = updateableComponents.size() > 0 || isGuiEditor;
  while (hasUpdate) {
    puleLog("<--> frame start <-->");
    if (platform.id) {
      puleGfxFrameStart();
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
      pulBase.imguiRender(
        pulBase.renderGraph_commandListRecorder(
          pulBase.renderGraphNodeFetch(
            renderGraph, pulBase.cStr("imgui-render")
          ),
          &fetchResourceHandle,
          reinterpret_cast<void *>(payload.id)
        )
      );
    } else {
      for (auto const componentUpdateFn : updateableComponents) {
        componentUpdateFn(payload);
      }
    }
    if (renderGraph.id != 0) {
      pulBase.renderGraphFrameEnd(renderGraph);
    }
    if (platform.id != 0) {
      puleGfxFrameEnd();
      pulePlatformSwapFramebuffer(platform);
    }
    if (fileWatcherCheckAll) {
      pulBase.fileWatchCheckAll();
    }
    pulBase.fileWatchCheckAll();
    puleLog("<--> frame end <-->");
    //{ // debug early exit
    //  static size_t frameCount = 0;
    //  if (++frameCount > 2) { break; }
    //}
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

  puleLog(
    "[PuleApplication] Shutting down...",
    guiEditorFns.size()
  );

  // try to unload components
  for (size_t const componentPluginId : componentPluginIds) {
    void (*componentUnloadFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentUnloadFn, componentPluginId, "pulcComponentUnload");
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
  puleLog("[PuleApplication] Exit finished");
  return 0;
}

// jryy znlor guvf pbhyq or gur raqvat jvgu abguvat yrsg bs lbh n uhaqerq
// jvfurf pbhyqa'g fnl v qba'g jnag gb
