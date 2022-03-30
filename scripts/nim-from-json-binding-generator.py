#!/usr/bin/python3

# Parses json generated from json-binding-generator.py and outputs the
#   corresponding num bindings

import argparse
import json
import os
import re

parser = argparse.ArgumentParser("Parses json to nim")
parser.add_argument("--output", "-o", default="bindings/nim/pulchritude.nim")
parser.add_argument("--input", "-i", default="bindings/json/bindings.json")
inputArgs = vars(parser.parse_args())

# read file to json
inputJsonFile = open(inputArgs["input"])
inputJson = json.load(inputJsonFile)
inputJsonFile.close()

# this should only rename non-exported keywords (such as function parameters,
#   struct member names, etc)
def keywordRename(token):
  # rename tokens if they are keywords in nim
  keywordTranslator = {
    "none": "none0", # in Nim, none_ = none; WTF WERE THEY THINKING!?
    "end": "end0",
    "object": "object0",
    "iterator": "iterator0",
  }
  if (token in keywordTranslator):
    return keywordTranslator[token]
  return token

def extractType(typeArray, isReturnOrStructUnionFieldType, symbolLabel):
  string = ""

  typeTranslator = {
    "*": "ptr",
    "size_t": "csize_t",
    "uint8_t":  "uint8", "uint16_t": "uint16", "uint32_t": "uint32",
    "uint64_t": "uint64",
    "int": "cint",
    "int8_t":  "int8", "int16_t": "int16", "int32_t": "int32", "int64_t": "int64",
    "float": "float32",
    "double": "float64",
    "const": "",
  }

  # nim demands all function parameters are constant
  if (not isReturnOrStructUnionFieldType and typeArray[-1] != "const"):
    print(
      f"WARNING: the last const of type {typeArray} from {symbolLabel} "
      "is not constant :/"
    )
  elif (not isReturnOrStructUnionFieldType):
    typeArray = typeArray[0:-1]

  # reverse order for nim (int const *) -> ([*c] const int)
  typeArray = list(reversed(typeArray))
  idiotSkip = 0
  arrayEnds = 0
  for paramIt, param in enumerate(typeArray):
    if (idiotSkip > 0):
      idiotSkip -= 1
      continue
    if (arrayEnds > 0):
      arrayEnds -= 1
      if (arrayEnds == 0):
        string += "] "
    if (
      len(typeArray) - paramIt > 1
      and param == "*" and typeArray[paramIt+1] == "void"
    ):
      string += "pointer"
      idiotSkip = 1; # skip void
    elif (
      len(typeArray) - paramIt > 2
      and param == "*" and typeArray[paramIt+1] == "const"
      and typeArray[paramIt+2] == "void"
    ):
      string += "pointer"
      idiotSkip = 2; # skip const void
    elif (
      len(typeArray) - paramIt > 1
      and param == "*" and typeArray[paramIt+1] == "char"
    ):
      string += "cstring";
      idiotSkip = 1; # skip char
    elif (
      len(typeArray) - paramIt > 2
      and param == "*" and typeArray[paramIt+1] == "const"
      and typeArray[paramIt+2] == "char"
    ):
      string += "cstring";
      idiotSkip = 2; # skip const char
    elif (
      len(typeArray) - paramIt > 1
      and param[0] == "[" and param[-1] == "]"
    ):
      string += "array["
      string += (
        re.sub(
          "([a-z0-9A-Z]+)_End([^a-z0-9A-Z_])",
          "\\1.End\\2",
          param
        )[1:-1]
      );
      string += ", "
      arrayEnds = 2
    elif (param in typeTranslator):
      string += typeTranslator[param]
    else:
      # cheeky way to replace SomeEnum_End -> SomeEnum.end
      # needs to be fixed by actually checking the enum exists
      string += (
        re.sub(
          "([a-z0-9A-Z]+)_End([^a-z0-9A-Z_])",
          "@enumToInt(\\1.End)\\2",
          param
        )
      );

    string += " "

  if (arrayEnds > 0):
    arrayEnds -= 1
    if (arrayEnds == 0):
      string += "] "
  return string[0:-1]

def extractField(symbol, isStructOrUnion):
  string = ""
  if (symbol["meta-type"] == "fn-ptr"):
    pass
  elif (symbol["meta-type"] == "standard"):
    string += "    " + keywordRename(param["label"]) + ": "
    string += extractType(param["type"], isStructOrUnion, label)
    if (not isStructOrUnion):
      string += ",\n"
    else:
      string += "\n"
  elif (symbol["meta-type"] == "variadic"):
    pass
  else:
    print(f"WARNING: unknown metatype: '{symbol['meta-type']} in {symbol}'")
  return string

def extractFieldHeader(symbol, isStructOrUnion):
  if (symbol["meta-type"] == "variadic"):
    return ", varargs"
  return ""

os.makedirs(os.path.dirname(inputArgs["output"]), exist_ok=True)
outFile = open(inputArgs["output"], "w+")
outFile.write("{.experimental: \"codeReordering\".}\n")
for symbol in inputJson:
  # extern fn label(params...) callconv(.C) returntype
  if (symbol["type"] == "function"):
    label = symbol["label"]
    returnType = (extractType(symbol["return-type"], True, label))
    parameters = ""
    fieldHeader = "{.importc, header: \"<" + symbol["file"] + ">\""
    for param in symbol["parameters"]:
      parameters += extractField(param, isStructOrUnion=False)
      fieldHeader += extractFieldHeader(param, isStructOrUnion=False)
    fieldHeader += ".}"

    # fn()\n vs fn(\nparam\n)\n
    if (parameters != ""):
      parameters = "\n" + parameters

    outFile.write(
      f"proc {label}({parameters}) : {returnType} {fieldHeader}\n"
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
      valueLabel = keywordRename(valueLabel)

      if (valueLabel == "MaxEnum"):
        if (value["value"] == 0x7FFFFFFF):
          enumType = "u32"
        else:
          print(f"unknown enum value: {value}")

      # nim doesn't need this C hack
      if (valueLabel == "MaxEnum"):
        continue

      values += f"    {valueLabel} = {value['value']},\n"
    outFile.write(
      f"type\n  {label} {{.pure.}} = enum\n{values}\n"
    )

  # pub const label = struct { fields... }
  if (symbol["type"] == "struct" or symbol["type"] == "union"):
    label = symbol["label"]
    fields = ""
    for param in symbol["fields"]:
      fields += extractField(param, isStructOrUnion=True)
    header = symbol["file"]
    outFile.write(
      f"type\n  {label} {{.importc: \"struct {label}\", header: "
      + f"\"<{header}>\".}} = object\n{fields}\n"
    )

outFile.close()
