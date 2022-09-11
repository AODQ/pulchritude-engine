#include <pulchritude-time/time.h>

#include <errno.h>
#include <time.h>

PuleMicrosecond puleMicrosecond(int64_t const value) {
  PuleMicrosecond ms;
  ms.value = value;
  return ms;
}

void puleSleepMicrosecond(PuleMicrosecond const us) {
  if (us.value < 0) return;
  struct timespec ts;
  ts.tv_sec = us.value / 1000000;
  ts.tv_nsec = (us.value % 1000000) * 1000;
  while (nanosleep(&ts, &ts) && (errno == EINTR));
}
