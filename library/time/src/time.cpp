#include <pulchritude/time.h>

#include <errno.h>
#include <time.h>

extern "C" {

PuleMicrosecond puleMicrosecond(int64_t const value) {
  PuleMicrosecond us;
  us.us = value;
  return us;
}

void puleSleepMicrosecond(PuleMicrosecond const us) {
  if (us.us < 0) return;
  struct timespec ts;
  ts.tv_sec = us.us / 1000000;
  ts.tv_nsec = (us.us % 1000000) * 1000;
  while (nanosleep(&ts, &ts) && (errno == EINTR));
}

PuleTimestamp puleTimestampNow() {
  // time since epoch in milliseconds
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  PuleTimestamp timestamp;
  timestamp.valueUnixTs = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
  return timestamp;
}

PuleMillisecond puleTimeMicroToMilli(PuleMicrosecond const us) {
  PuleMillisecond ms;
  ms.valueMilli = us.us / 1000;
  return ms;
}

} // extern "C"

PuleMillisecond operator"" _pms(unsigned long long const ms) {
  return PuleMillisecond { .valueMilli = (int64_t)ms };
}
