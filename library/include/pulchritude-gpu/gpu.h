#pragma once

// These bindings are implementable in all platforms/renderers (OpenGL, GLES,
//   Vulkan, DX9, Metal, etc)
//
// Right now only OpenGL4.6 renderer exists. In the future, an OpenGL 3.0
//   renderer will also exist (to target more machines). At some point if this
//   library does well a vulkan backend will also be written.
//
// The intent for 3D games that require more hardware capability is to 'extend'
//   the gfx to support more features, though this is the one exception where
//   there might be incompatilibies between libraries. It's best to try to
//   avoid this, of course. For example, enums could be extended, new functions
//   can be added, and most resources are just 'id' so that the underlying
//   implementation can change.

#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-gpu/synchronization.h>
#include <pulchritude-platform/platform.h>

#include <pulchritude-gpu/synchronization.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorGfx_none,
  PuleErrorGfx_creationFailed,
  PuleErrorGfx_shaderModuleCompilationFailed,
  PuleErrorGfx_invalidDescriptorSet,
  PuleErrorGfx_invalidCommandList,
  PuleErrorGfx_invalidFramebuffer,
  PuleErrorGfx_submissionFenceWaitFailed,
} PuleErrorGfx;

typedef struct {
  uint64_t id;
} PuleGpuBuffer;

typedef  struct {
  uint32_t vertexCount;
  uint32_t instanceCount;
  uint32_t vertexOffset;
  uint32_t instanceOffset;
} PuleGpuDrawIndirectArrays;

// TODO rename to remove _buffer
typedef enum {
  PuleGpuBufferUsage_attribute = 0x1,
  PuleGpuBufferUsage_element = 0x2,
  PuleGpuBufferUsage_uniform = 0x4,
  PuleGpuBufferUsage_storage = 0x8,
  PuleGpuBufferUsage_accelerationStructure = 0x10,
  PuleGpuBufferUsage_indirect = 0x20,
} PuleGpuBufferUsage;

typedef enum {
  PuleGpuBufferBindingDescriptor_uniform,
  PuleGpuBufferBindingDescriptor_storage,
  PuleGpuBufferBindingDescriptor_accelerationStructure,
} PuleGpuBufferBindingDescriptor;

typedef enum {
  PuleGpuBufferVisibilityFlag_deviceOnly = 0x1, // incompatible with rest
  PuleGpuBufferVisibilityFlag_hostVisible = 0x2,
  PuleGpuBufferVisibilityFlag_hostWritable = 0x4,
} PuleGpuBufferVisibilityFlag;

PULE_exportFn PuleGpuBuffer puleGpuBufferCreate(
  PuleStringView const name,
  void const * const optionalInitialData, // TODO switch to PuleBufferView
  size_t const byteLength, // must be >0
  PuleGpuBufferUsage const usage,
  PuleGpuBufferVisibilityFlag const visibility
);
PULE_exportFn void puleGpuBufferDestroy(PuleGpuBuffer const buffer);

typedef enum {
  PuleGpuBufferMapAccess_hostVisible = 0x1,
  PuleGpuBufferMapAccess_hostWritable = 0x2,
  PuleGpuBufferMapAccess_invalidate = 0x4,
} PuleGpuBufferMapAccess;

typedef struct {
  PuleGpuBuffer buffer;
  PuleGpuBufferMapAccess access;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuBufferMapRange;

typedef struct {
  PuleGpuBuffer buffer;
  size_t byteOffset;
  size_t byteLength;
} PuleGpuBufferMappedFlushRange;

PULE_exportFn void * puleGpuBufferMap(PuleGpuBufferMapRange const range);
// TODO invalidate mapped buffer
PULE_exportFn void puleGpuBufferMappedFlush(
  PuleGpuBufferMappedFlushRange const range
);
PULE_exportFn void puleGpuBufferUnmap(PuleGpuBuffer const buffer);

PULE_exportFn void puleGpuInitialize(
  PulePlatform const platform,
  PuleError * const error
);
PULE_exportFn void puleGpuShutdown();

// prints all debug information
// TODO this should just stream out or something maybe serialize
PULE_exportFn void puleGpuDebugPrint();

PULE_exportFn PuleStringView puleGpuBufferUsageLabel(
  PuleGpuBufferUsage const usage
);

#ifdef __cplusplus
} // extern C
#endif
