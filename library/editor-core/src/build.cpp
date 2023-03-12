#include "util.h"

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-core/core.h>
#include <pulchritude-file/file.h>
#include <pulchritude-string/string.h>

#include <array>
#include <regex>
#include <string>

/*
This generates CMake configuration, runs CMake and
then runs ninja

It is also responsible for generating source files, such as bindings.

The structure looks like

- project-name/
  - builds/
  - pulchritude-engine/ ; symlink
  - applications/
  - plugins/
  - build-husk/
  project.pds ; this is what we write into and used to build
*/

void dumpToFile(
  std::string const & filename,
  std::string contents
) {
  // remove trailing spaces
  contents = std::regex_replace(contents, std::regex("[ ]+\n"), "\n");

  // open file, write contents & close
  PuleError err = puleError();
  PuleFile const masterFile = (
    puleFileOpen(
      puleCStr(filename.c_str()),
      PuleFileDataMode_text,
      PuleFileOpenMode_writeOverwrite,
      &err
    )
  );
  puleErrorConsume(&err);
  puleFileWriteBytes(
    masterFile,
    PuleArrayView {
      .data = reinterpret_cast<uint8_t const *>(contents.c_str()),
      .elementStride = 1,
      .elementCount = contents.size()
    }
  );
  puleFileClose(masterFile);
}

static void generateMasterCmakefile(PuleDsValue const projectValue) {
  puleLogDebug("Create master cmakefile");
  #include "mixin-cmake-master.inl"
  // insert constants to top level CMake
  for (auto const & label : { "project-name", "supported-languages", }) {
    cmakeContents = (
      std::regex_replace(
        cmakeContents,
        std::regex(std::string("%") + std::string(label)),
        std::string(puleDsMemberAsString(projectValue, label).contents)
      )
    );
  }

  PuleDsValue const buildInfoValue = (
    puleDsObjectMember(projectValue, "build-info")
  );
  std::string subdirs = "";
  { // add plugin subdirs
    PuleDsValueArray const plugins = (
      puleDsMemberAsArray(buildInfoValue, "plugins")
    );
    for (size_t pluginIt = 0; pluginIt < plugins.length; ++ pluginIt) {
      subdirs += (
        std::string("add_subdirectory(")
        + puleDsMemberAsString(plugins.values[pluginIt], "path").contents
        + std::string(")\n")
      );
    }
  }

  cmakeContents = (
    std::regex_replace(cmakeContents, std::regex("%add_subdirs"), subdirs)
  );

  dumpToFile("build-husk/CMakeLists.txt", cmakeContents);
}

// will generate list of source files for consumption by CMake `target_sources`
static std::string generateCmakeSourceFiles(PuleDsValueArray const files) {
  std::string sourceFiles;
  for (size_t fileIt = 0; fileIt < files.length; ++ fileIt) {
    auto const file = (
      std::string(puleDsAsString(files.values[fileIt]).contents)
    );
    // TODO get this from proper source languages
    for (auto const & language : std::vector<std::string>{".cpp", ".c"}) {
      if (
        file.size() > language.size()
        && file.substr(file.size()-language.size()) == language
      ) {
        sourceFiles += std::string("${CMAKE_SOURCE_DIR}/") + file;
        sourceFiles += "\n    ";
      }
    }
  }
  return sourceFiles;
}

static void generatePluginCmakefile(PuleDsValue const pluginValue) {
  puleLogDebug("Create plugin cmakefile");
  #include "mixin-cmake-plugin.inl"
  auto pluginName = (
    std::string(puleDsMemberAsString(pluginValue, "name").contents)
  );
  // replace plugin-name
  cmakeContents = (
    std::regex_replace(cmakeContents, std::regex("%plugin-name"), pluginName)
  );

  // replace source-files
  std::string const sourceFiles = (
    generateCmakeSourceFiles(puleDsMemberAsArray(pluginValue, "known-files"))
    + (
      generateCmakeSourceFiles(
        puleDsMemberAsArray(pluginValue, "generated-hidden-files")
      )
    )
  );
  cmakeContents = (
    std::regex_replace(
      cmakeContents,
      std::regex("%source-files"),
      sourceFiles
    )
  );

  // replace linked libraries
  std::string linkedLibs = "";
  PuleDsValueArray const linkedLibsArray = (
    puleDsMemberAsArray(pluginValue, "linked-libraries")
  );
  for (size_t libIt = 0; libIt < linkedLibsArray.length; ++ libIt) {
    linkedLibs += (
      std::string(puleDsAsString(linkedLibsArray.values[libIt]).contents)
    );
    if (libIt+1 < linkedLibsArray.length) {
      linkedLibs += "\n    ";
    }
  }
  cmakeContents = (
    std::regex_replace(
      cmakeContents,
      std::regex("%linked-libraries"),
      linkedLibs
    )
  );
  dumpToFile(
    (
      std::string("build-husk/")
      + puleDsMemberAsString(pluginValue, "path").contents
      + std::string("/CMakeLists.txt")
    ),
    cmakeContents
  );
}

static bool generateHuskDirectory(PuleDsValue const value) {
  puleLogDebug("Creating husk directory for ");
  puleAssetPdsWriteToStdout(value);
  { // create directory
    PuleString const huskpath = (
      puleStringFormatDefault(
        "build-husk/%s",
        puleDsMemberAsString(value, "path").contents
      )
    );
    puleFileDirectoryCreate(puleStringView(huskpath));
    puleStringDestroy(huskpath);
  }

  { // create symbolic link to plugin/application files
    PuleDsValueArray const knownFiles = (
      puleDsMemberAsArray(value, "known-files")
    );
    for (size_t fileIt = 0; fileIt < knownFiles.length; ++ fileIt) {
      // symlink knownFile -> husk file
      std::string knownFilepath = (
        puleDsAsString(knownFiles.values[fileIt]).contents
      );
      { // create directories to the filepath
        auto path = knownFilepath;
        path = (
          path.replace(knownFilepath.find_last_of("/"), std::string::npos, "")
        );
        puleFileDirectoryCreateRecursive(
          puleCStr(("build-husk/" + path).c_str())
        );
      }
      PuleString const huskFilepath = (
        puleStringFormatDefault("build-husk/%s", knownFilepath.c_str())
      );
      // count '/', to the base filepath, in order to make the symbolink link
      // relative to the husk (rather than an absolute path)
      size_t slashCount = 0;
      for (size_t it = 0; it < knownFilepath.size(); ++ it) {
        if (knownFilepath[it] != '/') {
          continue;
        }
        slashCount ++;
        // skip repeating slash
        while (it+1 < knownFilepath.size() && knownFilepath[it+1] == '/') {
          ++ it;
        }
      }
      // prepend corrrect number of ../
      for (size_t slashIt = 0; slashIt < slashCount; ++ slashIt) {
        knownFilepath = "../" + knownFilepath;
      }
      knownFilepath = "../" + knownFilepath; // get out of husk
      // symlink
      if (
        !puleFilesystemSymlinkCreate(
          puleCStr(knownFilepath.c_str()),
          puleStringView(huskFilepath)
        )
      ) {
        return false;
      }
    }
  }
  return true;
}

static bool generateBuildHusk() {
  PuleError err = puleError();

  puleLogDebug("loading project pds");
  // load project pds value
  PuleDsValue const projectValue = (
    puleAssetPdsLoadFromFile(
      puleAllocateDefault(),
      puleCStr("assets/project.pds"),
      &err
    )
  );
  if (puleErrorConsume(&err)) {
    return false;
  }

  // create husk directory
  puleLogDebug("Creating husk directory");
  if (puleFilesystemPathExists(puleCStr("build-husk"))) {
    puleFileRemoveRecursive(puleCStr("build-husk"));
  }
  puleFileDirectoryCreate(puleCStr("build-husk"));
  puleFileDirectoryCreate(puleCStr("build-husk/applications"));
  puleFileDirectoryCreate(puleCStr("build-husk/build-install"));
  puleFileDirectoryCreate(puleCStr("build-husk/install"));
  puleFileDirectoryCreate(puleCStr("build-husk/plugins"));

  PuleDsValue const buildInfoValue = (
    puleDsObjectMember(projectValue, "build-info")
  );

  // create symbolic link to include
  PuleString const exePath = (
    puleFilesystemExecutablePath(puleAllocateDefault())
  );
  std::string includePath = std::string(exePath.contents) + "/engine-include";
  puleLog("Include path: %s", includePath.c_str());
  puleStringDestroy(exePath);
  if (
    !puleFilesystemSymlinkCreate(
      // TODO get this path from a known relpath
      puleCStr(includePath.c_str()),
      puleCStr("build-husk/engine-include")
    )
  ) {
    return false;
  }

  /* { // create applications husk */
  /*   PuleDsValueArray const applications = ( */
  /*     puleDsMemberAsArray(buildInfoValue, "applications") */
  /*   ); */
  /*   for (size_t appIt = 0; appIt < applications.length; ++ appIt) { */
  /*     generateHuskDirectory(applications.values[appIt]); */
  /*   } */
  /* } */

  { // create plugins husk
    PuleDsValueArray const plugins = (
      puleDsMemberAsArray(buildInfoValue, "plugins")
    );
    puleLogDebug("Creating plugins husk for %u plugins", plugins.length);
    for (size_t pluginIt = 0; pluginIt < plugins.length; ++ pluginIt) {
      if (!generateHuskDirectory(plugins.values[pluginIt])) {
        return false;
      }
    }
  }

  // generate CMake files
  puleLogDebug("Generating CMake files");
  generateMasterCmakefile(projectValue);

  { // generate plugin CMake files
    PuleDsValueArray const plugins = (
      puleDsMemberAsArray(buildInfoValue, "plugins")
    );
    for (size_t pluginIt = 0; pluginIt < plugins.length; ++ pluginIt) {
      generatePluginCmakefile(plugins.values[pluginIt]);
    }
  }

  puleLogDebug("Finished preparing for build");
  return true;
}

// this isn't in pulchritude because system commands don't really belong as
// a first party plugin, at least for now. However to issue CMake + build
// commands from just C there aren't many other options
std::string systemExecute(char const * const command) {
  puleLogDebug("Executing command: %s\n", command);
  std::array<char, 4> buffer;
  std::string result = "";
  FILE * pipe = popen(command, "r");
  if (!pipe) {
    return "fail";
  }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
  }
  return result;
}

void systemExecuteInteractive(char const * const command) {
  system(command);
}

void systemExecuteLog(char const * const command) {
  puleLogDebug("Executing command: %s\n", command);
  std::array<char, 2> buffer;
  FILE * pipe = popen(command, "r");
  if (!pipe) {
    puleLogError("Failed to run command");
    return;
  }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    printf("%s", buffer.data());
  }
}

bool refreshAssets(PuleAllocator const allocator, PuleError * const error) {
  (void)allocator;
  (void)error;
  puleFilesystemSymlinkCreate(
    puleCStr("../../assets"),
    puleCStr("build-husk/install/assets")
  );
  // TODO strip out unnecessary metadata (like source files)
  return true;
}

bool runPuleDataProcessing(
  PuleAllocator const allocator,
  PuleError * const error
) {
  // refresh all data before building
  puleLog("--- Refreshing all pule data");
  if (!refreshEcsMainComponentList(allocator, error)) {
    return false;
  }
  if (!refreshEcsMainSystemList(allocator, error)) {
    return false;
  }
  if (!refreshAssets(allocator, error)) {
    return false;
  }
  return true;
}

bool runBuild(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleError * const error
) {
  std::string cmakeResult = (
    systemExecute(
      "cd build-husk/build-install;"
      " cmake -G \"Ninja\""
      " -DCMAKE_BUILD_TYPE=Debug"
      " -DCMAKE_INSTALL_PREFIX=../install"
      " -DCMAKE_c++_COMPILER=/usr/bin/clang++"
      " -DCMAKE_c_COMPILER=/usr/bin/clang"
      " ../ 2>&1"
    )
  );
  puleLog("-----cmake:\n%s\n----", cmakeResult.c_str());
  for (
    auto const & failStr :
    std::vector<std::string> { "FAIL", "failed", "CMake Error" }
  ) {
    if (cmakeResult.find(failStr) != std::string::npos) {
      return false;
    }
  }

  std::string buildResult = (
    systemExecute(
      "cd build-husk/build-install;"
      " ninja install 2>&1"
    )
  );
  puleLog("-----build:\n%s\n----", buildResult.c_str());
  for (auto const & failStr : std::vector<std::string> { "FAIL", "failed" }) {
    if (buildResult.find(failStr) != std::string::npos) {
      return false;
    }
  }

  puleLog("... Build seems to be succesful.");

  return true;
}

extern "C" {
bool editorBuildInitiate(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  [[maybe_unused]] PuleError * const error
) {
  // generate build-husk
  if (!generateBuildHusk()) {
    return false;
  }
  if (!runPuleDataProcessing(allocator, error)) {
    return false;
  }
  return runBuild(allocator, error);
}
}

void buildClean() {
  puleLog("Cleaning build");
  puleFileRemoveRecursive(puleCStr("build-husk"));
}
