#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

#include <cstdio>

#include <algorithm>
#include <string>
#include <vector>

namespace {

struct LocalCommandInfo {
  std::vector<size_t> ids;
  int64_t currentHeadIdsIdx = -1;
};

LocalCommandInfo getLocalCommands(PuleDsValue const commandsFileValue) {
  PuleDsValue const commandsValue = (
    puleDsObjectMember(commandsFileValue, "commands")
  );
  PuleDsValueArray const commandsAsArray = puleDsAsArray(commandsValue);

  int64_t const currentHeadIdx = (
    puleDsAsUSize(puleDsObjectMember(commandsFileValue, "current-head-idx"))
  );
  int64_t const branchHeadIdx = (
    puleDsAsUSize(puleDsObjectMember(commandsFileValue, "branch-head-idx"))
  );

  PULE_assert(currentHeadIdx != -1);
  PULE_assert(branchHeadIdx != -1);

  int64_t itHeadIdx = branchHeadIdx;
  LocalCommandInfo localCommand;

  while (true) {
    PuleDsValue const commandValue = commandsAsArray.values[itHeadIdx];

    // store index into localCommands for current command
    if (itHeadIdx == currentHeadIdx) {
      localCommand.currentHeadIdsIdx = localCommand.ids.size();
    }

    // iterate parent
    localCommand.ids.emplace_back(itHeadIdx);
    int64_t const parentHeadIdx = (
      puleDsAsI64(puleDsObjectMember(commandValue, "parent"))
    );
    if (parentHeadIdx == -1) {
      break;
    }
    itHeadIdx = parentHeadIdx;
  }
  PULE_assert(localCommand.currentHeadIdsIdx != -1);

  return localCommand;
}

} // namespace

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

void applyRedoUndo(
  bool const trueForwardFalseRewind,
  PuleAllocator const allocator,
  PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  int64_t levels = puleDsAsI64(puleDsObjectMember(input, "levels"));
  if (levels < 0) {
    PULE_error(1, "levels must be positive");
    return;
  }

  // get command info
  PuleDsValue const commandsFileValue = (
    puleAssetPdsLoadFromFile(allocator, "./editor/commands.pds", error)
  );
  if (puleErrorExists(error)) { return; }

  // get a 'slice' of the actions (from 0 to current to branch head)
  LocalCommandInfo const localCommand = getLocalCommands(commandsFileValue);
  PULE_assert(localCommand.currentHeadIdsIdx >= 0);

  // clamp levels to slice (ex it's okay if the user wants --level 99999)
  levels = (
    std::clamp(
      levels,
      localCommand.currentHeadIdsIdx,
      (
        static_cast<int64_t>(localCommand.ids.size())
        - localCommand.currentHeadIdsIdx
      )
    )
  );

  { // apply undos, store undos to main
    PuleDsValueArray const commandsAsArray = (
      puleDsAsArray(puleDsObjectMember(commandsFileValue, "commands"))
    );
    PuleDsValue const commandToApplyValue = (
      puleDsAssignObjectMember(
        main,
        puleStringViewCStr("commands-to-apply"),
        puleDsCreateArray(allocator)
      )
    );
    puleDsAssignObjectMember(
      main,
      puleStringViewCStr("command-true-forward-false-rewind"),
      puleDsCreateI64(trueForwardFalseRewind)
    );
    for (int64_t levelIdx = 0; levelIdx < levels; ++ levelIdx) {
      int64_t const itHeadsIdIdx = (
        localCommand.currentHeadIdsIdx
        + (trueForwardFalseRewind ? (-levelIdx-1) : levelIdx)
      );
      PuleDsValue const commandValue = (
        commandsAsArray.values[localCommand.ids[itHeadsIdIdx]]
      );
      PuleDsValue const itValue = (
        puleDsAppendArray(commandToApplyValue, puleDsCreateObject(allocator))
      );
      puleDsAssignObjectMember(
        itValue,
        puleStringViewCStr("command"),
        puleDsValueCloneRecursively(
          puleDsObjectMember(commandValue, "command"),
          allocator
        )
      );
      puleDsAssignObjectMember(
        itValue,
        puleStringViewCStr("parameters"),
        puleDsValueCloneRecursively(
          puleDsObjectMember(commandValue, "parameters"),
          allocator
        )
      );
    }
  }

  { // write new current idx
    int64_t newCurrentHeadIdsIdx = -1;
    {
      // undo goes 'forward' in ids (because they are recorded backwards)
      newCurrentHeadIdsIdx = (
        localCommand.currentHeadIdsIdx
        + (trueForwardFalseRewind ? -(levels) : levels)
      );
    }
    PULE_assert(newCurrentHeadIdsIdx >= 0);
    PULE_assert(
      newCurrentHeadIdsIdx < static_cast<int64_t>(localCommand.ids.size())
    );

    puleDsOverwriteObjectMember(
      commandsFileValue,
      puleStringViewCStr("current-head-idx"),
      puleDsCreateI64(localCommand.ids[newCurrentHeadIdsIdx])
    );
  }

  puleAssetPdsWriteToFile(commandsFileValue, "editor/commands.pds", error);
  // fallthrough to destructor
  puleDsDestroy(commandsFileValue);
}

} // C

extern "C" {

void undotreeUndo(
  PuleAllocator const allocator,
  PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  applyRedoUndo(false, allocator, main, input, error);
}

void undotreeRedo(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {

  applyRedoUndo(true, allocator, main, input, error);
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
  bool const showall = (
    puleDsAsBool(puleDsObjectMember(input, "all"))
  );

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

  // grab a slice of commits to show;
  //  start at head, get to current, then go 5 past if possible
  struct LocalCommand {
    std::string command;
    std::string parameter;
    std::string children;
  };
  std::vector<LocalCommand> localCommands;
  size_t localCommandCurrentHeadIdx = -1ul;

  int64_t itHeadIdx = branchHeadIdx;
  // recurse command tree to print useful diagram
  while (true) {
    // get command and print info
    PuleDsValue const commandValue = commandsAsArray.values[itHeadIdx];
    PuleDsValueArray const commandLabels = (
      puleDsAsArray(puleDsObjectMember(commandValue, "command"))
    );
    PuleDsValueArray const childrenIndices = (
      puleDsAsArray(puleDsObjectMember(commandValue, "children"))
    );
    PuleDsValueObject const paramLabels = (
      puleDsAsObject(puleDsObjectMember(commandValue, "parameters"))
    );

    LocalCommand localCommand {};

    // commands
    for (size_t commandIt = 0; commandIt < commandLabels.length; ++ commandIt) {
      auto str = puleDsAsString(commandLabels.values[commandIt]);
      for (size_t strIt = 0; strIt < str.len; ++ strIt) {
        localCommand.command += str.contents[strIt];
      }
      localCommand.command += ' ';
    }

    // parameters
    for (size_t paramIt = 0; paramIt < paramLabels.length; ++ paramIt) {
      auto const paramLabelStr = paramLabels.labels[paramIt];
      auto const paramVal = paramLabels.values[paramIt];
      localCommand.parameter += "--";
      for (size_t strIt = 0; strIt < paramLabelStr.len; ++ strIt) {
        localCommand.parameter += paramLabelStr.contents[strIt];
      }
      localCommand.parameter += ' ';
      if (puleDsIsI64(paramVal)) {
        localCommand.parameter += std::to_string(puleDsAsI64(paramVal));
      } else if (puleDsIsF64(paramVal)) {
        localCommand.parameter += std::to_string(puleDsAsF64(paramVal));
      } else if (puleDsIsString(paramVal)) {
        auto const paramValStr = puleDsAsString(paramVal);
        for (size_t strIt = 0; strIt < paramValStr.len; ++ strIt) {
          localCommand.parameter += paramValStr.contents[strIt];
        }
      } else if (puleDsIsArray(paramVal)) {
        localCommand.parameter += "[[TODO]]";
      } else if (puleDsIsObject(paramVal)) {
        localCommand.parameter += "[[TODO]]";
      }
      localCommand.parameter += ' ';
    }

    // children
    if (childrenIndices.length > 1) {
      localCommand.children = (
        "[" + std::to_string(childrenIndices.length) + " branches]"
      );
    }
    /*for (size_t childIt = 0; childIt < childrenIndices.length; ++ childIt) {*/

    // store index into localCommands for current command
    if (itHeadIdx == currentHeadIdx) {
      localCommandCurrentHeadIdx = localCommands.size();
    }

    // iterate parent
    localCommands.emplace_back(localCommand);
    int64_t const parentHeadIdx = (
      puleDsAsI64(puleDsObjectMember(commandValue, "parent"))
    );
    if (parentHeadIdx == -1) {
      break;
    }
    itHeadIdx = parentHeadIdx;
  }

  // -- print out the slice

  size_t const commandItRadius = 5;
  size_t commandItBegin = (
    localCommandCurrentHeadIdx < commandItRadius
    ? 0 : localCommandCurrentHeadIdx
  );
  size_t commandItEnd = (
    localCommandCurrentHeadIdx+commandItRadius > localCommands.size()
    ? localCommands.size() : localCommandCurrentHeadIdx+commandItRadius
  );
  PULE_assert(commandItBegin < commandItEnd);
  PULE_assert(commandItEnd <= localCommands.size());

  if (showall) {
    commandItBegin = 0;
    commandItEnd = localCommands.size();
  }

  for (
    size_t commandIt = commandItBegin;
    commandIt < commandItEnd;
    ++ commandIt
  ) {
    auto & command = localCommands[commandIt];
    printf(
      "[%c] %s%s -- %s\n",
      commandIt == localCommandCurrentHeadIdx ? '*' : ' ',
      command.command.c_str(),
      command.children.c_str(),
      command.parameter.c_str()
    );
  }

  PULE_assert(localCommandCurrentHeadIdx != -1ul);

  puleDsDestroy(commandsFileValue);
}

} // C
