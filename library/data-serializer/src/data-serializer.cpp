#include <pulchritude-data-serializer/data-serializer.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {

size_t pdsHash(PuleStringView const view) {
  auto stringView = std::string_view(view.contents, view.len);
  auto hash = std::hash<std::string_view>{}(stringView);
  return hash;
}

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
  size_t const hash = ::pdsHash(label);
  PULE_assert(object.values.find(hash) == object.values.end());

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
double puleDsAsF64(PuleDsValue const value) {
  return *std::get_if<double>(&::pdsValues.at(value.id));
}
bool puleDsAsBool(PuleDsValue const value) {
  return *std::get_if<bool>(&::pdsValues.at(value.id));
}
PuleStringView puleDsAsString(PuleDsValue const value) {
  std::string & asString = (
    std::get_if<PdsString>(&::pdsValues.at(value.id))->value
  );
  return (
    PuleStringView { .contents = asString.c_str(), .len = asString.size(), }
  );
}
PuleDsValueArray puleDsAsArray(PuleDsValue const value) {
  auto const & array = *std::get_if<PdsArray>(&::pdsValues.at(value.id));
  return { .values = array.data(), .length = array.size(), };
}
PuleDsValueObject puleDsAsObject(PuleDsValue const value) {
  auto & object = *std::get_if<PdsObject>(&::pdsValues.at(value.id));

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
bool puleDsIsObject(PuleDsValue const value) {
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
PuleDsValue puleDsCreateString(PuleStringView const stringView) {
  return {::pdsValueAdd(PdsString{.value = std::string(stringView.contents)})};
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
  auto & array = *std::get_if<PdsArray>(&::pdsValues.at(arrayValue.id));
  array.emplace_back(value);
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
  PuleStringView const memberLabel
) {
  auto const & object = (
    *std::get_if<PdsObject>(&::pdsValues.at(objectValue.id))
  );
  auto const hash = pdsHash(memberLabel);
  return object.values.at(hash);
}

void puleDsAssignObjectMember(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
) {
  auto & object = *std::get_if<PdsObject>(&::pdsValues.at(objectValue.id));
  ::pdsObjectAdd(object, memberLabel, valueToEmplace);
}

} // C
