#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

#include <cstdio>

#include <string>

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

  PuleDsValue const commandsFileValue = (
    puleAssetPdsLoadFromFile(allocator, "./editor/commands.pds", error)
  );
  if (puleErrorExists(error)) {
    return;
  }

  PuleDsValue const commandsValue = (
    puleDsObjectMember(commandsFileValue, "commands")
  );
  PuleDsValueArray const commandsAsArray = puleDsAsArray(commandsValue);

  if (commandsAsArray.length == 0) {
    printf("no history found\n");
    return;
  }

  int64_t const currentHeadIdx = (
    puleDsAsUSize(puleDsObjectMember(commandsFileValue, "current-head-idx"))
  );
  int64_t const branchHeadIdx = (
    puleDsAsUSize(puleDsObjectMember(commandsFileValue, "branch-head-idx"))
  );

  PULE_assert(currentHeadIdx != -1);
  PULE_assert(branchHeadIdx != -1);

  int64_t itHeadIdx = currentHeadIdx;
  // recurse command tree to print useful diagram
  while (true) {
    // get command and print info
    PuleDsValue const commandValue = commandsAsArray.values[itHeadIdx];
    PuleDsValueArray const commandLabels = (
      puleDsAsArray(puleDsObjectMember(commandValue, "command"))
    );
    PuleDsValueObject const paramLabels = (
      puleDsAsObject(puleDsObjectMember(commandValue, "parameters"))
    );
    std::string command = "";
    for (size_t commandIt = 0; commandIt < commandLabels.length; ++ commandIt) {
      auto str = puleDsAsString(commandLabels.values[commandIt]);
      for (size_t strIt = 0; strIt < str.len; ++ strIt) {
        command += str.contents[strIt];
      }
      command += ' ';
    }

    for (size_t paramIt = 0; paramIt < paramLabels.length; ++ paramIt) {
      auto const paramLabelStr = paramLabels.labels[paramIt];
      auto const paramVal = paramLabels.values[paramIt];
      command += "--";
      for (size_t strIt = 0; strIt < paramLabelStr.len; ++ strIt) {
        command += paramLabelStr.contents[strIt];
      }
      command += ' ';
      if (puleDsIsI64(paramVal)) {
        command += std::to_string(puleDsAsI64(paramVal));
      } else if (puleDsIsF64(paramVal)) {
        command += std::to_string(puleDsAsF64(paramVal));
      } else if (puleDsIsBool(paramVal)) {
        command += std::to_string(puleDsAsBool(paramVal));
      } else if (puleDsIsString(paramVal)) {
        auto const paramValStr = puleDsAsString(paramVal);
        for (size_t strIt = 0; strIt < paramValStr.len; ++ strIt) {
          command += paramValStr.contents[strIt];
        }
      } else if (puleDsIsArray(paramVal)) {
        command += "[[TODO]]";
      } else if (puleDsIsObject(paramVal)) {
        command += "[[TODO]]";
      }
      command += ' ';
    }
    printf(
      "[%c] %s\n",
      itHeadIdx == currentHeadIdx ? '*' : ' ',
      command.c_str()
    );

    // iterate
    int64_t const parentHeadIdx = (
      puleDsAsI64(puleDsObjectMember(commandValue, "parent"))
    );

    if (parentHeadIdx == -1) {
      break;
    }
    itHeadIdx = parentHeadIdx;
  }

  puleDsDestroy(commandsFileValue);
}

} // C
