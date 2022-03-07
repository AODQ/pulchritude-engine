#!/usr/bin/python3

# Parses json generated from json-binding-generator.py and outputs the
#   corresponding zig bindings

import argparse
import json
import os

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
  }
  if (token in keywordTranslator):
    return keywordTranslator[token]
  return token

def extractType(typeArray, isReturnOrStructUnionFieldType, symbolLabel):
  string = ""

  typeTranslator = {
    "char": "u8",
    "*": "[*c]",
    "size_t": "usize",
    "uint8_t":  "u8", "uint16_t": "u16", "uint32_t": "u32", "uint64_t": "u64",
    "int8_t":  "i8", "int16_t": "i16", "int32_t": "i32", "int64_t": "i64",
    "float": "f32",
  }

  # zig demands all function parameters are constant
  if (not isReturnOrStructUnionFieldType and typeArray[-1] != "const"):
    print(
      f"WARNING: the last const of type {typeArray} from {symbolLabel} "
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
      string += "* anyopaque"
      idiotSkip = 1; # skip void
    elif (
      len(typeArray) - paramIt > 2
      and param == "*" and typeArray[paramIt+1] == "const"
      and typeArray[paramIt+2] == "void"
    ):
      string += "* const anyopaque"
      idiotSkip = 2; # skip const void
    elif (param in typeTranslator):
      string += typeTranslator[param]
    else:
      string += param
    string += " "
  return string[0:-1]

def extractField(symbol, isStructOrUnion):
  string = ""
  if (symbol["meta-type"] == "fn-ptr"):
    pass
  elif (symbol["meta-type"] == "standard"):
    string += "  " + keywordRename(param["label"]) + ": "
    string += extractType(param["type"], isStructOrUnion, label)
    string += ",\n"
  elif (symbol["meta-type"] == "variadic"):
    string += "  ...\n"
  else:
    print(f"WARNING: unknown metatype: '{symbol['meta-type']} in {symbol}'")
  return string

outFile = open(inputArgs["output"], 'w')
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

      # zig has max and similar so these aren't necessary
      if (valueLabel == "MaxEnum" or valueLabel == "End"):
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
