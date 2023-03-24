#pragma once

#include <pulchritude-core/core.h>

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-script/script.h>
#include <pulchritude-task-graph/task-graph.h>

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
