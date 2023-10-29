#!/usr/bin/python3

# compiles all the shaders for the engine, and writes them out to engine
# files to be included in the engine

import argparse
import os
import subprocess

parser = argparse.ArgumentParser("")
parser.add_argument("--dir", "-i", default="bindings/json/bindings.json")
inputArgs = vars(parser.parse_args())

# use glslangValidator to compile the shaders
def compileShader(contentName, filename, outFilename):
  # print current directory
  print("Compiling shader: ", filename, " to ", outFilename)
  filename = inputArgs["dir"] + "/" + filename
  outFilename = inputArgs["dir"] + "/" + outFilename
  ret = subprocess.run(
    ["glslangValidator", filename, "-V", "-o", "out.spv"],
    capture_output=True
  )
  if (ret.returncode != 0):
    print("Failed to compile shader: ", filename)
    print(ret.stderr.decode("utf-8"))
    print(ret.stdout.decode("utf-8"))
    exit(1)
  # read in the binary file, and convert it to a C array
  outStr = "static const uint8_t " + contentName + "[] = {"
  with open("out.spv", "rb") as f:
    byte = f.read(1)
    while byte:
      outStr += "0x{:02x}, ".format(ord(byte))
      byte = f.read(1)
  outStr += "};\n"
  # write the C array to the output file
  with open(outFilename, "w") as f:
    f.write(outStr)
  # delete the temporary binary file
  os.remove("out.spv")

compileShader(
  "debugGfxLineVert",
  "shaders/debug-gfx-line.vert",
  "library/gfx-debug/src/autogen-debug-gfx-line.vert.spv"
)

compileShader(
  "debugGfxLineFrag",
  "shaders/debug-gfx-line.frag",
  "library/gfx-debug/src/autogen-debug-gfx-line.frag.spv"
)

compileShader(
  "textBitmapVert",
  "shaders/text-bitmap.vert",
  "library/text/src/autogen-text-bitmap.vert.spv"
)

compileShader(
  "textBitmapFrag",
  "shaders/text-bitmap.frag",
  "library/text/src/autogen-text-bitmap.frag.spv"
)

compileShader(
  "imguiVert",
  "shaders/imgui.vert",
  "library/imgui/src/autogen-imgui.vert.spv"
)

compileShader(
  "imguiFrag",
  "shaders/imgui.frag",
  "library/imgui/src/autogen-imgui.frag.spv"
)
