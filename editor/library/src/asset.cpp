#include <pulchritude/allocator.h>
#include <pulchritude/asset-pds.h>
#include <pulchritude/error.h>
#include <pulchritude/log.h>
#include <pulchritude/string.h>

extern "C" {

PuleDsValue puldRegisterCommands(
  PuleAllocator const allocator,
  PuleError * const err
);
PuleDsValue puldRegisterCLICommands(
  PuleAllocator const allocator,
  PuleError * const err
);

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
  PuleDsValue const assetValue = (
    puleAssetPdsLoadFromFile(
      allocator,
      puleCStr("./editor/assets.pds"),
      error
    )
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
      PULE_error(
        1,
        "file '%s' already exists in assets.pds (%s)",
        source.contents
      );
    }
  }

  { // append file reference
    PuleDsValue fileObj = puleDsCreateObject(allocator);
    puleDsObjectMemberAssign(
      fileObj, puleCStr("path"), puleDsCreateString(source)
    );
    puleDsObjectMemberAssign(
      fileObj, puleCStr("tracking"), puleDsCreateI64(false)
    );
    puleDsArrayAppend(filesValue, fileObj);
  }
  puleAssetPdsWriteToFile(assetValue, puleCStr("editor/assets.pds"), error);
  puleDsDestroy(assetValue);
  if (puleErrorExists(error)) {
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
  puleLog("removing resource at '%s'", source.contents);

  PuleDsValue const assetValue = (
    puleAssetPdsLoadFromFile(
      allocator,
      puleCStr("./editor/assets.pds"),
      error
    )
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
      if (puleStringViewEq(filepath, source)) {
        puleDsArrayRemoveAt(filesValue, it);
        foundFileToRemove = true;
        break;
      }
    }
    if (!foundFileToRemove) {
      puleLogError("failed to find the file reference to remove");
    }
  }
  puleAssetPdsWriteToFile(assetValue, puleCStr("editor/assets.pds"), error);
  puleDsDestroy(assetValue);

  if (puleErrorExists(error)) {
    return;
  }
}

void assetList(
  PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  PuleError * const error
) {
  PuleDsValue const assetValue = (
    puleAssetPdsLoadFromFile(
      allocator,
      puleCStr("./editor/assets.pds"),
      error
    )
  );
  if (puleErrorExists(error)) {
    return;
  }
  puleAssetPdsWriteToStdout(assetValue);
  puleDsDestroy(assetValue);
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
    puleAssetPdsLoadFromFile(
      allocator,
      puleCStr("./editor/assets.pds"),
      error
    )
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
      puleDsDestroy(assetValue);
      return;
    }
  }

  puleLog("file '%s' is not being tracked", source.contents);
  puleDsDestroy(assetValue);
}

} // C
