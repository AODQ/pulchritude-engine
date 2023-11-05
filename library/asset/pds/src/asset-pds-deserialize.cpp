#include <pulchritude-asset/pds.h>

#include <pulchritude-core/core.h>

#include <string>
#include <vector>

namespace pint {
} // pint

extern "C" {

#if 0
typedef struct {
  void * data;
  char const * label;
  char const * format;
} PuleAssetPdsDescription;

typedef struct {
  PuleDsValue const value,
  PuleAssetPdsDescription const * const descriptions,
  size_t const descriptionCount,
} PuleAssetPdsDeserializeInfo;
#endif

void puleAssetPdsDeserialize(
  PuleAssetPdsDeserializeInfo const info,
  PuleError * const error
) {
  for (size_t it = 0; it < info.descriptionCount; ++ it) {
    PuleAssetPdsDescription const description = info.descriptions[it];
    PuleDsValue const value = puleDsObjectMember(info.value, description.label);
    if (value.id == 0) {
      PULE_error(PuleErrorAssetPds_decode, "label missing");
      return;
    }
    if (description.format == nullptr) {
      PULE_error(PuleErrorAssetPds_decode, "format missing");
      return;
    }
    // TODO check for type mismatch

    // TODO better parsing (support for arrays, buffers, structs, strings, etc)
    #define Memcpy(formatStr, fn) \
      (strcmp(description.format, formatStr) == 0) { \
        auto const data = fn(value); \
        memcpy(description.data, &data, sizeof(data)); \
      }

    if Memcpy("{i8}", puleDsAsI8)
    else if Memcpy("{u8}", puleDsAsU8)
    else if Memcpy("{i32}", puleDsAsI32)
    else if Memcpy("{u32}", puleDsAsU32)
    else if Memcpy("{u64}", puleDsAsU64)
    else if Memcpy("{i64}", puleDsAsI64)
    else if Memcpy("{usize}", puleDsAsUSize)
    else if Memcpy("{f64}", puleDsAsF64)
    else if Memcpy("{f32}", puleDsAsF32)
    else if Memcpy("{bool}", puleDsAsBool)
    else {
      PULE_error(PuleErrorAssetPds_decode, "format not supported");
      return;
    }

    #undef Memcpy
  }
}

} // extern C
