#include "util.h"

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-file/file.h>
#include <pulchritude-string/string.h>

#include <regex>

bool refreshEcsMainComponentList(
  PuleAllocator const allocator,
  PuleError * const error
) {
  PuleDsValue const ecsMainValue = (
    puleAssetPdsLoadFromFile(
      allocator,
      puleCStr("./puldata/ecs.pds"),
      error
    )
  );
  if (puleErrorExists(error)) { return false; }

  PuleDsValue const plugins = puleDsObjectMember(ecsMainValue, "plugins");
  PuleDsValueObject const pluginsObject = puleDsAsObject(plugins);

  for (size_t pluginIt = 0; pluginIt < pluginsObject.length; ++ pluginIt) {
    PuleDsValueArray const components = (
      puleDsAsArray(
        puleDsObjectMember(pluginsObject.values[pluginIt], "components")
      )
    );
    PuleString const pluginsComponentHeaderPath = (
      puleStringFormatDefault(
        "build-husk/plugins/%s/components/module.h",
        pluginsObject.labels[pluginIt].contents
      )
    );
    PuleString const pluginsComponentSourcePath = (
      puleStringFormatDefault(
        "build-husk/plugins/%s/components/module.c",

        pluginsObject.labels[pluginIt].contents
      )
    );
    { // write out header
      PuleFile const pluginsComponentHeaderFile = (
        puleFileOpen(
          puleStringView(pluginsComponentHeaderPath),
          PuleFileDataMode_text,
          PuleFileOpenMode_writeOverwrite,
          error
        )
      );
      if (puleErrorExists(error)) {
        puleDsDestroy(ecsMainValue);
         return 0;
      }
      std::string content = "";
      content += "#include \"pulchritude-ecs/ecs.h\"\n";
      content += "#ifdef __cplusplus\nextern \"C\" {\n#endif\n";
      content += "\nvoid pulcRegisterComponents(PuleEcsWorld const world);\n";
      content += "#ifdef __cplusplus\n}\n#endif\n";
      puleFileWriteString(
        pluginsComponentHeaderFile,
        puleCStr(content.c_str())
      );
      puleFileClose(pluginsComponentHeaderFile);
    }
    { // write out source file
      PuleFile const pluginsComponentSourceFile = (
        puleFileOpen(
          puleStringView(pluginsComponentSourcePath),
          PuleFileDataMode_text,
          PuleFileOpenMode_writeOverwrite,
          error
        )
      );
      if (puleErrorExists(error)) {
        puleDsDestroy(ecsMainValue);
        return false;
      }
      std::string content = "";
      // write headers
      content += "#include \"pulchritude-ecs/ecs.h\"\n";
      for (size_t compIt = 0; compIt < components.length; ++ compIt) {
        content += "#include \"";
        content += (
          puleDsMemberAsString(components.values[compIt], "name").contents
        );
        content += ".h\"\n";
      }
      // write registry
      content += (
        "void pulcRegisterComponents(PuleEcsWorld const world) {\n"
      );
      for (size_t compIt = 0; compIt < components.length; ++ compIt) {
        std::string label = (
          puleDsMemberAsString(components.values[compIt], "name").contents
        );
        // fix label, such that node-foo.h becomes PulcComponentNodeFoo
        label[0] = ::toupper(label[0]);
        for (size_t it = 0; it < label.size()-1; ++ it) {
          if (label[it+1] == '-') {
            label.erase(it+1, 1);
            label[it+1] = ::toupper(label[it+1]);
          }
        }
        label = "PulcComponent" + label;

        content += (
          "  { // " + label + "\n"
          "    PuleEcsComponentCreateInfo const compInfo = {\n"
          "        .label = \"" + label + "\",\n"
          "        .byteLength = sizeof(" + label + "),\n"
          "        .byteAlignment = _Alignof(" + label + "),\n"
          "    };\n"
          "    puleLogDebug(\"Registering component '" + label + "'\");\n"
          "    puleEcsComponentCreate(world, compInfo);\n"
          "  }\n"
        );
      }
      content += "}";
      puleFileWriteString(
        pluginsComponentSourceFile,
        puleCStr(content.c_str())
      );
      puleFileClose(pluginsComponentSourceFile);
    }
  }

  puleDsDestroy(ecsMainValue);
  return true;
}

namespace {
bool pluginExists(PuleStringView const pluginName, PuleError * const error) {
  PuleDsValue const projectValue = (
    puleAssetPdsLoadFromFile(
      puleAllocateDefault(),
      puleCStr("project.pds"),
      error
    )
  );
  if (puleErrorConsume(error)) { return false; }

  PuleDsValue const plugins = (
    puleDsObjectMember(
      puleDsObjectMember(projectValue, "build-info"),
      "plugins"
    )
  );
  PuleDsValueArray const pluginsArray = puleDsAsArray(plugins);
  for (size_t it = 0; it < pluginsArray.length; ++ it) {
    PuleStringView const name = (
      puleDsMemberAsString(pluginsArray.values[it], "name")
    );
    if (puleStringViewEq(name, pluginName)) {
      puleDsDestroy(projectValue);
      return true;
    }
  }
  puleDsDestroy(projectValue);
  return false;
}
}

/*
Responsible for handling plugins
*/

extern "C" { // editor new/rewind
bool editorPluginNew(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  [[maybe_unused]] PuleError * const error
) {
  // THIS NEEDS TO BE REWORKEWD TO USE PROJECT.pds
  /* PuleStringView const inputName = puleDsMemberAsString(input, "name"); */
  /* PuleStringView const inputType = puleDsMemberAsString(input, "type"); */
  /* (void)inputType; */

  /* // check doesn't already exist */
  /* if (pluginExists(inputName, error)) { */
  /*     PULE_error(1, "plugin '%s' already exists", inputName.contents); */
  /*     return false; */
  /* } */

  /* // load plugins data */
  /* PuleDsValue const pluginsValue = ( */
  /*   puleAssetPdsLoadFromFile( */
  /*     allocator, */
  /*     puleCStr("./puldata/plugins.pds"), */
  /*     error */
  /*   ) */
  /* ); */
  /* if (puleErrorExists(error)) { return false; } */
  /* PuleDsValue const plugins = puleDsObjectMember(pluginsValue, "plugins"); */
  /* PuleString const pluginPath = ( */
  /*   puleStringFormat(allocator, "plugins/%s", inputName.contents) */
  /* ); */
  /* PuleString const sourcePath = ( */
  /*   puleStringFormat(allocator, "plugins/%s/source", inputName.contents) */
  /* ); */
  /* if (puleFilesystemPathExists(puleStringView(pluginPath))) { */
  /*   PULE_error(1, "plugin path '%s' already exists", pluginPath.contents); */
  /*   puleDsDestroy(pluginsValue); */
  /*   puleStringDestroy(pluginPath); */
  /*   puleStringDestroy(sourcePath); */
  /*   return false; */
  /* } */

  /* // record its existence, creating an initial entry for source file */
  /* { */
  /*   PuleDsValue const pluginInfo = ( */
  /*     puleDsValueCloneRecursively(input, allocator) */
  /*   ); */
  /*   PuleDsValue const pluginSourceFiles = puleDsCreateArray(allocator); */
  /*   puleDsArrayAppend( */
  /*     pluginSourceFiles, */
  /*     puleDsCreateString(puleCStr("source.cpp")) */
  /*   ); */
  /*   puleDsArrayAppend(plugins, pluginInfo); */
  /* } */
  /* puleAssetPdsWriteToFile( */
  /*   pluginsValue, puleCStr("./puldata/plugins.pds"), error */
  /* ); */
  /* if (puleErrorExists(error)) { return false; } */

  /* // create the plugin directory */
  /* puleFileDirectoryCreate(puleStringView(pluginPath)); */
  /* puleFileDirectoryCreate(puleStringView(sourcePath)); */

  /* // create the plugin cmakelists */
  /* #include "mixin-cmake-plugin-library.inl" */
  /* for (auto const & label : { "name", }) { */
  /*   cmakeContents = ( */
  /*     std::regex_replace( */
  /*       cmakeContents, */
  /*       std::regex(std::string("%") + std::string(label)), */
  /*       std::string(inputName.contents) */
  /*     ) */
  /*   ); */
  /* } */
  /* dumpToFile( */
  /*   std::string(pluginPath.contents) + "/CMakeLists.txt", */
  /*   cmakeContents */
  /* ); */

  /* // create the base source file */
  /* #include "mixin-plugin-source.inl" */
  /* mixinPluginSource = ( */
  /*   std::regex_replace( */
  /*     mixinPluginSource, */
  /*     std::regex("\\$\\$"), */
  /*     std::string("#") */
  /*   ) */
  /* ); */
  /* dumpToFile( */
  /*   std::string(sourcePath.contents) + "/source.cpp", */
  /*   mixinPluginSource */
  /* ); */

  /* puleDsDestroy(pluginsValue); */
  /* puleStringDestroy(pluginPath); */
  /* puleStringDestroy(sourcePath); */
  return true;
}

bool editorPluginNewRewind(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  [[maybe_unused]] PuleError * const error
) {
  // THIS NEEDS TO BE REWORKEWD TO USE PROJECT.pds
  /* PuleDsValue const pluginsValue = ( */
  /*   puleAssetPdsLoadFromFile( */
  /*     allocator, */
  /*     puleCStr("./puldata/plugins.pds"), */
  /*     error */
  /*   ) */
  /* ); */
  /* if (puleErrorExists(error)) { return false; } */

  /* // remove entry from array, but don't remove from filesystem to prevent */
  /* // accidental deletion of files/source-code */

  /* PuleStringView const inputName = puleDsMemberAsString(input, "name"); */
  /* PuleDsValue const plugins = puleDsObjectMember(pluginsValue, "plugins"); */
  /* PuleDsValueArray const pluginsArray = puleDsAsArray(plugins); */

  /* bool found = false; */
  /* for (size_t it = 0; it < pluginsArray.length; ++ it) { */
  /*   PuleStringView const name = ( */
  /*     puleDsMemberAsString(pluginsArray.values[it], "name") */
  /*   ); */
  /*   if (puleStringViewEq(name, inputName)) { */
  /*     found = true; */
  /*     puleDsArrayRemoveAt(plugins, it); */
  /*     break; */
  /*   } */
  /* } */
  /* if (!found) { */
  /*   PULE_error(1, "plugin '%s' not found", inputName.contents); */
  /*   return false; */
  /* } */

  /* puleAssetPdsWriteToFile( */
  /*   pluginsValue, puleCStr("./puldata/plugins.pds"), error */
  /* ); */
  /* if (puleErrorExists(error)) { return false; } */

  /* puleDsDestroy(pluginsValue); */
  return true;
}
}

extern "C" { // ECS component add/remove
bool editorEcsComponentAdd(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  // split out input
  PuleStringView const inputName = puleDsMemberAsString(input, "name");
  PuleStringView const inputLibrary = (
    puleDsMemberAsString(input, "library")
  );

  // check plugin exists
  if (!pluginExists(inputLibrary, error)) {
    PULE_error(
      1,
      "referenced library '%s' does not exist", inputLibrary.contents
    );
    return false;
  }

  // get main ECS data from project file
  PuleDsValue const ecsMainValue = (
    puleAssetPdsLoadFromFile(
      allocator,
      puleCStr("./puldata/ecs.pds"),
      error
    )
  );
  if (puleErrorExists(error)) { return false; }

  // get the plugin ECS object, creating if necessary
  PuleDsValue const ecsPluginValue = (
    puleDsObjectMember(ecsMainValue, "plugins")
  );
  if (!puleDsObjectMember(ecsPluginValue, inputLibrary.contents).id) {
    PuleDsValue const pluginInfo = puleDsCreateObject(allocator);
    puleDsObjectMemberAssign(ecsPluginValue, inputLibrary, pluginInfo);
    { // fill in plugin info
      puleDsObjectMemberAssign(
        pluginInfo, puleCStr("components"), puleDsCreateArray(allocator)
      );
    }
  }

  // fetch the components array from respective ecs main's plugin
  PuleDsValue const ecsPluginComponentValue = (
    puleDsObjectMember(ecsPluginValue, "components")
  );
  PuleDsValueArray const componentsArray = (
    puleDsAsArray(ecsPluginComponentValue)
  );

  // check doesn't already exist
  for (size_t it = 0; it < componentsArray.length; ++ it) {
    PuleStringView const name = (
      puleDsMemberAsString(componentsArray.values[it], "name")
    );
    if (puleStringViewEq(name, inputName)) {
      PULE_error(1, "component '%s' already exists", name.contents);
      return false;
    }
  }

  // create main component directory if it doesn't exist yet
  PuleString const componentDirectoryPath = (
    puleStringFormat(allocator, "plugins/%s/components", inputLibrary.contents)
  );
  PuleString const componentHeaderPath = (
    puleStringFormat(allocator, "components/include/%s", inputName.contents)
  );
  if (!puleFilesystemPathExists(puleStringView(componentDirectoryPath))) {
    puleFileDirectoryCreate(puleStringView(componentDirectoryPath));
  }

  // record its existence into components
  {
    PuleDsValue const componentRegistryInfo = puleDsCreateObject(allocator);
    puleDsObjectMemberAssign(
      componentRegistryInfo,
      puleCStr("name"),
      puleDsCreateString(inputName)
    );
    puleDsObjectMemberAssign(
      componentRegistryInfo,
      puleCStr("header"),
      puleDsCreateString(puleStringView(componentHeaderPath))
    );
    puleDsArrayAppend(ecsPluginComponentValue, componentRegistryInfo);
  }
  puleAssetPdsWriteToFile(ecsMainValue, puleCStr("./puldata/ecs.pds"), error);
  if (puleErrorExists(error)) { return false; }

  // create default component file
  if (!puleFilesystemPathExists(puleStringView(componentHeaderPath))) {
    PuleFile const componentFile = (
      puleFileOpen(
        puleStringView(componentHeaderPath),
        PuleFileDataMode_text,
        PuleFileOpenMode_writeOverwrite,
        error
      )
    );
    if (puleErrorExists(error)) { return false; }

    #include "mixin-ecs-component-default-file.inl"
    mixinEcsComponentDefaultFile = (
      std::regex_replace(
        mixinEcsComponentDefaultFile,
        std::regex("%component-name"),
        std::string(inputName.contents)
      )
    );
    mixinEcsComponentDefaultFile  = (
      std::regex_replace(
        mixinEcsComponentDefaultFile,
        std::regex("\\$\\$"),
        std::string("#")
      )
    );

    puleFileWriteString(
      componentFile,
      puleCStr(mixinEcsComponentDefaultFile.c_str())
    );
    puleFileClose(componentFile);
  }

  // cleanup
  puleDsDestroy(ecsMainValue);
  puleStringDestroy(componentHeaderPath);
  puleStringDestroy(componentDirectoryPath);

  // refresh the source listing
  return refreshEcsMainComponentList(allocator, error);
}

bool editorEcsComponentRemove(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  // split input
  PuleStringView const inputName = puleDsMemberAsString(input, "name");
  PuleStringView const inputLibrary = (
    puleDsMemberAsString(input, "library")
  );

  // check plugin exists
  if (!pluginExists(inputLibrary, error)) {
    PULE_error(
      1,
      "referenced library '%s' does not exist", inputLibrary.contents
    );
    return false;
  }

  // get main
  PuleDsValue const ecsMainValue = (
    puleAssetPdsLoadFromFile(
      allocator,
      puleCStr("./puldata/ecs.pds"),
      error
    )
  );
  if (puleErrorExists(error)) { return false; }

  // fetch the components array from ecs main
  PuleDsValue const ecsPluginValue = (
    puleDsObjectMember(ecsMainValue, "plugins")
  );
  PuleDsValue const ecsPluginComponentValue = (
    puleDsObjectMember(ecsPluginValue, "components")
  );
  PuleDsValueArray const componentsArray = (
    puleDsAsArray(ecsPluginComponentValue)
  );

  // check doesn't already exist
  bool found = false;
  for (size_t it = 0; it < componentsArray.length; ++ it) {
    PuleStringView const name = (
      puleDsMemberAsString(componentsArray.values[it], "name")
    );
    if (puleStringViewEq(name, inputName)) {
      found = true;
      puleDsArrayRemoveAt(ecsPluginComponentValue, it);
      break;
    }
  }
  if (!found) {
    PULE_error(1, "component '%s' not found", inputName.contents);
    return false;
  }

  puleAssetPdsWriteToFile(ecsMainValue, puleCStr("./puldata/ecs.pds"), error);
  if (puleErrorExists(error)) { return false; }

  puleDsDestroy(ecsMainValue);
  return true;
}

}
