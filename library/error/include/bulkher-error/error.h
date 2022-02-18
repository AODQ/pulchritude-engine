#pragma once

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

char const * pluginLabel();

typedef struct {
  char const * description;
  uint32_t id;
} PulError;

uint32_t puleConsumeError(PulError const error);
