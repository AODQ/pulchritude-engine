#!/usr/bin/python3

# converts JSON file to PDS
#  TODO - this should be a binary that uses pulchritude's internal PDS, that way
#         it works no matter what the PDS implementation is

import argparse
import os
import json

parser = argparse.ArgumentParser("Parses json to PDS")
parser.add_argument("--input", "-i")
parser.add_argument("--output", "-o")
inputArgs = vars(parser.parse_args())

if "input" not in inputArgs or inputArgs["input"] == None:
  print("must pass input")
  exit()
inputFile = inputArgs["input"]

outputFile = inputFile[:inputFile.rfind(".")] + ".pds"
if "output" in inputArgs and inputArgs["output"] != None:
  outputFile = inputArgs["output"]

print(f"converting '{inputFile}' -> '{outputFile}'")

fileRead = open(inputFile, 'r')
inputJson = json.load(fileRead)
fileRead.close()

fileWrite = open(outputFile, 'w')

tabber = 0

def tabs():
  global tabber
  for i in range(tabber):
    fileWrite.write("\t")

def parseJson(obj):
  global tabber
  if obj and type(obj) is dict:
    fileWrite.write("{\n")
    tabber += 1
    for key in obj:
      tabs()
      fileWrite.write(f"{key}: ")
      parseJson(obj[key])
    tabber -= 1
    tabs()
    fileWrite.write("},\n")

  elif obj and type(obj) is list:
    fileWrite.write("[\n")
    tabber += 1
    for item in obj:
      tabs()
      parseJson(item)
    tabber -= 1
    tabs()
    fileWrite.write("],\n")
  else:
    if (type(obj) is str):
      fileWrite.write(f"\"{obj}\",\n")
    elif (type(obj) is bool):
      fileWrite.write(f"{'true' if obj else 'false'},\n")
    else:
      fileWrite.write(f"{obj},\n")

parseJson(inputJson)
fileWrite.close()
