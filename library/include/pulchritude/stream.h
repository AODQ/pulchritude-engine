/* auto generated file stream */
#pragma once
#include "core.h"

#include "array.h"
#include "string.h"
#include "allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs
struct PuleStreamRead;
struct PuleStreamWrite;

typedef struct PuleStreamRead {
  void * userdata;
  uint8_t(* readByte)(void *);
  uint8_t(* peekByte)(void *);
  bool(* isDone)(void *);
  void(* destroy)(void *);
} PuleStreamRead;
typedef struct PuleStreamWrite {
  void * userdata;
  void(* writeBytes)(void *, uint8_t const *, size_t);
  void(* flush)(void *);
  void(* destroy)(void *);
} PuleStreamWrite;

// functions
PULE_exportFn uint8_t puleStreamReadByte(PuleStreamRead stream);
/*  TODO this can be done with ptr to PuleString maybe? */
PULE_exportFn PuleString puleStreamReadLine(PuleStreamRead stream, PuleAllocator alloc);
PULE_exportFn uint8_t puleStreamPeekByte(PuleStreamRead stream);
PULE_exportFn bool puleStreamReadIsDone(PuleStreamRead stream);
PULE_exportFn void puleStreamReadDestroy(PuleStreamRead stream);
PULE_exportFn PuleBuffer puleStreamDumpToBuffer(PuleStreamRead stream);
PULE_exportFn PuleStreamRead puleStreamReadFromString(PuleStringView stringView);
PULE_exportFn PuleStreamRead puleStreamReadFromBuffer(PuleBufferView bufferView);
PULE_exportFn void puleStreamWriteBytes(PuleStreamWrite stream, uint8_t const * bytes, size_t length);
PULE_exportFn void puleStreamWriteStrFormat(PuleStreamWrite stream, char const * format, ...);
PULE_exportFn void puleStreamWriteFlush(PuleStreamWrite stream);
PULE_exportFn void puleStreamWriteDestroy(PuleStreamWrite stream);
PULE_exportFn PuleStreamWrite puleStreamStdoutWrite();

#ifdef __cplusplus
} // extern C
#endif
