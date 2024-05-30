#!/usr/bin/python3
import argparse, os, subprocess

parser = argparse.ArgumentParser("")
parser.add_argument("--dir", "-i", default="bindings/json/bindings.json")
inputArgs = vars(parser.parse_args())

os.chdir(f"{inputArgs['dir']}/util/binding-generator/")
ret = (
  subprocess.run(
    ["puledit", "run", "--error-segfaults",],
    capture_output=True,
  )
)
if (ret.returncode != 0):
  print("Failed to generate bindings")
  print(ret.stdout.decode("utf-8"))
  print(ret.stderr.decode("utf-8"))
  exit(1)
