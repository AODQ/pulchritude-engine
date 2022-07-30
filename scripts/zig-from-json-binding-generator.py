#!/usr/bin/python3

# Parses json generated from json-binding-generator.py and outputs the
#   corresponding zig bindings

import argparse
import json
import os
import re

parser = argparse.ArgumentParser("Parses json to zig")
parser.add_argument("--output", "-o", default="bindings/zig/pulchritude.zig")
parser.add_argument("--input", "-i", default="bindings/json/bindings.json")
inputArgs = vars(parser.parse_args())

# read file to json
inputJsonFile = open(inputArgs["input"])
inputJson = json.load(inputJsonFile)
inputJsonFile.close()

# this should only rename non-exported keywords (such as function parameters,
#   struct member names, etc)
def keywordRename(token):
  # rename tokens if they are keywords in zig
  keywordTranslator = {
    "error": "err",
    "fn": "func",
  }
  if (token in keywordTranslator):
    return keywordTranslator[token]
  return token

def extractType(typeArray, isReturnOrStructUnionFieldType, symbolLabelForDebug):
  string = ""

  typeTranslator = {
    "char": "u8",
    "*": "[*c]",
    "size_t": "usize",
    "uint8_t":  "u8", "uint16_t": "u16", "uint32_t": "u32", "uint64_t": "u64",
    "int8_t":  "i8", "int16_t": "i16", "int32_t": "i32", "int64_t": "i64",
    "float": "f32",
    "double": "f64",
  }

  # zig demands all function parameters are constant
  if (not isReturnOrStructUnionFieldType and typeArray[-1] != "const"):
    print(
      f"WARNING: the last const of type {typeArray} from {symbolLabelForDebug} "
      "is not constant :/"
    )
  elif (not isReturnOrStructUnionFieldType):
    typeArray = typeArray[0:-1]

  # reverse order for zig (int const *) -> ([*c] const int)
  typeArray = list(reversed(typeArray))
  idiotSkip = 0
  for paramIt, param in enumerate(typeArray):
    if (idiotSkip > 0):
      idiotSkip -= 1
      continue
    if (
      len(typeArray) - paramIt > 1
      and param == "*" and typeArray[paramIt+1] == "void"
    ):
      string += "?* anyopaque"
      idiotSkip = 1; # skip void
    elif (
      len(typeArray) - paramIt > 2
      and param == "*" and typeArray[paramIt+1] == "const"
      and typeArray[paramIt+2] == "void"
    ):
      string += "?* const anyopaque"
      idiotSkip = 2; # skip const void
    elif (param in typeTranslator):
      string += typeTranslator[param]
    else:
      # cheeky way to replace SomeEnum_End -> SomeEnum.end
      # needs to be fixed by actually checking the enum exists
      string += (
        re.sub(
          "([a-z0-9A-Z]+)_(End|Size)([^a-z0-9A-Z_])",
          "@enumToInt(\\1.\\2)\\3",
          param
        )
      );

    string += " "
  return string[0:-1]

def extractField(symbol, isStructOrUnion):
  string = ""
  if (symbol["meta-type"] == "fn-ptr"):
    string += "  " + keywordRename(symbol["label"]) + ": "
    string += "?* const fn("
    for parameter in symbol["parameters"]:
      string += parameter[-1] + ": "
      string += extractType(parameter[:-1], False, parameter[-1])
      string += ", "
    string += ") "
    string += extractType(symbol["return-type"], isStructOrUnion, "")
    string += ",\n"
  elif (symbol["meta-type"] == "standard"):
    string += "  " + keywordRename(symbol["label"]) + ": "
    string += extractType(symbol["type"], isStructOrUnion, label)
    string += ",\n"
  elif (symbol["meta-type"] == "variadic"):
    string += "  ...\n"
  else:
    print(f"WARNING: unknown metatype: '{symbol['meta-type']} in {symbol}'")
  return string

os.makedirs(os.path.dirname(inputArgs["output"]), exist_ok=True)
outFile = open(inputArgs["output"], "w+")
for symbol in inputJson:
  # extern fn label(params...) callconv(.C) returntype
  if (symbol["type"] == "function"):
    label = symbol["label"]
    returnType = (extractType(symbol["return-type"], True, label))
    parameters = ""
    for param in symbol["parameters"]:
      parameters += extractField(param, isStructOrUnion=False)

    # fn()\n vs fn(\nparam\n)\n
    if (parameters != ""):
      parameters = "\n" + parameters

    outFile.write(
      f"pub extern fn {label}({parameters}) callconv(.C) {returnType};\n"
    )

  # pub const label = enum(Type) { values... }
  if (symbol["type"] == "enum"):
    label = symbol["label"]
    enumType = "u32"
    values = ""
    for value in symbol["values"]:
      # chop off '_'
      valueLabel = value['label']
      valueLabel = valueLabel[valueLabel.find('_')+1:]

      if (valueLabel == "MaxEnum"):
        if (value["value"] == 0x7FFFFFFF):
          enumType = "u32"
        else:
          print(f"unknown enum value: {value}")

      # zig doesn't need this C hack
      if (valueLabel == "MaxEnum"):
        continue

      values += f"  {valueLabel} = {value['value']},\n"
    outFile.write(
      f"pub const {label} = enum({enumType}) {{\n{values}}};\n"
    )

  # pub const label = struct { fields... }
  if (symbol["type"] == "struct" or symbol["type"] == "union"):
    label = symbol["label"]
    fields = ""
    for param in symbol["fields"]:
      fields += extractField(param, isStructOrUnion=True)
    outFile.write(
      f"pub const {label} = extern {symbol['type']} {{\n{fields}}};\n"
    )

outFile.close()
