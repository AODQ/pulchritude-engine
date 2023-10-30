#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

extern "C" {

PuleDsValue puldRegisterCommands(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleError * const error
) {
  // all here must also replicate in CLI commands
  char const * const commandRegisterCStr = PULE_multilineString(
    commands: [
      {label: "undo", apply-label: "editorUndotreeUndo",},
      {label: "redo", apply-label: "editorUndotreeRedo",},
      {label: "undotree-show", apply-label: "editorUndotreeShow",},
      {label: "build", apply-label: "editorBuildInitiate",},
      {label: "run", apply-label: "editorBuildRunApplication",},
      {label: "gui", apply-label: "editorGui",},
      {
        label: "plugin-new",
        forward-label: "editorPluginNew",
        rewind-label: "editorPluginNewRewind",
      },
    ],
  );

  return (
    puleAssetPdsLoadFromRvalStream(
      allocator,
      puleStreamReadFromString(puleCStr(commandRegisterCStr)),
      error
    )
  );
}

PuleDsValue puldRegisterCLICommands(
  PuleAllocator const allocator,
  PuleError * const error
) {
  char const * const CLIRegisterCStr = PULE_multilineString(
    undo: [ { label: "levels", type: "int", default-value: 1, }, ],
    redo: [ { label: "levels", type: "int", default-value: 1, }, ],
    undotree: {
      show: [ { label: "all", type: "bool", default-value: false, }, ],
    },
    build: [],
    gui: [
      { label: "debug", type: "bool", default-value: false, },
      { label: "error-segfaults", type: "bool", default-value: false, },
      { label: "early-exit", type: "bool", default-value: false, },
      { label: "debug-layer", type: "bool", default-value: false, },
      { label: "gdb",   type: "bool", default-value: false, },
      { label: "valgrind",   type: "bool", default-value: false, },
      { label: "strace",   type: "bool", default-value: false, },
      { label: "clear", type: "bool", default-value: false, },
    ],
    run: [
      { label: "debug", type: "bool", default-value: false, },
      { label: "error-segfaults", type: "bool", default-value: false, },
      { label: "early-exit", type: "bool", default-value: false, },
      { label: "debug-layer", type: "bool", default-value: false, },
      { label: "gdb",   type: "bool", default-value: false, },
      { label: "valgrind",   type: "bool", default-value: false, },
      { label: "strace",   type: "bool", default-value: false, },
      { label: "clear", type: "bool", default-value: false, },
    ],
    plugin: {
      new: [
        { label: "name", type: "string", opt: false, },
        { label: "type", type: "string", opt: false, },
      ],
    },
  );
  return (
    puleAssetPdsLoadFromRvalStream(
      allocator,
      puleStreamReadFromString(puleCStr(CLIRegisterCStr)),
      error
    )
  );
}

}
