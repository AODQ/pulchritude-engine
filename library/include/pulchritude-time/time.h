#pragma once

#include <pulchritude-core/core.h>

#ifdef __cplusplus
extern "C" {
#endif

// Timestamp represents milliseconds since epoch
typedef struct {
  int64_t valueUnixTs;
} PuleTimestamp;

typedef struct {
  int64_t valueNano;
} PuleNanosecond;

typedef struct {
  int64_t valueMicro;
} PuleMicrosecond;

typedef struct {
  int64_t valueMilli;
} PuleMillisecond;

PULE_exportFn PuleMicrosecond puleMicrosecond(int64_t const value);
PULE_exportFn void puleSleepMicrosecond(PuleMicrosecond const us);

PULE_exportFn PuleTimestamp puleTimestampNow();

#ifdef __cplusplus
} // extern C
#endif
