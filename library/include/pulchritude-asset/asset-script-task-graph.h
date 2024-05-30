#pragma once

#include <pulchritude/core.h>

#include <pulchritude/allocator.h>
#include <pulchritude/data-serializer.h>
#include <pulchritude/script.h>
#include <pulchritude/task-graph.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn PuleTaskGraph puleAssetScriptTaskGraphFromPds(
  PuleAllocator const allocator,
  PuleScriptContext const scriptContext,
  PuleDsValue const taskGraphValue,
  PuleStringView const assetPath
);

#ifdef __cplusplus
}
#endif
