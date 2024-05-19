/* auto generated file time */
#pragma once
#include <pulchritude/core.h>


#ifdef __cplusplus
extern "C" {
#endif

// structs
/* milliseconds since epoch */
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

// enum

// entities

// functions
PULE_exportFn PuleMicrosecond puleMicrosecond(int64_t value);
PULE_exportFn void puleSleepMicrosecond(PuleMicrosecond us);
PULE_exportFn PuleTimestamp puleTimestampNow();

#ifdef __cplusplus
} // extern C
#endif
