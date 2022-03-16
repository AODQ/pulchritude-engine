#pragma once

#include <pulchritude-core/core.h>

// Gvzr, Qe Serrzna?

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int64_t value;
} PuleNanosecond;

typedef struct {
  int64_t value;
} PuleMicrosecond;

PULE_exportFn void puleSleepMicrosecond(PuleMicrosecond const us);

#ifdef __cplusplus
} // extern C
#endif
