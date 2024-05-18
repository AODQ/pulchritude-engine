#pragma once
#include "commands.h"
#include "gpu.h"
#include "image.h"
#include "pipeline.h"
#include "synchronization.h"

#ifdef __cplusplus
#include <pulchritude-string/string.hpp>
namespace pule {
pule::string toStr(PuleGpuResourceAccess const access);
char const * toStr(PuleGpuImageLayout const layout);
} // namespace pule
#endif // __cplusplus
