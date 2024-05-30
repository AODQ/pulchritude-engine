#include <pulchritude/asset-script-task-graph.h>

#include <pulchritude/asset-pds.h>
#include <pulchritude/script.h>

#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace { // \\---------------------------------------------------------------

struct ScriptTaskGraphInternal {
  PuleScriptContext context;
};

std::unordered_map<uint64_t, ScriptTaskGraphInternal> internalTaskGraphs;
uint64_t internalTaskGraphCounter = 1;

} // namespace \\---------------------------------------------------------------

extern "C" { // ----------------------------------------------------------------

PuleTaskGraph puleAssetScriptTaskGraphFromPds(
  PuleAllocator const allocator,
  PuleScriptContext const scriptContext,
  PuleDsValue const dsScriptGraphValue,
  PuleStringView const assetPath
) {
  PuleTaskGraph const graph = puleTaskGraphCreate(allocator);
  PuleDsValueArray const dsGraphNodes = (
    puleDsMemberAsArray(dsScriptGraphValue, "task-graph")
  );
  // first create nodes
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    puleTaskGraphNodeCreate(
      graph,
      puleDsMemberAsString(dsGraphNode, "function-label")
    );
  }

  // create relationships
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    PuleDsValueArray const dsDependsOn = (
      puleDsMemberAsArray(dsGraphNode, "depends-on")
    );
    for (size_t depIt = 0; depIt < dsDependsOn.length; ++ depIt) {
      puleTaskGraphNodeRelationSet(
        puleTaskGraphNodeFetch(
          graph,
          puleDsMemberAsString(dsGraphNode, "function-label")
        ),
        PuleTaskGraphNodeRelation_dependsOn,
        puleTaskGraphNodeFetch(
          graph,
          puleDsAsString(dsDependsOn.values[depIt])
        )
      );
    }
  }

  // create script module attributes
  PuleError err = puleError();
  for (size_t it = 0; it < dsGraphNodes.length; ++ it) {
    PuleDsValue const dsGraphNode = dsGraphNodes.values[it];
    PuleTaskGraphNode const graphNode = (
      puleTaskGraphNodeFetch(
        graph,
        puleDsMemberAsString(dsGraphNode, "function-label")
      )
    );

    PuleStringView const filepathView = (
      puleDsMemberAsString(dsGraphNode, "file")
    );
    std::string const filepath = (
      std::string(assetPath.contents) + std::string(filepathView.contents)
    );

    auto scriptModuleRet = (
      puleScriptModuleFileWatch(
        scriptContext,
        puleAllocateDefault(),
        puleCStr(filepath.c_str()),
        PuleScriptDebugSymbols_enable, // TODO allow configurable
        &err
      )
    );

    puleLog("script module.. %u", scriptModuleRet.scriptModule.id);
    puleTaskGraphNodeAttributeStoreU64(
      graphNode,
      puleCStr("script-module"),
      scriptModuleRet.scriptModule.id
    );
    puleTaskGraphNodeAttributeStoreU64(
      graphNode,
      puleCStr("file-watcher"),
      scriptModuleRet.watcher.id
    );
  }

  internalTaskGraphs.emplace(
    internalTaskGraphCounter++,
    ScriptTaskGraphInternal{.context = scriptContext,}
  );

  return PuleTaskGraph{internalTaskGraphCounter};
}

} // - extern C ----------------------------------------------------------------
