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
  std::unordered_map<size_t, PuleStringView> labels = {};
  // this provides memory backing to the labels
  std::vector<std::string> linearLabels;
  std::vector<PuleDsValue> linearValues;

  // this is used store information provided to the user
  std::vector<PuleStringView> linearLabelsAsViews;
};

struct PdsString {
  std::string value;
};

void pdsObjectAdd(
  PdsObject & object,
  PuleStringView const label,
  PuleDsValue const value
) {
  size_t const hash = puleStringViewHash(label);
  if (object.values.find(hash) != object.values.end()) {
    puleLogError(
      "adding object '%s' which would overwrite existing label",
      label.contents
    );
    return;
  }

  object.values.emplace(hash, value);
  object.linearValues.emplace_back(value);

  // copy string to local storage
  object.linearLabels.emplace_back(std::string(label.contents, label.len));
  auto const stringView = PuleStringView {
    .contents = object.linearLabels.back().c_str(),
    .len = label.len
  };
  object.labels.emplace(hash, stringView);
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
  bool,
  PdsString,
  PdsArray,
  PdsObject
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
uint64_t puleDsAsU32(PuleDsValue const value) {
  return (
    static_cast<uint32_t>(*std::get_if<int64_t>(&::pdsValues.at(value.id)))
  );
}
double puleDsAsF64(PuleDsValue const value) {
  return *std::get_if<double>(&::pdsValues.at(value.id));
}
bool puleDsAsBool(PuleDsValue const value) {
  return *std::get_if<bool>(&::pdsValues.at(value.id));
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
  PdsArray * asArray = getArrayElement(value);
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
  // have to refresh the object's linear label as view pointers
  object.linearLabelsAsViews.resize(object.linearValues.size());
  for (size_t it = 0; it < object.linearLabels.size(); ++ it) {
    object.linearLabelsAsViews[it] = PuleStringView {
      .contents = object.linearLabels[it].c_str(),
      .len = object.linearLabels[it].size(),
    };
  }

  PULE_assert(object.linearLabelsAsViews.size() == object.linearValues.size());

  return PuleDsValueObject {
    .labels = object.linearLabelsAsViews.data(),
    .values = object.linearValues.data(),
    .length = object.linearValues.size(),
  };
}

//------------------------------------------------------------------------------
bool puleDsIsI64(PuleDsValue const value) {
  return std::get_if<int64_t>(&::pdsValues.at(value.id)) != nullptr;
}
bool puleDsIsF64(PuleDsValue const value) {
  return std::get_if<double>(&::pdsValues.at(value.id)) != nullptr;
}
bool puleDsIsBool(PuleDsValue const value) {
  return std::get_if<bool>(&::pdsValues.at(value.id)) != nullptr;
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

//------------------------------------------------------------------------------
PuleDsValue puleDsCreateI64(int64_t const value) {
  return {::pdsValueAdd(value)};
}
PuleDsValue puleDsCreateF64(double const value) {
  return {::pdsValueAdd(value)};
}
PuleDsValue puleDsCreateBool(bool const value) {
  return {::pdsValueAdd(value)};
}
PuleDsValue puleDsCreateString(PuleStringView const stringView) {
  std::string contents = std::string(stringView.contents);
  contents.resize(stringView.len);
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

  if (value.id != 0) {
    ::pdsValues.erase(value.id);
  }
}

//------------------------------------------------------------------------------
void puleDsAppendArray(PuleDsValue const arrayValue, PuleDsValue const value) {
  PdsArray * asArray = getArrayElement(arrayValue);
  if (!asArray) { return; }
  asArray->emplace_back(value);
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
  auto const hash = puleStringViewHash(puleStringViewCStr(memberLabel));
  auto member = object.values.find(hash);
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
    PuleStringView const stringView = oldObj.labels[it];
    puleDsAssignObjectMember(
      newObj,
      stringView,
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
    puleDsAppendArray(newArr, puleDsValueCloneRecursively(value, allocator));
  }
  return newArr;
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
  if (puleDsIsBool(value)) {
    return puleDsCreateBool(puleDsAsBool(value));
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

PuleDsValue puleDsAssignObjectMember(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
) {
  auto & object = *std::get_if<PdsObject>(&::pdsValues.at(objectValue.id));
  ::pdsObjectAdd(object, memberLabel, valueToEmplace);
  return puleDsObjectMember(objectValue, memberLabel.contents);
}

} // C
