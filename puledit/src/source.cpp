/* pulchritude editor | github.com/aodq/pulchritude-engine | aodq.net */

#include <pulchritude/allocator.h>
#include <pulchritude/asset-pds.h>
#include <pulchritude/core.hpp>
#include <pulchritude/data-serializer.h>
#include <pulchritude/error.h>
#include <pulchritude/file.h>
#include <pulchritude/log.h>
#include <pulchritude/plugin.h>
#include <pulchritude/tui.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace { // error
PuleError err = { {}, 0, nullptr, 0, 0 };
}

namespace { // command registry

template <typename T>
void tryLoadFn(T & fn, size_t const pluginId, char const * const label) {
  fn = reinterpret_cast<T>(pulePluginLoadFnTry(pluginId, label));
}

struct CommandRegistry {
  std::string label;
  bool (*forward)(
    PuleAllocator const,
    PuleDsValue const,
    PuleDsValue const,
    PuleError * const
  );
  bool (*rewind)(
    PuleAllocator const,
    PuleDsValue const,
    PuleDsValue const,
    PuleError * const
  );
  bool (*apply)(
    PuleAllocator const,
    PuleDsValue const,
    PuleDsValue const,
    PuleError * const
  );
};

std::unordered_map<size_t, CommandRegistry> commandRegistry;

} // namespace command registry

namespace { // argument parsing

// this would be read back as `{ asset: {info: {source: {"some-string"}}} }
// and written to as ./pulchritude asset info --source some-string
// it has to cascade from object to array

enum class ActionDirection { Forward, Rewind };
enum class ApplyToActionHistory { Yes, No };

// where ./application <arguments>,
// arguments = <commands> <parameters>
bool parseArguments(
  PuleDsValue const cliArgumentLayout,
  PuleDsValue const userArguments,
  ActionDirection const actionDirection,
  ApplyToActionHistory const applyToActionHistory
) {
  /* if (*puleLogDebugEnabled()) { */
  /*   puleLogDebug("--- cli argument layout ---"); */
  /*   puleAssetPdsWriteToStdout(cliArgumentLayout); */
  /*   puleLogDebug("------ user arguments -----"); */
  /*   puleAssetPdsWriteToStdout(userArguments); */
  /* } */
  // if we get here we can assume the command is correct and any validation is
  //   redundant
  PuleDsValueArray const userCommand = (
    puleDsAsArray(puleDsObjectMember(userArguments, "command"))
  );
  std::string userCommandJoined = "";

  // userCommands.join('-')
  for (
    size_t userCommandIt = 0;
    userCommandIt < userCommand.length;
    ++ userCommandIt
  ) {
    PuleStringView const userCommandWord = (
      puleDsAsString(userCommand.values[userCommandIt])
    );

    if (userCommandIt != 0) {
      userCommandJoined += "-";
    }
    userCommandJoined += (
      std::string_view(userCommandWord.contents, userCommandWord.len)
    );
  }

  size_t const userCommandHash = (
    puleStringViewHash(puleCStr(userCommandJoined.c_str()))
  );
  auto const command = commandRegistry.find(userCommandHash);
  if (command == commandRegistry.end()) {
    puleLogError("failed to locate command: '%s'", userCommandJoined.c_str());
    return false;
  }

  PuleAllocator const allocator = puleAllocateDefault();

  if (command->second.forward) {
    bool result = true;
    if (actionDirection == ActionDirection::Forward) {
      result = command->second.forward(
        allocator,
        PuleDsValue { 0 },
        puleDsObjectMember(userArguments, "parameters"),
        &err
      );
    } else {
      result = command->second.rewind(
        allocator,
        PuleDsValue { 0 },
        puleDsObjectMember(userArguments, "parameters"),
        &err
      );
    }
    if (puleErrorConsume(&err) || !result) {
      puleLogError(
        "failed to %s action '%s'",
        actionDirection == ActionDirection::Forward ? "forward" : "reverse",
        command->second.label.c_str()
      );
      return false;
    }
    // TODO after command succeeds, add to commands list
    PuleDsValue const commandsFileValue = (
      puleAssetPdsLoadFromFile(
        allocator,
        puleCStr("./assets/puldata/commands.pds"),
        &err
      )
    );
    if (puleErrorConsume(&err)) { return false; }

    puleLogDebug("----- assets/puldata/commands.pds ------");
    if (*puleLogDebugEnabled()) {
      puleAssetPdsWriteToStdout(commandsFileValue);
    }
    puleLogDebug("--------------------------------");

    PuleDsValue const commandsValue = (
      puleDsObjectMember(commandsFileValue, "commands")
    );
    PuleDsValueArray const commandsAsArray = puleDsAsArray(commandsValue);
    int64_t const parentHeadIdx = (
      puleDsAsI64(puleDsObjectMember(commandsFileValue, "current-head-idx"))
    );

    puleDsObjectMemberOverwrite(
      commandsFileValue,
      puleCStr("branch-head-idx"),
      puleDsCreateI64(commandsAsArray.length)
    );

    puleDsObjectMemberOverwrite(
      commandsFileValue,
      puleCStr("current-head-idx"),
      puleDsCreateI64(commandsAsArray.length)
    );

    // append the command to the children of current, if we have any command
    if (commandsAsArray.length != 0) {
      puleDsArrayAppend(
        puleDsObjectMember(commandsAsArray.values[parentHeadIdx], "children"),
        puleDsCreateI64(commandsAsArray.length)
      );
    }

    { // create new command and append to commands list
      PuleDsValue const newCommand = puleDsCreateObject(allocator);
      puleDsObjectMemberAssign(
        newCommand,
        puleCStr("parent"),
        puleDsCreateI64(parentHeadIdx)
      );
      puleDsObjectMemberAssign(
        newCommand,
        puleCStr("command"),
        puleDsValueCloneRecursively(
          puleDsObjectMember(userArguments, "command"),
          allocator
        )
      );
      puleDsObjectMemberAssign(
        newCommand,
        puleCStr("parameters"),
        puleDsValueCloneRecursively(
          puleDsObjectMember(userArguments, "parameters"),
          allocator
        )
      );
      puleDsObjectMemberAssign(
        newCommand,
        puleCStr("children"),
        puleDsCreateArray(allocator)
      );

      puleDsArrayAppend(commandsValue, newCommand);
    }

    if (applyToActionHistory == ApplyToActionHistory::Yes) {
      puleAssetPdsWriteToFile(
        commandsFileValue,
        puleCStr("assets/puldata/commands.pds"),
        &err
      );
    }
    puleDsDestroy(commandsFileValue);
    if (puleErrorConsume(&err)) { return false; }
  }
  else
  if (command->second.apply) {
    PuleDsValue const mainValue = puleDsCreateObject(allocator);
    bool result = command->second.apply(
      allocator,
      mainValue,
      puleDsObjectMember(userArguments, "parameters"),
      &err
    );

    if (puleErrorConsume(&err) || !result) { return false; }

    // if the user requests to apply more actions, (ex undo/redo), apply them
    PuleDsValue const commandsToApply = (
      puleDsObjectMember(mainValue, "commands-to-apply")
    );
    if (commandsToApply.id > 0) {
      PuleDsValue const commandForward = (
        puleDsObjectMember(mainValue, "command-true-forward-false-rewind")
      );
      PULE_assert(commandForward.id > 0);
      ActionDirection const newActionDirection = (
        puleDsAsBool(commandForward)
        ? ActionDirection::Forward : ActionDirection::Rewind
      );
      PuleDsValueArray const commandsToApplyAsArray = (
        puleDsAsArray(commandsToApply)
      );
      for (
        size_t commandIt = 0;
        commandIt < commandsToApplyAsArray.length;
        ++ commandIt
      ) {
        if (
          !parseArguments(
            cliArgumentLayout,
            commandsToApplyAsArray.values[commandIt],
            newActionDirection,
            ApplyToActionHistory::No
          )
        ) {
          return false;
        }
      }
    }
    puleDsDestroy(mainValue);
  }
}

} // namespace argument parsing

extern "C" {

void loadEditorDataFromPlugins(
  PulePluginInfo const plugin,
  void * const userdata
) {
  (void)userdata;
  auto const cliArgumentLayout = (
    *reinterpret_cast<PuleDsValue const *>(userdata)
  );

  PuleError error = puleError();

  // register editor commands from plugins
  PuleDsValue (*registerCommandsFn)(
    PuleAllocator const,
    PuleError * const
  ) = nullptr;
  ::tryLoadFn(registerCommandsFn, plugin.id, "puldRegisterCommands");
  if (registerCommandsFn) {
    puleLogDebug("Registering editor commands from plugin '%s'", plugin.name);
    PuleDsValue const registeredCommands = (
      registerCommandsFn(puleAllocateDefault(), &error)
    );
    if (!puleErrorConsume(&error) && registeredCommands.id != 0) {
      /* puleLogDebug("Registry info: "); */
      /* if (puleLogDebugEnabled()) { */
      /*   puleAssetPdsWriteToStdout(registeredCommands); */
      /*   printf("\n"); */
      /* } */
      PuleDsValueArray commands = (
        puleDsAsArray(puleDsObjectMember(registeredCommands, "commands"))
      );
      for (size_t it = 0; it < commands.length; ++ it) {
        PuleDsValue const command = commands.values[it];
        PuleStringView const label = (
          puleDsAsString(puleDsObjectMember(command, "label"))
        );
        CommandRegistry registry;
        registry.forward = registry.rewind = registry.apply = nullptr;
        registry.label = (
          std::string(std::string_view(label.contents, label.len))
        );
        PuleDsValue const
          forwardLabel = puleDsObjectMember(command, "forward-label"),
          rewindLabel = puleDsObjectMember(command, "rewind-label"),
          applyLabel = puleDsObjectMember(command, "apply-label")
        ;
        if (forwardLabel.id != 0) {
          ::tryLoadFn(
            registry.forward,
            plugin.id,
            puleDsAsString(forwardLabel).contents
          );
        }
        if (rewindLabel.id != 0) {
          ::tryLoadFn(
            registry.rewind,
            plugin.id,
            puleDsAsString(rewindLabel).contents
          );
        }
        if (applyLabel.id != 0) {
          ::tryLoadFn(
            registry.apply,
            plugin.id,
            puleDsAsString(applyLabel).contents
          );
        }
        if (
             static_cast<bool>(registry.forward)
          != static_cast<bool>(registry.rewind)
        ) {
          puleLogError(
            "registry command '%s' has %s but not %s",
            label.contents,
            (registry.forward ? "forward" : "rewind"),
            (registry.forward ? "rewind" : "forward")
          );
        }
        if ((registry.forward || registry.rewind) && registry.apply) {
          puleLogError(
            "registry command '%s' has both forward/rewind and apply",
            label.contents
          );
        }
        if (!registry.forward && !registry.rewind && !registry.apply) {
          puleLogError(
            "registry command '%s' has no rewind/forward/apply action",
            label.contents
          );
        }
        ::commandRegistry.emplace(puleStringViewHash(label), registry);
      }
      puleDsDestroy(registeredCommands);
    }
  }

  PuleDsValue (*registerCLICommands)(
    PuleAllocator const,
    PuleError * const
  ) = nullptr;
  ::tryLoadFn(registerCLICommands, plugin.id, "puldRegisterCLICommands");
  if (registerCLICommands) {
    PuleDsValue const registeredCLICommands = (
      registerCLICommands(puleAllocateDefault(), &error)
    );
    if (!puleErrorConsume(&error) && registeredCLICommands.id != 0) {
      PuleDsValueObject const cliCommands = (
        puleDsAsObject(registeredCLICommands)
      );
      for (size_t it = 0; it < cliCommands.length; ++ it) {
        PuleStringView const label = cliCommands.labels[it];
        PuleDsValue const value = cliCommands.values[it];
        PULE_assert(
          puleDsObjectMember(cliArgumentLayout, label.contents).id == 0
        );
        puleDsObjectMemberAssign(
          cliArgumentLayout,
          label,
          puleDsValueCloneRecursively(value, puleAllocateDefault())
        );
      }
      puleDsDestroy(registeredCLICommands);
    }
  }
}

} // C

int32_t main(
  [[maybe_unused]] int32_t const userArgumentLength,
  [[maybe_unused]] char const * const * const userArguments
) {
  //*puleLogDebugEnabled() = true;
  *puleLogErrorSegfaultsEnabled() = true;
  { // load plugins
    PuleStringView const path = puleFilesystemExecutablePath();
    PuleString pathPlugin = (
      puleStringFormatDefault("%s/plugins/", path.contents)
    );
    PuleStringView const pathView = puleStringView(pathPlugin);
    pulePluginsLoad(&pathView, 1);
    puleStringDestroy(pathPlugin);
  }

  // TODO parse plugins
  PuleDsValue const cliArgumentLayout = (
    puleDsCreateObject(puleAllocateDefault())
  );
  pulePluginIterate(
    ::loadEditorDataFromPlugins,
    const_cast<PuleDsValue *>(&cliArgumentLayout)
  );

  bool userRequestedHelp = false;
  PuleDsValue const userArgs = (
    puleAssetPdsLoadFromCommandLineArguments(
      PuleAssetPdsCommandLineArgumentsInfo {
        .allocator = puleAllocateDefault(),
        .layout = cliArgumentLayout,
        .argumentLength = userArgumentLength, .arguments = userArguments,
        .userRequestedHelpOutNullable = &userRequestedHelp,
      },
      &err
    )
  );
  puleScopeExit { puleDsDestroy(userArgs); };
  if (puleErrorConsume(&err)) {
    return 0;
  }

  // check if user passed anything in
  if (userRequestedHelp) {}
  else if (userArgs.id != 0) {
    bool result = (
      parseArguments(
        cliArgumentLayout,
        userArgs,
        ActionDirection::Forward,
        ApplyToActionHistory::Yes
      )
    );
    if (!result) return 1;
  } else {
    puleLog("no parameters passed in");
  }

  pulePluginsFree();

  return 0;
}

// ubj pna v erpbyyrpg fbzrbar jub fnlf nyy v unir fgbccrq yvfgravat bapr erireg
// genfu guvf genfu gung nofgenpg njnl nyy nznytbhf jbeevrf juvpu rire unir orra
// v nethr va snvgu ybat ybat fvtu jungrire vg gnxrf qevsg njnl qevsg qevsg fvtu

// below is for TUI at some point
  /* // if no input then request */
  /* //PuleTuiWindow const window = puleTuiInitialize(); */
  /* //auto const winDim = puleTuiWindowDim(window); */
  /* bool const hadInputValue = inputValue != ""; */
  /* while (!hadInputValue) { */
  /*   // -- render */
  /*   puleTuiClear(window); */
  /*   puleTuiMoveCursor(window, PuleI32v2{10, winDim.y-1}); */
  /*   puleTuiRenderString( */
  /*     window, puleCStr("Command: "), PuleF32v3(0.9f, 0.2f, 0.2f) */
  /*   ); */
  /*   puleTuiRenderString( */
  /*     window, puleCStr(inputValue.c_str()), PuleF32v3(0.9f, 0.2f, 0.2f) */
  /*   ); */
  /*   renderAutocompletion(window, inputValue); */

  /*   // -- parse input */
  /*   puleTuiRefresh(window); */
  /*   int32_t c = puleTuiReadInputCharacterBlocking(window); */

  /*   // exit on ctrl-c */
  /*   if (c == ('c' & 0x1f)) { */
  /*     exit(0); */
  /*   } */
  /*   // ignore on backspace, and remove previous input */
  /*   if (c == 0407 || c == 127 || c == '\b') { */
  /*     if (inputValue.size() > 0) { */
  /*       inputValue.resize(inputValue.size()-1); */
  /*     } */
  /*     continue; */
  /*   } */

  /*   // on ctrl-u clear input */
  /*   if (c == ('u' & 0x1f)) { */
  /*     inputValue = ""; */
  /*     continue; */
  /*   } */

  /*   // submit command on new line */
  /*   if (c == '\n' || c == '\r') { */
  /*     break; */
  /*   } */

  /*   // parse as valid input */
  /*   inputValue += static_cast<char>(c); */
  /* } */
