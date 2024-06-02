// convert JSON to PDS format
#include <pulchritude/asset-pds.h>
#include <pulchritude/core.h>
#include <pulchritude/core.hpp>
#include <pulchritude/log.h>
#include <pulchritude/string.h>

#include <string>
#include "cJSON.h"

static PuleDsValue jsonConv(cJSON * item) {
  switch (item->type) {
    case cJSON_False: return puleDsCreateBool(false);
    case cJSON_True: return puleDsCreateBool(true);
    case cJSON_NULL: return puleDsCreateBool(false);
    case cJSON_Number: {
      // determine if this is an integer or a float
      if ((int32_t)item->valuedouble == item->valueint) {
        return puleDsCreateI64((int64_t)item->valueint);
      }
      return puleDsCreateF64(item->valuedouble);
    }
    case cJSON_String: {
      return puleDsCreateString(puleCStr(item->valuestring));
    }
    case cJSON_Array: {
      PuleDsValue array = puleDsCreateArray(puleAllocateDefault());
      cJSON * arrItem;
      cJSON_ArrayForEach(arrItem, item) {
        puleDsArrayAppend(array, jsonConv(arrItem));
      }
      return array;
    }
    case cJSON_Object: {
      PuleDsValue object = puleDsCreateObject(puleAllocateDefault());
      cJSON * objItem;
      cJSON_ArrayForEach(objItem, item) {
        puleDsObjectMemberAssign(
          object,
          puleCStr(objItem->string),
          jsonConv(objItem)
        );
      }
      return object;
    }
    case cJSON_Raw: PULE_assert(false);
  }
  PULE_assert(false);
}

static PuleDsValue jsonToPdsValue(char const * json) {
  cJSON * root = cJSON_Parse(json);
  if (!root) {
    puleLogError("Failed to parse JSON: %s", cJSON_GetErrorPtr());
    return PuleDsValue();
  }

  PuleDsValue value = jsonConv(root);
  cJSON_Delete(root);
  return value;
}

extern "C" {
bool editorJsonConvert(
  [[maybe_unused]] PuleAllocator const allocator,
  [[maybe_unused]] PuleDsValue const main,
  [[maybe_unused]] PuleDsValue const input,
  [[maybe_unused]] PuleError * const error
) {
  PuleStringView srcPath = puleDsMemberAsString(input, "src");
  PuleStringView dstPath = puleDsMemberAsString(input, "dst");
  if (srcPath.len == 0) {
    puleLogError("src path must be provided");
    return false;
  }
  if (!puleFilesystemPathExists(srcPath)) {
    puleLogError("src path must exist");
    return false;
  }

  PuleError err = puleError();
  PuleString contents = puleFileDump(srcPath, PuleFileDataMode_text, &err);
  if (puleErrorConsume(&err)) { return false; }
  puleScopeExit { puleStringDestroy(contents); };

  PuleDsValue const jsonConv = jsonToPdsValue(contents.contents);
  puleScopeExit { puleDsDestroy(jsonConv); };

  std::string dstPathFixed;
  if (dstPath.len > 0) {
    dstPathFixed = std::string(dstPath.contents, dstPath.len);
  } else {
    // replace the extension with .pds
    dstPathFixed = std::string(srcPath.contents, srcPath.len);
    size_t const lastDot = dstPathFixed.find_last_of('.');
    if (lastDot == std::string::npos) {
      puleLogError("src path must have an extension");
      return false;
    }
    dstPathFixed = dstPathFixed.substr(0, lastDot) + ".pds";
  }

  puleLog("Writing to: %s", dstPathFixed.c_str());
  puleAssetPdsWriteToFile(jsonConv, puleCStr(dstPathFixed.c_str()), &err);
  if (puleErrorConsume(&err)) { return false; }

  return true;
}
} // extern C
