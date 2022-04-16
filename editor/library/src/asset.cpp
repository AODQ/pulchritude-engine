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

static void assetAddForward(
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
  puleLog(
    "adding resource from '%s' to '%s'",
    source.contents,
    destination.contents
  );

  if (!puleFileCopy(source, destination)) {
    return;
  }

  PuleDsValue const assetValue = (
    puleAssetPdsLoadFromFile(allocator, "./assets/assets.pds", error)
  );
  if (puleErrorExists(error)) {
    return;
  }

  { // append file reference
    PuleDsValue const filesArray = puleDsObjectMember(assetValue, "files");
    puleDsAppendArray(
      filesArray,
      puleDsCreateString(destination)
    );
  }
  puleAssetPdsWriteToFile(assetValue, error);

  puleDsDestroy(assetValue);
}

static void assetAddRewind(
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
    PuleDsValue const filesArray = puleDsObjectMember(assetValue, "files");
    // TODO add function that can remove array elements
  }
  puleAssetPdsWriteToFile(assetValue, error);
  puleDsDestroy(assetValue);

  if (puleErrorExists(err)) {
    return;
  }

  // only remove the file if the user didn't put the file there manually,
  //   TODO - and if the hash of both files match
  if (!puleStringViewEq(source, destination)) {
    puleFileRemove(destination);
  }
}

PuleDsValue puldRegisterCommands(
  PuleAllocator const allocator,
  PuleError * const error
) {
  char const * const commandRegisterCStr = PULE_multilineString(
    commands: [
      {
        label: "asset-add",
        forward-label: "assetAddForward",
        rewind-label: "assetAddRewind",
      },
    ],
  );

  return (
    puleAssetPdsLoadFromRvalStream(
      allocator,
      puleStreamReadFromString(puleStringViewCStr(commandRegisterCStr)),
      &error
    )
  );
}

PuleDsValue puldRegisterCLICommands(
  PuleAllocator const allocator,
  PuleError * const error
) {
  char const * const CLIRegisterCStr = PULE_multilineString(
    asset: {
      add: [
        { label: "source", type: "url", opt: false, },
        { label: "destination", type: "url", opt: false, },
      ],
      info: [
        { label: "source", type: "url", opt: false, },
      ],
    },
  );

  return (
    puleAssetPdsLoadFromRvalStream(
      allocator,
      puleStreamReadFromString(puleStringViewCStr(CLIRegisterCStr)),
      &error
    )
  );
}

} // C
