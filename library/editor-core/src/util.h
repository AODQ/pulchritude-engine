#pragma once

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-data-serializer/data-serializer.h>
#include <pulchritude-error/error.h>

#include <string>

extern "C" {
bool editorBuildInitiate(
  PuleAllocator const allocator,
  PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
);
}

bool refreshEcsMainComponentList(
  PuleAllocator const allocator,
  PuleError * const error
);

void buildClean();
std::string systemExecute(char const * const command);
void systemExecuteLog(char const * const command);
void dumpToFile(std::string const & filename, std::string contents);
