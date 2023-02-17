/* pulchritude engine | github.com/aodq/pulchritude-engine | aodq.net */

// pybfr lbhe rlrf naq erzrzore jung'f ybpxrq njnl sbe v ungr gur ynzre naq here

#include <pulchritude-asset/pds.h>
#include <pulchritude-log/log.h>
#include <pulchritude-plugin/engine.h>
#include <pulchritude-plugin/plugin.h>
#include <pulchritude-task-graph/task-graph.h>

// need struct definitions, but don't call anything from here directly
#include <pulchritude-gfx/gfx.h>
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
}

// -- render task graph callbacks
namespace {

void renderTaskGraphStartup(
  PuleTaskGraphNode const node,
  void * const
) {
  // fetch command lists
  auto const commandListStartup = PuleGfxCommandList {
    puleTaskGraphNodeAttributeFetchU64(node, puleCStr("command-list-startup"))
  };
  auto const commandListPrimaryRecorder = PuleGfxCommandListRecorder {
    puleTaskGraphNodeAttributeFetchU64(
      node, puleCStr("command-list-primary-recorder"))
  };

  // reset primary command list & set first action to be startup command list
  puleGfxCommandListRecorderReset(commandListPrimaryRecorder);
  puleGfxCommandListAppendAction(
    commandListPrimaryRecorder,
    PuleGfxCommand {
      .dispatchCommandList {
        .action = PuleGfxAction_dispatchCommandList,
        .submitInfo = {
          .commandList = commandListStartup,
          .fenceTargetStart = nullptr,
          .fenceTargetFinish = nullptr,
        },
      },
    }
  );
}

static bool debugRenderTaskGraphDump = true;
void renderTaskGraphFinish(
  PuleTaskGraphNode const node,
  void * const
) {
  // fetch & execute command list
  auto const commandListPrimary = PuleGfxCommandList {
    puleTaskGraphNodeAttributeFetchU64(node, puleCStr("command-list-primary"))
  };
  auto const commandListPrimaryRecorder = PuleGfxCommandListRecorder {
    puleTaskGraphNodeAttributeFetchU64(
      node, puleCStr("command-list-primary-recorder"))
  };
  puleGfxCommandListRecorderFinish(commandListPrimaryRecorder);
  PuleError err = puleError();
  if (debugRenderTaskGraphDump && *puleLogDebugEnabled()) {
    puleGfxCommandListDump(commandListPrimary);
  }
  puleGfxCommandListSubmit(
    {
      .commandList = commandListPrimary,
      .fenceTargetStart = nullptr,
      .fenceTargetFinish = nullptr,
    },
    &err
  );
  puleErrorConsume(&err);
}

} // namespace

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

std::unordered_map<std::string, KnownParameterInfo> knownProgramParameters = {
  { "--asset-path",      {.hasParameter = true,  } },
  { "--debug",           {.hasParameter = false, } },
  { "--error-segfaults", {.hasParameter = false, } },
  { "--plugin-layer",    {.hasParameter = true,  } },
  { "--plugin-path",     {.hasParameter = true,  } },
};
std::vector<ParameterInfo> programParameters;

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
      puleLogError("Unknown parameter '%s'", label.c_str());
      return 1;
    }
    if (knownInfo->second.hasParameter) {
      if (argumentIt+1 >= argumentLength) {
        puleLogError("Expecting parameter for '%s", label.c_str());
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
      puleLogDebug("Debug enabled");
    } else if (param.label == "--error-segfaults") {
      *puleLogErrorSegfaultsEnabled() = true;
      puleLogDebug("Segfault on error enabled");
    } else if (param.label == "--plugin-layer") {
      pluginLayers.emplace_back(
        puleString(puleAllocateDefault(), param.content.c_str())
      );
    }
  }

  if (assetPath.len == 0) {
    puleLogError("no asset path specified");
    return false;
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
    puleLog("Loading plugin layer '%s'", pluginLayerName.contents);
    PuleEngineLayer engineLayer;
    memset(&engineLayer, 0, sizeof(PuleEngineLayer));
    pulePluginLoadEngineLayer(
      &engineLayer,
      puleStringView(pluginLayerName),
      layers.size() > 0 ? &layers.back() : nullptr
    );
    puleLog("Finished loading");
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
    "Initializing application from project.pds with layer '%s'",
    pulBase.layerName.contents
  );

  // store asset path
  pulePluginPayloadStore(payload, puleCStr("pule-fs-asset-path"), &assetPath);

  PulePlatform platform = { .id = 0, };
  PuleEcsWorld ecsWorld = { .id = 0, };
  PuleTaskGraph renderTaskGraph = { .id = 0, };
  bool ecsWorldAdvance = false;

  // TODO break these out into functions
  { // -- load project.pds, use base layer to allow them to capture startup
    PuleError err = puleError();
    PuleDsValue const projectValue = (
      pulBase.assetPdsLoadFromFile(
        pulBase.allocateDefault(),
        pulBase.cStr(
          (std::string(assetPath.contents) + "/project.pds").c_str()
        ),
        &err
      )
    );
    if (pulBase.errorConsume(&err)) { return false; }


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
        puleLogDebug("Application creating platform");
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
        if (pulBase.errorConsume(&err)) { return false; }
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
        if (pulBase.errorConsume(&err)) { return false; }
      }
      // -- graphics create
      PuleDsValue const payloadGfx = (
        pulBase.dsObjectMember(entryPayload, "gfx")
      );
      if (!pulBase.dsIsNull(payloadGfx)) {
        puleLogDebug("Application creating graphics context");
        if (pulBase.dsMemberAsBool(payloadGfx, "initialize")) {
          if (platform.id == 0) {
            pulBase.logError(
              "Trying to initialize gfx context without platform"
            );
            return false;
          }
          pulBase.gfxInitialize(&err);
          if (pulBase.errorConsume(&err)) { return false; }
        }
        auto renderGraphPath = (
          pulBase.dsObjectMember(payloadGfx, "render-graph-path")
        );
        if (!pulBase.dsIsNull(renderGraphPath)) {
          puleLogDebug("Application creating render graph");
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
          if (pulBase.errorConsume(&err)) { return false; }
          renderTaskGraph = (
            puleAssetRenderTaskGraphFromPds(
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
        /* if(!pulBase.dsMemberAsBool(generateWindowDepthFramebuffer)) { */
        /* } */
      }
      // -- ecs create
      PuleDsValue const payloadEcs = (
        pulBase.dsObjectMember(entryPayload, "ecs")
      );
      if (
        !pulBase.dsIsNull(payloadEcs)
        && pulBase.dsMemberAsBool(payloadEcs, "create-world")
      ) {
        puleLogDebug("Application creating ECS");
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
    if (renderTaskGraph.id != 0) {
      pulBase.pluginPayloadStoreU64(
        payload,
        pulBase.cStr("pule-render-task-graph"),
        renderTaskGraph.id
      );
    }
  }

  // initiate all plugins
  puleLogDebug("initializing plugins");
  for (size_t const componentPluginId : componentPluginIds) {
    // try to load component
    void (*componentLoadFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentLoadFn, componentPluginId, "pulcComponentLoad");
    if (componentLoadFn) {
      puleLogDebug("Loading function");
      componentLoadFn(payload);
    }

    // check if they have an update function
    void (*componentUpdateFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentUpdateFn, componentPluginId, "pulcComponentUpdate");
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
  bool hasUpdate = updateableComponents.size() > 0;
  while (hasUpdate) {
    if (renderTaskGraph.id != 0) {
      puleTaskGraphExecuteInOrder(PuleTaskGraphExecuteInfo {
        .graph = renderTaskGraph,
        .callback = &renderTaskGraphStartup,
        .userdata = nullptr,
        .multithreaded = false,
      });
    }
    if (ecsWorldAdvance) {
      pulBase.ecsWorldAdvance(ecsWorld, 16.0f);
    }
    for (auto const componentUpdateFn : updateableComponents) {
      componentUpdateFn(payload);
    }
    if (renderTaskGraph.id != 0) {
      puleTaskGraphExecuteInOrder(PuleTaskGraphExecuteInfo {
        .graph = renderTaskGraph,
        .callback = &renderTaskGraphFinish,
        .userdata = nullptr,
        .multithreaded = false,
      });
      debugRenderTaskGraphDump = false;
    }
    if (platform.id != 0) {
      pulePlatformSwapFramebuffer(platform);
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
    void (*componentUnloadFn)(PulePluginPayload const) = nullptr;
    ::tryLoadFn(componentUnloadFn, componentPluginId, "pulcComponentUnload");
    if (componentUnloadFn) {
      componentUnloadFn(payload);
    }
  }

  for (auto const pluginLayerName : pluginLayers) {
    puleStringDestroy(pluginLayerName);
  }

  puleStringDestroy(assetPath);
  pulePluginPayloadDestroy(payload);

  if (platform.id == 0) {
    pulBase.platformDestroy(platform);
    pulBase.ecsWorldDestroy(ecsWorld);
  }

  pulePluginsFree();
  return 0;
}

// jryy znlor guvf pbhyq or gur raqvat jvgu abguvat yrsg bs lbh n uhaqerq
// jvfurf pbhyqa'g fnl v qba'g jnag gb
