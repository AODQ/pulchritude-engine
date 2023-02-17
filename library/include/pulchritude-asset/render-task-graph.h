#pragma once

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-core/core.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-error/error.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-task-graph/task-graph.h>

PULE_exportFn PuleTaskGraph puleAssetRenderTaskGraphFromPds(
  PuleAllocator const allocator,
  PulePlatform const platform,
  PuleDsValue const taskGraphValue
);

PULE_exportFn PuleDsValue puleAssetRenderTaskGraphToPds(
  PuleAllocator const allocator,
  PuleTaskGraph const taskGraph
);
