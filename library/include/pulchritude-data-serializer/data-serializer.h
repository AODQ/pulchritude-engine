#pragma once

// this is a data-serialization parser & writer; 'Pule Data Serializer'
// Everything from data to configs is stored in this format. It is very similar
//   to JSON, but is meant to have more relaxed rules.
//
// Of course, you can completely replace this serializer with your own backend
//   and use your own serialization format. I imagine a popular alternative
//   would be to implement a JSON-compliant parser, or something like
//   flatbuffers. Since the engine will just use these functions anyways, it
//   doesn't matter as long as the data is prepared beforehand.
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
//
// spec:
//    StartingState: ObjectMember*
//    WS: [ \n\t\r]*
//    Object: '{'ObjectMember*'}'
//    ObjectMember: WS Label WS ':' WS Value
//    Label: [0-9a-zA-z_\-]+
//    Value: (Object/I64/F64/Array/String) WS ','
//    Array: '[' WS Value* WS ']
//    StringCharacter: '\"'
//    String: '"' (StringCharacter / !'"' .)* '"'
//
// multiline strings are handled such that
//   myAmazingStory:
//   "
//     *it goes like this and
//     * then it went like that!
//   ",
// and
//   myAmazingStory: "it goes like this and then it went like that!",
// both evaluate to the same thing. The '*' denotes to "join" the line
//
// as well
//  myAmazingStory:
//  "
//    *it goes like this and
//    %then it went like that!
//  ",
// and
//   myAmazingStory: "it goes like this and\nthen it went like that!",
// also evaluate to the same thing. The '%' denotes to not join the line.


#include <pulchritude-allocator/allocator.h>
#include <pulchritude-core/core.h>
#include <pulchritude-error/error.h>
#include <pulchritude-file/file.h>
#include <pulchritude-string/string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  PuleErrorDataSerializer_none,
  PuleErrorDataSerializer_invalidFormat,
} PuleErrorDataSerializer;

typedef struct {
  uint64_t id;
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

PULE_exportFn int64_t puleDsAsI64(PuleDsValue const value);
PULE_exportFn double puleDsAsF64(PuleDsValue const value);
PULE_exportFn bool puleDsAsBool(PuleDsValue const value);
PULE_exportFn PuleStringView puleDsAsString(PuleDsValue const value);
PULE_exportFn PuleDsValueArray puleDsAsArray(PuleDsValue const value);
PULE_exportFn PuleDsValueObject puleDsAsObject(PuleDsValue const value);

// below are just convenient type conversions of (T)puleDsAsI64
PULE_exportFn int32_t puleDsAsI32(PuleDsValue const value);
PULE_exportFn size_t puleDsAsUSize(PuleDsValue const value);
PULE_exportFn uint64_t puleDsAsU64(PuleDsValue const value);
PULE_exportFn uint64_t puleDsAsU32(PuleDsValue const value);

PULE_exportFn bool puleDsIsI64(PuleDsValue const value);
PULE_exportFn bool puleDsIsF64(PuleDsValue const value);
PULE_exportFn bool puleDsIsString(PuleDsValue const value);
PULE_exportFn bool puleDsIsArray(PuleDsValue const value);
PULE_exportFn bool puleDsIsObject(PuleDsValue const value);

// recursive destroy
PULE_exportFn void puleDsDestroy(PuleDsValue const value);

PULE_exportFn PuleDsValue puleDsCreateI64(int64_t const value);
PULE_exportFn PuleDsValue puleDsCreateF64(double const value);
// TODO this needs to pass in an allocator
PULE_exportFn PuleDsValue puleDsCreateString(PuleStringView const stringView);
PULE_exportFn PuleDsValue puleDsCreateArray(PuleAllocator const allocator);
PULE_exportFn PuleDsValue puleDsCreateObject(PuleAllocator const allocator);

PULE_exportFn PuleDsValue puleDsAppendArray(
  PuleDsValue const array,
  PuleDsValue const value
);
PULE_exportFn void puleDsArrayPopBack(PuleDsValue const array);
PULE_exportFn void puleDsArrayPopFront(PuleDsValue const array);
PULE_exportFn void puleDsArrayRemoveAt(
  PuleDsValue const array,
  size_t const idx
);
PULE_exportFn size_t puleDsArrayLength(PuleDsValue const array);
PULE_exportFn PuleDsValue puleDsArrayElementAt(
  PuleDsValue const array,
  size_t const idx
);

// object[memberLabel] , returns { 0 } if no member
PULE_exportFn PuleDsValue puleDsObjectMember(
  PuleDsValue const object,
  char const * const memberLabel
);

PULE_exportFn PuleDsValue puleDsValueCloneRecursively(
  PuleDsValue const object,
  PuleAllocator const allocator
);

PULE_exportFn PuleDsValue puleDsAssignObjectMember(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
);

PULE_exportFn PuleDsValue puleDsOverwriteObjectMember(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
);

#ifdef __cplusplus
} // extern C
#endif
