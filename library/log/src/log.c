#include <pulchritude-log/log.h>

#include <pulchritude-error/error.h>

#include <signal.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __unix__
#include <execinfo.h>
#endif

typedef enum {
  LogType_normal,
  LogType_debug,
  LogType_dev,
  LogType_error,
  LogType_warn,
  LogType_raw,
} LogType;

static bool debugEnabled = false;
static bool errorSegfaultsEnabled = false;

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

bool * puleLogErrorSegfaultsEnabled() {
  return &errorSegfaultsEnabled;
}

static void logger(
  LogType logType,
  bool standardFormatting,
  char const * const formatCStr,
  va_list args
) {
  // TODO lock thread

  if (logType == LogType_debug && !debugEnabled) {
    return;
  }

  if (standardFormatting) {
    switch (logType) {
      case LogType_dev:
        printf("["COLOR_CODE_BLU"DEV"COLOR_CODE_WHT"] ");
      break;
      case LogType_debug:
        if (!debugEnabled) {
          return;
        }
        printf("["COLOR_CODE_CYN"DBG"COLOR_CODE_WHT"] ");
      break;
      case LogType_normal:
        printf("["COLOR_CODE_GRN"NOR"COLOR_CODE_WHT"] ");
      break;
      case LogType_warn:
        printf("["COLOR_CODE_YEL "WRN"COLOR_CODE_WHT"] ");
      break;
      case LogType_error:
        printf("["COLOR_CODE_RED"ERR"COLOR_CODE_WHT"] ");
      break;
      case LogType_raw: break;
    }
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
  vprintf (formatCStr, args);
#pragma GCC diagnostic pop

  if (standardFormatting) {
    printf("\n");
  }
  if (logType == LogType_warn || logType == LogType_error) {
    // print stack trace
    void * callstack[128];
    int32_t const numFrames = backtrace(callstack, 128);
    char ** const symbols = backtrace_symbols(callstack, numFrames);
    printf("\t");
    if (symbols != NULL) {
      for (int32_t i = 1; i < numFrames; ++ i) { // skip log function
        char * const parenStr = strchr(symbols[i], '(');
        char * const plusStr = strchr(symbols[i], '+');
        PULE_assert(parenStr != NULL && plusStr != NULL);
        if ((int32_t)(plusStr - parenStr) > 1) {
          if (strncmp(parenStr+1, "main", 4) == 0) { break; }
          printf(
            " -> %.*s",
            (int32_t)(plusStr - parenStr-1), parenStr+1
          );
        }
      }
      free(symbols);
    }
    printf("\n\n");
  }
}

void puleLog(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_normal, true, formatCStr, args);
  va_end(args);
}

void puleLogDebug(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_debug, true, formatCStr, args);
  va_end(args);
}

void puleLogWarn(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_warn, true, formatCStr, args);
  va_end(args);
}

void puleLogError(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_error, true, formatCStr, args);
  va_end(args);
  if (errorSegfaultsEnabled) {
    raise(SIGSEGV);
  }
}

void puleLogDev(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_dev, true, formatCStr, args);
  va_end(args);
}

void puleLogLn(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_normal, false, formatCStr, args);
  va_end(args);
}

void puleLogRaw(char const * const formatCStr, ...) {
  va_list args;
  va_start(args, formatCStr);
  logger(LogType_raw, false, formatCStr, args);
  va_end(args);
}
