
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

PULE_externFn PuleFile puleFileOpen(
  char const * const filename,
  PuleFileDataMode dataMode,
  PuleFileOpenMode openMode,
  PuleError * const error
);
PULE_externFn void puleFileClose(PuleFile const file);

PULE_externFn bool puleFileIsDone(PuleFile const file);

PULE_externFn uint8_t puleFileReadByte(PuleFile const file);
PULE_externFn size_t puleFileReadBytes(
  PuleFile const file,
  PuleArrayView const destination
);
PULE_externFn size_t puleFileReadBytesWithStride(
  PuleFile const file,
  PuleArrayView const destination
);
PULE_externFn void puleFileWriteBytes(
  PuleFile const file,
  PuleArrayView const source
);
PULE_externFn uint64_t puleFileSize(PuleFile const file);
PULE_externFn void puleFileAdvanceFromStart(
  PuleFile const file,
  int64_t const offset
);
PULE_externFn void puleFileAdvanceFromEnd(
  PuleFile const file,
  int64_t const offset
);
PULE_externFn void puleFileAdvanceFromCurrent(
  PuleFile const file,
  int64_t const offset
);

#ifdef __cplusplus
}
#endif
