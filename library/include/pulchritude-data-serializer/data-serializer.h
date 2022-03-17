// this is a data-serialization parser & writer; 'Pule Data Serializer'
// Everything from data to configs is stored in this format. It is very similar
//   to JSON, but is meant to have more relaxed rules, including support for
//   more native data-types, support importing raw buffers as data, etc.
//
// Of course, you can completely replace this serializer with your own backend
//   and use your own serialization format. I imagine a popular alternative
//   would be to implement a JSON-compliant parser. Since the engine will just
//   use these functions anyways, it doesn't matter as long as the data is
//   prepared beforehand.
//
// notes
//  - fastest way to parse a float array? (or other specific data-types)
//      a faster option is to store data in an external file, point to the file
//        and then open the entire thing and cast its' raw binary data to the
//        storage type pointer, and memcpy
//      if you want to embed into the file, use BASE64 and cast raw binary data
//        to the underlying storage type pointer. This will require some extra
//        work and consume more data but it could be sufficient for your needs
//      the slowest possible option is to use arrays, at least in the cases
//        where you are parsing megabytes of data. Not only are they stored as
//        int64s or doubles, which reduces overall transmission and requires
//        casting each value to the underlying type, but they will consume
//        additional time spent parsing the file. Try to only store meta-data
//        where possible.

#include <pulchritude-allocator/allocator.h>
#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorDataSerializer_none,
  PuleErrorDataSerializer_fileOpen,
  PuleErrorDataSerializer_invalidFormat,
} PuleErrorDataSerializer;

typedef struct {
  int64_t id;
} PuleDsValue;

typedef struct {
  PuleDsValue const * values;
  size_t length;
} PuleDsValueArray;

typedef struct {
  PuleStringView const * labels;
  PuleDsValue const * values;
  size_t length;
} PuleDsValueObject;

PULE_externFn int64_t puleDsAsI64(PuleDsValue const value);
PULE_externFn double puleDsAsF64(PuleDsValue const value);
PULE_externFn PuleStringView puleDsAsString(PuleDsValue const value);
PULE_externFn PuleDsValueArray puleDsAsArray(PuleDsValue const value);
PULE_externFn PuleDsValueObject puleDsAsObject(PuleDsValue const value);

PULE_externFn bool puleDsIsI64(PuleDsValue const value);
PULE_externFn bool puleDsIsF64(PuleDsValue const value);
PULE_externFn bool puleDsIsString(PuleDsValue const value);
PULE_externFn bool puleDsIsArray(PuleDsValue const value);
PULE_externFn bool puleDsIsObject(PuleDsValue const value);

PULE_externFn PuleDsValue puleDsLoadFromFile(
  PuleAllocator const allocator,
  char const * const filename,
  PuleError * const error
);

// recursive destroy
PULE_externFn void puleDsDestroy(PuleDsValue const value);

PULE_externFn PuleDsValue puleDsCreateI64(int64_t const value);
PULE_externFn PuleDsValue puleDsCreateF64(double const value);
// TODO this thing is odd because i need to copy it locally anyway,
//    so i might just want to set a default allocator for a PDS
PULE_externFn PuleDsValue puleDsCreateString(PuleStringView const stringView);
PULE_externFn PuleDsValue puleDsCreateArray(PuleAllocator const allocator);
PULE_externFn PuleDsValue puleDsCreateObject(PuleAllocator const allocator);

PULE_externFn void puleDsAppendArray(
  PuleDsValue const array,
  PuleDsValue const value
);
PULE_externFn size_t puleDsArrayLength(PuleDsValue const array);
PULE_externFn PuleDsValue puleDsArrayElementAt(
  PuleDsValue const array,
  size_t const idx
);

// object[memberLabel]
PULE_externFn PuleDsValue puleDsObjectMember(
  PuleDsValue const object,
  PuleStringView const memberLabel
);

// object[memberLabel] = value;
// ASSERT(!object.exists(memberLabel))
PULE_externFn void puleDsAssignObjectMember(
  PuleDsValue const object,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
);

#ifdef __cplusplus
} // extern C
#endif
