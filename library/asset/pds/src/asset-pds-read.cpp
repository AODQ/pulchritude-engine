#include <pulchritude-asset/pds.h>

#include <string>

namespace {
PuleDsValue pdsParseValue(
  PuleAllocator const allocator,
  PuleStreamRead const stream
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

// accounts for comments
char pdsReadByte(PuleStreamRead const stream) {
  char character = puleStreamReadByte(stream);
  if (character != '#') return character;
  while (
    !puleStreamReadIsDone(stream)
    && puleStreamReadByte(stream) != '\n'
  );
  return puleStreamReadByte(stream);
}

std::string trimBeginEndWhitespace(std::string const & s) {
  size_t const start = s.find_first_not_of(whitespaceAsString);
  if (start == std::string::npos) { return ""; }
  size_t const end = s.find_last_not_of(whitespaceAsString);
  return s.substr(start, end-start+1);
}

std::string pdsParseLabel(PuleStreamRead const stream) {
  bool firstLetter = false;
  bool invalidLabel = false;
  std::string label;
  char character = 0;
  while (true) {
    if (puleStreamReadIsDone(stream)) {
      puleLogError("hit EOF trying to parse label '%s'", label.c_str());
      return "";
    }
    character = pdsReadByte(stream);
    if (
      firstLetter
      && !(
           (character >= 'a' && character <= 'z')
        || (character >= 'A' && character <= 'Z')
        || character == '_'
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
        || (character >= 'A' && character <= 'Z')
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
    puleLogError("invalid label: '%s'", label.c_str());
  }
  return label;
}

int64_t parseHex(std::string const & value) {
  int64_t hex = 0;
  for (size_t it = 0; it < value.size(); ++ it) {
    char const ch = value[it];
    if (ch >= '0' && ch <= '9') {
      hex = hex*16 + (ch-'0');
    }
    else
    if (ch >= 'A' && ch <= 'F') {
      hex = hex*16 + (ch-'A') + 10;
    }
    else
    if (ch >= 'a' && ch <= 'f') {
      hex = hex*16 + (ch-'a') + 10;
    } else {
      puleLogError("could not parse hex value from: '%s'", value.c_str());
    }
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
    }
    else
    if (ch == '1') {
      bin = bin*2 + 1;
    } else {
      puleLogError("could not parse bin value from: '%s'", value.c_str());
    }
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
    else
    if (ch >= '0' && ch <= '9') {
      dec = dec*10 + (ch-'0');
      continue;
    }
    else {
      puleLogError("could not parse dec value from: '%s'", value.c_str());
    }
    break;
  }
  return sign*dec;
}

PuleDsValue parseBase64(std::string const & value) {
  puleLogError("base 64 encoding not yet implemented: '%s'", value.c_str());
  return { 0 };
}

PuleDsValue pdsParseArray(
  PuleAllocator const allocator,
  PuleStreamRead const stream
) {
  PuleDsValue object = puleDsCreateArray(allocator);
  while (true) {
    while (::isWhitespace(puleStreamPeekByte(stream))) {
      pdsReadByte(stream);
      continue;
    }
    if (puleStreamReadIsDone(stream)) {
      puleLogError("hit EOF while parsing object");
      return {0};
    }
    if (puleStreamPeekByte(stream) == ']') {
      break;
    }
    puleDsAppendArray(object, pdsParseValue(allocator, stream));
  }

  // consume ]
  pdsReadByte(stream);
  // consume ','
  while (::isWhitespace(puleStreamPeekByte(stream))) {
    pdsReadByte(stream);
  }
  pdsReadByte(stream);

  return object;
}

PuleDsValue pdsParseObject(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  bool inObject=false
) {
  char character = ' ';
  while (!inObject && character != '{') {
    if (puleStreamReadIsDone(stream)) {
      puleLogError("hit EOF while parsing object");
      return {0};
    }
    character = pdsReadByte(stream);
  }

  PuleDsValue object = puleDsCreateObject(allocator);
  while (true) {
    // skip whitespace
    while (::isWhitespace(puleStreamPeekByte(stream))) {
      pdsReadByte(stream);
    }
    if (puleStreamReadIsDone(stream)) {
      puleLogError("hit EOF while parsing object");
      return {0};
    }
    if (puleStreamPeekByte(stream) == '}') {
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

  // consume }
  pdsReadByte(stream);
  // consume ','
  while (::isWhitespace(puleStreamPeekByte(stream))) {
    pdsReadByte(stream);
  }
  pdsReadByte(stream);

  return object;
}

PuleDsValue pdsParseValue(
  PuleAllocator const allocator,
  PuleStreamRead const stream
) {
  while (::isWhitespace(puleStreamPeekByte(stream))) {
    pdsReadByte(stream);
  }
  char character = pdsReadByte(stream);

  if (character == '[') {
    return pdsParseArray(allocator, stream);
  }

  if (character == '{') {
    return pdsParseObject(allocator, stream, true);
  } (void)'}';

  // first get this thing into a value to check what type it is
  std::string value = "";
  value += character;
  {
    char prevChar = '\\';
    bool inString = character == '"';
    while (true) {
      prevChar = character;
      character = pdsReadByte(stream);

      if (!inString && character == ',') {
        break;
      }

      value += character;

      if (inString && prevChar != '\\' && character == '"') {
        // get & consume to the ','
        while (::isWhitespace(puleStreamPeekByte(stream))) {
          pdsReadByte(stream);
        }
        [[maybe_unused]]
        char const c = pdsReadByte(stream);
        PULE_assert(c == ',');
        break;
      }

      if (character == '[' || character == '{') {
        break;
      }

      if (!inString && (character == '}' || character == ']')) {
        puleLogError("seems like you're missing a comma");
      }
    }
  }

  value = trimBeginEndWhitespace(value);

  if (value == "") {
    puleLogError("assigning empty value to label");
    return { 0 };
  }

  if (value == "true" || value = "false") {
    return puleDsCreateBool(value == "true");
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
  if (value.find_first_of('.') != std::string::npos) {
    return puleDsCreateF64(std::stod(value));
  }

  return puleDsCreateI64(parseDec(value));
}

} // namespace

extern "C" {

PuleDsValue puleAssetPdsLoadFromStream(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  PuleError * const error
) {
  (void)error;
  auto const defaultObject = puleDsCreateObject(allocator);

  while (!puleStreamReadIsDone(stream)) {
    // skip whitespace
    if (::isWhitespace(puleStreamPeekByte(stream))) {
      pdsReadByte(stream);
      continue;
    }
    if (puleStreamReadIsDone(stream)) { break; }
    std::string memberLabel = pdsParseLabel(stream);
    puleDsAssignObjectMember(
      defaultObject,
      PuleStringView {
        .contents = memberLabel.c_str(),
        .len = memberLabel.size()
      },
      pdsParseValue(allocator, stream)
    );
  }

  return defaultObject;
}


}
