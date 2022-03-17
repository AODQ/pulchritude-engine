#include <pulchritude-data-serializer/data-serializer.h>

#include <pulchritude-file/file.h>

#include <unordered_map>
#include <string_view>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {

struct pdsObject {
  std::map<size_t, PuleDsValue> values = {};
  std::map<size_t, PuleStringView> labels = {};
  std::vector<std::string> linearLabels;
  std::vector<PuleDsValue> linearValues;
};

void pdsObjectAdd(
  pdsObject & object,
  size_t const hash,
  PuleStringView const label,
  PuleDsValue const value
) {
  PULE_assert(object.values.find(hash) == object.values.end());

  object.values.emplace({hash, value});
  object.linearValues.emplace_back(value);

  // copy string to local storage
  object.linearLabels.emplace_back(std::string(label.contents, label.len));
  auto const stringView = PuleStringView {
    .contents = object.linearLabels.back().c_str(), .len = label.len
  };
  object.labels.emplace({hash, stringView});
}

union pdsValueUnion {
  int64_t asI64;
  double asF64;
  PuleStringView asString;
  std::vector<PuleDsValue> asArray;
  std::map<size_t, PuleDsValue> asObject;
};

enum class pdsValueUnionTag : uint32_t {
  i64, f64, string, array, object,
};

struct pdsValue {
  pdsValueUnion value;
  pdsValueUnionTag tag;
};

std::unordered_map<int64_t, pdsValue> pdsValues;
size_t pdsValueIt = 1;

bool pdsValueIs(PuleDsValue const value, pdsValueUnionTag const tag) {
  return pdsValues.at(value.id).tag == tag;
}

void pdsValueAdd(pdsValueUnion const value, pdsValueUnionTag const tag) {
  pdsValues.emplace({pdsValueIt, {.value = value, .tag = tag,});
  pdsValueIt += 1;
}

size_t pdsHash(PuleStringView const view) {
  return std::hash<std::string_view>(std::string_view(view.contents, view.len));
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

extern "C" {

//------------------------------------------------------------------------------
int64_t puleDsIsI64(PuleDsValue const value) {
  return ::pdsValues.at(value.id).value.asI64;
}
double puleDsIsF64(PuleDsValue const value) {
  return ::pdsValues.at(value.id).value.asF64;
}
PuleStringView puleDsIsString(PuleDsValue const value) {
  return ::pdsValues.at(value.id).value.asString;
}
PuleDsValueArray puleDsIsArray(PuleDsValue const value) {
  auto const & array = ::pdsValues.at(value.id).value.asArray;
  return { .values = array.data(), .length = array.size(), };
}
PuleDsValueObject puleDsIsObject(PuleDsValue const value) {
  auto const & object = ::pdsValues.at(value.id).value.asObject;
  return {
    .labels = object.linearLabels.data(),
    .values = object.linearValues.data(),
    .length = object.linearValues.size(),
  };
}

//------------------------------------------------------------------------------
bool puleDsIsI64(PuleDsValue const value) {
  return ::pdsValueIs(value, ::pdsValueUnionTag::i64);
}
bool puleDsIsF64(PuleDsValue const value) {
  return ::pdsValueIs(value, ::pdsValueUnionTag::f64);
}
bool puleDsIsString(PuleDsValue const value) {
  return ::pdsValueIs(value, ::pdsValueUnionTag::string);
}
bool puleDsIsArray(PuleDsValue const value) {
  return ::pdsValueIs(value, ::pdsValueUnionTag::array);
}
bool puleDsIsObject(PuleDsValue const value) {
  return ::pdsValueIs(value, ::pdsValueUnionTag::object);
}

//------------------------------------------------------------------------------
PuleDsValue puleDsCreateI64(int64_t const value) {
  ::pdsValueAdd({.asI64 = value}, ::pdsValueUnionTag::i64);
}
PuleDsValue puleDsCreateF64(double const value) {
  ::pdsValueAdd({.asF64 = value}, ::pdsValueUnionTag::f64);
}
PuleDsValue puleDsCreateString(PuleStringView const stringView) {
  ::pdsValueAdd({.asString = stringView}, ::pdsValueUnionTag::string);
}
PuleDsValue puleDsCreateArray(PuleAllocator const allocator) {
  (void)allocator;
  ::pdsValueAdd({.asArray = {}}, ::pdsValueUnionTag::array);
}
PuleDsValue puleDsCreateObject(PuleAllocator const allocator) {
  (void)allocator;
  ::pdsValueAdd({.asObject = value}, ::pdsValueUnionTag::object);
}
void puleDsDestroy(PuleDsValue const value) {
  if (value.tag == ::pdsValueUnionTag::array) {
    for (auto const value : value.value.asArray) {
      pulePsDestroy(value);
    }
  }

  if (value.tag == ::pdsValueUnionTag::object) {
    for (auto const valuePair : value.value.asObject.values) {
      pulePsDestroy(valuePair.second);
    }
  }

  if (value.id != 0) {
    ::pdsValues.erase(value.id);
  }
}

//------------------------------------------------------------------------------
void puleDsAppendArray(PuleDsValue const arrayValue, PuleDsValue const value) {
  auto const & array = ::pdsValues.at(arrayValue.id).value.asArray;
  array.emplace_back(value);
}

size_t puleDsArrayLength(PuleDsValue const arrayValue) {
  auto const & array = ::pdsValues.at(arrayValue.id).value.asArray;
  return array.size();
}

PuleDsValue puleDsArrayElementAt(
  PuleDsValue const arrayValue,
  size_t const idx
) {
  auto const & array = ::pdsValues.at(arrayValue.id).value.asArray;
  return array[idx];
}

PuleDsValue puleDsObjectMember(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel
) {
  auto const & object = ::pdsValues.at(objectValue.id).value.asObject;
  auto const hash = pdsHash(memberLabel);
  return object.values.at(hash);
}

void puleDsAssignObjectMember(
  PuleDsValue const objectValue,
  PuleStringView const memberLabel,
  PuleDsValue const valueToEmplace
) {
  auto const & object = ::pdsValues.at(objectValue.id).value.asObject;
  ::pdsObjectAdd(object, ::pdsHash(memberLabel), valueToEmplace);
}

} // C

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {

bool isWhitespace(char const character) {
  return character == ' ' || character == '\t' || character == '\n';
}

std::string pdsParseLabel(PuleFile const file) {
  bool firstLetter = false
  bool invalidLabel = false;
  std::string label;
  char character = 0;
  while (true) {
    if (puleFileIsDone(file)) {
      puleLogError("hit EOF trying to parse label '%s', label);
      return "";
    }
    character = puleFileReadByte(file);
    if (
      firstLetter
      && !(
           character >= 'a' && character <= 'z'
        || character >= 'a' && character <= 'Z'
      )
    ) {
      invalidLabel = true;
    }
    if (character == ':' || isWhitespace(character)) {
      break;
    }
    if (
      !(
           character >= 'a' && character <= 'z'
        || character >= 'a' && character <= 'Z'
        || character >= '0' && character <= '9'
        || character == '-' || character == '_'
      )
    ) {
      invalidLabel = true;
    }
    label += character;
  }
  // get to : in case we hit whitespace
  while (character != ':') {
    character = puleFileReadByte(file);
  }
  if (invalidLabel || label == "") {
    puleLogError("invalid label: '%s'", label);
  }
  return label;
}

// accounts for comments
char pdsReadByte(PuleFile const file) {
  char character = puleFileReadByte(file);
  if (character != '#') return character;
  while (!puleFileIsDone(file) && puleFileReadByte(file) != '\n');
  return puleFileReadByte(file);
}

PuleDsValue pdsParseValue(PuleFile const file) {
  char character = ' ';
  // skip whitespace
  while (::isWhitespace(character)) {
    character = puleFileReadByte(file);
    continue;
  }

  // first get this thing into a value to check what type it is
  std::string value = "";
  while (character != "," || character == "\n") {
    value += character;
    character = puleFileReadByte(file);
  }

  if (value == "") {
    puleLogError("assigning empty value to label");
    return { 0 };
  }

  // just assume string for now
  return (
    puleDsCreateString(
      PuleStringView { .contents = value.c_str(), .len = value.length(), }
    )
  );
}

PuleDsValue pdsParseObjectValue(PuleFile const file) {
  char character = puleFileReadByte(file);
  // skip whitespace
  while (::isWhitespace(character)) {
    character = puleFileReadByte(file);
    continue;
  }
  if (character != ':') {
    puleLogError("was expecting : to assign value to member");
    return;
  }

  return pdsParseValue(file);
}

PuleDsValue pdsParseObject(PuleFile const file) {
  char character = ' ';
  while (character != '{') {
    if (puleFileIsDone(file)) {
      puleLogError("hit EOF while parsing object");
      return {0};
    }
    character = puleFileReadByte(file);
  }

  PuleDsValue object;
  while (true) {
    character = puleFileReadByte(file);
    // skip whitespace
    if (::isWhitespace(character)) {
      continue;
    }
    if (puleFileIsDone(file)) {
      puleLogError("hit EOF while parsing object");
      return {0};
    }
    if (character == '}') {
      break;
    }
    std::string memberLabel = pdsParseLabel(file);
    puleDsAssignObjectMember(
      object,
      memberLabel,
      pdsParseValue(file)
    );
  }

  return object;
}

} // namespace

PuleDsValue puleDsLoadFromFile(
  PuleAllocator const allocator,
  char const * const filename,
  PuleError * const error
) {
  PuleFile file = (
    puleFileOpen(
      filename,
      PuleFileDataMode_text,
      PuleFileOpenMode_read,
      &err
    )
  );
  if (puleErrorConsume(&err) > 0) {
    PULE_error(
      PuleErrorDataSerializer_fileOpen
      "failed to open: %s", filename
    );
    return { 0 };
  }

  auto const defaultObject = puleDsCreateObject(allocator);

  // TODO use puleFileReadBytes i guess to parse faster
  /* uint8_t pdsData[128]; */
  /* PuleArrayView pdsDataView = { */
  /*   .data = &pdsData[0], */
  /*   .elementStride = sizeof(uint8_t), */
  /*   .elementCount = 128, */
  /* }; */
  /* size_t pdsDataLength = 0; */

  /* while (!puleFileIsDone(file)) { */
  /*   size_t const pdsDataLength = puleFileReadBytes(file, pdsDataView); */
  /*   if (pdsDataLength == 0) { */
  /*     break; */
  /*   } */
  /* } */

  while (!puleFileIsDone(file)) {
    uint8_t character = puleFileReadByte(file);
    // skip whitespace
    if (::isWhitespace(character)) {
      continue;
    }
    std::string memberLabel = pdsParseLabel(file);
    puleDsAssignObjectMember(
      defaultObject,
      memberLabel,
      pdsParseObject(file)
    );
  }

  puleFileClose(file);
}

} // C
