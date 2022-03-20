#include <pulchritude-stream/stream.h>

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
  void * const bytes,
  size_t const length
) {
  stream.writeBytes(stream.userdata, bytes, length);
}
