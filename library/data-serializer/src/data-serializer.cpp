#include <pulchritude-data-serializer/data-serializer.h>

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {

struct PdsObject {
  std::unordered_map<size_t, PuleDsValue> values = {};
  std::unordered_map<size_t, std::string> labels = {};
  std::unordered_map<size_t, std::vector<uint8_t>> buffers = {};
  // this provides memory backing to the labels
  std::vector<std::string> linearLabels;
  std::vector<PuleDsValue> linearValues;

  // this is used store information provided to the user
  std::vector<PuleStringView> linearLabelsAsViews;
};

struct PdsString {
  std::string value;
};

struct PdsBuffer {
  std::vector<uint8_t> value;
};

void pdsObjectAdd(
  PdsObject & object,
  PuleStringView const label,
  PuleDsValue const value,
  bool const overwrite
) {
  size_t const hash = puleStringViewHash(label);
  if (object.values.find(hash) != object.values.end()) {
    if (overwrite) {
      PuleDsValue const oldValue = object.values.at(hash);
      // erase hashmap values & linearly iterate linear values to erase
      object.values.erase(hash);
      for (size_t it = 0; it < object.linearValues.size(); ++ it) {
        if (object.linearValues[it].id == oldValue.id) {
          object.linearValues.erase(object.linearValues.begin() + it);
          object.linearLabels.erase(object.linearLabels.begin() + it);
          break;
        }
      }
      // destroy overwritten value
      puleDsDestroy(oldValue);
    } else {
      PULE_prettyError(
        "%s: adding object '%s' which would overwrite existing label",
        label.contents
      );
      return;
    }
  }

  object.values.emplace(hash, value);
  object.linearValues.emplace_back(value);

  // copy string to local storage
  std::string const labelStr = std::string(label.contents, label.len);
  object.linearLabels.emplace_back(labelStr);
  object.labels.emplace(hash, labelStr);
}

union pdsValueUnion {
  int64_t asI64 = 0;
  double asF64;
  bool asBool;
  PuleStringView asString;
  std::vector<PuleDsValue> asArray;
  std::unordered_map<size_t, PuleDsValue> asObject;

  pdsValueUnion() {
    this->asF64 = 0.0;
  }
};

using PdsArray = std::vector<PuleDsValue>;

using PdsValue = std::variant<
  int64_t,
  double,
  PdsString,
  PdsArray,
  PdsObject,
  PdsBuffer
>;

std::unordered_map<uint64_t, PdsValue> pdsValues;
uint64_t pdsValueIt = 1;

PdsArray * getArrayElement(PuleDsValue const arrayValue) {
  auto const valuePtr = ::pdsValues.find(arrayValue.id);
  if (valuePtr == ::pdsValues.end()) {
    puleLogError("array value '%zu' does not exist", arrayValue.id);
    return nullptr;
  }
  auto const asArray = std::get_if<PdsArray>(&valuePtr->second);
  if (!asArray) {
    puleLogError("array value '%zu' is not an array", arrayValue.id);
    return nullptr;
  }
  return asArray;
}

template <typename T>
uint64_t pdsValueAdd(T const value) {
  pdsValues.emplace(pdsValueIt, value);
  return pdsValueIt ++;
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern "C" {

//------------------------------------------------------------------------------
int64_t puleDsAsI64(PuleDsValue const value) {
  return *std::get_if<int64_t>(&::pdsValues.at(value.id));
}
int32_t puleDsAsI32(PuleDsValue const value) {
  return (
    static_cast<int32_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
size_t puleDsAsUSize(PuleDsValue const value) {
  return (
    static_cast<size_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
uint64_t puleDsAsU64(PuleDsValue const value) {
  return (
    static_cast<uint64_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
uint32_t puleDsAsU32(PuleDsValue const value) {
  return (
    static_cast<uint32_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
uint16_t puleDsAsU16(PuleDsValue const value) {
  return (
    static_cast<uint16_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
int16_t puleDsAsI16(PuleDsValue const value) {
  return (
    static_cast<int16_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
uint8_t puleDsAsU8(PuleDsValue const value) {
  return (
    static_cast<uint8_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
int8_t puleDsAsI8(PuleDsValue const value) {
  return (
    static_cast<int8_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
bool puleDsAsBool(PuleDsValue const value) {
  auto isBool = std::get_if<int64_t>(&::pdsValues.at(value.id));
  assert(isBool && "value is not bool");
  return (*isBool) != 0;
}
double puleDsAsF64(PuleDsValue const value) {
  auto * valuePtr = &::pdsValues.at(value.id);
  auto asDouble = std::get_if<double>(valuePtr);
  return (
    asDouble ? *asDouble : static_cast<double>(*std::get_if<int64_t>(valuePtr))
  );
}
float puleDsAsF32(PuleDsValue const value) {
  return static_cast<float>(puleDsAsF64(value));
}
PuleStringView puleDsAsString(PuleDsValue const value) {
  auto valuePtr = ::pdsValues.find(value.id);
  if (valuePtr == ::pdsValues.end()) {
    puleLogError("string value '%zu' does not exist", value.id);
    return { .contents = nullptr, .len = 0 };
  }
  auto const & asString = std::get_if<PdsString>(&valuePtr->second);
  if (!asString) {
    puleLogError("string value '%zu' is not a string", value.id);
    return { .contents = nullptr, .len = 0 };
  }
  auto const & strValue = asString->value;
  return (
    PuleStringView { .contents = strValue.c_str(), .len = strValue.size(), }
  );
}
PuleDsValueArray puleDsAsArray(PuleDsValue const value) {
  PdsArray * const asArray = getArrayElement(value);
  if (!asArray) {
    return { .values = nullptr, .length = 0 };
  }
  return { .values = asArray->data(), .length = asArray->size(), };
}
PuleDsValueObject puleDsAsObject(PuleDsValue const value) {
  auto const valuePtr = ::pdsValues.find(value.id);
  if (valuePtr == ::pdsValues.end()) {
    puleLogError("object value '%zu' does not exist", value.id);
    return { .labels = nullptr, .values = nullptr, .length = 0 };
  }
  auto const asObject = std::get_if<PdsObject>(&valuePtr->second);
  if (!asObject) {
    puleLogError("object value '%zu' is not an object", value.id);
    return { .labels = nullptr, .values = nullptr, .length = 0 };
  }

  auto & object = *asObject;

  PULE_assert(object.linearLabels.size() == object.linearValues.size());
  // have to refresh the object's linear label as view pointers
  object.linearLabelsAsViews.resize(object.linearLabels.size());
  for (size_t it = 0; it < object.linearLabels.size(); ++ it) {
    object.linearLabelsAsViews[it] = PuleStringView {
      .contents = object.linearLabels[it].c_str(),
      .len = object.linearLabels[it].size(),
    };
  }

  PULE_assert(object.linearLabelsAsViews.size() == object.linearLabels.size());

  return PuleDsValueObject {
    .labels = object.linearLabelsAsViews.data(),
    .values = object.linearValues.data(),
    .length = object.linearValues.size(),
  };
}
PuleDsValueBuffer puleDsAsBuffer(PuleDsValue const bufferValue) {
  auto const bufferValuePtr = ::pdsValues.find(bufferValue.id);
  if (bufferValuePtr == ::pdsValues.end()) {
    PULE_assert(false && "not a known pds value");
    puleLogError("pds value '%zu' is not being tracked", bufferValue.id);
    return { .data = nullptr, .byteLength = 0, };
  }
  auto const asBuffer = std::get_if<PdsBuffer>(&bufferValuePtr->second);
  if (!asBuffer) {
    PULE_assert(false && "not a buffer");
    puleLogError("pds value '%zu' is not a buffer", bufferValue.id);
    return { .data = nullptr, .byteLength = 0, };
  }
  return (
    PuleDsValueBuffer {
      .data = asBuffer->value.size() > 0 ? asBuffer->value.data() : nullptr,
      .byteLength = asBuffer->value.size(),
    }
  );
}


//------------------------------------------------------------------------------
bool puleDsIsI64(PuleDsValue const value) {
  return std::get_if<int64_t>(&::pdsValues.at(value.id)) != nullptr;
}
bool puleDsIsF64(PuleDsValue const value) {
  return std::get_if<double>(&::pdsValues.at(value.id)) != nullptr;
}
bool puleDsIsString(PuleDsValue const value) {
  return std::get_if<PdsString>(&::pdsValues.at(value.id)) != nullptr;
}
bool puleDsIsArray(PuleDsValue const value) {
  return std::get_if<PdsArray>(&::pdsValues.at(value.id)) != nullptr;
}
bool puleDsIsObject(PuleDsValue const value) {
  return std::get_if<PdsObject>(&::pdsValues.at(value.id)) != nullptr;
}
bool puleDsIsBuffer(PuleDsValue const value) {
  return std::get_if<PdsBuffer>(&::pdsValues.at(value.id)) != nullptr;
}

//------------------------------------------------------------------------------
PuleDsValue puleDsCreateI64(int64_t const value) {
  return {::pdsValueAdd(value)};
}
PuleDsValue puleDsCreateBool(bool const value) {
  return {::pdsValueAdd(value)};
}
PuleDsValue puleDsCreateU64(uint64_t const value) {
  union Convert {
    uint64_t asu64 = 0;
    int64_t asi64;
  };
  Convert conv; conv.asu64 = value;
  return {::pdsValueAdd(conv.asi64)};
}
PuleDsValue puleDsCreateF64(double const value) {
  return {::pdsValueAdd(value)};
}
PuleDsValue puleDsCreateString(PuleStringView const stringView) {
  std::string const contents = std::string(stringView.contents, stringView.len);
  return {::pdsValueAdd(PdsString{.value = contents})};
}
PuleDsValue puleDsCreateArray(PuleAllocator const allocator) {
  (void)allocator;
  return {::pdsValueAdd(PdsArray{})};
}
PuleDsValue puleDsCreateObject(PuleAllocator const allocator) {
  (void)allocator;
  return {::pdsValueAdd(PdsObject{})};
}
void puleDsDestroy(PuleDsValue const value) {
  if (value.id == 0) {
    return;
  }
  if (auto const asArray = std::get_if<PdsArray>(&::pdsValues.at(value.id))) {
    for (auto const pdsValue : *asArray) {
      puleDsDestroy(pdsValue);
    }
  }

  if (auto const asObject = std::get_if<PdsObject>(&::pdsValues.at(value.id))) {
    for (auto const pdsValue : asObject->linearValues) {
      puleDsDestroy(pdsValue);
    }
  }

  ::pdsValues.erase(value.id);
}
PuleDsValue puleDsCreateBuffer(
  PuleAllocator const allocator,
  PuleArrayView const data
) {
  (void)allocator;
  std::vector<uint8_t> contents;
  for (size_t it = 0; it < data.elementCount; ++ it) {
    contents.emplace_back(data.data[data.elementStride*it]);
  }
  return {::pdsValueAdd(PdsBuffer{.value = std::move(contents)})};
}

//------------------------------------------------------------------------------
PuleDsValue puleDsArrayAppend(
  PuleDsValue const arrayValue,
  PuleDsValue const value
) {
  if (arrayValue.id == value.id) {
    puleLogError("trying to append array %zu to self", arrayValue.id);
    return {.id = 0,};
  }
  PdsArray * asArray = getArrayElement(arrayValue);
  if (!asArray) { return { 0 }; }
  asArray->emplace_back(value);
  return value;
}

void puleDsArrayPopBack(PuleDsValue const arrayValue) {
  PdsArray * asArray = getArrayElement(arrayValue);
  if (!asArray) { return; }
  asArray->pop_back();
}

void puleDsArrayPopFront(PuleDsValue const arrayValue) {
  PdsArray * asArray = getArrayElement(arrayValue);
  if (!asArray) { return; }
  asArray->erase(asArray->begin());
}

void puleDsArrayRemoveAt(PuleDsValue const arrayValue, size_t const idx) {
  PdsArray * asArray = getArrayElement(arrayValue);
  if (!asArray) { return; }
  asArray->erase(asArray->begin() + idx);
}

size_t puleDsArrayLength(PuleDsValue const arrayValue) {
  auto const & array = *std::get_if<PdsArray>(&::pdsValues.at(arrayValue.id));
  return array.size();
}

PuleDsValue puleDsArrayElementAt(
  PuleDsValue const arrayValue,
  size_t const idx
) {
  auto const & array = *std::get_if<PdsArray>(&::pdsValues.at(arrayValue.id));
  return array[idx];
}

PuleDsValue puleDsObjectMember(
  PuleDsValue const objectValue,
  char const * const memberLabel
) {
  auto valuePtr = ::pdsValues.find(objectValue.id);
  if (valuePtr == ::pdsValues.end()) {
    puleLogError("object value {%zu} does not exist", objectValue.id);
    return { 0 };
  }

  auto objectPtr = std::get_if<PdsObject>(&valuePtr->second);
  if (!objectPtr) {
    puleLogError(
      "object value {%zu} from member '%s' is not an object",
      objectValue.id,
      memberLabel
    );
    return { 0 };
  }
  auto const & object = *objectPtr;
  auto const hash = puleStringViewHash(puleCStr(memberLabel));
  auto const member = object.values.find(hash);
  if (member == object.values.end()) {
    return { 0 };
  }
  return member->second;
}

static PuleDsValue puleDsArrayCloneRecursively(
  PuleDsValue const array,
  PuleAllocator const allocator
); // prototype for object recursive
static PuleDsValue puleDsObjectCloneRecursively(
  PuleDsValue const object,
  PuleAllocator const allocator
) {
  PuleDsValue const newObj = puleDsCreateObject(allocator);
  PuleDsValueObject const oldObj = puleDsAsObject(object);
  for (size_t it = 0; it < oldObj.length; ++ it) {
    PuleDsValue const value = oldObj.values[it];
    puleDsObjectMemberAssign(
      newObj,
      oldObj.labels[it],
      puleDsValueCloneRecursively(value, allocator)
    );
  }
  return newObj;
}

static PuleDsValue puleDsArrayCloneRecursively(
  PuleDsValue const array,
  PuleAllocator const allocator
) {
  PuleDsValue const newArr = puleDsCreateArray(allocator);
  PuleDsValueArray const oldArr = puleDsAsArray(array);
  for (size_t it = 0; it < oldArr.length; ++ it) {
    PuleDsValue const value = oldArr.values[it];
    puleDsArrayAppend(newArr, puleDsValueCloneRecursively(value, allocator));
  }
  return newArr;
}

bool puleDsIsNull(PuleDsValue const object) {
  return (object.id == 0);
}

PuleDsValue puleDsValueCloneRecursively(
  PuleDsValue const value,
  PuleAllocator const allocator
) {
  if (value.id == 0) { return { 0 }; }
  if (puleDsIsI64(value)) {
    return puleDsCreateI64(puleDsAsI64(value));
  }
  if (puleDsIsF64(value)) {
    return puleDsCreateF64(puleDsAsF64(value));
  }
  if (puleDsIsString(value)) {
    return puleDsCreateString(puleDsAsString(value));
  }
  if (puleDsIsArray(value)) {
    return puleDsArrayCloneRecursively(value, allocator);
  }
  if (puleDsIsObject(value)) {
    return puleDsObjectCloneRecursively(value, allocator);
  }
  return { 0 };
}

PuleDsValue puleDsObjectMemberAssign(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
) {
  if (objectValue.id == valueToEmplace.id) {
    puleLogError("trying to assign object %zu to self", objectValue.id);
    return {.id = 0,};
  }
  auto valPtr = ::pdsValues.find(objectValue.id);
  if (valPtr == ::pdsValues.end()) {
    puleLogError("value %zu does not exist", objectValue.id);
    exit(1);
    return {.id = 0,};
  }
  auto objPtr = std::get_if<PdsObject>(&valPtr->second);
  if (!objPtr) {
    puleLogError("value %zu is not an object", objectValue.id);
    exit(1);
    return {.id = 0,};
  }
  auto & object = *objPtr;
  ::pdsObjectAdd(object, memberLabel, valueToEmplace, false);
  return puleDsObjectMember(objectValue, memberLabel.contents);
}

PuleDsValue puleDsObjectMemberOverwrite(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
) {
  if (objectValue.id == valueToEmplace.id) {
    puleLogError("trying to assign object %zu to self", objectValue.id);
    return {.id = 0,};
  }
  auto valPtr = ::pdsValues.find(objectValue.id);
  if (valPtr == ::pdsValues.end()) {
    puleLogError("value %zu does not exist", objectValue.id);
    return {.id = 0,};
  }
  auto objPtr = std::get_if<PdsObject>(&valPtr->second);
  if (!objPtr) {
    puleLogError("value %zu is not an object", objectValue.id);
    return {.id = 0,};
  }
  auto & object = *objPtr;
  ::pdsObjectAdd(object, memberLabel, valueToEmplace, true);
  return puleDsObjectMember(objectValue, memberLabel.contents);
}

int64_t puleDsMemberAsI64(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return puleDsIsNull(member) ? 0 : puleDsAsI64(member);
}
double puleDsMemberAsF64(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return puleDsIsNull(member) ? 0.0 : puleDsAsF64(member);
}
float puleDsMemberAsF32(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return puleDsIsNull(member) ? 0.0 : puleDsAsF64(member);
}
bool puleDsMemberAsBool(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return puleDsIsNull(member) ? false : puleDsAsBool(member);
}
PuleStringView puleDsMemberAsString(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return (
    puleDsIsNull(member)
    ? PuleStringView{ .contents = nullptr, .len = 0, }
    : puleDsAsString(member)
  );
}
PuleDsValueArray puleDsMemberAsArray(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return (
    puleDsIsNull(member)
    ? PuleDsValueArray{ .values = nullptr, .length = 0, }
    : puleDsAsArray(member)
  );
}
PuleDsValueObject puleDsMemberAsObject(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return (
    puleDsIsNull(member)
    ? PuleDsValueObject{ .labels = nullptr, .values = nullptr, .length = 0, }
    : puleDsAsObject(member)
  );
}
PuleDsValueBuffer puleDsMemberAsBuffer(
  PuleDsValue const obj, char const * const label
) {
  auto const member = puleDsObjectMember(obj, label);
  return (
    puleDsIsNull(member)
    ? PuleDsValueBuffer{ .data = nullptr, .byteLength = 0, }
    : puleDsAsBuffer(member)
  );
}

} // C

// -- math ---------------------------------------------------------------------
extern "C" {

PuleF32v2 puleDsAsF32v2(PuleDsValue const value) {
  PuleF32v2 vec = {};
  PuleDsValueArray valueArr = puleDsAsArray(value);
  vec.x = puleDsAsF32(valueArr.values[0]);
  vec.y = puleDsAsF32(valueArr.values[1]);
  return vec;
}

PuleF32v3 puleDsAsF32v3(PuleDsValue const value) {
  PuleF32v3 vec = {};
  PuleDsValueArray valueArr = puleDsAsArray(value);
  vec.x = puleDsAsF32(valueArr.values[0]);
  vec.y = puleDsAsF32(valueArr.values[1]);
  vec.z = puleDsAsF32(valueArr.values[2]);
  return vec;
}

PuleF32v4 puleDsAsF32v4(PuleDsValue const value) {
  PuleF32v4 vec = {};
  PuleDsValueArray valueArr = puleDsAsArray(value);
  vec.x = puleDsAsF32(valueArr.values[0]);
  vec.y = puleDsAsF32(valueArr.values[1]);
  vec.z = puleDsAsF32(valueArr.values[2]);
  vec.w = puleDsAsF32(valueArr.values[3]);
  return vec;
}

PuleDsValue  puleDsCreateF32v2(
  PuleAllocator const allocator,
  PuleF32v2 const value
) {
  PuleDsValue obj = puleDsCreateArray(allocator);
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.x));
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.y));
  return obj;
}

PuleDsValue  puleDsCreateF32v3(
  PuleAllocator const allocator,
  PuleF32v3 const value
) {
  PuleDsValue obj = puleDsCreateArray(allocator);
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.x));
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.y));
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.z));
  return obj;
}

PuleDsValue  puleDsCreateF32v4(
  PuleAllocator allocator,
  PuleF32v4 const value
) {
  PuleDsValue obj = puleDsCreateArray(allocator);
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.x));
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.y));
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.z));
  puleDsArrayAppend(obj, puleDsCreateF64((double)value.w));
  return obj;
}

} // extern "C"

// -- struct serialization -----------------------------------------------------

namespace pint {

PuleDsValue serialize(
  PuleAllocator const alloc,
  PuleDsStructField const & field,
  void const * const ptr
) {
  uint8_t const * fieldPtr = ((uint8_t const *)ptr) + field.fieldByteOffset;
  if (field.fieldCount > 1) {
    puleLogError("pds struct-serialize field array not yet implemented");
  }
  #define fieldCase(_enum, _fnlabel, _type) \
    case PuleDt_ ## _enum: \
      return puleDsCreate ## _fnlabel(*((_type const *)fieldPtr))
  #define fieldCaseAlloc(_enum, _fnlabel, _type) \
    case PuleDt_ ## _enum: \
      return puleDsCreate ## _fnlabel(alloc, *((_type const *)fieldPtr))

  switch (field.dt) {
    default:
      puleLogError("pds struct-serialize unknown dt '%d'", field.dt);
    break;
    case PuleDt_ptr:
      puleLogError(
        "pds struct-serialize ptr not implemented, must be done manually"
      );
    break;
    fieldCase(i8, I64, int8_t);
    fieldCase(i16, I64, int16_t);
    fieldCase(i32, I64, int32_t);
    fieldCase(i64, I64, int64_t);
    fieldCase(u8, U64, uint8_t);
    fieldCase(u16, U64, uint16_t);
    fieldCaseAlloc(f32v2, F32v2, PuleF32v2);
    fieldCaseAlloc(f32v3, F32v3, PuleF32v3);
    fieldCaseAlloc(f32v4, F32v4, PuleF32v4);
  }
  #undef fieldCase
  #undef fieldCaseAlloc
  return { 0 };
}

void deserialize(
  PuleDsValue const value,
  PuleDsStructField const & field,
  void const * const ptr
) {
  uint8_t const * fieldPtr = ((uint8_t const *)ptr) + field.fieldByteOffset;
  if (field.fieldCount > 1) {
    puleLogError("pds struct-serialize field array not yet implemented");
  }
  #define fieldCase(_enum, _fnlabel, _type) \
    case PuleDt_ ## _enum: \
      *(_type *)fieldPtr = puleDsAs ## _fnlabel(value); break

  switch (field.dt) {
    default:
      puleLogError("pds struct-serialize unknown dt '%d'", field.dt);
    break;
    case PuleDt_ptr:
      puleLogError(
        "pds struct-serialize ptr not implemented, must be done manually"
      );
    break;
    fieldCase(i8, I64, int8_t);
    fieldCase(i16, I64, int16_t);
    fieldCase(i32, I64, int32_t);
    fieldCase(i64, I64, int64_t);
    fieldCase(u8, U64, uint8_t);
    fieldCase(u16, U64, uint16_t);
    fieldCase(f32v2, F32v2, PuleF32v2);
    fieldCase(f32v3, F32v3, PuleF32v3);
    fieldCase(f32v4, F32v4, PuleF32v4);
  }
  #undef fieldCase
}

} // namespace pint

extern "C" {

void puleDsStructSerialize(
  PuleDsValue const writeObjectPds,
  PuleAllocator const allocator,
  PuleDsStructField const * const fields,
  void const * const structInstancePtr
) {
  for (size_t it = 0; fields[it].fieldCount > 0; ++ it) {
    PuleDsStructField const & field = fields[it];
    puleDsObjectMemberAssign(
      writeObjectPds,
      puleCStr(std::to_string(field.dt).c_str()),
      pint::serialize(allocator, field, structInstancePtr)
    );
  }
}

void puleDsStructDeserialize(
  PuleDsValue const serializedStruct,
  PuleDsStructField const * const fields,
  void * const structInstancePtr
) {
  for (size_t it = 0; fields[it].fieldCount > 0; ++ it) {
    PuleDsStructField const & field = fields[it];
    pint::deserialize(
      puleDsObjectMember(serializedStruct, std::to_string(field.dt).c_str()),
      field,
      structInstancePtr
    );
  }
}

} // extern "C"
