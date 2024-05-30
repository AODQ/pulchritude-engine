#include <pulchritude/asset-pds.h>

#include <string>
#include <vector>

namespace {
PuleDsValue pdsParseValue(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  PuleError * const error
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

bool pdsIsDone(PuleStreamRead const stream) {
  if (puleStreamReadIsDone(stream)) {
    return true;
  }
  // if it's not done, check for comments (in case file ends with comment)
  char character = puleStreamPeekByte(stream);
  if (character != '#') {
    return false;
  }
  while (
    !puleStreamReadIsDone(stream)
    && puleStreamReadByte(stream) != '\n'
  );
  return puleStreamReadIsDone(stream);
}

// accounts for comments
char pdsReadByte(PuleStreamRead const stream) {
  char character = puleStreamReadByte(stream);
  if (character != '#') return character;
  while (
    !puleStreamReadIsDone(stream)
    && puleStreamReadByte(stream) != '\n'
  );
  if (puleStreamReadIsDone(stream)) {
    PULE_assert("hit EOF while parsing comment (faulty reading logic)");
    return 0;
  }
  return puleStreamReadByte(stream);
}

std::string trimBeginEndWhitespace(std::string const & s) {
  size_t const start = s.find_first_not_of(whitespaceAsString);
  if (start == std::string::npos) { return ""; }
  size_t const end = s.find_last_not_of(whitespaceAsString);
  return s.substr(start, end-start+1);
}

std::string pdsParseLabel(
  PuleStreamRead const stream,
  PuleError * const error
) {
  bool firstLetter = false;
  bool invalidLabel = false;
  char invalidChar = '\0';
  std::string label;
  char character = 0;
  while (true) {
    if (pdsIsDone(stream)) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "hit EOF trying to parse label '%s'", label.c_str()
      );
      return "";
    }
    character = pdsReadByte(stream);
    if (character == ':' || isWhitespace(character)) {
      break;
    }
    if (
      ( // invalid letters to begin with
        firstLetter
        && !(
             (character >= 'a' && character <= 'z')
          || (character >= 'A' && character <= 'Z')
          || character == '_'
        )
      )
      || // invalid letters no matter what
      !(
           (character >= 'a' && character <= 'z')
        || (character >= 'A' && character <= 'Z')
        || (character >= '0' && character <= '9')
        || character == '_' || character == '-'
      )
    ) {
      invalidLabel = true;
      invalidChar = character;
    }
    label += character;
  }
  // get to : in case we hit whitespace
  while (character != ':') {
    if (!isWhitespace(character)) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "PDS parsing error: expected whitespace after '%s', found '%c'",
        label.c_str(), character
      );
      return label;
    }
    if (pdsIsDone(stream)) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "hit EOF trying to parse label '%s'", label.c_str()
      );
      return "";
    }
    character = pdsReadByte(stream);
  }
  if (invalidLabel || label == "") {
    char const * note = "";
    if (label.front() == '"' && label.back() == '"') {
      note = "\n\tNOTE: did you try to omit parenthesis in label?";
    }
    PULE_error(
      PuleErrorAssetPds_decode,
      "PDS parsing error: invalid label '%s' (%c)%s",
      label.c_str(), invalidChar, note
    );
    return "";
  }
  return label;
}

int64_t parseHex(std::string const & value, PuleError * const error) {
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
      PULE_error(
        PuleErrorAssetPds_decode,
        "could not parse hex value from: '%s'",
        value.c_str()
      );
      return 0;
    }
    break;
  }
  return hex;
}

int64_t parseBin(std::string const & value, PuleError * const error) {
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
      PULE_error(
        PuleErrorAssetPds_decode,
        "could not parse bin value from: '%s'", value.c_str()
      );
      return 0;
    }
    break;
  }
  return bin;
}

int64_t parseDec(std::string const & value, PuleError * const error) {
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
      PULE_error(
        PuleErrorAssetPds_decode,
        "could not parse dec value from: '%s'", value.c_str()
      );
    }
    break;
  }
  return sign*dec;
}

PuleDsValue parseBuffer(
  PuleAllocator const allocator,
  PuleStringView const stringView,
  PuleError * const error
) {
  std::vector<uint8_t> buffer;

  // -- decode base64
  static int32_t charToCode[80];
  static bool hasInstantiatedDecodeTable = false;
  if (!hasInstantiatedDecodeTable) {
    base64GenerateDecodeTable(charToCode);
    hasInstantiatedDecodeTable = true;
  }

  if (stringView.len % 4 != 0) {
    PULE_error(
      PuleErrorAssetPds_decode,
      "base64 buffer length must be a multiple of 4"
    );
    return {0};
  }

  for (size_t it = 0; it < stringView.len;) {
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

#define ErrorFileDone() \
  if (pdsIsDone(stream)) { \
    PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing file"); \
    return {0}; \
  }


PuleDsValue pdsParseArray(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  PuleError * const error
) {
  PuleDsValue object = puleDsCreateArray(allocator);
  while (true) {
    while (
      !pdsIsDone(stream)
      && ::isWhitespace(puleStreamPeekByte(stream))
    ) {
      pdsReadByte(stream);
      continue;
    }
    if (pdsIsDone(stream)) {
      PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing object");
      return {0};
    }
    if (puleStreamPeekByte(stream) == ']') {
      break;
    }
    puleDsArrayAppend(object, pdsParseValue(allocator, stream, error));
    if (error->id) { return {0}; }
  }

  // consume ], we know it's a ']' since it's the only way to break out of loop
  PULE_assert(pdsReadByte(stream) == ']');
  // consume ','
  while (
    !pdsIsDone(stream)
    && ::isWhitespace(puleStreamPeekByte(stream))
  ) {
    pdsReadByte(stream);
  }
  if (pdsIsDone(stream)) {
    PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing object");
    return {0};
  }
  if (pdsReadByte(stream) != ',') {
    PULE_error(PuleErrorAssetPds_decode, "expected ',' after array");
    return {0};
  }

  return object;
}

PuleDsValue pdsParseObject(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  PuleError * const error,
  bool inObject=false
) {
  char character = ' ';
  while (!inObject && character != '{') {
    if (pdsIsDone(stream)) {
      PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing object");
      return {0};
    }
    character = pdsReadByte(stream);
  }

  PuleDsValue object = puleDsCreateObject(allocator);
  while (true) {
    // skip whitespace
    while (
      !pdsIsDone(stream)
      && ::isWhitespace(puleStreamPeekByte(stream))
    ) {
      pdsReadByte(stream);
    }
    if (pdsIsDone(stream)) {
      PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing object");
      return {0};
    }
    if (puleStreamPeekByte(stream) == '}') {
      break;
    }
    std::string const memberLabel = pdsParseLabel(stream, error);
    if (error->id) { return {0}; }
    puleDsObjectMemberAssign(
      object,
      PuleStringView {
        .contents = memberLabel.c_str(),
        .len = memberLabel.size()
      },
      pdsParseValue(allocator, stream, error)
    );
    if (error->id) { return {0}; }
  }

  // consume }, we know it's a '}' since it's the only way to break out of loop
  PULE_assert(pdsReadByte(stream) == '}');
  // consume ','
  while (
    !pdsIsDone(stream)
    && ::isWhitespace(puleStreamPeekByte(stream))
  ) {
    pdsReadByte(stream);
  }
  if (pdsIsDone(stream)) {
    PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing object");
    return {0};
  }
  if (pdsReadByte(stream) != ',') {
    PULE_error(PuleErrorAssetPds_decode, "expected ',' after object");
    return {0};
  }

  return object;
}

PuleDsValue pdsParseValue(
  PuleAllocator const allocator,
  PuleStreamRead const stream,
  PuleError * const error
) {
  while (
    !pdsIsDone(stream)
    && ::isWhitespace(puleStreamPeekByte(stream))
  ) {
    pdsReadByte(stream);
  }
  if (pdsIsDone(stream)) {
    PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing value");
    return {0};
  }
  char character = pdsReadByte(stream);

  if (character == '[') {
    return pdsParseArray(allocator, stream, error);
  } (void)']'; // this is to prevent editor confusing ] matching

  if (character == '{') {
    return pdsParseObject(allocator, stream, error, true);
  } (void)'}'; // this is to prevent editor confusing } matching

  // first get this thing into a value to check what type it is
  std::string value = "";
  value += character;
  {
    char prevChar = '\\';
    bool inString = character == '"';
    while (true) {
      if (pdsIsDone(stream)) {
        PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing value");
        return {0};
      }
      prevChar = character;
      character = pdsReadByte(stream);

      if (!inString && character == ',') {
        break;
      }

      value += character;

      if (inString && prevChar != '\\' && character == '"') {
        // get & consume to the ','
        while (
          !pdsIsDone(stream)
          && ::isWhitespace(puleStreamPeekByte(stream))
        ) {
          pdsReadByte(stream);
        }
        if (pdsIsDone(stream)) {
          PULE_error(PuleErrorAssetPds_decode, "hit EOF while parsing value");
          return {0};
        }
        if (pdsReadByte(stream) != ',') {
          PULE_error(PuleErrorAssetPds_decode, "expected ',' after string");
          return {0};
        }
        break;
      }

      if (character == '[' || character == '{') {
        break;
      }

      if (!inString && (character == '}' || character == ']')) {
        PULE_error(
          PuleErrorAssetPds_decode,
          "seems like you're missing a comma"
        );
      }
    }
  }

  value = trimBeginEndWhitespace(value);

  if (value == "") {
    PULE_error(PuleErrorAssetPds_decode, "assigning empty value to label");
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
    return puleDsCreateI64(parseHex(value.substr(2, value.size()), error));
  }

  // parse binary, example, 0b00, 0b001000
  if (value.size() > 2 && value[0] == '0' && value[1] == 'b') {
    return puleDsCreateI64(parseBin(value.substr(2, value.size()), error));
  }

  // parse buffer, example, *"ABC"
  if (value.size() > 1 && value[0] == '*') {
    PULE_assert(value.size() > 3);
    return (
      parseBuffer(
        allocator,
        PuleStringView{.contents = value.c_str()+2, .len = value.size()-3,},
        error
      )
    );
  }

  // parse float, example, 0.0 3238.238230
  if (value.find_first_of('.') != std::string::npos) {
    // assert all characters are digits
    bool hasDot = false;
    // it's not safe to end with '.'
    if (value.ends_with('.')) {
      PULE_error(
        PuleErrorAssetPds_decode,
        "could not parse float value from: '%s', ends with '.'", value.c_str()
      );
      return {0};
    }
    // unless it's proceeded by an 'f'
    if (value.ends_with('f')) {
      value = value.substr(0, value.size()-1);
    }
    // check all characters are digits, and no repeating '.'
    for (auto ch : value) {
      if (hasDot && ch == '.') {
        PULE_error(
          PuleErrorAssetPds_decode,
          "could not parse float value from: '%s', multiple '.'", value.c_str()
        );
        return {0};
      }
      if (!(ch >= '0' && ch <= '9') && ch != '.') {
        PULE_error(
          PuleErrorAssetPds_decode,
          "could not parse float value from: '%s', multiple '.'", value.c_str()
        );
        return {0};
      }
      hasDot = ch == '.';
    }
    return puleDsCreateF64(std::stod(value));
  }

  // make sure all characters are digits
  for (auto ch : value) {
    if (ch < '0' || ch > '9') {
      PULE_error(
        PuleErrorAssetPds_decode,
        "could not parse int value from: '%s'", value.c_str()
      );
      return {0};
    }
  }

  return puleDsCreateI64(parseDec(value, error));
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

  while (!pdsIsDone(stream)) {
    // skip whitespace
    if (::isWhitespace(puleStreamPeekByte(stream))) {
      pdsReadByte(stream);
      continue;
    }
    if (pdsIsDone(stream)) { break; }
    std::string const memberLabel = pdsParseLabel(stream, error);
    if (error->id) {
      puleDsDestroy(defaultObject);
      return {0};
    }
    puleDsObjectMemberAssign(
      defaultObject,
      PuleStringView {
        .contents = memberLabel.c_str(),
        .len = memberLabel.size()
      },
      pdsParseValue(allocator, stream, error)
    );
    if (error->id) {
      puleDsDestroy(defaultObject);
      return {0};
    }
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
  if (error->id > 0) {
    PULE_error(PuleErrorAssetPds_decode, "failed to open file");
    return { 0 };
  }

  // create a file stream, using a storage buffer for the stream to store
  //   intermediary data in
  uint8_t streamStorage[512];
  PuleBufferViewMutable streamStorageView = {
    .data = &streamStorage[0],
    .byteLength = 512,
  };

  // plug the stream into the data-serializer to deserialize its contents
  PuleStreamRead const stream = puleFileStreamRead(file, streamStorageView);
  PuleDsValue const head = puleAssetPdsLoadFromStream(allocator, stream, error);

  // even if error occured we don't have to do anything special
  puleFileClose(file);
  puleStreamReadDestroy(stream);

  if (error->id) {
    PULE_error(PuleErrorAssetPds_decode, "failed to load file '%s'", filename);
  }

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
  PuleDsValue emitParametersVarargsValue = (
    puleDsObjectMemberAssign(
      emitParametersValue,
      puleCStr("varargs"),
      puleDsCreateArray(info.allocator)
    )
  );

  PuleDsValue layoutIt = info.layout;
  char const * helpPreviousLayoutArgument = info.arguments[0];

  // parse varargs
  bool parsingVarargs = false;
  for (int32_t argumentIt = 1; argumentIt < info.argumentLength; ++ argumentIt) {
    char const * argument = info.arguments[argumentIt];

    if (parsingVarargs) {
      puleLog("parsing vararg: %s", argument);
      puleDsArrayAppend(
        emitParametersVarargsValue, puleDsCreateString(puleCStr(argument))
      );
      continue;
    }

    if (strcmp(argument, "--") == 0 && argument[2] == '\0') {
      puleLog("parsing --- %s", argument);
      parsingVarargs = true;
      continue;
    }
  }

  // parse objects
  int32_t argumentIt = 1;
  parsingVarargs = false;
  char const * argument; // track last argument outside of function scope
  for (; argumentIt < info.argumentLength; ++ argumentIt) {
    argument = info.arguments[argumentIt];


    if (strcmp(argument, "--") == 0 && argument[2] == '\0') {
      break;
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
  parsingVarargs = false;
  for (; argumentIt < info.argumentLength; ++ argumentIt) {
    argument = info.arguments[argumentIt];
    size_t const argumentLen = strlen(argument);

    if (strcmp(argument, "--") == 0 && argument[2] == '\0') {
      parsingVarargs = true;
      break;
    }

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
          puleDsCreateI64(
            parseDec(std::string(info.arguments[argumentIt+1]), error)
          )
        );
        if (error->id) {
          puleDsDestroy(emitValue);
          return {0};
        }
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
