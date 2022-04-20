#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

#include <cstdio>

extern "C" { // mind

PuleDsValue puldRegisterCommands(
  PuleAllocator const allocator,
  PuleError * const err
);
PuleDsValue puldRegisterCLICommands(
  PuleAllocator const allocator,
  PuleError * const err
);
void puldGuiUpdate();

} // C

extern "C" {

void undotreeUndo(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  (void)allocator;
  (void)input;
  (void)error;
  puleLog("will undo");
}

void undotreeRedo(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  (void)allocator;
  (void)input;
  (void)error;
  puleLog("will redo");
}

void undotreeShow(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  (void)allocator;
  (void)input;
  (void)error;
  printf(" | { add-file .. }\n");
  printf(" * { add-file .. }\n");
  printf(" | { add-file .. }\n");
  printf(" | { add-file .. }\n");
}

} // C
