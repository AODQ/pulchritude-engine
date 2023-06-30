#pragma once

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-core/core.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gpu/commands.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-render-graph/render-graph.h>

#ifdef __cplusplus
extern "C" {
#endif

PULE_exportFn PuleRenderGraph puleAssetRenderGraphFromPds(
  PuleAllocator const allocator,
  PulePlatform const platform,
  PuleDsValue const renderGraphValue
);

// FULE_exportFn PuleDsValue puleAssetRenderGraphToPds(
//   PuleAllocator const allocator,
//   PuleRenderGraph const renderGraph
// );

#ifdef __cplusplus
}
#endif
