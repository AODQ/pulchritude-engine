#!/usr/bin/python3

import os
import json

def strCmp(contents, y):
  if (len(contents) < len(y)):
    return False;
  return contents[:len(y)] == y;

def exportJsonFromFile(contents):
  exportedSymbols = [];

  contentLen = len(contents);
  it = 0;
  while it < contentLen:
    if (strCmp(contents[it:], "PULE_exportFn")):
      returnType = [];
      fnName = ""
      parameters = []
      it += len("PULE_exportFn");

      # get return type and fn name (last ret type)
      while (contents[it] != "("):
        while (contents[it].isspace()):
          it += 1;
        p = ""
        if (contents[it] == "("):
          break;
        while (not contents[it].isspace() and contents[it] != "("):
          p += contents[it];
          it += 1;
        if (p != ""):
          returnType += [p];

      assert(contents[it] == "(");
      it += 1;

      while (contents[it] != ")"):
        while (contents[it].isspace()):
          it += 1;

        if (contents[it] == ")"):
          break;

        params = [];
        while (contents[it] != ")" and contents[it] != ","):
          while (contents[it].isspace()):
            it += 1;

          p = "";
          while (
            not contents[it].isspace()
            and contents[it] != "," and contents[it] != ")"
          ):
            p += contents[it];
            it += 1;
          if (p != ""):
            params += [p];
        if (params != []):
          if (params[0] == "..."):
            params = ["...", "variadic"];
          parameters += [{"label": params[-1], "type": params[0:-1]}];

        if (contents[it] == ","):
          it += 1;

      # transform returntype -> fnname
      fnName = returnType[-1];
      returnType = returnType[:-1];

      exportedSymbols += [{
        "type": "function",
        "return-type": returnType,
        "label": fnName,
        "parameters": parameters,
      }];
    elif (strCmp(contents[it:], "typedef enum")):
      enums = [];
      while (contents[it] != "{"):
        it += 1;
      it += 1;

      enumVal = 0;
      # --- enum list end
      while (contents[it] != "}"):
        while (contents[it].isspace()):
          it += 1;
        e = "";
        while (
          not contents[it].isspace()
          and contents[it] != "," and contents[it] != "}"
          and contents[it] != "="
        ):
          e += contents[it];
          it += 1;
        while (
          not contents[it].isspace()
          and contents[it] != "," and contents[it] != "}"
          and contents[it] != "="
        ):
          it += 1;
        if (contents[it] == "="): # get the puleenum_val = >>value<<
          it += 1;
          while (contents[it].isspace()):
            it += 1;
          v = ""
          while (
            not contents[it].isspace()
            and contents[it] != "," and contents[it] != "}"
            and contents[it] != "="
          ):
            v += contents[it];
            it += 1;
          enumVal = int(v, 0); # this 0 at end means assume base

        if (e != ""):
          enums += [{"label": e, "value": enumVal}];
          enumVal += 1;

        while (contents[it].isspace()):
          it += 1;
        if (contents[it] == ","):
          it += 1;

      assert(contents[it] == "}");
      it += 1;
      while (contents[it].isspace()):
        it += 1;
      label = "";
      while (contents[it] != ";" and not contents[it].isspace()):
        label += contents[it]
        it += 1;

      exportedSymbols += [{
        "type": "enum",
        "label": label,
        "values": enums,
      }];
    elif (strCmp(contents[it:], "typedef struct")):
      fields = [];
      while (contents[it] != "{"):
        it += 1;
      it += 1;

      while (contents[it] != "}"):
        while (contents[it].isspace()):
          it += 1;
        field = [];
        while (contents[it] != ";"):
          f = ""
          while (not contents[it].isspace() and contents[it] != ";"):
            f += contents[it];
            it += 1;
          if (f != ""):
            field += [f];
          while (contents[it].isspace()):
            it += 1;
          print(f"ok: {contents[it]}");

        if (f != ""):
          fields += [{"label": field}];

        while (contents[it].isspace()):
          it += 1;
        if (contents[it] == ";"):
          it += 1;
        while (contents[it].isspace()):
          it += 1;

      assert(contents[it] == "}");
      it += 1;
      while (contents[it].isspace()):
        it += 1;
      label = "";
      while (contents[it] != ";" and not contents[it].isspace()):
        label += contents[it]
        it += 1;

      exportedSymbols += [{
        "type": "struct",
        "label": label,
        "fields": fields,
      }];
    else:
      it += 1;

  return exportedSymbols;

exportJson = [];
for subdir, dirs, files in os.walk("library/include/"):
  for filename in files:
    if (not filename.endswith(".h")):
      continue;

    if (filename == "core.h"):
      continue;

    print(f"checking {filename}");
    file = open(subdir + "/" + filename);
    exportJson += [exportJsonFromFile(file.read())];
    file.close();
print(json.dumps(exportJson));
