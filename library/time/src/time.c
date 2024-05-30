#include <pulchritude/time.h>

#include <errno.h>
#include <time.h>

PuleMicrosecond puleMicrosecond(int64_t const value) {
  PuleMicrosecond ms;
  ms.valueMicro = value;
  return ms;
}

void puleSleepMicrosecond(PuleMicrosecond const us) {
  if (us.valueMicro < 0) return;
  struct timespec ts;
  ts.tv_sec = us.valueMicro / 1000000;
  ts.tv_nsec = (us.valueMicro % 1000000) * 1000;
  while (nanosleep(&ts, &ts) && (errno == EINTR));
}

PULE_exportFn PuleTimestamp puleTimestampNow() {
  // time since epoch in milliseconds
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  PuleTimestamp timestamp;
  timestamp.valueUnixTs = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
  return timestamp;
}
