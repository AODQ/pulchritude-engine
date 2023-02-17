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
} PuleGfxGpuBuffer;

typedef  struct {
  uint32_t vertexCount;
  uint32_t instanceCount;
  uint32_t vertexOffset;
  uint32_t instanceOffset;
} PuleGfxDrawIndirectArrays;

// this should be a bitfield in the future (this only tells us what the buffer
//   may be bound to for use in the future);
typedef enum {
  PuleGfxGpuBufferUsage_bufferAttribute,
  PuleGfxGpuBufferUsage_bufferElement,
  PuleGfxGpuBufferUsage_bufferUniform,
  PuleGfxGpuBufferUsage_bufferStorage, // unsupported GL3.3
  PuleGfxGpuBufferUsage_bufferAccelerationStructure, // unsupported GL3.3
  PuleGfxGpuBufferUsage_bufferIndirect, // unsupported GL3.3
} PuleGfxGpuBufferUsage;

typedef enum {
  PuleGfxGpuBufferVisibilityFlag_deviceOnly = 0x1, // incompatible with rest
  PuleGfxGpuBufferVisibilityFlag_hostVisible = 0x2,
  PuleGfxGpuBufferVisibilityFlag_hostWritable = 0x4,
} PuleGfxGpuBufferVisibilityFlag;

PULE_exportFn PuleGfxGpuBuffer puleGfxGpuBufferCreate(
  void const * const optionalInitialData,
  size_t const byteLength, // must be >0
  PuleGfxGpuBufferUsage const usage,
  PuleGfxGpuBufferVisibilityFlag const visibility
);
PULE_exportFn void puleGfxGpuBufferDestroy(PuleGfxGpuBuffer const buffer);

typedef enum {
  PuleGfxGpuBufferMapAccess_hostVisible = 0x1,
  PuleGfxGpuBufferMapAccess_hostWritable = 0x2,
  PuleGfxGpuBufferMapAccess_invalidate = 0x4,
} PuleGfxGpuBufferMapAccess;

typedef struct {
  PuleGfxGpuBuffer buffer;
  PuleGfxGpuBufferMapAccess access;
  size_t byteOffset;
  size_t byteLength;
} PuleGfxGpuBufferMapRange;

typedef struct {
  PuleGfxGpuBuffer buffer;
  size_t byteOffset;
  size_t byteLength;
} PuleGfxGpuBufferMappedFlushRange;

PULE_exportFn void * puleGfxGpuBufferMap(PuleGfxGpuBufferMapRange const range);
PULE_exportFn void puleGfxGpuBufferMappedFlush(
  PuleGfxGpuBufferMappedFlushRange const range
);
PULE_exportFn void puleGfxGpuBufferUnmap(PuleGfxGpuBuffer const buffer);

PULE_exportFn void puleGfxInitialize(PuleError * const error);
PULE_exportFn void puleGfxShutdown();

PULE_exportFn void puleGfxFrameStart();
PULE_exportFn void puleGfxFrameEnd();

// prints all debug information
// TODO this should just stream out or something maybe serialize
PULE_exportFn void puleGfxDebugPrint();

#ifdef __cplusplus
} // extern C
#endif
