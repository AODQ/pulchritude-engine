#!/usr/bin/python3

# Parses json generated from json-binding-generator.py and outputs the
#   corresponding 'pule' C struct to enable layer support
#
# This emits into library/include/pulchritude-plugin/engine.h

import argparse
import json
import os
import re

parser = argparse.ArgumentParser("Parses json to lua")
parser.add_argument("--input", "-i", default="bindings/json/bindings.json")
parser.add_argument(
  "--output-header",
  default="library/include/pulchritude-plugin/engine.h"
)
parser.add_argument(
  "--output-load-mixin",
  default="library/plugin/src/engine-loader-mixin.inl"
)
parser.add_argument("--source", "-s", default=".")
inputArgs = vars(parser.parse_args())

with open(inputArgs['input']) as inputJsonFile:
  inputJson = json.load(inputJsonFile)

cheader = (
  "#pragma once\n"
  "// File automatically generated by c-layer-from-json-binding-generator.py\n"
  "// Use the functions provided by this file to allow layering support\n"
)

# --- binding includes
outFile = open(f"{inputArgs['output_header']}", "w+")
outFileMixin = open(f"{inputArgs['output_load_mixin']}", "w+")
outFile.write(cheader)
pout = (
  "\ntypedef struct PuleEngineLayer {\n"
  "  struct PuleEngineLayer * parent;\n"
  "  PuleString layerName;\n"
)
mixinout = ''

def cleanSymbol(symbol):
  return f"{symbol[4].lower()}{symbol[5:]}"

# returns function pointers from given function symbol
def extractFnToFnPtr(symbol, needsLabel = True):
  params = []
  for param in symbol['parameters']:
    if (param['meta-type'] == 'standard'):
      params.append(' '.join(param['type']))
    elif param['meta-type'] == 'variadic':
      params.append('...')
    elif param['meta-type'] == 'fn-ptr':
      params.append(extractFnToFnPtr(param, False))
    else:
      print(f"unknown meta type {param['meta-type']}")
  params = ', '.join(params)
  returntype = ' '.join(symbol['return-type'])
  label = cleanSymbol(symbol['label']) if needsLabel else ''
  return f"{returntype} (* {label})({params})"

# iterate thru all symbols and generate function pointers & loads
prevModule = ''
knownModules = []
mixinInclude = ''
for symbol in inputJson:
  # only operate on functions
  if (symbol['type'] != "function"):
    continue
  # don't load pulePluginLoadEngineLayer
  if (symbol['label'] == "pulePluginLoadEngineLayer"):
    continue
  if prevModule != symbol['module']:
    prevModule = symbol['module']
    pout += '  // ' + prevModule + '\n'
    mixinout += '  // ' + prevModule + '\n'
  pout += '  ' + extractFnToFnPtr(symbol) + ';\n'
  mixinout += (
    f"  layer->{cleanSymbol(symbol['label'])} = (\n"
    f"    reinterpret_cast<decltype(layer->{cleanSymbol(symbol['label'])})>(\n"
    f"      ::loadSymbol(\"{symbol['label']}\", layerName)\n"
    f"    )\n"
    f"  );\n"
    #f"  PULE_assert(layer->{cleanSymbol(symbol['label'])});\n" TODO enable this
  );
  header = f"pulchritude-{symbol['module']}/{symbol['header']}"
  if symbol['module'][:len('asset')] == 'asset':
    header = f"pulchritude-asset/{symbol['header']}"
  if header not in knownModules:
    knownModules.append(header)
    mixinInclude += f"#include <{header}>\n"

pout += "} PuleEngineLayer;\n"
outFile.write(mixinInclude);
outFile.write('\n');
outFile.write(pout);
outFile.write(
  "\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif"
  "\n\nPULE_exportFn void pulePluginLoadEngineLayer(\n"
  "  PuleEngineLayer * engineLayerBase,\n"
  "  PuleStringView const layerLabel, // ie 'default', 'debug'\n"
  "  PuleEngineLayer * layerParentNullable\n"
  ");\n"
  "\n\n#ifdef __cplusplus\n} // extern C\n#endif"
);

outFile.close()

outFileMixin.write(mixinout);
outFileMixin.close()

inputJsonFile.close()
