#include <bulkher-log/log.h>

#include <stdio.h>
#include <stdarg.h>

void puleLog(char const * const formatCStr, ...) {
  va_list args;
  va_start (args, formatCStr);
  vprintf (formatCStr, args);
  va_end (args);

  printf("\n");
}
