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

#print(f"converting '{inputFile}' -> '{outputFile}'")

fileRead = open(inputFile, 'r')
inputJson = json.load(fileRead)
fileRead.close()

fileWrite = open(outputFile, 'w')

tabber = 0

def tabs():
  global tabber
  for i in range(tabber):
    fileWrite.write("\t")

def parseJson(obj, onFirstPass, inArray = False):
  global tabber
  if obj and type(obj) is dict:
    if (not onFirstPass):
      fileWrite.write("{\n")
      tabber += 1
    for key in obj:
      tabs()
      fileWrite.write(f"{key}: ")
      parseJson(obj[key], onFirstPass=False, inArray=False)
    if (not onFirstPass):
      tabber -= 1
    tabs()
    if (not onFirstPass):
      fileWrite.write("},\n")

  elif obj and type(obj) is list:
    if (onFirstPass):
      print("can't have array as top level object in PDS")
      exit(0)
    fileWrite.write("[")
    for item in obj:
      parseJson(item, onFirstPass=False, inArray=True)
    tabs()
    fileWrite.write("],\n")
  else:
    if (onFirstPass):
      print("can't have values as top level object in PDS")
      exit(0)
    if (type(obj) is str):
      if (obj[-5:] == ".json"):
        obj = obj[:-5] + ".pds"
      fileWrite.write(f"\"{obj}\",")
    elif (type(obj) is bool):
      fileWrite.write(f"{'true' if obj else 'false'},")
    else:
      fileWrite.write(f"{obj},")
    if (not inArray):
      fileWrite.write("\n")

parseJson(inputJson, onFirstPass=True)
fileWrite.close()
