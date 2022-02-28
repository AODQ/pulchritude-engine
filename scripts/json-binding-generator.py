#!/usr/bin/python3

# Parses Pulchritude's C headers, extracts exported symbols (structs, enums,
#   consts and functions), then places them into a JSON array and writes them out

import argparse
import os
import json

parser = argparse.ArgumentParser("Parses json to zig")
parser.add_argument("--output", "-o", default="bindings/json/bindings.json")
parser.add_argument("--input", "-i", default="library/include/")
inputArgs = vars(parser.parse_args())

def strCmp(contents, y):
  if (len(contents) < len(y)):
    return False
  return contents[:len(y)] == y

# https://stackoverflow.com/a/241506
def comment_remover(text):
  import re
  def replacer(str):
    s = str.group(0)
    return " " if s.startswith('/') else s
  pattern = re.compile(
    r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
    re.DOTALL | re.MULTILINE
  )
  return re.sub(pattern, replacer, text)

def exportJsonFromFile(contents):
  contents = comment_remover(contents)
  exportedSymbols = []

  contentLen = len(contents)
  it = 0
  while it < contentLen:
    if (strCmp(contents[it:], "PULE_exportFn")):
      returnType = []
      fnName = ""
      parameters = []
      it += len("PULE_exportFn")

      # get return type and fn name (last ret type)
      while (contents[it] != "("):
        while (contents[it].isspace()):
          it += 1
        p = ""
        if (contents[it] == "("):
          break
        while (not contents[it].isspace() and contents[it] != "("):
          p += contents[it]
          it += 1
        if (p != ""):
          returnType += [p]

      assert(contents[it] == "(")
      it += 1

      fnPtrOpenBracketSum=0
      while (contents[it] != ")"):
        while (contents[it].isspace()):
          it += 1

        if (contents[it] == ")"):
          break

        params = []
        while (contents[it] != ")" and contents[it] != ","):
          while (contents[it].isspace()):
            it += 1

          p = ""
          while (
            not contents[it].isspace()
            and (contents[it] != "," or fnPtrOpenBracketSum>0)
            and (contents[it] != ")" or fnPtrOpenBracketSum>0)
          ):
            if (contents[it] == ")" and fnPtrOpenBracketSum>0):
              fnPtrOpenBracketSum -= 1
            if (contents[it] == "("):
              fnPtrOpenBracketSum += 1
            p += contents[it]
            it += 1
          if (p != ""):
            params += [p]
        if (params != []):
          metatype = "standard"
          if (params[0] == "..."):
            params = ["..."]
            metatype = "variadic"
          if (params[-1][-1] == ")"):
            params += ["fn-ptr"]
          parameters += [
            {"label": params[-1], "type": params[0:-1], "meta-type": metatype}
          ]

        if (contents[it] == ","):
          it += 1

      # transform returntype -> fnname
      fnName = returnType[-1]
      returnType = returnType[:-1]

      exportedSymbols += [{
        "type": "function",
        "return-type": returnType,
        "label": fnName,
        "parameters": parameters,
      }]
    elif (strCmp(contents[it:], "typedef enum")):
      enums = []
      while (contents[it] != "{"):
        it += 1
      it += 1

      enumVal = 0
      # --- enum list end
      while (contents[it] != "}"):
        while (contents[it].isspace()):
          it += 1
        e = ""
        while (
          not contents[it].isspace()
          and contents[it] != "," and contents[it] != "}"
          and contents[it] != "="
        ):
          e += contents[it]
          it += 1
        while (
          contents[it].isspace()
          and contents[it] != "," and contents[it] != "}"
          and contents[it] != "="
        ):
          it += 1
        if (contents[it] == "="): # get the puleenum_val = >>value<<
          it += 1
          while (contents[it].isspace()):
            it += 1
          v = ""
          while (
            not contents[it].isspace()
            and contents[it] != "," and contents[it] != "}"
            and contents[it] != "="
          ):
            v += contents[it]
            it += 1
          enumVal = int(v, 0); # this 0 at end means assume base

        if (e != ""):
          enums += [{"label": e, "value": enumVal}]
          enumVal += 1

        while (contents[it].isspace()):
          it += 1
        if (contents[it] == ","):
          it += 1

      assert(contents[it] == "}")
      it += 1
      while (contents[it].isspace()):
        it += 1
      label = ""
      while (contents[it] != ";" and not contents[it].isspace()):
        label += contents[it]
        it += 1

      if ("MaxEnum" not in enums):
        print(f"WARNING: enum {label} has no MaxEnum")
      if ("End" not in enums):
        print(f"WARNING: enum {label} has no End")

      exportedSymbols += [{
        "type": "enum",
        "label": label,
        "values": enums,
      }]
    elif (strCmp(contents[it:], "typedef struct")):
      fields = []
      while (contents[it] != "{"):
        it += 1
      it += 1

      while (contents[it] != "}"):
        while (contents[it].isspace()):
          it += 1
        field = []
        while (contents[it] != ";"):
          f = ""
          while (not contents[it].isspace() and contents[it] != ";"):
            f += contents[it]
            it += 1
          if (f != ""):
            field += [f]
          while (contents[it].isspace()):
            it += 1

        if (f != ""):
          fields += [{
            "label": field[-1],
            "type": field[:-1],
            "meta-type": "standard"
          }]

        while (contents[it].isspace()):
          it += 1
        if (contents[it] == ";"):
          it += 1
        while (contents[it].isspace()):
          it += 1

      assert(contents[it] == "}")
      it += 1
      while (contents[it].isspace()):
        it += 1
      label = ""
      while (contents[it] != ";" and not contents[it].isspace()):
        label += contents[it]
        it += 1

      exportedSymbols += [{
        "type": "struct",
        "label": label,
        "fields": fields,
      }]
    else:
      it += 1

  return exportedSymbols

exportJson = []
for subdir, dirs, files in os.walk(inputArgs["input"]):
  for filename in files:
    if (not filename.endswith(".h")):
      continue

    if (filename == "core.h"):
      continue

    print(f"checking {filename}")
    file = open(subdir + "/" + filename)
    exportJson += exportJsonFromFile(file.read())
    file.close()

# transform function pointers in structs & functions
for objkey, obj in enumerate(exportJson):
  if (obj["type"] != "struct" and obj["type"] != "function"):
    continue
  isStruct = obj["type"] == "struct"

  fieldsOrPameter = "fields" if isStruct else "parameters"

  for fieldkey, field in enumerate(obj[fieldsOrPameter]):
    if (isStruct and field["label"][-1][-1] != ")"):
      continue
    if (not isStruct and field["label"] != "fn-ptr"):
      continue

    ptr = ' '.join(field["type"])
    if (isStruct):
      ptr += field["label"]

    retType = []
    params = []
    label = ""
    it = 0

    # return type
    while (ptr[it] != "("):
      while (ptr[it].isspace()):
        it += 1
      p = ""
      while (not ptr[it].isspace()):
        if (ptr[it] == "("):
          break
        p += ptr[it]
        it += 1
      if (p != ""):
        retType += [p]
    assert(ptr[it] == "(")
    it += 1

    while (ptr[it] != "*"):
      it += 1
    it += 1

    # skip const
    shouldBeConst = ""
    while (ptr[it].isspace()):
      it += 1
    if (not isStruct or ptr[it:it+4] == "const"):
      while (not ptr[it].isspace()):
        shouldBeConst += ptr[it]
        it += 1
      assert shouldBeConst == "const", f"{obj['label']}"

    # label
    while (ptr[it].isspace()):
      it += 1
    while (not ptr[it].isspace() and ptr[it] != ")"):
      label += ptr[it]
      it += 1
    while (ptr[it] != ")"):
      it += 1
    it += 1
    while (ptr[it] != "("):
      it += 1
    it += 1
    param = []
    while (ptr[it] != ")"):
      while (ptr[it].isspace()):
        it += 1

      p = ""
      while (not ptr[it].isspace() and ptr[it] != "," and ptr[it] != ")"):
        p += ptr[it]
        it += 1
      if (p != ""):
        param += [p]
      if (ptr[it] == "," or ptr[it] == ")"):
        params += [param]
        param = []
        if (ptr[it] == ","):
          it += 1
    exportJson[objkey][fieldsOrPameter][fieldkey] = {
      "label": label,
      "meta-type": "fn-ptr",
      "return-type": retType,
      "parameters": params,
    }

fileWrite = open(inputArgs["output"], "w")
fileWrite.write(json.dumps(exportJson, indent=2))
fileWrite.close()
