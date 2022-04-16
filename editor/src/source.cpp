/* pulchritude editor | github.com/aodq/pulchritude-engine | aodq.net */

// ubj pna v erpbyyrpg fbzrbar jub fnlf nyy v unir fgbccrq yvfgravat bapr erireg
// lbh jnagrq gb pbqr lbh jnagrq gb pbqr lbh jnagrq gb pbqr lbh ner nyybjrq gb

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <string>
#include <unordered_map>
#include <vector>

#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-plugin/plugin.h>

namespace {
PuleError err = { {}, 0, nullptr, 0 };
}

#define PARAMETER_LAYOUT_MULTINE_STR(...) #__VA_ARGS__

// this would be read back as `{ asset: {info: {source: {"some-string"}}} }
// and written to as ./pulchritude asset info --source some-string
// it has to cascade from object to array

void parseArguments(PuleDsValue const args) {
  if (
    PuleDsValue const launch = puleDsObjectMember(args, "launch"); launch.id
  ) {
    puleLog("launching application");
    return;
  }

  if (
    PuleDsValue const init = puleDsObjectMember(args, "init"); init.id
  ) {
    puleLog("NOT YET IMPLEMENTED :(");
    return;
  }

  if (
    PuleDsValue const init = puleDsObjectMember(args, "init"); init.id
  ) {
    puleLog("initializing project");
  }

  if (
    PuleDsValue const asset = puleDsObjectMember(args, "asset"); asset.id
  ) {
    PuleDsValue assetParam;
    if (assetParam = puleDsObjectMember(asset, "add"); assetParam.id) {
      PuleStringView const source = (
        puleDsAsString(puleDsObjectMember(assetParam, "source"))
      );
      PuleStringView const destination = (
        puleDsAsString(puleDsObjectMember(assetParam, "destination"))
      );
      puleLog(
        "adding resource from '%s' to '%s'",
        source.contents,
        destination.contents
      );
    }

    /* if (assetParam = puleDsObjectMember(asset, "info"); assetParam.id) { */
    /*   PuleStringView const source = ( */
    /*     puleDsAsString(puleDsObjectMember(asset, "source")) */
    /*   ); */
    /*   puleLog("info '%s'", source.contents); */
    /*   puleLog("\tin application fileystem? %s", "NO"); */
    /*   puleLog("\twatched? %s", "NO"); */
    /*   puleLog("\tconversion? %s", "NO"); */
    /* } */
  }
}

namespace {

template <typename T>
void tryLoadFn(T & fn, size_t const pluginId, char const * const label) {
  fn = reinterpret_cast<T>(puleTryPluginLoadFn(pluginId, label));
}

struct CommandRegistry {
  std::string label;
  void (*forward)(
    PuleAllocator const,
    PuleDsValue const,
    PuleDsValue const,
    PuleError * const
  );
  void (*rewind)(
    PuleAllocator const,
    PuleDsValue const,
    PuleDsValue const,
    PuleError * const
  );
};

std::unordered_map<size_t, CommandRegistry> commandRegistry;

}

extern "C" {

void iteratePlugins(PulePluginInfo const plugin, void * const userdata) {
  (void)userdata;
  auto const cliArguments = *reinterpret_cast<PuleDsValue const *>(userdata);

  PuleError error = puleError();

  PuleDsValue (*registerCommandsFn)(
    PuleAllocator const,
    PuleError * const
  ) = nullptr;
  ::tryLoadFn(registerCommandsFn, plugin.id, "puldRegisterCommands");
  if (registerCommandsFn) {
    PuleDsValue const registeredCommands = (
      registerCommandsFn(puleAllocateDefault(), &error)
    );
    if (!puleErrorConsume(&error) && registeredCommands.id != 0) {
      PuleDsValueArray commands = (
        puleDsAsArray(puleDsObjectMember(registeredCommands, "commands"))
      );
      for (size_t it = 0; it < commands.length; ++ it) {
        PuleDsValue const command = commands.values[it];
        PuleStringView const label = (
          puleDsAsString(puleDsObjectMember(command, "label"))
        );
        CommandRegistry registry;
        registry.label = (
          std::string(std::string_view(label.contents, label.len))
        );
        ::tryLoadFn(
          registry.forward,
          plugin.id,
          puleDsAsString(puleDsObjectMember(command, "forward-label")).contents
        );
        ::tryLoadFn(
          registry.rewind,
          plugin.id,
          puleDsAsString(puleDsObjectMember(command, "rewind-label")).contents
        );
        ::commandRegistry.emplace({pdsHash(label), registry});
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
        assert(puleDsObjectMember(cliArguments, label).id == 0);
        puleDsAssignObjectMember(
          cliArguments,
          label.contents,
          puleDsValueCloneRecursively(value, puleAllocateDefault())
        );
      }
      puleDsDestroy(registeredCLICommands);
    }
  }
}

} // C

int32_t main(
  int32_t const userArgumentLength,
  char const * const * const userArguments
) {
  // TODO parse plugins
  PuleDsValue const cliArguments = puleDsCreateObject(puleAllocateDefault());
  puleIteratePlugins(::iteratePlugins, &cliArguments);

  bool userRequestedHelp = false;
  PuleDsValue const userArgs = (
    puleAssetPdsLoadFromCommandLineArguments(
      {
        .allocator = puleAllocateDefault(),
        .layout = commandLineParameterLayout,
        .argumentLength = userArgumentLength, .arguments = userArguments,
        .userRequestedHelpOutNullable = &userRequestedHelp,
      },
      &err
    )
  );
  if (puleErrorConsume(&err)) {
    return 0;
  }

  // check if user passed anything in
  if (userRequestedHelp) {}
  else if (userArgs.id != 0) {
    parseArguments(args);
  } else {
    puleLog("no parameters passed in");
  }

  puleDsDestroy(args);
}

// genfu guvf genfu gung nofgenpg njnl nyy nznytbhf jbeevrf juvpu rire unir orra
// v nethr va snvgu ybat ybat fvtu jungrire vg gnxrf qevsg njnl qevsg qevsg fvtu
