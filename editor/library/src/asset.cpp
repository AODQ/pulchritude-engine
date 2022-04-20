#include <pulchritude-allocator/allocator.h>
#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-string/string.h>

extern "C" {

PuleDsValue puldRegisterCommands(
  PuleAllocator const allocator,
  PuleError * const err
);
PuleDsValue puldRegisterCLICommands(
  PuleAllocator const allocator,
  PuleError * const err
);
void puldGuiUpdate();

} // C

extern "C" {

void assetAddForward(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  PuleStringView const source = (
    puleDsAsString(puleDsObjectMember(input, "source"))
  );
  PuleStringView const destination = (
    puleDsAsString(puleDsObjectMember(input, "destination"))
  );
  bool const overwrite = puleDsAsBool(puleDsObjectMember(input, "overwrite"));
  puleLog(
    "adding resource from '%s' to '%s'",
    source.contents,
    destination.contents
  );
  PuleDsValue const assetValue = (
    puleAssetPdsLoadFromFile(allocator, "./assets/assets.pds", error)
  );
  if (puleErrorExists(error)) {
    return;
  }

  PuleDsValue const filesValue = puleDsObjectMember(assetValue, "files");
  PuleDsValueArray const filesArray = puleDsAsArray(filesValue);

  // check exists
  // TODO need to 'simplify'/remove redundancies in path
  for (size_t it = 0; it < filesArray.length; ++ it) {
    PuleStringView const filepath = (
      puleDsAsString(puleDsObjectMember(filesArray.values[it], "path"))
    );
    if (puleStringViewEq(filepath, destination)) {
      if (!overwrite) {
        PULE_error(
          1,
          "file '%s' already exists in assets.pds (%s)",
          destination.contents
        );
        return;
      }
      puleDsArrayRemoveAt(filesValue, it);
      // there should never be any duplicates; otherwise though you could do
      // it -= 1; filesArray = puleDsAsArray(filesValue);
      break;
    }
  }

  { // append file reference
    PuleDsValue fileObj = puleDsCreateObject(allocator);
    puleDsAssignObjectMember(
      fileObj, puleStringViewCStr("path"), puleDsCreateString(destination)
    );
    puleDsAssignObjectMember(
      fileObj, puleStringViewCStr("tracking"), puleDsCreateBool(false)
    );
    puleDsAppendArray(filesValue, fileObj);
  }
  puleAssetPdsWriteToFile(assetValue, "assets/assets.pds", error);
  puleDsDestroy(assetValue);
  if (puleErrorExists(error)) {
    return;
  }

  if (!puleFileCopy(source, destination)) {
    puleDsArrayPopBack(filesValue);
    return;
  }
}

void assetAddRewind(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  PuleStringView const source = (
    puleDsAsString(puleDsObjectMember(input, "source"))
  );
  PuleStringView const destination = (
    puleDsAsString(puleDsObjectMember(input, "destination"))
  );
  puleLog("removing resource at '%s'", destination.contents);

  PuleDsValue const assetValue = (
    puleAssetPdsLoadFromFile(allocator, "./assets/assets.pds", error)
  );
  if (puleErrorExists(error)) {
    return;
  }

  { // remove file reference
    PuleDsValue const filesValue = puleDsObjectMember(assetValue, "files");
    PuleDsValueArray const filesArray = puleDsAsArray(filesValue);
    bool foundFileToRemove = false;
    for (size_t it = 0; it < filesArray.length; ++ it) {
      PuleStringView const filepath = (
        puleDsAsString(puleDsObjectMember(filesArray.values[it], "path"))
      );
      if (puleStringViewEq(filepath, destination)) {
        puleDsArrayRemoveAt(filesValue, it);
        foundFileToRemove = true;
        break;
      }
    }
    if (!foundFileToRemove) {
      puleLogError("failed to find the file reference to remove");
    }
  }
  puleAssetPdsWriteToFile(assetValue, "assets/assets.pds", error);
  puleDsDestroy(assetValue);

  if (puleErrorExists(error)) {
    return;
  }

  // only remove the file if the user didn't put the file there manually,
  //   TODO - and if the hash of both files match
  if (!puleStringViewEq(source, destination)) {
    puleFileRemove(destination);
  }
}

void assetInfo(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  PuleDsValue const input,
  PuleError * const error
) {
  PuleStringView const source = (
    puleDsAsString(puleDsObjectMember(input, "source"))
  );

  PuleDsValue const assetValue = (
    puleAssetPdsLoadFromFile(allocator, "./assets/assets.pds", error)
  );
  if (puleErrorExists(error)) {
    return;
  }

  PuleDsValue const filesValue = puleDsObjectMember(assetValue, "files");
  PuleDsValueArray const filesArray = puleDsAsArray(filesValue);

  // check exists
  // TODO need to 'simplify'/remove redundancies in path
  for (size_t it = 0; it < filesArray.length; ++ it) {
    PuleStringView const filepath = (
      puleDsAsString(puleDsObjectMember(filesArray.values[it], "path"))
    );
    if (puleStringViewEq(filepath, source)) {
      puleAssetPdsWriteToStdout(filesArray.values[it]);
      return;
    }
  }

  puleLog("file '%s' is not being tracked", source.contents);
}

} // C
