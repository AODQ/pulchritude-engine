#include <pulchritude-data-serializer/data-serializer.h>

#include <pulchritude-file/file.h>

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
  std::vector<PuleStringView> linearLabelsAsViews;
  std::vector<PuleDsValue> linearValues;
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
    .contents = object.linearLabels.back().c_str(), .len = label.len
  };
  object.linearLabelsAsViews.emplace_back(stringView);
  object.labels.emplace(hash, stringView);
}

union pdsValueUnion {
  int64_t asI64 = 0;
  double asF64;
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
  PuleStringView,
  PdsArray,
  PdsObject
>;

std::unordered_map<uint64_t, PdsValue> pdsValues;
uint64_t pdsValueIt = 1;

template <typename T>
uint64_t pdsValueAdd(T const value) {
  pdsValues.emplace(pdsValueIt++, value);
  return pdsValueIt;
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
PuleStringView puleDsAsString(PuleDsValue const value) {
  return *std::get_if<PuleStringView>(&::pdsValues.at(value.id));
}
PuleDsValueArray puleDsAsArray(PuleDsValue const value) {
  auto const & array = *std::get_if<PdsArray>(&::pdsValues.at(value.id));
  return { .values = array.data(), .length = array.size(), };
}
PuleDsValueObject puleDsAsObject(PuleDsValue const value) {
  auto const & object = *std::get_if<PdsObject>(&::pdsValues.at(value.id));
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
bool puleDsIsString(PuleDsValue const value) {
  return std::get_if<PuleStringView>(&::pdsValues.at(value.id)) != nullptr;
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
PuleDsValue puleDsCreateString(PuleStringView const stringView) {
  return {::pdsValueAdd(stringView)};
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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace {
PuleDsValue pdsParseValue(
  PuleAllocator const allocator,
  PuleFileStream const stream
);
} // namespace

namespace {

const std::string whitespaceAsString = " \n\r\t\f\v";
bool isWhitespace(char const character) {
  return (
    character == ' ' || character == '\t' || character == '\n'
    || character == '\r'
  );
}

// TODO maybe somehow move to file.h?
struct pdsStreamer {
  PuleFile file = { 0 };
  uint8_t pdsData[128] = { 0 };
  size_t pdsDataLength = 0;
  size_t pdsDataIter = 0;
};

// accounts for comments
char pdsReadByte(PuleFileStream const stream) {
  char character = puleStreamReadByte(stream);
  if (character != '#') return character;
  while (!puleStreamIsDone(stream) && puleStreamReadByte(stream) != '\n');
  return puleStreamReadByte(stream);
}

std::string trimBeginEndWhitespace(std::string const & s) {
  size_t const start = s.find_first_not_of(whitespaceAsString);
  if (start == std::string::npos) { return ""; }
  size_t const end = s.find_last_not_of(whitespaceAsString);
  return s.substr(start, end+1);
}

std::string pdsParseLabel(PuleFileStream const stream) {
  bool firstLetter = false;
  bool invalidLabel = false;
  std::string label;
  char character = 0;
  while (true) {
    if (puleStreamIsDone(stream)) {
      puleLogError("hit EOF trying to parse label '%s', label");
      return "";
    }
    character = pdsReadByte(stream);
    if (
      firstLetter
      && !(
           (character >= 'a' && character <= 'z')
        || (character >= 'a' && character <= 'Z')
      )
    ) {
      invalidLabel = true;
    }
    if (character == ':' || isWhitespace(character)) {
      break;
    }
    if (
      !(
           (character >= 'a' && character <= 'z')
        || (character >= 'a' && character <= 'Z')
        || (character >= '0' && character <= '9')
        || (character == '-' || character == '_')
      )
    ) {
      invalidLabel = true;
    }
    label += character;
  }
  // get to : in case we hit whitespace
  while (character != ':') {
    character = pdsReadByte(stream);
  }
  if (invalidLabel || label == "") {
    puleLogError("invalid label: '%s'", label);
  }
  return label;
}

int64_t parseHex(std::string const & value) {
  int64_t hex = 0;
  for (size_t it = 0; it < value.size(); ++ it) {
    char const ch = value[it];
    if (ch >= '0' && ch <= '9') {
      hex = hex*16 + (ch-'0');
    } else if (ch >= 'A' && ch <= 'F') {
      hex = hex*16 + (ch-'A') + 10;
    } else if (ch >= 'a' && ch <= 'f') {
      hex = hex*16 + (ch-'a') + 10;
    }
    puleLogError("could not parse hex value from: '%s'", value);
    break;
  }
  return hex;
}

int64_t parseBin(std::string const & value) {
  int64_t bin = 0;
  for (size_t it = 0; it < value.size(); ++ it) {
    char const ch = value[it];
    if (ch == '0') {
      bin *= 2;
    } else if (ch == '1') {
      bin = bin*2 + 1;
    }
    puleLogError("could not parse bin value from: '%s'", value);
    break;
  }
  return bin;
}

int64_t parseDec(std::string const & value) {
  int64_t dec = 0;
  int64_t sign = 1;
  for (size_t it = 0; it < value.size(); ++ it) {
    char const ch = value[it];
    if (it == 0 && ch == '-') {
      sign = -1;
      continue;
    }
    if (ch >= '0' && ch <= '9') {
      dec = dec*10 + (ch-'0');
    }
    puleLogError("could not parse dec value from: '%s'", value);
    break;
  }
  return sign*dec;
}

PuleDsValue parseBase64(std::string const & value) {
  puleLogError("base 64 encoding not yet implemented: '%s'", value);
  return { 0 };
}

PuleDsValue pdsParseArray(
  PuleAllocator const allocator,
  PuleFileStream const stream
) {
  assert(character == '[');

  PuleDsValue object = puleDsCreateArray(allocator);;
  while (true) {
    // skip whitespace
    if (::isWhitespace(pdsReadByte(stream))) {
      continue;
    }
    puleDsAppendArray(object, pdsParseValue(allocator, stream));
  }

  return object;
}

PuleDsValue pdsParseObject(
  PuleAllocator const allocator,
  PuleFileStream const stream,
  bool inObject=false
) {
  char character = ' ';
  while (!inObject && character != '{') {
    if (puleStreamIsDone(stream)) {
      puleLogError("hit EOF while parsing object");
      return {0};
    }
    character = pdsReadByte(stream);
  }
  assert(character == '{');

  PuleDsValue object = puleDsCreateObject(allocator);
  while (true) {
    character = pdsReadByte(stream);
    // skip whitespace
    if (::isWhitespace(character)) {
      continue;
    }
    if (puleStreamIsDone(stream)) {
      puleLogError("hit EOF while parsing object");
      return {0};
    }
    if (character == '}') {
      break;
    }
    std::string memberLabel = pdsParseLabel(stream);
    puleDsAssignObjectMember(
      object,
      PuleStringView {
        .contents = memberLabel.c_str(),
        .len = memberLabel.size()
      },
      pdsParseValue(allocator, stream)
    );
  }

  return object;
}

PuleDsValue pdsParseValue(
  PuleAllocator const allocator,
  PuleFileStream const stream
) {
  char character = ' ';
  // skip whitespace
  while (::isWhitespace(character)) {
    character = pdsReadByte(stream);
    continue;
  }

  // first get this thing into a value to check what type it is
  std::string value = "";
  {
    char prevChar = ' ';
    bool inString = false;
    while (!inString && character != ',') {
      value += character;
      character = pdsReadByte(stream);
      if (character == '[' || character == '{') {
        break;
      }
      if (prevChar != '\"' && character == '"') {
        inString ^= 1;
      }
      prevChar = character;

      if (!inString && (character == '}' || character == ']')) {
        puleLogError("seems like you're missing a comma");
      }
    }
  }

  if (character == '[') {
    return pdsParseArray(allocator, stream);
  }

  if (character == '{') {
    return pdsParseObject(allocator, stream, true);
  }

  value = trimBeginEndWhitespace(value);

  if (value == "") {
    puleLogError("assigning empty value to label");
    return { 0 };
  }

  // example, "" "asdf"
  if (value.size() > 2 && value[0] == '"' && value[value.size()-1] == '"') {
    value = value.substr(1, value.size()-1);
    return puleDsCreateString(
      PuleStringView { .contents = value.c_str(), .len = value.size(), }
    );
  }

  // example, 0x0 0x00fE30
  if (value.size() > 2 && value[0] == '0' && value[1] == 'x') {
    return puleDsCreateI64(parseHex(value.substr(2, value.size())));
  }

  // example, 0b00, 0b001000
  if (value.size() > 2 && value[0] == '0' && value[1] == 'b') {
    return puleDsCreateI64(parseBin(value.substr(2, value.size())));
  }

  // example, 64xY38zWxs
  if (
    value.size() > 3 && value[0] == '6' && value[1] == '4' && value[2] == 'x'
  ) {
    return parseBase64(value.substr(2, value.size()));
  }

  // example, 0.0 3238.238230
  if (value.find_first_not_of('.') != std::string::npos) {
    return puleDsCreateF64(std::stod(value));
  }

  return puleDsCreateI64(parseDec(value));
}

} // namespace

extern "C" {

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
      error
    )
  );
  if (puleErrorConsume(error) > 0) {
    PULE_error(
      PuleErrorDataSerializer_fileOpen,
      "failed to open: '%s'", filename
    );
    return { 0 };
  }

  uint8_t pdsData[128];
  PuleArrayViewMutable pdsDataView = {
    .data = &pdsData[0],
    .elementStride = sizeof(uint8_t),
    .elementCount = 128,
  };
  PuleFileStream stream = puleFileStream(file, pdsDataView);

  auto const defaultObject = puleDsCreateObject(allocator);

  while (!puleStreamIsDone(stream)) {
    uint8_t character = pdsReadByte(stream);
    // skip whitespace
    if (::isWhitespace(character)) {
      continue;
    }
    std::string memberLabel = pdsParseLabel(stream);
    puleDsAssignObjectMember(
      defaultObject,
      PuleStringView {
        .contents = memberLabel.c_str(),
        .len = memberLabel.size()
      },
      pdsParseObject(allocator, stream)
    );
  }

  puleFileStreamDestroy(stream);
  puleFileClose(file);

  return defaultObject;
}

} // extern C

namespace {

void addTab(PuleDsWriteInfo info, uint32_t const tabLevel, std::string & out) {
  if (!info.prettyPrint) {
    return;
  }
  for (uint32_t it = 0; it < tabLevel; ++ it) {
    for (uint32_t itspace = 0; itspace < info.spacePerTab; ++ itspace) {
      out += " ";
    }
  }
}

void pdsIterateWriteToStdout(
  PuleDsWriteInfo const info,
  uint32_t const tabLevel,
  std::string & out
) {
  if (puleDsIsI64(info.head)) {
    out += std::to_string(puleDsAsI64(info.head));
    out += info.prettyPrint ? ",\n" : ",";
  }
  else
  if (puleDsIsF64(info.head)) {
    out += std::to_string(puleDsAsF64(info.head));
    out += info.prettyPrint ? ",\n" : ",";
  }
  else
  if (puleDsIsString(info.head)) {
    PuleStringView const stringView = puleDsAsString(info.head);
    out += '\'';
    out += std::string_view(stringView.contents, stringView.len);
    out += '\'';
    out += info.prettyPrint ? ",\n" : ",";
  }
  else
  if (puleDsIsArray(info.head)) {
    PuleDsValueArray const array = puleDsAsArray(info.head);
    out += info.prettyPrint ? "[\n" : "[";
    for (size_t it = 0; it < array.length; ++ it) {
      addTab(info, tabLevel+1, out);
      PuleDsWriteInfo childInfo = info;
      childInfo.head = array.values[it];
      pdsIterateWriteToStdout(childInfo, tabLevel+1, out);
    }
    addTab(info, tabLevel, out);
    out += info.prettyPrint ? "],\n" : "]";
  }
  else
  if (puleDsIsObject(info.head)) {
    PuleDsValueObject const object = puleDsAsObject(info.head);
    out += info.prettyPrint ? "{\n" : "{";
    for (size_t it = 0; it < object.length; ++ it) {
      addTab(info, tabLevel+1, out);
      // write label
      PuleStringView const stringView = puleDsAsString(info.head);
      out += std::string_view(stringView.contents, stringView.len);
      out += info.prettyPrint ? ":" : ": ";

      // write value
      PuleDsWriteInfo childInfo = info;
      childInfo.head = object.values[it];
      pdsIterateWriteToStdout(childInfo, tabLevel+1, out);
    }
    addTab(info, tabLevel, out);
    out += info.prettyPrint ? "},\n" : "}";
  }
}

} // namespace

extern "C" {

void puleDsWriteToStdout(PuleDsWriteInfo const info) {
  if (info.head.id == 0) {
    return;
  }

  std::string out;
  ::pdsIterateWriteToStdout(info, info.initialTabLevel, out);
  printf("---------------\n%s\n---------------\n", out.c_str());
}

} // C
