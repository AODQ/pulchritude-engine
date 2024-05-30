#pragma once

#include <pulchritude/allocator.h>
#include <pulchritude/data-serializer.h>
#include <pulchritude/error.h>

#include <string>

extern "C" {
bool editorBuildInitiate(
  PuleAllocator const allocator,
  PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
);
bool editorGenerateInitiate(
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

bool refreshEcsMainSystemList(
  PuleAllocator const allocator,
  PuleError * const error
);

void buildClean();
std::string systemExecute(char const * const command);
void systemExecuteLog(char const * const command);
void systemExecuteInteractive(char const * const command);
void dumpToFile(std::string const & filename, std::string contents);
