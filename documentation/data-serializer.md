# data-serializer

## structs
### PuleDsValueArray
```c
struct {
  values : PuleDsValue const ptr;
  length : size_t;
};
```
### PuleDsValueObject
```c
struct {
  labels : PuleStringView const ptr;
  values : PuleDsValue const ptr;
  length : size_t;
};
```
### PuleDsValueBuffer

  arrays store PDS values, which will reference the data through a layer of
    indirection (PuleDsValue)
  buffers just store raw data, there is no additional indirection.

```c
struct {
  data : uint8_t const ptr;
  byteLength : size_t;
};
```
### PuleDsStructField
```c
struct {
  dt : PuleDt;
  fieldByteOffset : size_t;
  /*  set field count to 0 to terminate array  */
  fieldCount : size_t;
};
```
### PULE_DS_TODODOMP

#define PULE_dsStructField(structtype, fieldtype, member, count) \
  { PuleDt_##fieldtype , offsetof(structtype, member) , count , }
#define PULE_dsStructTerminator { PuleDt_ptr , 0 , 0 , }
PuleDsValue operator ""_pds(char const ptr str, size_t len);
#
```c
struct {
  temp : uint8_t;
};
```

## enums
### PuleErrorDataSerializer
```c
enum {
  none,
  invalidFormat,
}
```

## entities
### PuleDsValue

## functions
### puleDsAsI64
```c
puleDsAsI64(
  value : PuleDsValue
) int64_t;
```
### puleDsAsF64
```c
puleDsAsF64(
  value : PuleDsValue
) double;
```
### puleDsAsBool
```c
puleDsAsBool(
  value : PuleDsValue
) bool;
```
### puleDsAsString
```c
puleDsAsString(
  value : PuleDsValue
) PuleStringView;
```
### puleDsAsArray
```c
puleDsAsArray(
  value : PuleDsValue
) PuleDsValueArray;
```
### puleDsAsObject
```c
puleDsAsObject(
  value : PuleDsValue
) PuleDsValueObject;
```
### puleDsAsBuffer
```c
puleDsAsBuffer(
  value : PuleDsValue
) PuleDsValueBuffer;
```
### puleDsAsF32
```c
puleDsAsF32(
  value : PuleDsValue
) float;
```
### puleDsAsI32
```c
puleDsAsI32(
  value : PuleDsValue
) int32_t;
```
### puleDsAsUSize
```c
puleDsAsUSize(
  value : PuleDsValue
) size_t;
```
### puleDsAsU64
```c
puleDsAsU64(
  value : PuleDsValue
) uint64_t;
```
### puleDsAsU32
```c
puleDsAsU32(
  value : PuleDsValue
) uint32_t;
```
### puleDsAsU16
```c
puleDsAsU16(
  value : PuleDsValue
) uint16_t;
```
### puleDsAsI16
```c
puleDsAsI16(
  value : PuleDsValue
) int16_t;
```
### puleDsAsU8
```c
puleDsAsU8(
  value : PuleDsValue
) uint8_t;
```
### puleDsAsI8
```c
puleDsAsI8(
  value : PuleDsValue
) int8_t;
```
### puleDsIsI64
```c
puleDsIsI64(
  value : PuleDsValue
) bool;
```
### puleDsIsF64
```c
puleDsIsF64(
  value : PuleDsValue
) bool;
```
### puleDsIsString
```c
puleDsIsString(
  value : PuleDsValue
) bool;
```
### puleDsIsArray
```c
puleDsIsArray(
  value : PuleDsValue
) bool;
```
### puleDsIsObject
```c
puleDsIsObject(
  value : PuleDsValue
) bool;
```
### puleDsIsBuffer
```c
puleDsIsBuffer(
  value : PuleDsValue
) bool;
```
### puleDsDestroy
 recursive destroy 
```c
puleDsDestroy(
  value : PuleDsValue
) void;
```
### puleDsCreateI64
```c
puleDsCreateI64(
  value : int64_t
) PuleDsValue;
```
### puleDsCreateBool
```c
puleDsCreateBool(
  value : bool
) PuleDsValue;
```
### puleDsCreateU64
```c
puleDsCreateU64(
  value : uint64_t
) PuleDsValue;
```
### puleDsCreateF64
```c
puleDsCreateF64(
  value : double
) PuleDsValue;
```
### puleDsCreateString
```c
puleDsCreateString(
  stringView : PuleStringView
) PuleDsValue;
```
### puleDsCreateArray
```c
puleDsCreateArray(
  allocator : PuleAllocator
) PuleDsValue;
```
### puleDsCreateObject
```c
puleDsCreateObject(
  allocator : PuleAllocator
) PuleDsValue;
```
### puleDsCreateBuffer
```c
puleDsCreateBuffer(
  allocator : PuleAllocator,
  data : PuleArrayView
) PuleDsValue;
```
### puleDsArrayAppend
```c
puleDsArrayAppend(
  array : PuleDsValue,
  value : PuleDsValue
) PuleDsValue;
```
### puleDsArrayPopBack
```c
puleDsArrayPopBack(
  array : PuleDsValue
) void;
```
### puleDsArrayPopFront
```c
puleDsArrayPopFront(
  array : PuleDsValue
) void;
```
### puleDsArrayRemoveAt
```c
puleDsArrayRemoveAt(
  array : PuleDsValue,
  idx : size_t
) void;
```
### puleDsArrayLength
```c
puleDsArrayLength(
  array : PuleDsValue
) size_t;
```
### puleDsArrayElementAt
```c
puleDsArrayElementAt(
  array : PuleDsValue,
  idx : size_t
) PuleDsValue;
```
### puleDsObjectMember
object[memberLabel] , returns a null object if no member
```c
puleDsObjectMember(
  object : PuleDsValue,
  memberLabel : char const ptr
) PuleDsValue;
```
### puleDsIsNull
```c
puleDsIsNull(
  object : PuleDsValue
) bool;
```
### puleDsValueCloneRecursively
```c
puleDsValueCloneRecursively(
  object : PuleDsValue,
  allocator : PuleAllocator
) PuleDsValue;
```
### puleDsObjectMemberAssign
```c
puleDsObjectMemberAssign(
  objectValue : PuleDsValue,
  memberLabel : PuleStringView,
  valueToEmplace : PuleDsValue
) PuleDsValue;
```
### puleDsObjectMemberOverwrite
```c
puleDsObjectMemberOverwrite(
  objectValue : PuleDsValue,
  memberLabel : PuleStringView,
  valueToEmplace : PuleDsValue
) PuleDsValue;
```
### puleDsMemberAsI64
returns 0 if nonexistent
```c
puleDsMemberAsI64(
  obj : PuleDsValue,
  label : char const ptr
) int64_t;
```
### puleDsMemberAsF64
returns 0.0 if nonexistent
```c
puleDsMemberAsF64(
  object : PuleDsValue,
  memberLabel : char const ptr
) double;
```
### puleDsMemberAsF32
returns 0.0f if nonexistent
```c
puleDsMemberAsF32(
  object : PuleDsValue,
  memberLabel : char const ptr
) float;
```
### puleDsMemberAsBool
returns false if nonexistent
```c
puleDsMemberAsBool(
  object : PuleDsValue,
  memberLabel : char const ptr
) bool;
```
### puleDsMemberAsString
returns null string view if nonexistent
```c
puleDsMemberAsString(
  object : PuleDsValue,
  memberLabel : char const ptr
) PuleStringView;
```
### puleDsMemberAsArray
returns empty array if nonexistent
```c
puleDsMemberAsArray(
  object : PuleDsValue,
  memberLabel : char const ptr
) PuleDsValueArray;
```
### puleDsMemberAsObject
returns empty object if nonexistent
```c
puleDsMemberAsObject(
  object : PuleDsValue,
  memberLabel : char const ptr
) PuleDsValueObject;
```
### puleDsMemberAsBuffer
returns empty buffer if nonexistent
```c
puleDsMemberAsBuffer(
  object : PuleDsValue,
  memberLabel : char const ptr
) PuleDsValueBuffer;
```
### puleDsAsF32v2
```c
puleDsAsF32v2(
  value : PuleDsValue
) PuleF32v2;
```
### puleDsAsF32v3
```c
puleDsAsF32v3(
  value : PuleDsValue
) PuleF32v3;
```
### puleDsAsF32v4
```c
puleDsAsF32v4(
  value : PuleDsValue
) PuleF32v4;
```
### puleDsCreateF64v2
```c
puleDsCreateF64v2(
  allocator : PuleAllocator const,
  value : PuleF32v2
) PuleDsValue;
```
### puleDsCreateF64v3
```c
puleDsCreateF64v3(
  allocator : PuleAllocator const,
  value : PuleF32v3
) PuleDsValue;
```
### puleDsCreateF64v4
```c
puleDsCreateF64v4(
  allocator : PuleAllocator const,
  value : PuleF32v4
) PuleDsValue;
```
### puleDsStructSerialize

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

```c
puleDsStructSerialize(
  writeObjectPds : PuleDsValue,
  allocator : PuleAllocator,
  fields : PuleDsStructField const ptr,
  structInstancePtr : void const ptr
) void;
```
### puleDsStructDeserialize
```c
puleDsStructDeserialize(
  serializedStruct : PuleDsValue,
  fields : PuleDsStructField const ptr,
  structInstancePtr : void ptr
) void;
```
