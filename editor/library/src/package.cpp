#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

extern "C" {

PuleDsValue puldRegisterCommands(
  PuleAllocator const allocator,
  PuleError * const error
) {
  char const * const commandRegisterCStr = PULE_multilineString(
    commands: [
      {label: "undo", apply-label: "undotreeUndo",},
      {label: "redo", apply-label: "undotreeRedo",},
      {label: "undotree-show", apply-label: "undotreeShow",},
      {
        label: "asset-add",
        forward-label: "assetAddForward",
        rewind-label: "assetAddRewind",
      },
      {
        label: "asset-info",
        apply-label: "assetInfo",
      },
    ],
  );

  return (
    puleAssetPdsLoadFromRvalStream(
      allocator,
      puleStreamReadFromString(puleStringViewCStr(commandRegisterCStr)),
      error
    )
  );
}

PuleDsValue puldRegisterCLICommands(
  PuleAllocator const allocator,
  PuleError * const error
) {
  char const * const CLIRegisterCStr = PULE_multilineString(
    undo: [],
    redo: [],
    undotree: {
      show: [],
    },
    asset: {
      add: [
        { label: "source", type: "url", },
        { label: "destination", type: "url", },
        { label: "overwrite", type: "bool", default-value: false, },
      ],
      info: [
        { label: "source", type: "url", opt: false, },
      ],
    },
  );

  return (
    puleAssetPdsLoadFromRvalStream(
      allocator,
      puleStreamReadFromString(puleStringViewCStr(CLIRegisterCStr)),
      error
    )
  );
}

} // C
