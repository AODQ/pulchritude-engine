/* auto generated file time */
#pragma once
#include "core.h"


#ifdef __cplusplus
extern "C" {
#endif

// enum

// entities

// structs
struct PuleTimestamp;
struct PuleNanosecond;
struct PuleMicrosecond;
struct PuleMillisecond;

/* milliseconds since epoch */
typedef struct PuleTimestamp {
  int64_t valueUnixTs;
} PuleTimestamp;
typedef struct PuleNanosecond {
  int64_t valueNano;
} PuleNanosecond;
typedef struct PuleMicrosecond {
  int64_t valueMicro;
} PuleMicrosecond;
typedef struct PuleMillisecond {
  int64_t valueMilli;
} PuleMillisecond;

// functions
PULE_exportFn PuleMicrosecond puleMicrosecond(int64_t value);
PULE_exportFn void puleSleepMicrosecond(PuleMicrosecond us);
PULE_exportFn PuleTimestamp puleTimestampNow();

#ifdef __cplusplus
} // extern C
#endif
