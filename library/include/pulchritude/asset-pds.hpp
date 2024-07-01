/* auto generated file asset-pds */
#pragma once
#include "core.hpp"

#include "asset-pds.h"
#include "error.hpp"
#include "data-serializer.hpp"
#include "allocator.hpp"
#include "stream.hpp"

namespace pule {
using AssetPdsWriteInfo = PuleAssetPdsWriteInfo;
}
namespace pule {
using AssetPdsCommandLineArgumentsInfo = PuleAssetPdsCommandLineArgumentsInfo;
}
namespace pule {
using AssetPdsDescription = PuleAssetPdsDescription;
}
namespace pule {
using AssetPdsDeserializeInfo = PuleAssetPdsDeserializeInfo;
}
#include "string.hpp"
#include <string>
namespace pule { //tostr 
inline pule::str toStr(PuleErrorAssetPds const e) {
  switch (e) {
    case PuleErrorAssetPds_none: return puleString("none");
    case PuleErrorAssetPds_decode: return puleString("decode");
    default: return puleString("N/A");
  }
}
}
