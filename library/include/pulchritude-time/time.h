#pragma once

#include <pulchritude-core/core.h>

// Gvzr, Qe Serrzna?

#ifdef __cplusplus
extern "C" {
#endif

// Timestamp represents milliseconds since epoch
typedef struct {
  uint64_t value;
} PuleTimestamp;

typedef struct {
  int64_t value;
} PuleNanosecond;

typedef struct {
  int64_t value;
} PuleMicrosecond;

PULE_exportFn PuleMicrosecond puleMicrosecond(int64_t const value);
PULE_exportFn void puleSleepMicrosecond(PuleMicrosecond const us);

#ifdef __cplusplus
} // extern C
#endif
