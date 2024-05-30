/* auto generated file data-serializer */
#pragma once
#define PULE_dsStructField(structtype, fieldtype, member, count) \
  { PuleDt_##fieldtype , offsetof(structtype, member) , count , }
#define PULE_dsStructTerminator { PuleDt_ptr , 0 , 0 , }

#include "core.h"

#include "allocator.h"
#include "error.h"
#include "file.h"
#include "math.h"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// enum
typedef enum {
  PuleErrorDataSerializer_none = 0,
  PuleErrorDataSerializer_invalidFormat = 1,
} PuleErrorDataSerializer;
const uint32_t PuleErrorDataSerializerSize = 2;

// entities
typedef struct PuleDsValue { uint64_t id; } PuleDsValue;

// structs
struct PuleDsValueArray;
struct PuleDsValueObject;
struct PuleDsValueBuffer;
struct PuleDsStructField;
struct PULE_DS_TODODOMP;

typedef struct PuleDsValueArray {
  PuleDsValue const * values;
  size_t length;
} PuleDsValueArray;
typedef struct PuleDsValueObject {
  PuleStringView const * labels;
  PuleDsValue const * values;
  size_t length;
} PuleDsValueObject;
/* 
  arrays store PDS values, which will reference the data through a layer of
    indirection (PuleDsValue)
  buffers just store raw data, there is no additional indirection.
 */
typedef struct PuleDsValueBuffer {
  uint8_t const * data;
  size_t byteLength;
} PuleDsValueBuffer;
typedef struct PuleDsStructField {
  PuleDt dt;
  size_t fieldByteOffset;
  /*  set field count to 0 to terminate array  */
  size_t fieldCount;
} PuleDsStructField;
/* 
#define PULE_dsStructField(structtype, fieldtype, member, count) \
  { PuleDt_##fieldtype , offsetof(structtype, member) , count , }
#define PULE_dsStructTerminator { PuleDt_ptr , 0 , 0 , }
PuleDsValue operator ""_pds(char const ptr str, size_t len);
# */
typedef struct PULE_DS_TODODOMP {
  uint8_t temp;
} PULE_DS_TODODOMP;

// functions
PULE_exportFn int64_t puleDsAsI64(PuleDsValue value);
PULE_exportFn double puleDsAsF64(PuleDsValue value);
PULE_exportFn bool puleDsAsBool(PuleDsValue value);
PULE_exportFn PuleStringView puleDsAsString(PuleDsValue value);
PULE_exportFn PuleDsValueArray puleDsAsArray(PuleDsValue value);
PULE_exportFn PuleDsValueObject puleDsAsObject(PuleDsValue value);
PULE_exportFn PuleDsValueBuffer puleDsAsBuffer(PuleDsValue value);
PULE_exportFn float puleDsAsF32(PuleDsValue value);
PULE_exportFn int32_t puleDsAsI32(PuleDsValue value);
PULE_exportFn size_t puleDsAsUSize(PuleDsValue value);
PULE_exportFn uint64_t puleDsAsU64(PuleDsValue value);
PULE_exportFn uint32_t puleDsAsU32(PuleDsValue value);
PULE_exportFn uint16_t puleDsAsU16(PuleDsValue value);
PULE_exportFn int16_t puleDsAsI16(PuleDsValue value);
PULE_exportFn uint8_t puleDsAsU8(PuleDsValue value);
PULE_exportFn int8_t puleDsAsI8(PuleDsValue value);
PULE_exportFn bool puleDsIsI64(PuleDsValue value);
PULE_exportFn bool puleDsIsF64(PuleDsValue value);
PULE_exportFn bool puleDsIsString(PuleDsValue value);
PULE_exportFn bool puleDsIsArray(PuleDsValue value);
PULE_exportFn bool puleDsIsObject(PuleDsValue value);
PULE_exportFn bool puleDsIsBuffer(PuleDsValue value);
/*  recursive destroy  */
PULE_exportFn void puleDsDestroy(PuleDsValue value);
PULE_exportFn PuleDsValue puleDsCreateI64(int64_t value);
PULE_exportFn PuleDsValue puleDsCreateBool(bool value);
PULE_exportFn PuleDsValue puleDsCreateU64(uint64_t value);
PULE_exportFn PuleDsValue puleDsCreateF64(double value);
PULE_exportFn PuleDsValue puleDsCreateString(PuleStringView stringView);
PULE_exportFn PuleDsValue puleDsCreateArray(PuleAllocator allocator);
PULE_exportFn PuleDsValue puleDsCreateObject(PuleAllocator allocator);
PULE_exportFn PuleDsValue puleDsCreateBuffer(PuleAllocator allocator, PuleArrayView data);
PULE_exportFn PuleDsValue puleDsArrayAppend(PuleDsValue array, PuleDsValue value);
PULE_exportFn void puleDsArrayPopBack(PuleDsValue array);
PULE_exportFn void puleDsArrayPopFront(PuleDsValue array);
PULE_exportFn void puleDsArrayRemoveAt(PuleDsValue array, size_t idx);
PULE_exportFn size_t puleDsArrayLength(PuleDsValue array);
PULE_exportFn PuleDsValue puleDsArrayElementAt(PuleDsValue array, size_t idx);
/* object[memberLabel] , returns a null object if no member */
PULE_exportFn PuleDsValue puleDsObjectMember(PuleDsValue object, char const * memberLabel);
PULE_exportFn bool puleDsIsNull(PuleDsValue object);
PULE_exportFn PuleDsValue puleDsValueCloneRecursively(PuleDsValue object, PuleAllocator allocator);
PULE_exportFn PuleDsValue puleDsObjectMemberAssign(PuleDsValue objectValue, PuleStringView memberLabel, PuleDsValue valueToEmplace);
PULE_exportFn PuleDsValue puleDsObjectMemberOverwrite(PuleDsValue objectValue, PuleStringView memberLabel, PuleDsValue valueToEmplace);
/* returns 0 if nonexistent */
PULE_exportFn int64_t puleDsMemberAsI64(PuleDsValue obj, char const * label);
/* returns 0.0 if nonexistent */
PULE_exportFn double puleDsMemberAsF64(PuleDsValue object, char const * memberLabel);
/* returns 0.0f if nonexistent */
PULE_exportFn float puleDsMemberAsF32(PuleDsValue object, char const * memberLabel);
/* returns false if nonexistent */
PULE_exportFn bool puleDsMemberAsBool(PuleDsValue object, char const * memberLabel);
/* returns null string view if nonexistent */
PULE_exportFn PuleStringView puleDsMemberAsString(PuleDsValue object, char const * memberLabel);
/* returns empty array if nonexistent */
PULE_exportFn PuleDsValueArray puleDsMemberAsArray(PuleDsValue object, char const * memberLabel);
/* returns empty object if nonexistent */
PULE_exportFn PuleDsValueObject puleDsMemberAsObject(PuleDsValue object, char const * memberLabel);
/* returns empty buffer if nonexistent */
PULE_exportFn PuleDsValueBuffer puleDsMemberAsBuffer(PuleDsValue object, char const * memberLabel);
PULE_exportFn PuleF32v2 puleDsAsF32v2(PuleDsValue value);
PULE_exportFn PuleF32v3 puleDsAsF32v3(PuleDsValue value);
PULE_exportFn PuleF32v4 puleDsAsF32v4(PuleDsValue value);
PULE_exportFn PuleDsValue puleDsCreateF64v2(PuleAllocator const allocator, PuleF32v2 value);
PULE_exportFn PuleDsValue puleDsCreateF64v3(PuleAllocator const allocator, PuleF32v3 value);
PULE_exportFn PuleDsValue puleDsCreateF64v4(PuleAllocator const allocator, PuleF32v4 value);
/* 
  can help serialize/deserialize POD structs, using a C-like format string, use
  the following macros to generate the format string

  example

  #define structField(fieldtype, member) \
    PULE_dsStructField(PuleSceneComponentModelData, fieldtype, member, 1)
  static PuleDsStructField const componentModelFields[] = {
    structField(i64, type),
    PULE_dsStructTerminator,
  };
  #undef structField
 */
PULE_exportFn void puleDsStructSerialize(PuleDsValue writeObjectPds, PuleAllocator allocator, PuleDsStructField const * fields, void const * structInstancePtr);
PULE_exportFn void puleDsStructDeserialize(PuleDsValue serializedStruct, PuleDsStructField const * fields, void * structInstancePtr);

#ifdef __cplusplus
} // extern C
#endif
