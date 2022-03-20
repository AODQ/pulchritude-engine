#pragma once
// this provides mechanisms to stream data between modules, such that a
//   function doesn't have to worry about the source of some memory (such as if
//   it's from a file, network, buffer, etc)

#include <pulchritude-core/core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void * userdata;
  uint8_t (*readByte)(void * const userdata);
  uint8_t (*peekByte)(void * const userdata);
  bool (*isDone)(void * const userdata);
  void (*destroy)(void * const userdata);
} PuleStreamRead;

PULE_exportFn uint8_t puleStreamReadByte(PuleStreamRead const stream);
PULE_exportFn uint8_t puleStreamPeekByte(PuleStreamRead const stream);
PULE_exportFn bool puleStreamReadIsDone(PuleStreamRead const stream);
PULE_exportFn void puleStreamReadDestroy(PuleStreamRead const stream);

typedef struct {
  void * userdata;
  void (*writeBytes)(
    void * const userdata,
    void * const bytes,
    size_t const length
  );
} PuleStreamWrite;

PULE_exportFn void puleStreamWriteBytes(
  PuleStreamWrite const stream,
  void * const bytes,
  size_t const length
);

#ifdef __cplusplus
}
#endif
