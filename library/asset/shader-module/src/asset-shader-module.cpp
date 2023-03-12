#include <pulchritude-asset/shader-module.h>

#include <pulchritude-file/file.h>
#include <pulchritude-time/time.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct AssetShaderModule {
  std::string label;
  std::string pathFragment;
  std::string pathVertex;
  PuleGfxShaderModule shaderModule;
  PuleFileWatcher watcherFragment;
  PuleFileWatcher watcherVertex;
};

std::unordered_map<uint64_t, AssetShaderModule> shaderModules;
uint64_t shaderModuleCount = 1;

} // namespace

namespace {

std::vector<uint8_t> readFileContents(PuleStringView const filename) {
  std::vector<uint8_t> filebuffer;
  PuleError err = puleError();
  PuleFile const file = (
    puleFileOpen(filename, PuleFileDataMode_text, PuleFileOpenMode_read, &err)
  );
  if (puleErrorConsume(&err)) { return {}; }
  uint64_t const filesize = puleFileSize(file);
  filebuffer.resize(filesize);
  puleFileReadBytes(
    file,
    PuleArrayViewMutable {
      .data = filebuffer.data(),
      .elementStride = 1,
      .elementCount = filebuffer.size(),
    }
  );
  puleFileClose(file);
  return filebuffer;
}

void shaderWatchFileUpdatedCallback(
  [[maybe_unused]] PuleStringView const filename,
  void * const userdata
) {
  puleSleepMicrosecond({.value=100,});
  uint64_t const assetHandle = reinterpret_cast<uint64_t>(userdata);
  AssetShaderModule & shaderModule = ::shaderModules.at(assetHandle);
  // deallocate/destroy
  if (shaderModule.shaderModule.id != 0) {
    puleGfxShaderModuleDestroy(shaderModule.shaderModule);
  }

  // load contents, need to do for all files regardless which have changed
  std::vector<uint8_t> vertexFilebuffer = (
    readFileContents(puleCStr(shaderModule.pathVertex.c_str()))
  );
  vertexFilebuffer.emplace_back('\0');
  std::vector<uint8_t> fragmentFilebuffer = (
    readFileContents(puleCStr(shaderModule.pathFragment.c_str()))
  );
  fragmentFilebuffer.emplace_back('\0');
  PuleError err = puleError();
  shaderModule.shaderModule = (
    puleGfxShaderModuleCreate(
      puleCStr(reinterpret_cast<char const *>(vertexFilebuffer.data())),
      puleCStr(reinterpret_cast<char const *>(fragmentFilebuffer.data())),
      &err
    )
  );
  puleLogDebug(
    "reloaded shader module '%s' with id %d",
    shaderModule.label.c_str(),
    shaderModule.shaderModule
  );
  puleErrorConsume(&err);
}

void shaderWatchDeallocateUserdata(void * const userdata) {
  (void)userdata;
}

} // namespace

extern "C" {

PuleAssetShaderModule puleAssetShaderModuleCreateFromPaths(
  PuleStringView const shaderModuleLabel,
  PuleStringView const pathVertex,
  PuleStringView const pathFragment
) {
  // prepare shader module
  auto const shaderModule = AssetShaderModule {
    .label = std::string(shaderModuleLabel.contents),
    .pathFragment = std::string(pathFragment.contents),
    .pathVertex = std::string(pathVertex.contents),
    .shaderModule = { .id = 0, },
    .watcherFragment = { .id = 0, },
    .watcherVertex = { .id = 0, },
  };
  shaderModules.emplace(shaderModuleCount, shaderModule);
  auto & shaderModuleRef = shaderModules.at(shaderModuleCount);
  auto * const shaderModuleUserdata = (
    reinterpret_cast<uint64_t *>(shaderModuleCount)
  );

  // setup callbacks
  shaderModuleRef.watcherVertex = puleFileWatch({
    .fileUpdatedCallback = &::shaderWatchFileUpdatedCallback,
    .deallocateUserdataCallback = &::shaderWatchDeallocateUserdata,
    .filename = puleCStr(""),
    .userdata = shaderModuleUserdata,
  });
  shaderModuleRef.watcherFragment = puleFileWatch({
    .fileUpdatedCallback = &::shaderWatchFileUpdatedCallback,
    .deallocateUserdataCallback = &::shaderWatchDeallocateUserdata,
    .filename = puleCStr(""),
    .userdata = shaderModuleUserdata,
  });

  // load shader module with callback manually
  shaderWatchFileUpdatedCallback(puleCStr(""), shaderModuleUserdata);

  puleLogDebug("new asset shader module %d", shaderModuleCount);
  return { .id = shaderModuleCount ++, };
}

PuleStringView puleAssetShaderModuleLabel(
  PuleAssetShaderModule const pMod
) {
  auto & shaderModule = shaderModules.at(pMod.id);
  return puleCStr(shaderModule.label.c_str());
}

PuleGfxShaderModule puleAssetShaderModuleGfxHandle(
  PuleAssetShaderModule const pMod
) {
  auto & shaderModule = shaderModules.at(pMod.id);
  return shaderModule.shaderModule;
}

} // C
