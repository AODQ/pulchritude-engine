/* auto generated file asset-script-task-graph */
#pragma once
#include "core.h"

#include "allocator.h"
#include "data-serializer.h"
#include "script.h"
#include "task-graph.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs


// functions
PULE_exportFn PuleTaskGraph puleAssetScriptTaskGraphFromPds(PuleAllocator allocator, PuleScriptContext scriptContext, PuleDsValue taskGraphValue, PuleStringView assetPath);

#ifdef __cplusplus
} // extern C
#endif
