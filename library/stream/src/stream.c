#include <pulchritude/stream.h>

#include <stdio.h>
#include <string.h>

uint8_t puleStreamReadByte(PuleStreamRead const stream) {
  return stream.readByte(stream.userdata);
}
PuleString puleStreamReadLine(
  PuleStreamRead const stream, PuleAllocator const allocator
) {
  PuleBuffer buffer = puleBufferCreate(allocator);
  while (!puleStreamReadIsDone(stream)) {
    uint8_t const byte = puleStreamReadByte(stream);
    if (byte == '\n') {
      break;
    }
    puleBufferAppend(&buffer, &byte, 1);
  }
  uint8_t const nullByte = 0;
  puleBufferAppend(&buffer, &nullByte, 1);
  PuleString string = {
    .allocator = allocator,
    .len = buffer.byteLength - 1,
    .contents = (char *)(buffer.data),
  };
  return string;
}

uint8_t puleStreamPeekByte(PuleStreamRead const stream) {
  return stream.peekByte(stream.userdata);
}

bool puleStreamReadIsDone(PuleStreamRead const stream) {
  return stream.isDone(stream.userdata);
}

void puleStreamReadDestroy(PuleStreamRead const stream) {
  if (stream.destroy) {
    stream.destroy(stream.userdata);
  }
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

void puleStreamWriteStrFormat(
  PuleStreamWrite const stream,
  char const * const format,
  ...
) {
  va_list args;
  va_start(args, format);
  PuleString str = puleStringFormatVargs(puleAllocateDefault(), format, args);
  va_end(args);
  puleStreamWriteBytes(stream, (uint8_t *)str.contents, str.len);
  puleStringDestroy(str);
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
  if (stream.flush) {
    stream.flush(stream.userdata);
  }
}
PULE_exportFn void puleStreamWriteDestroy(PuleStreamWrite const stream) {
  puleStreamWriteFlush(stream);
  if (stream.destroy) {
    stream.destroy(stream.userdata);
  }
}

// -- from c string --
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

// -- from buffer view --
static uint8_t puleStreamReadFromBufferViewReadByte(void * const userdata) {
  PuleBufferView * const bufferView = (PuleBufferView *)(userdata);
  uint8_t const retValue = *(uint8_t *)(bufferView->data);
  bufferView->data += 1;
  if (bufferView->byteLength > 0) {
    bufferView->byteLength -= 1;
  }
  return retValue;
}
static uint8_t puleStreamReadFromBufferViewPeekByte(void * const userdata) {
  PuleBufferView * const bufferView = (PuleBufferView *)(userdata);
  return *(uint8_t *)(bufferView->data);
}
static bool puleStreamReadFromBufferViewIsDone(void * const userdata) {
  PuleBufferView * const bufferView = (PuleBufferView *)(userdata);
  return bufferView->byteLength == 0;
}
static void puleStreamReadFromBufferViewDestroy(void * const userdata) {
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

PULE_exportFn PuleStreamRead puleStreamReadFromBuffer(
  PuleBufferView const bufferView
) {
  PuleStringView * const bufferViewPtr = malloc(sizeof(PuleBufferView));
  memcpy(bufferViewPtr, &bufferView, sizeof(PuleBufferView));
  PuleStreamRead streamRead = {
    .userdata = bufferViewPtr,
    .readByte = &puleStreamReadFromBufferViewReadByte,
    .peekByte = &puleStreamReadFromBufferViewPeekByte,
    .isDone = &puleStreamReadFromBufferViewIsDone,
    .destroy = &puleStreamReadFromBufferViewDestroy,
  };
  return streamRead;
}
