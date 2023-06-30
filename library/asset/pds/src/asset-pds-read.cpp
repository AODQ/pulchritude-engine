#include <pulchritude-asset/pds.h>

#include <string>
#include <string.h>
#include <vector>

namespace {
PuleDsValue pdsParseValue(
  PuleAllocator const allocator,
  PuleStreamRead const stream
);
} // namespace

namespace {

static char const * const base64Chars = (
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
);

[[maybe_unused]]
size_t base64DecodedSize(PuleStringView const stringView) {
  size_t len = stringView.len/4 * 3;
  // account for '=' padding
  for (int32_t it = stringView.len-1; -- it > 0;) {
    if (stringView.contents[it] != '=') {
      break;
    }
    len -= 1;
  }
  return len;
}

static constexpr int32_t base64CharOffset = '+';

void base64GenerateDecodeTable(int32_t charToCode[80]) {
  memset(charToCode, -1, 80);
  // fill table with correct mappings
  for (size_t it = 0; it < 79; ++ it) {
    charToCode[base64Chars[it]-base64CharOffset] = static_cast<uint32_t>(it);
  }
}

[[maybe_unused]]
bool base64IsValid(char const c) {
  return(
       (c >= '0' && c <= '9')
    || (c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    || (c == '+' || c == '/' || c == '=')
  );
}

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
    if (!isWhitespace(character)) {
      puleLogError(
        "PDS parsing error: expected whitespace after '%s', found '%c'",
        label.c_str(), character
      );
      return label;
    }
    character = pdsReadByte(stream);
  }
  if (invalidLabel || label == "") {
    puleLogError("PDS parsing error: invalid label '%s'", label.c_str());
    if (label.front() == '"' && label.back() == '"') {
      puleLog("\tNOTE: did you mean to omit parenthesis in label?");
    }
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

PuleDsValue parseBuffer(
  PuleAllocator const allocator,
  PuleStringView const stringView
) {
  std::vector<uint8_t> buffer;

  // -- decode base64
  static int32_t charToCode[80];
  static bool hasInstantiatedDecodeTable = false;
  if (!hasInstantiatedDecodeTable) {
    base64GenerateDecodeTable(charToCode);
    hasInstantiatedDecodeTable = true;
  }

  PULE_assert(stringView.len % 4 == 0);

  for (size_t it = 0; it < stringView.len; ++ it) {
    int32_t value = 0;
    for (size_t baseIt = 0; baseIt < 4; ++ baseIt) {
      value = (
        stringView.contents[it] == '='
        ? (value << 6)
        : (
          (value << 6) | charToCode[stringView.contents[it] - base64CharOffset]
        )
      );
      ++ it;
    }

    buffer.emplace_back((value >> 16) & 0xFF);
    buffer.emplace_back((value >>  8) & 0xFF);
    buffer.emplace_back((value >>  0) & 0xFF);
  }

  return (
    puleDsCreateBuffer(
      allocator,
      PuleArrayView{
        .data = buffer.data(),
        .elementStride = sizeof(uint8_t),
        .elementCount = buffer.size(),
      }
    )
  );
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
    puleDsArrayAppend(object, pdsParseValue(allocator, stream));
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
    puleDsObjectMemberAssign(
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

  if (value == "true" || value == "false") {
    return puleDsCreateI64(value == "true");
  }

  // parse string, example, "", "asdf"
  if (value.size() > 1 && value[0] == '"' && value[value.size()-1] == '"') {
    value = value.substr(1, value.size()-2);
    auto str = puleDsCreateString(
      PuleStringView { .contents = value.c_str(), .len = value.size(), }
    );
    return str;
  }

  // parse hex, example, 0x0, 0x00fE30
  if (value.size() > 2 && value[0] == '0' && value[1] == 'x') {
    return puleDsCreateI64(parseHex(value.substr(2, value.size())));
  }

  // parse binary, example, 0b00, 0b001000
  if (value.size() > 2 && value[0] == '0' && value[1] == 'b') {
    return puleDsCreateI64(parseBin(value.substr(2, value.size())));
  }

  // parse buffer, example, *"ABC"
  if (value.size() > 1 && value[0] == '*') {
    PULE_assert(value.size() > 3);
    return (
      parseBuffer(
        allocator,
        PuleStringView{.contents = value.c_str()+2, .len = value.size()-3,}
      )
    );
  }

  // parse float, example, 0.0 3238.238230
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
    puleDsObjectMemberAssign(
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

PuleDsValue puleAssetPdsLoadFromRvalStream(
  PuleAllocator const allocator,
  PuleStreamRead const rvalStream,
  PuleError * const error
) {
  PuleDsValue const layoutValue = (
    puleAssetPdsLoadFromStream(
      allocator,
      rvalStream,
      error
    )
  );
  puleStreamReadDestroy(rvalStream);
  return layoutValue;
}

PuleDsValue puleAssetPdsLoadFromFile(
  PuleAllocator const allocator,
  PuleStringView const filename,
  PuleError * const error
) {
  PuleFile const file = (
    puleFileOpen(
      filename,
      PuleFileDataMode_text,
      PuleFileOpenMode_read,
      error
    )
  );
  if (puleErrorConsume(error)) {
    PULE_error(PuleErrorAssetPds_decode, "failed to open file");
    return { 0 };
  }

  // create a file stream, using a storage buffer for the stream to store
  //   intermediary data in
  uint8_t streamStorage[512];
  PuleArrayViewMutable streamStorageView = {
    .data = &streamStorage[0],
    .elementStride = sizeof(uint8_t),
    .elementCount = 512,
  };

  // plug the stream into the data-serializer to deserialize its contents
  PuleStreamRead const stream = puleFileStreamRead(file, streamStorageView);
  PuleDsValue const head = puleAssetPdsLoadFromStream(allocator, stream, error);

  // even if error occured we don't have to do anything special
  puleFileClose(file);
  puleStreamReadDestroy(stream);
  return head;
}

PuleDsValue puleAssetPdsLoadFromString(
  PuleAllocator const allocator,
  PuleStringView const contents,
  PuleError * const error
) {
  PuleStreamRead const stream = puleStreamReadFromString(contents);
  PuleDsValue const head = puleAssetPdsLoadFromStream(allocator, stream, error);
  puleStreamReadDestroy(stream);
  return head;
}

static void printCommandLineParameterHelp(
  char const * const argumentName,
  PuleDsValue const layoutValue
) {
  printf("---- help for '%s' ----\n", argumentName);
  puleAssetPdsWriteToStdout(layoutValue);
}

PuleDsValue puleAssetPdsLoadFromCommandLineArguments(
  PuleAssetPdsCommandLineArgumentsInfo const info,
  PuleError * const error
) {
  // if no work to do just return
  if (info.argumentLength <= 1) {
    return { 0 };
  }

  // parse the user requested layout
  PULE_assert(info.layout.id != 0);

  // TODO assert the layoutValue is in correct format

  // prepare to iterate over the user requested layout, and emit the arguments
  //   to a separate PDS
  PuleDsValue const emitValue = puleDsCreateObject(info.allocator);

  PuleDsValue emitCommandValue = (
    puleDsObjectMemberAssign(
      emitValue,
      puleCStr("command"),
      puleDsCreateArray(info.allocator)
    )
  );

  PuleDsValue emitParametersValue = (
    puleDsObjectMemberAssign(
      emitValue,
      puleCStr("parameters"),
      puleDsCreateObject(info.allocator)
    )
  );

  PuleDsValue layoutIt = info.layout;
  char const * helpPreviousLayoutArgument = info.arguments[0];

  // parse objects
  int32_t argumentIt = 1;
  char const * argument; // track last argument outside of function scope
  for (; argumentIt < info.argumentLength; ++ argumentIt) {
    argument = info.arguments[argumentIt];

    if (strcmp(argument, "--help") == 0) {
      printCommandLineParameterHelp(helpPreviousLayoutArgument, layoutIt);
      if (info.userRequestedHelpOutNullable) {
        *info.userRequestedHelpOutNullable = true;
      }
      puleDsDestroy(emitValue);
      return { 0 };
    }
    helpPreviousLayoutArgument = info.arguments[argumentIt];

    PuleDsValue arg = puleDsObjectMember(layoutIt, argument);
    if (arg.id == 0) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "unexpected parameter: '%s'", argument
      );
      puleDsDestroy(emitValue);
      return { 0 };
    }
    layoutIt = arg;
    puleDsArrayAppend(
      emitCommandValue, puleDsCreateString(puleCStr(argument))
    );
    if (puleDsIsObject(arg)) {
      continue;
    }
    if (puleDsIsArray(arg)) {
      argumentIt += 1;
      break;
    }
  }

  if (strcmp(argument, "--help") == 0) {
    printCommandLineParameterHelp(helpPreviousLayoutArgument, layoutIt);
    if (info.userRequestedHelpOutNullable) {
      *info.userRequestedHelpOutNullable = true;
    }
    puleDsDestroy(emitValue);
    return { 0 };
  }
  helpPreviousLayoutArgument = info.arguments[argumentIt];

  PuleDsValueArray layoutArray;

  if (!puleDsIsArray(layoutIt)) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "not enough parameters passed from '%s'", argument
      );
      puleDsDestroy(emitValue);
      return { 0 };
    goto finishArrayParsing;
  }

  layoutArray = puleDsAsArray(layoutIt);

  // parse arrays
  // TODO parse types (ints, floats, etc)
  // TODO enforce opt=true
  for (; argumentIt < info.argumentLength; ++ argumentIt) {
    argument = info.arguments[argumentIt];
    size_t const argumentLen = strlen(argument);

    if (strcmp(argument, "--help") == 0) {
      printCommandLineParameterHelp(helpPreviousLayoutArgument, layoutIt);
      if (info.userRequestedHelpOutNullable) {
        *info.userRequestedHelpOutNullable = true;
      }
      puleDsDestroy(emitValue);
      return { 0 };
    }

    // at this point everything must have preceding '--'
    if (argument[0] != '-' || argument[1] != '-') {
      PULE_error(
        PuleErrorAssetPds_decode,
        "unexpected parameter: '%s'", argument
      );
      puleDsDestroy(emitValue);
      return { 0 };
    }

    puleLogDebug("Working on argument %s", argument);

    // find matching parameter in layout
    bool parameterFound = false;
    for (size_t arrIt = 0; arrIt < layoutArray.length; ++ arrIt) {
      PuleDsValue const obj = layoutArray.values[arrIt];
      PuleStringView const objLabel = (
        puleDsAsString(puleDsObjectMember(obj, "label"))
      );

      // if input parameter != layout-iter parameter
      if (strncmp(objLabel.contents, argument+2, argumentLen)) {
        continue;
      }
      parameterFound = true;

      PuleStringView const objType = (
        puleDsAsString(puleDsObjectMember(obj, "type"))
      );

      if (
           puleStringViewEqCStr(objType, "url")
        || puleStringViewEqCStr(objType, "string")
      ) {
        if (argumentIt+1 >= info.argumentLength) {
          PULE_error(
            PuleErrorAssetPds_decode,
            "missing argument for '%s'", argument
          );
          puleDsDestroy(emitValue);
          return { 0 };
        }

        puleDsObjectMemberAssign(
          emitParametersValue,
          objLabel,
          puleDsCreateString(puleCStr(info.arguments[argumentIt+1]))
        );
        argumentIt += 1;
      }
      else
      if (puleStringViewEqCStr(objType, "bool")) {
        bool isTrue = true;
        if (argumentIt+1 < info.argumentLength) {
          if (strcmp(info.arguments[argumentIt+1], "true") == 0) {
            argumentIt += 1;
          }
          else
          if (strcmp(info.arguments[argumentIt+1], "false") == 0) {
            isTrue = false;
            argumentIt += 1;
          }
        }
        puleDsObjectMemberAssign(
          emitParametersValue,
          objLabel,
          puleDsCreateI64(isTrue)
        );
      }
      else
      if (puleStringViewEqCStr(objType, "int")) {
        if (argumentIt+1 >= info.argumentLength) {
          PULE_error(
            PuleErrorAssetPds_decode,
            "missing argument for '%s'", argument
          );
          puleDsDestroy(emitValue);
          return { 0 };
        }

        puleDsObjectMemberAssign(
          emitParametersValue,
          objLabel,
          puleDsCreateI64(parseDec(std::string(info.arguments[argumentIt+1])))
        );
      }

      break;
    }

    if (!parameterFound) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "unknown option: '%s'", argument
      );
      puleDsDestroy(emitValue);
      return { 0 };
    }
  }

  // assign default values for optional layout values
  for (size_t arrIt = 0; arrIt < layoutArray.length; ++ arrIt) {
    PuleDsValue const obj = layoutArray.values[arrIt];
    PuleDsValue const defaultValue = puleDsObjectMember(obj, "default-value");
    PuleStringView const objLabel = (
      puleDsAsString(puleDsObjectMember(obj, "label"))
    );

    // check if optional
    if (defaultValue.id == 0) { continue; }

    // check if user already supplied argument
    if (puleDsObjectMember(emitParametersValue, objLabel.contents).id != 0) {
      continue;
    }

    if (defaultValue.id == 0) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "layout for '%s' is optional but doesn't have a default value",
        objLabel.contents
      );
      puleDsDestroy(emitValue);
      return { 0 };
    }

    // clone object into the emit values
    puleDsObjectMemberAssign(
      emitParametersValue,
      objLabel,
      puleDsValueCloneRecursively(defaultValue, info.allocator)
    );
  }

  finishArrayParsing:

  return emitValue;
}

} // C
