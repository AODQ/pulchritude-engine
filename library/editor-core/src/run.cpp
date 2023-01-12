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
    execute += "unbuffer ./pulchritude-application ";
    execute += "--plugin-path " + std::string(exePath.contents) + "/plugins/ ";
    execute += (
      "--plugin-path "
      + std::string(currentPath.contents)
      + "/build-husk/install/bin/plugins/ "
    );

    if (puleDsObjectMember(input, "debug").id != 0) {
      execute += "--debug";
    }

    // cleanup
    puleStringDestroy(filesystemPath);
    puleStringDestroy(exePath);
    puleStringDestroy(currentPath);
  }
  // execute
  systemExecuteLog(execute.c_str());
  return true;
}
}
