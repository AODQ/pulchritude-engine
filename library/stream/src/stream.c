#include <pulchritude-stream/stream.h>

#include <stdio.h>
#include <string.h>

uint8_t puleStreamReadByte(PuleStreamRead const stream) {
  return stream.readByte(stream.userdata);
}

uint8_t puleStreamPeekByte(PuleStreamRead const stream) {
  return stream.peekByte(stream.userdata);
}

bool puleStreamReadIsDone(PuleStreamRead const stream) {
  return stream.isDone(stream.userdata);
}

void puleStreamReadDestroy(PuleStreamRead const stream) {
  stream.destroy(stream.userdata);
}

PuleBuffer puleStreamDumpToBuffer(PuleStreamRead const stream) {
  PuleBuffer buffer = puleBufferCreate(puleAllocateDefault());
  while (!puleStreamReadIsDone(stream)) {
    uint8_t const byte = puleStreamReadByte(stream);
    puleBufferAppend(&buffer, &byte, 1);
  }
  uint8_t const nullByte = 0;
  puleBufferAppend(&buffer, &nullByte, 1);
  return buffer;
}

void puleStreamWriteBytes(
  PuleStreamWrite const stream,
  uint8_t const * const bytes,
  size_t const length
) {
  stream.writeBytes(stream.userdata, bytes, length);
}

static void stdoutWriteBytes(
  void * const userdata,
  uint8_t const * const bytes,
  size_t const length
) {
  (void)userdata;
  for (size_t it = 0; it < length; ++ it)
    printf("%c", ((char const *)(bytes))[it]);
}
static void stdoutFlush(void * const userdata) { (void)userdata; }
static void stdoutDestroy(void * const userdata) { (void)userdata; }

PuleStreamWrite puleStreamStdoutWrite() {
  PuleStreamWrite writer;
  writer.userdata = NULL;
  writer.writeBytes = &stdoutWriteBytes;
  writer.flush = &stdoutFlush;
  writer.destroy = &stdoutDestroy;
  return writer;
}

PULE_exportFn void puleStreamWriteFlush(PuleStreamWrite const stream) {
  stream.flush(stream.userdata);
}
PULE_exportFn void puleStreamWriteDestroy(PuleStreamWrite const stream) {
  stream.flush(stream.userdata);
  stream.destroy(stream.userdata);
}

static uint8_t puleStreamReadFromCStringReadByte(void * const userdata) {
  PuleStringView * const stringView = (PuleStringView *)(userdata);
  uint8_t const retValue = *(uint8_t *)(stringView->contents);
  stringView->contents += 1;
  if (stringView->len > 0) {
    stringView->len -= 1;
  }
  return retValue;
}
static uint8_t puleStreamReadFromCStringPeekByte(void * const userdata) {
  PuleStringView * const stringView = (PuleStringView *)(userdata);
  return *(uint8_t *)(stringView->contents);
}
static bool puleStreamReadFromCStringIsDone(void * const userdata) {
  PuleStringView * const stringView = (PuleStringView *)(userdata);
  return stringView->len == 0;
}
static void puleStreamReadFromCStringDestroy(void * const userdata) {
  free(userdata);
}

PULE_exportFn PuleStreamRead puleStreamReadFromString(
  PuleStringView const stringView
) {
  PuleStringView * const stringViewPtr = malloc(sizeof(PuleStringView));
  memcpy(stringViewPtr, &stringView, sizeof(PuleStringView));
  PuleStreamRead streamRead = {
    .userdata = stringViewPtr,
    .readByte = &puleStreamReadFromCStringReadByte,
    .peekByte = &puleStreamReadFromCStringPeekByte,
    .isDone = &puleStreamReadFromCStringIsDone,
    .destroy = &puleStreamReadFromCStringDestroy,
  };
  return streamRead;
}
