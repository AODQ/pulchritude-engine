#include "util.h"

#include <pulchritude/allocator.h>
#include <pulchritude/asset-pds.h>
#include <pulchritude/core.h>
#include <pulchritude/file.h>
#include <pulchritude/string.hpp>

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
  - libs/ ; contains both libraries built and imported
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
  puleLogDebug("Wrote file: %s", filename.c_str());

}

bool systemExecuteErrorCheck(
  char const * const command,
  char const * const label
) {
  puleLogDebug("Executing command: %s\n", command);
  std::array<char, 2> buffer;
  FILE * pipe = popen(command, "r");
  if (!pipe) {
    puleLogError("Failed to run command");
    return false;
  }
  std::string result = "";
  std::string resultFormatted = "";
  std::string lineBuffer = "";
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    result += buffer.data();
    // print line by line to decorate it
    for (size_t it = 0; it < buffer.size(); ++ it) {
      if (buffer[it] == '\0') { break; }
      char const bufferChar = buffer[it];
      if (bufferChar == '\n') {
        if (lineBuffer.size() == 0) { continue; }
        auto str = (
          puleStringFormatDefault("  [%s] %s\n", label, lineBuffer.c_str())
        );
        resultFormatted += str.contents;
        puleStringDestroy(str);
        lineBuffer = "";
        continue;
      }
      lineBuffer += bufferChar;
    }
  }
  for (auto const & failStr : std::vector<std::string> { "FAIL", "failed" }) {
    if (result.find(failStr) != std::string::npos) {
      puleLogError("failed to run command '%s'", label);
      puleLog("%s", resultFormatted.c_str());
      return false;
    }
  }
  return true;
}


// returns list of imported library directories
static std::string generateCmakefileImportedLibs(
  PuleDsValue const projectValue
) {
  puleLogDebug("generating cmakefile imported libs");
  PuleDsValue const buildInfoValue = (
    puleDsObjectMember(projectValue, "build-info")
  );
  PuleDsValueArray const importLibs = (
    puleDsMemberAsArray(buildInfoValue, "imported-libs")
  );
  std::string importedLibsStr = "# imported libs\n";
  for (size_t libIt = 0; libIt < importLibs.length; ++ libIt) {
    PuleStringView const libType = (
      puleDsMemberAsString(importLibs.values[libIt], "type")
    );
    if (puleStringViewEqCStr(libType, "relpath")) {
      importedLibsStr += (
          std::string("add_subdirectory(")
        + puleDsMemberAsString(importLibs.values[libIt], "path").contents
        + "/build-husk/"
        + ")\n"
      );
    } else {
      PULE_assert(false && "Unknown lib type");
    }
  }
  return importedLibsStr;
}

static void generateMasterCmakefile(
  PuleDsValue const projectValue,
  std::string const & projectPathLabel
) {
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

  { // add library subdirs
    PuleDsValueArray const libs = (
      puleDsMemberAsArray(buildInfoValue, "libs")
    );
    for (size_t libIt = 0; libIt < libs.length; ++ libIt) {
      subdirs += (
          std::string("add_subdirectory(")
        + puleDsMemberAsString(libs.values[libIt], "path").contents
        + std::string(")\n")
      );
    }
  }

  cmakeContents = (
    std::regex_replace(cmakeContents, std::regex("%add_subdirs"), subdirs)
  );
  cmakeContents = (
    std::regex_replace(
      cmakeContents, std::regex("%project-path"), projectPathLabel
    )
  );

  cmakeContents = (
    std::regex_replace(
      cmakeContents,
      std::regex("%add_imported_subdirs"),
      generateCmakefileImportedLibs(projectValue)
    )
  );

  dumpToFile("build-husk/CMakeLists.txt", cmakeContents);
}

static void generateAssets(PuleDsValue const projectValue) {
  PuleDsValue const assetsValue = (
    puleDsObjectMember(projectValue, "assets")
  );
  if (assetsValue.id == 0) { return; }
  PuleDsValueArray const assetShaders = (
    puleDsMemberAsArray(assetsValue, "compile-shaders")
  );
  // TODO this should be in the cmake file probably, idk
  for (size_t shaderIt = 0; shaderIt < assetShaders.length; ++ shaderIt) {
    PuleDsValue const shader = assetShaders.values[shaderIt];
    PuleStringView const shaderType = puleDsMemberAsString(shader, "type");
    if (puleStringViewEqCStr(shaderType, "render")) {
      PuleStringView const fragmentPath = (
        puleDsMemberAsString(shader, "fragment-path")
      );
      PuleStringView const vertexPath = (
        puleDsMemberAsString(shader, "vertex-path")
      );
      puleLog(
        "Compiling shader: %s | %s",
        fragmentPath.contents, vertexPath.contents
      );
      // TODO this is pretty gross, but it works for now

      auto fp = popen(
        (
          std::string("glslangValidator -V -o ")
          + "assets/" + std::string(fragmentPath.contents)
          + std::string(".spv ")
          + "assets/" + std::string(fragmentPath.contents)
          + " 2>&1"
        ).c_str(),
        "r"
      );
      char strbuff[1024];
      while (fgets(strbuff, sizeof(strbuff), fp) != NULL) {
        puleLog("%s", strbuff);
      }
      pclose(fp);
      strbuff[0] = '\0';
      fp = popen(
        (
          std::string("glslangValidator -V -o ")
          + "assets/" + std::string(vertexPath.contents)
          + std::string(".spv ")
          + "assets/" + std::string(vertexPath.contents)
          + " 2>&1"
        ).c_str(),
        "r"
      );
      while (fgets(strbuff, sizeof(strbuff), fp) != NULL) {
        puleLog("%s", strbuff);
      }
      pclose(fp);
    }
  }
}

// will generate list of source files for consumption by CMake `target_sources`
static std::string generateCmakeSourceFiles(
  PuleDsValueArray const files,
  std::string const & projectPathLabel
) {
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
        sourceFiles += std::string("${") + projectPathLabel + "}/" + file;
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
      generateCmakeSourceFiles(
        puleDsMemberAsArray(pluginValue, "known-files"),
        "CMAKE_SOURCE_DIR"
      )
    + (
      generateCmakeSourceFiles(
        puleDsMemberAsArray(pluginValue, "generated-hidden-files"),
        "CMAKE_SOURCE_DIR"
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

static void generateLibraryCmakefile(
  PuleDsValue const libValue,
  std::string const & projectPathLabel
) {
  puleLogDebug("Create library cmakefile");
  #include "mixin-cmake-library.inl"
  // TODO this is just a copy of the plugin cmakefile function, perhaps merge
  auto libName = (
    std::string(puleDsMemberAsString(libValue, "name").contents)
  );
  // replace lib-name
  cmakeContents = (
    std::regex_replace(cmakeContents, std::regex("%lib-name"), libName)
  );
  // replace project-path
  cmakeContents = (
    std::regex_replace(
      cmakeContents, std::regex("%project-path"), projectPathLabel
    )
  );

  // replace source-files
  std::string const sourceFiles = (
      generateCmakeSourceFiles(
        puleDsMemberAsArray(libValue, "known-files"),
        projectPathLabel
      )
    + (
      generateCmakeSourceFiles(
        puleDsMemberAsArray(libValue, "generated-hidden-files"),
        projectPathLabel
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

  // replace linked-libraries
  std::string linkedLibs = "";
  PuleDsValueArray const linkedLibsArray = (
    puleDsMemberAsArray(libValue, "linked-libraries")
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
      + puleDsMemberAsString(libValue, "path").contents
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
      // count '/', to the base filepath, in order to make the symbolic link
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
      // prepend correct number of ../
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
      "assets/project.pds"_psv,
      &err
    )
  );
  if (puleErrorConsume(&err)) {
    return false;
  }

  // create husk directory
  puleLogDebug("Creating husk directory");
  if (puleFilesystemPathExists("build-husk"_psv)) {
    puleFileRemoveRecursive("build-husk"_psv);
  }
  puleFileDirectoryCreate("build-husk"_psv);
  puleFileDirectoryCreate("build-husk/applications"_psv);
  puleFileDirectoryCreate("build-husk/build-install"_psv);
  puleFileDirectoryCreate("build-husk/install"_psv);
  // theoretically CMake will install the below, but if there are no targets
  // then it won't
  puleFileDirectoryCreate("build-husk/install/bin"_psv);
  puleFileDirectoryCreate("build-husk/install/bin/plugins"_psv);
  puleFileDirectoryCreate("build-husk/plugins"_psv);
  puleFileDirectoryCreate("build-husk/library"_psv);
  // symlink to imported-libs
  if (puleFilesystemPathExists("imported-libs"_psv)) {
    puleFilesystemSymlinkCreate(
      "../imported-libs"_psv,
      "build-husk/imported-libs"_psv
    );
  }

  PuleDsValue const buildInfoValue = (
    puleDsObjectMember(projectValue, "build-info")
  );

  // run 'puledit generate' on all imported libraries
  PuleDsValueArray const importedLibs = (
    puleDsMemberAsArray(buildInfoValue, "imported-libs")
  );
  for (size_t ilIt = 0; ilIt < importedLibs.length; ++ ilIt) {
    auto const & lib = importedLibs.values[ilIt];
    auto const libType = puleDsMemberAsString(lib, "type");
    if (puleStringViewEqCStr(libType, "relpath")) {
      auto const libPath = puleDsMemberAsString(lib, "path");
      if (
        !puleFilesystemPathExists(
          puleCStr(
            (std::string(libPath.contents) + "/assets/project.pds").c_str()
          )
        )
      ) {
        puleLogError("Failed to find project.pds for %s", libPath.contents);
        PULE_assert(false);
      }
      puleLog("Running puledit generate on %s", libPath.contents);
      if (
        !systemExecuteErrorCheck(
          (
              std::string("cd ")
            + std::string(libPath.contents)
            + std::string("; puledit generate 2>&1")
          ).c_str(),
          libPath.contents
        )
      ) {
        PULE_assert(false && "Failed to run puledit generate");
      }
      puleLog("Finished running puledit generate on %s", libPath.contents);
    } else {
      PULE_assert(false && "Unknown lib type");
    }
  }

  // create symbolic link to include
  PuleStringView const exePath = puleFilesystemExecutablePath();
  std::string includePath = std::string(exePath.contents) + "/engine-include";
  puleLog("Include path: %s", includePath.c_str());
  if (
    !puleFilesystemSymlinkCreate(
      // TODO get this path from a known relpath
      puleCStr(includePath.c_str()),
      "build-husk/engine-include"_psv
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

  { // create library husk
    PuleDsValueArray const libs = (
      puleDsMemberAsArray(buildInfoValue, "libs")
    );
    puleLogDebug("Creating library husk for %u libraries", libs.length);
    for (size_t libIt = 0; libIt < libs.length; ++ libIt) {
      if (!generateHuskDirectory(libs.values[libIt])) {
        return false;
      }
    }
    // library needs to symlink to the absolute include path
    puleFilesystemSymlinkCreate(
      "../../library/include"_psv,
      "build-husk/library/include"_psv
    );
  }

  // generate CMake files
  puleLogDebug("Generating CMake files");
  std::string const projectPathLabel = (
    std::string(puleDsMemberAsString(projectValue, "project-name").contents)
  );
  generateMasterCmakefile(projectValue, projectPathLabel);

  { // generate plugin CMake files
    PuleDsValueArray const plugins = (
      puleDsMemberAsArray(buildInfoValue, "plugins")
    );
    for (size_t pluginIt = 0; pluginIt < plugins.length; ++ pluginIt) {
      generatePluginCmakefile(plugins.values[pluginIt]);
    }
  }

  { // generate library CMake files
    PuleDsValueArray const libs = (
      puleDsMemberAsArray(buildInfoValue, "libs")
    );
    for (size_t libIt = 0; libIt < libs.length; ++ libIt) {
      generateLibraryCmakefile(libs.values[libIt], projectPathLabel);
    }
  }

  puleLogDebug("Finished preparing for build");
  return true;
}

// this isn't in pulchritude because system commands don't really belong as
// a first party plugin, at least for now. However to issue CMake + build
// commands from just C there aren't many other options
std::string systemExecute(char const * const command, bool printout=false) {
  puleLogDebug("Executing command: %s\n", command);
  std::array<char, 2> buffer;
  std::string result = "";
  FILE * pipe = popen(command, "r");
  if (!pipe) {
    return "fail";
  }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    if (printout) {
      printf("%s", buffer.data());
    }
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
    "../../assets"_psv,
    "build-husk/install/assets"_psv
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
      " ../ 2>&1",
      false
    )
  );
  //printf("-----cmake:\n%s\n----", cmakeResult.c_str());
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
      " ninja install 2>&1",
      true
    )
  );
  //printf("-----build:\n%s\n----", buildResult.c_str());
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
  generateBuildHusk();
  if (!runPuleDataProcessing(allocator, error)) {
    return false;
  }
  return runBuild(allocator, error);
}
bool editorAssetRefresh(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  [[maybe_unused]] PuleError * const error
) {
  PuleError err = puleError();
  PuleDsValue const projectValue = (
    puleAssetPdsLoadFromFile(
      puleAllocateDefault(),
      "assets/project.pds"_psv,
      &err
    )
  );
  if (puleErrorConsume(&err)) { return false; }
  generateAssets(projectValue);
  return true;
}
bool editorGenerateInitiate(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  [[maybe_unused]] PuleError * const error
) {
  // generate build-husk
  return generateBuildHusk();
}

bool editorInitProject(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  [[maybe_unused]] PuleError * const error
) {
  // can only initialize if project doesn't exist
  if (puleFilesystemPathExists("assets/project.pds"_psv)) {
    puleLogError("Project already exists");
    return false;
  }

  // create assets & src dir
  if (!puleFilesystemPathExists("assets"_psv)) {
    puleFileDirectoryCreate("assets"_psv);
  }
  if (!puleFilesystemPathExists("src"_psv)) {
    puleFileDirectoryCreate("src"_psv);
  }

  PuleStringView projectName = puleDsMemberAsString(input, "name");
  if (puleDsObjectMember(input, "support-gpu").id == 0) {
    puleLogError("Project GPU support must be provided");
    return false;
  }
  bool projectGpuSupport = puleDsMemberAsBool(input, "support-gpu");
  if (projectName.len == 0) {
    puleLogError("Project name must be provided");
    return false;
  }
  puleLogDev("project gpu support: %d", projectGpuSupport);

  std::string projectPds = R"(
project-name: "%s",
supported-languages: "CXX C",
entry-payload: { },
build-info: {
  applications: [ ],
  plugins: [
    {
      name: "%s",
      path: "src",
      known-files: [
        "src/main.cpp",
      ],
      generated-hidden-files: [],
      linked-libraries: [
        %gpu
        "pulchritude-allocator",
        "pulchritude-asset-pds",
        "pulchritude-asset-script-task-graph",
        "pulchritude-error",
        "pulchritude-file",
        "pulchritude-log",
        "pulchritude-math",
        "pulchritude-parser",
        "pulchritude-platform",
        "pulchritude-plugin",
        "pulchritude-script",
        "pulchritude-string",
        "pulchritude-text",
        "pulchritude-time",
      ],
    },
  ],
},
  )";
  std::string projectGpuSupportMixin = R"(
        "pulchritude-asset-font",
        "pulchritude-asset-image",
        "pulchritude-asset-pds",
        "pulchritude-asset-shader-module",
        "pulchritude-asset-shader-module",
        "pulchritude-camera",
        "pulchritude-gfx-debug",
        "pulchritude-gpu",
        "pulchritude-imgui",
        "pulchritude-imgui-engine",
        "pulchritude-render-graph",
  )";
  // replace %s with project name
  projectPds = std::regex_replace(
    projectPds, std::regex("%s"), projectName.contents
  );
  // replace %gpu with gpu support
  projectPds = std::regex_replace(
    projectPds,
    std::regex("%gpu"),
    projectGpuSupport ? projectGpuSupportMixin : ""
  );
  dumpToFile("assets/project.pds", projectPds);


  dumpToFile("src/main.cpp", R"(
#include <pulchritude/plugin.hpp>
#include <pulchritude/log.hpp>

extern "C" {

PulePluginType pulcPluginType() { return PulePluginType_component; }

void pulcComponentLoad([[maybe_unused]] PulePluginPayload const payload) {
  puleLog("Hello world");
}


void pulcComponentUnload([[maybe_unused]] PulePluginPayload const payload) {
}

} // extern C
  )");

  return true;
}
} // extern C

void buildClean() {
  puleLog("Cleaning build");
  puleFileRemoveRecursive("build-husk"_psv);
}
