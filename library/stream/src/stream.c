#include <pulchritude-stream/stream.h>

#include <stdio.h>

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

void puleStreamWriteBytes(
  PuleStreamWrite const stream,
  uint8_t const * const bytes,
  size_t const length
) {
  stream.writeBytes(stream.userdata, bytes, length);
}

static void writeBytes(
  void * const userdata,
  uint8_t const * const bytes,
  size_t const length
) {
  (void)userdata;
  for (size_t it = 0; it < length; ++ it)
    printf("%c", ((char const *)(bytes))[it]);
}
static void flush(void * const userdata) { (void)userdata; }
static void destroy(void * const userdata) { (void)userdata; }

PuleStreamWrite puleStreamStdoutWrite() {
  PuleStreamWrite writer;
  writer.userdata = NULL;
  writer.writeBytes = &writeBytes;
  writer.flush = &flush;
  writer.destroy = &destroy;
  return writer;
}

PULE_exportFn void puleStreamWriteFlush(PuleStreamWrite const stream) {
  stream.flush(stream.userdata);
}
PULE_exportFn void puleStreamWriteDestroy(PuleStreamWrite const stream) {
  stream.destroy(stream.userdata);
}

