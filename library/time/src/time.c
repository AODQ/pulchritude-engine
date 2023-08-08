#include <pulchritude-time/time.h>

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
