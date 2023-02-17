#include "util.h"

#include <pulchritude-file/file.h>

#include <string>

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
    PuleString const exePath = (
      puleFilesystemExecutablePath(puleAllocateDefault())
    );
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
    bool const isErrorSegfaults = puleDsMemberAsBool(input, "error-segfaults");
    bool const gdb = puleDsMemberAsBool(input, "gdb");
    bool const debugLayer = puleDsMemberAsBool(input, "debug-layer");
    bool const clear = puleDsMemberAsBool(input, "clear");
    if (clear) {
      execute += " clear ; ";
    }
    execute += "exec ";
    if (gdb) {
      execute += "gdb -ex run --args ";
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
      + "/build-husk/install/assets/ "
    );

    if (isDebug) {
      execute += "--debug ";
    }
    if (isErrorSegfaults) {
      execute += "--error-segfaults ";
    }
    if (debugLayer) {
      execute += "--plugin-layer debug ";
    }

    printf("------\n\t%s\n\n--------", execute.c_str());

    // cleanup
    puleStringDestroy(filesystemPath);
    puleStringDestroy(exePath);
    puleStringDestroy(currentPath);
  }
  // execute
  systemExecuteInteractive(execute.c_str());
  return true;
}
}
