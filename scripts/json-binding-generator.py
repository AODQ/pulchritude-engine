#!/usr/bin/python3

# Parses Pulchritude's C headers, extracts exported symbols (structs, enums,
#   consts and functions), then places them into a JSON array and writes them out

# this is extremely ugly, I would like if there were some sort of Python module
#   that could import C headers (even with heavy restrictions, which I have to
#   to already anyway), but unfortunately parsing like an idiot is the only
#   solution I can find or think of

import argparse
import os
import json
import re

parser = argparse.ArgumentParser("Parses C headers to JSON")
parser.add_argument("--output", "-o", default="bindings/json/bindings.json")
parser.add_argument("--input", "-i", default="library/include/")
inputArgs = vars(parser.parse_args())

print(" --- ", inputArgs['output'])
def pprint(label):
  if ("rts-game" in inputArgs['output']):
    print(label)

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

def macro_remover(text):
  import re
  # remove all instances of PULE_param and PULE_defaultField
  text = re.sub(r' PULE_defaultField\(.*?\);', ';', text);
  return re.sub(r' PULE_param\(.*?\);', ';', text);

def exportJsonFromFile(contents, modulename, header):
  contents = macro_remover(comment_remover(contents))
  # dump the contents to a file for debugging, if directory debug-binding exists
  #print("Exporting json for " + header + " from " + modulename + "...")
  if (not os.path.isdir("debug-binding")):
    os.mkdir("debug-binding")
  file = open("debug-binding/" + modulename + "-" + header, "w+")
  file.write(contents)
  file.close()

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

      # don't export va_list and maybe some others
      exportableFunction = True
      for param in parameters:
        if (len(param["type"]) > 0 and param["type"][0] == "va_list"):
          exportableFunction = False

      if (exportableFunction):
        exportedSymbols += [{
          "type": "function",
          "return-type": returnType,
          "label": fnName,
          "parameters": parameters,
          "module": modulename,
          "header": header,
        }]
    elif (strCmp(contents[it:], "using")): # TODO support C's "using" I guess
      # using +([^ =]+) *= *([^ ;]+) *;
      # 'using'
      while (not contents[it].isspace()):
        it += 1
      # \s*
      while (contents[it].isspace()):
        it += 1;
      # [^ =]+
      ulabel = ""
      while (not contents[it].isspace() and contents[it] != "="):
        ulabel += contents[it]
        it += 1;
      assert(ulabel != "")
      # \s*
      while (contents[it].isspace()):
        it += 1;
      # =
      assert(contents[it] == "=")
      it += 1
      # \s*
      while (contents[it].isspace()):
        it += 1;
      # [^ ;]+
      utype = ""
      while (not contents[it].isspace() and contents[it] != ";"):
        utype += contents[it]
        it += 1
      # \s*
      while (contents[it].isspace()):
        it += 1
      # ;
      assert(contents[it] == ";")
      it += 1

      exportedSymbols += [{
        "type": "using",
        "label": ulabel,
        "typelabel": utype,
      }]
    elif (strCmp(contents[it:], "typedef enum")):
      enums = []
      while (contents[it] != "{"):
        it += 1
      it += 1

      enumVal = 0
      # --- enum list end
      while (contents[it] != "}"):
        # \s*
        while (contents[it].isspace()):
          it += 1
        e = ""
        # [^\s,}=]
        while (
          not contents[it].isspace()
          and contents[it] != "," and contents[it] != "}"
          and contents[it] != "="
        ):
          e += contents[it]
          it += 1
        # \s
        while (contents[it].isspace()):
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

      #if ("MaxEnum" not in enums):
      #  print(f"WARNING: enum {label} has no MaxEnum")
      #if ("End" not in enums):
      #  print(f"WARNING: enum {label} has no End")

      exportedSymbols += [{
        "type": "enum",
        "label": label,
        "values": enums,
        "module": modulename,
      }]
    elif (
      strCmp(contents[it:], "typedef struct")
      or strCmp(contents[it:], "typedef union")
    ):
      isStructNotUnion = strCmp(contents[it:], "typedef struct");
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
        "type": "struct" if isStructNotUnion else "union",
        "label": label,
        "fields": fields,
        "module": modulename,
      }]
    else:
      it += 1

  return exportedSymbols

exportJson = []
modulePattern = re.compile(r'.+pulchritude-([\w\d\-]+)')
for subdir, dirs, files in os.walk(inputArgs['input']):
  for filename in files:
    if (not filename.endswith(".h")):
      continue

    if (filename == "core.h"):
      continue

    if (filename == "engine.h"):
      continue

    modulename = modulePattern.match(subdir).group(1)
    if (modulename == 'asset'):
      modulename += '-' + filename.replace('.h', '')

    file = open(subdir + "/" + filename)
    pprint(f"file open: {file}")
    exportJson += exportJsonFromFile(file.read(), modulename, filename)
    file.close()

# transform function pointers in structs & functions
for objkey, obj in enumerate(exportJson):
  if (
    obj["type"] != "struct"
    and obj["type"] != "union"
    and obj["type"] != "function"
  ):
    continue
  isStructOrUnion = obj["type"] == "struct" or obj["type"] == "union"

  fieldsOrPameter = "fields" if isStructOrUnion else "parameters"

  for fieldkey, field in enumerate(obj[fieldsOrPameter]):
    if (isStructOrUnion and field["label"][-1][-1] != ")"):
      continue
    if (not isStructOrUnion and field["label"] != "fn-ptr"):
      continue

    ptr = ' '.join(field["type"])
    if (isStructOrUnion):
      ptr += ' ' + field["label"]

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
    if (not isStructOrUnion or ptr[it:it+5] == "const"):
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
    if (label == "const"):
      print(f"failed to remove const from '{label}' [const: '{shouldBeConst}']")
      assert false
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

    params = list({'meta-type': 'standard', 'type': param} for param in params)
    for param in params:
      param['label'] = param['type'][-1]
      param['type'] = param['type'][:-1]
    exportJson[objkey][fieldsOrPameter][fieldkey] = {
      "label": label,
      "meta-type": "fn-ptr",
      "return-type": retType,
      "parameters": params,
    }

# move field/parameter array from label to type
for objkey, obj in enumerate(exportJson):
  if (
    obj["type"] != "struct"
    and obj["type"] != "union"
    and obj["type"] != "function"
  ):
    continue
  isStructOrUnion = obj["type"] == "struct" or obj["type"] == "union"

  fieldsOrPameter = "fields" if isStructOrUnion else "parameters"

  for fieldkey, field in enumerate(obj[fieldsOrPameter]):
    if (field["label"][-1] != "]"):
      continue
    it = 0;
    for zit, character in enumerate(field["label"]):
      if (character == "["):
        it = zit
        break
    assert(it > 0)
    exportJson[objkey][fieldsOrPameter][fieldkey]["type"] = (
      field["type"] + [field["label"][it:]]
    )
    exportJson[objkey][fieldsOrPameter][fieldkey]["label"] = (
      field["label"][:it]
    )

# interesting, now before I stamp these papers, make sure this information is
#   correct
for symbol in exportJson:
  #print(f"symbol: {symbol}")
  def symAssertStr(member, label):
    pass
    #if (label not in member or member[label] == ""):
    #  print(f"------ str invalid '{label}' in '{member}' [{symbol['label']}]: ");

  def symAssertArray(member, label, minLen):
    if (label not in member or len(member[label]) < minLen):
      print(f"------ arr invalid '{label}' in '{member}' [{symbol['label']}]: ");

  def symAssertEnum(member, label):
    if (label not in member or member[label] < 0):
      print(f"------ enum invalid '{label}' in '{member}' [{symbol['label']}]: ");

  if (symbol["type"] == "function"):
    symAssertStr(symbol, "return-type")
    symAssertStr(symbol, "label")
    symAssertArray(symbol, "parameters", 0)
    for parameter in symbol["parameters"]:
      # skip function pointer stuff for now
      if (parameter["meta-type"] == "fn-ptr"):
        continue

      symAssertStr(parameter, "label")
      symAssertStr(parameter, "meta-type")
      symAssertArray(parameter, "type", 0 if parameter["label"] == "..." else 1)
      for ptype in parameter["type"]:
        if (ptype == ""):
          print(f"---- invalid empty type in '{parameter}' [{symbol['label']}]")
  if (symbol["type"] == "enum"):
    symAssertStr(symbol, "label")
    symAssertArray(symbol, "values", 0)
    for value in symbol["values"]:
      symAssertEnum(value, "value")
      symAssertStr(value, "label")
  if (symbol["type"] == "struct"):
    symAssertStr(symbol, "label")
    symAssertArray(symbol, "fields", 0)
    for field in symbol["fields"]:
      symAssertStr(field, "label")
      symAssertStr(field, "meta-type")
      # skip function pointer stuff for now
      if (field["meta-type"] == "fn-ptr"):
        continue
      symAssertArray(field, "type", 1)
      for ptype in field["type"]:
        if (ptype == ""):
          print(f"---- invalid empty type in '{field}' [{symbol['label']}]")

os.makedirs(os.path.dirname(inputArgs["output"]), exist_ok=True)
fileWrite = open(inputArgs["output"], "w+")
fileWrite.write(json.dumps(exportJson, indent=2))
fileWrite.close()
