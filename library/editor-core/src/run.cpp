#include "util.h"

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-error/error.h>
#include <pulchritude-file/file.h>
#include <pulchritude-gpu/gpu.h>
#include <pulchritude-imgui/imgui.h>
#include <pulchritude-platform/platform.h>
#include <pulchritude-plugin/plugin.h>

#include <string>
#include <vector>

extern "C" {
bool editorBuildRunApplication(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  if (!editorBuildInitiate(allocator, main, input, error)) {
    return false;
  }
  PuleString const filesystemPath = (
    puleFilesystemCurrentPath(puleAllocateDefault())
  );
  std::string execute = "";
  {
    // launch application with correct plugin path
    PuleStringView const exePath = puleFilesystemExecutablePath();
    PuleString const currentPath = (
      puleFilesystemCurrentPath(puleAllocateDefault())
    );
    // cd & remove
    execute += "cd build-husk/install/bin; echo $PWD ;";
    execute += "rm pulchritude-application 2> /dev/null || true;";
    // link the application relatively
    execute += std::string("ln -s ") + std::string(exePath.contents);
    execute += "/pulchritude-application ";
    execute += " pulchritude-application;";
    // execute with plugin paths + user params
    execute += "";
    bool const isDebug = puleDsMemberAsBool(input, "debug");
    bool const isGuiEditor = puleDsMemberAsBool(input, "gui-editor");
    bool const isErrorSegfaults = puleDsMemberAsBool(input, "error-segfaults");
    bool const isEarlyExit = puleDsMemberAsBool(input, "early-exit");
    bool const runWithGdb = puleDsMemberAsBool(input, "gdb");
    bool const runWithValgrind = puleDsMemberAsBool(input, "valgrind");
    bool const runWithStrace = puleDsMemberAsBool(input, "strace");
    bool const debugLayer = puleDsMemberAsBool(input, "debug-layer");
    bool const clear = puleDsMemberAsBool(input, "clear");
    PULE_assert(
      !runWithValgrind || !runWithGdb
      && "Cannot run with valgrind and gdb at the same time"
    );
    PULE_assert(
      !runWithStrace || !runWithGdb
      && "Cannot run with strace and gdb at the same time"
    );
    PULE_assert(
      !runWithStrace || !runWithValgrind
      && "Cannot run with strace and valgrind at the same time"
    );
    if (clear) {
      execute += " clear ; ";
    }
    execute += "exec ";
    if (runWithStrace) {
      execute += "strace ";
    }
    if (runWithGdb) {
      execute += "gdb -ex run --args ";
    }
    if (runWithValgrind) {
      execute += "valgrind ";
    }
    execute += "./pulchritude-application ";
    execute += "--plugin-path " + std::string(exePath.contents) + "/plugins/ ";
    execute += (
      "--plugin-path "
      + std::string(currentPath.contents)
      + "/build-husk/install/bin/plugins/ "
    );
    execute += (
      "--asset-path "
      + std::string(currentPath.contents)
      + "/build-husk/install/ "
    );
    execute += " --allow-plugin-reload ";

    if (isDebug) {
      execute += "--debug ";
    }
    if (isGuiEditor) {
      execute += "--gui-editor ";
    }
    if (isErrorSegfaults) {
      execute += "--error-segfaults ";
    }
    if (isEarlyExit) {
      execute += "--early-exit ";
    }
    if (debugLayer) {
      execute += "--plugin-layer debug ";
    }
    // execute += (gdb ? "" : " | c++filt");

    // now add application v:arargs
    puleAssetPdsWriteToStdout(input);
    PuleDsValueArray const varargs = puleDsMemberAsArray(input, "varargs");
    execute += " -- ";
    for (size_t varargIt = 0; varargIt < varargs.length; ++ varargIt) {
      execute += (
          std::string(" ")
        + puleDsAsString(varargs.values[varargIt]).contents
        + " "
      );
    }

    printf("------\n\t%s\n\n--------", execute.c_str());

    // cleanup
    puleStringDestroy(filesystemPath);
    puleStringDestroy(currentPath);
  }
  // execute
  systemExecuteInteractive(execute.c_str());
  return true;
}

bool editorGui(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  puleDsObjectMemberAssign(
    input,
    puleCStr("gui-editor"),
    puleDsCreateBool(true)
  );
  return editorBuildRunApplication(allocator, main, input, error);
}

} // C
