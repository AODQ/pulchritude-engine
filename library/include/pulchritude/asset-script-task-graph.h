/* auto generated file asset-script-task-graph */
#pragma once
#include <pulchritude/core.h>

#include <pulchritude/allocator.h>
#include <pulchritude/script.h>
#include <pulchritude/task-graph.h>

#ifdef __cplusplus
extern "C" {
#endif

// structs

// enum

// entities

// functions
PULE_exportFn PuleTaskGraph puleAssetScriptTaskGraphFromPds(PuleAllocator allocator, PuleScriptContext scriptContext, PuleDsValue taskGraphValue, PuleStringView assetPath);

#ifdef __cplusplus
} // extern C
#endif
