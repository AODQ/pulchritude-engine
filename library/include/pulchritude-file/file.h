
#include <pulchritude-array/array.h>
#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorFile_none,
  PuleErrorFile_fileOpen,
} PuleErrorFile;

typedef enum {
  PuleFileDataMode_text,
  PuleFileDataMode_binary,
} PuleFileDataMode;

typedef enum {
  PuleFileOpenMode_read,
  PuleFileOpenMode_writeOverwrite,
  PuleFileOpenMode_writeAppend,
  PuleFileOpenMode_readWriteOverwrite,
  PuleFileOpenMode_readWrite,
} PuleFileOpenMode;

typedef struct {
  uint64_t id;
} PuleFile;

PULE_exportFn PuleFile puleFileOpen(
  char const * const filename,
  PuleFileDataMode dataMode,
  PuleFileOpenMode openMode,
  PuleError * const error
);
PULE_exportFn void puleFileClose(PuleFile const file);

PULE_exportFn bool puleFileIsDone(PuleFile const file);

PULE_exportFn uint8_t puleFileReadByte(PuleFile const file);
PULE_exportFn size_t puleFileReadBytes(
  PuleFile const file,
  PuleArrayViewMutable const destination
);
PULE_exportFn size_t puleFileReadBytesWithStride(
  PuleFile const file,
  PuleArrayViewMutable const destination
);
PULE_exportFn void puleFileWriteBytes(
  PuleFile const file,
  PuleArrayView const source
);
PULE_exportFn uint64_t puleFileSize(PuleFile const file);
PULE_exportFn void puleFileAdvanceFromStart(
  PuleFile const file,
  int64_t const offset
);
PULE_exportFn void puleFileAdvanceFromEnd(
  PuleFile const file,
  int64_t const offset
);
PULE_exportFn void puleFileAdvanceFromCurrent(
  PuleFile const file,
  int64_t const offset
);

// efficient manner to stream file (such as for parsing), meant to grab
//   one byte at a time rather than copying the entire contents
// the only limitation is that you can only have one stream open per file,
//   you can't use the normal file mechanisms without closing the stream,
//   and it's not thread-safe

typedef struct {
  uint64_t id;
} PuleFileStream;

PULE_exportFn PuleFileStream puleFileStream(
  PuleFile const file,
  PuleArrayViewMutable const view // PULE_lifetimeOfReturnObject
);

// closes the stream, does not close the file
//   the file will be advanced to the last byte you read from the stream
PULE_exportFn void puleFileStreamDestroy(PuleFileStream const stream);

PULE_exportFn uint8_t puleFileStreamReadByte(PuleFileStream const stream);
PULE_exportFn uint8_t puleFileStreamPeekByte(PuleFileStream const stream);
PULE_exportFn bool puleFileStreamIsDone(PuleFileStream const stream);

#ifdef __cplusplus
}
#endif
