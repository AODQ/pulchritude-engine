#include <pulchritude-log/log.h>

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>

typedef enum {
  LogType_normal,
  LogType_debug,
  LogType_error,
} LogType;

static bool debugEnabled;

#define COLOR_CODE_BLK "\033[0;30m"
#define COLOR_CODE_RED "\033[0;31m"
#define COLOR_CODE_GRN "\033[0;32m"
#define COLOR_CODE_YEL "\033[0;33m"
#define COLOR_CODE_BLU "\033[0;34m"
#define COLOR_CODE_MAG "\033[0;35m"
#define COLOR_CODE_CYN "\033[0;36m"
#define COLOR_CODE_WHT "\033[0;37m"

bool * puleLogDebugEnabled() {
  return &debugEnabled;
}

static void logger(LogType logType, char const * const formatCStr, va_list args) {
  // TODO lock thread

  if (logType == LogType_debug && !debugEnabled) {
    return;
  }

  switch (logType) {
    case LogType_debug:
      if (!debugEnabled) {
        return;
      }
      printf("["COLOR_CODE_CYN"DBG"COLOR_CODE_WHT"] ");
    break;
    case LogType_normal:
      printf("["COLOR_CODE_GRN"NOR"COLOR_CODE_WHT"] ");
    break;
    case LogType_error:
      printf("["COLOR_CODE_RED"ERR"COLOR_CODE_WHT"] ");
    break;
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
  vprintf (formatCStr, args);
#pragma GCC diagnostic pop

  printf("\n");
}

void puleLog(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_normal, formatCStr, args);
  va_end(args);
}

void puleLogDebug(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_debug, formatCStr, args);
  va_end(args);
}

void puleLogError(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_error, formatCStr, args);
  va_end(args);
  //raise(SIGSEGV);
}
