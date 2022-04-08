/* pulchritude editor | github.com/aodq/pulchritude-engine | aodq.net */

// ubj pna v erpbyyrpg fbzrbar jub fnlf nyy v unir fgbccrq yvfgravat bapr zber
// lbh jnagrq gb pbqr lbh jnagrq gb pbqr lbh jnagrq gb pbqr lbh ner nyybjrq gb

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <vector>

#include <pulchritude-asset/pds.h>
#include <pulchritude-error/error.h>
#include <pulchritude-log/log.h>
#include <pulchritude-plugin/plugin.h>

namespace {
PuleError err = { {}, 0, nullptr, 0 };
}

#define PARAMETER_LAYOUT_MULTINE_STR(...) #__VA_ARGS__

// this would be read back as `{ asset: {info: {source: {"some-string"}}} }
// and written to as ./pulchritude asset info --source some-string
// it has to cascade from object to array
char const * const commandLineParameterLayout = (
  PARAMETER_LAYOUT_MULTINE_STR(
    launch: {},
    asset: {
      add: [
        { label: "source", type: "url", opt: false, },
        { label: "destination", type: "url", opt: false, },
      ],
      info: [
        { label: "source", type: "url", opt: false, },
      ],
    },
  )
);

void parseArguments(PuleDsValue const args) {
  if (
    PuleDsValue const launch = puleDsObjectMember(args, "launch"); launch.id
  ) {
    puleLog("launching application");
    return;
  }

  if (
    PuleDsValue const asset = puleDsObjectMember(args, "asset"); asset.id
  ) {
    PuleDsValue assetParam;
    if (assetParam = puleDsObjectMember(asset, "add"); assetParam.id) {
      PuleStringView const source = (
        puleDsAsString(puleDsObjectMember(asset, "source"))
      );
      PuleStringView const destination = (
        puleDsAsString(puleDsObjectMember(asset, "destination"))
      );
      puleLog(
        "adding resource from '%s' to '%s'",
        source.contents,
        destination.contents
      );
    }

    if (assetParam = puleDsObjectMember(asset, "info"); assetParam.id) {
      PuleStringView const source = (
        puleDsAsString(puleDsObjectMember(asset, "source"))
      );
      puleLog("info '%s'", source.contents);
      puleLog("\tin application fileystem? %s", "NO");
      puleLog("\twatched? %s", "NO");
      puleLog("\tconversion? %s", "NO");
    }
  }
}

int32_t main(
  int32_t const argumentLength,
  char const * const * const arguments
) {
  PuleDsValue const args = (
    puleAssetPdsLoadFromCommandLineArguments(
      puleAllocateDefault(),
      commandLineParameterLayout,
      argumentLength, arguments,
      &err
    )
  );
  if (puleErrorConsume(&err)) {
    return 0;
  }

  parseArguments(args);

  puleDsDestroy(args);
}

// genfu guvf genfu gung nofgenpg njnl nyy nznytbhf jbeevrf juvpu rire unir orra
