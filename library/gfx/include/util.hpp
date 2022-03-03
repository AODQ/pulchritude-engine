#pragma once

// returns true if an error was processed
bool processGlErrors(
  char const * const file,
  int const line,
  char const * const function
);

#define UTIL_processGlErrors() processGlErrors(__FILE__, __LINE__, __func__)
