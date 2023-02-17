#!/usr/bin/python3

# Parses json generated from json-binding-generator.py and outputs the
#   a default layer (quick & easy way to start an engine layer),
#   this can be used as reference if you want to write a layer in another
#   language too
#
# This emits into tmp-layer.cpp

import argparse
import json
import os
import re

parser = argparse.ArgumentParser("Parses json to lua")
parser.add_argument("--input", "-i", default="bindings/json/bindings.json")
parser.add_argument(
  "--output-tmp",
  default="tmp-layer.cpp"
)
parser.add_argument("--source", "-s", default=".")
inputArgs = vars(parser.parse_args())

with open(inputArgs['input']) as inputJsonFile:
  inputJson = json.load(inputJsonFile)

# --- binding includes
outFile = open(f"{inputArgs['output_tmp']}", "w+")
pout = ""

pout += (
  "static PuleEngineLayer pul;\n"
  "extern \"C\" {\n"
  "void puleEngineLayerEntry(PuleEngineLayer * const parentLayer) {\n"
  "  puleLog(\"Using layer entry: %s\", parentLayer->layerName.contents);\n"
  "  pul = *parentLayer;\n"
  "}\n"
)

def cleanSymbol(symbol):
  return f"{symbol[4].lower()}{symbol[5:]}"

def extractFnPtrParameter(symbol, hasLabel = True):
  params = []
  for param in symbol['parameters']:
    if (param['meta-type'] == 'standard'):
      params.append(' '.join(param['type']) + ' ' + param['label'])
    elif param['meta-type'] == 'variadic':
      params.append('...')
    elif param['meta-type'] == 'fn-ptr':
      params.append(extractFnToFnPtr(param, False))
    else:
      print(f"unknown meta type {param['meta-type']}")
  params = ', '.join(params)
  returntype = ' '.join(symbol['return-type'])
  label = symbol['label'] if hasLabel else ''
  return f"{returntype} (* {label})({params})"

# returns function pointers from given function symbol
def extractFn(symbol):
  out = ' '.join(symbol['return-type'])
  out += f" {symbol['label']}("
  params = []
  for param in symbol['parameters']:
    if (param['meta-type'] == 'standard'):
      params.append(' '.join(param['type']) + ' ' + param['label'])
    elif param['meta-type'] == 'variadic':
      return f"// no variadic generation: {symbol['label']}\n"
    elif param['meta-type'] == 'fn-ptr':
      params.append(extractFnPtrParameter(param))
    else:
      print(f"unknown meta type {param['meta-type']}")

  # compute both variations of signature and pick one that fits in 80 columns
  # `void foo(int a, int b) {` vs `void foo(\n  int a,\n  int b\n) {`
  len80pout = out + "\n  " + ',\n  '.join(params) + "\n) {"
  len80out = out + f"{', '.join(params)}) {{"
  out = len80pout if len(len80out) >= 80 else len80out
  out += "\n"
  out += f"  debugLayerFnEntry();\n"
  # compute both variations again for return
  len80ret = (
    f"  return pul.{cleanSymbol(symbol['label'])}("
    + ', '.join(p['label'] for p in symbol['parameters'])
    + ");"
  )
  len80pret = (
    f"  return (\n    pul.{cleanSymbol(symbol['label'])}(\n      "
    + ',\n      '.join(p['label'] for p in symbol['parameters'])
    + "\n    )"
    + "\n  );"
  )
  out += len80pret if len(len80out) >= 80 else len80ret
  out += f"\n}}\n\n"
  return out

# iterate thru all symbols and generate functions & calls
prevModule = ''
knownModules = []
mixinInclude = ''
for symbol in inputJson:
  # only operate on functions
  if (symbol['type'] != "function"):
    continue
  # mark module split
  if prevModule != symbol['module']:
    prevModule = symbol['module']
    pout += '// ' + prevModule + '\n'

  pout += extractFn(symbol)
  header = f"pulchritude-{symbol['module']}/{symbol['header']}"
  if symbol['module'][:len('asset')] == 'asset':
    header = f"pulchritude-asset/{symbol['header']}"
  if header not in knownModules:
    knownModules.append(header)
    mixinInclude += f"#include <{header}>\n"

pout += "} // C"

outFile.write(mixinInclude)
outFile.write('\n')
outFile.write(pout)
outFile.close()

inputJsonFile.close()
