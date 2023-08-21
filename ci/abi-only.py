import argparse
import glob
import subprocess
import os
import json
import sys

parser = argparse.ArgumentParser()
parser.add_argument("dir", help="Folder the mlibc headers are in")
args = vars(parser.parse_args())

print("Running ABI-only checker on folder '{}'.".format(args["dir"]))

headers = []
include_path = os.path.join(args["dir"], "include")
bits_path = os.path.join(include_path, "bits")

def get_all_headers(folder):
    headers = list()
    if not os.path.isdir(folder):
        print("'{}' was expect to be a directory".format(folder))
        raise TypeError()

    make_abs = lambda x: os.path.join(folder, x)
    files = map(make_abs, os.listdir(folder))
    for f in files:
        if os.path.isdir(f):
            subdir_headers = get_all_headers(f)
            if subdir_headers:
                headers += subdir_headers
        elif f.endswith(".h"):
            headers.append(f)

    return headers

for f in get_all_headers(include_path):
    if bits_path in f:
        continue
    headers += [f]

command = ["clang", "-Xclang", "-ast-dump=json", "-fsyntax-only",
           "-D__MLIBC_ABI_ONLY", "-includestdint.h",
           "-isystem" + include_path]
full_output = []

for h in headers:
    process = subprocess.run(command + [h], capture_output=True, encoding="utf-8")
    full_output += [process.stdout]
    print("Dumped ast from '{}'.".format(h))

def get_file_from_fn(fn):
    try:
        str = ", included from {}"
        return str.format(fn["loc"]["includedFrom"]["file"])
    except KeyError as e:
        try:
            str = ", declared in {}"
            return str.format(fn["range"]["begin"]["expansionLoc"]["file"])
        except KeyError as e:
            return " from an unknown file"
    
seen_functions = set()
exit_code = 0
for o in full_output:
    decoded = None
    try:
        decoded = json.loads(o)
    except Exception as e:
        print(o)
        exit(1)

    if decoded["kind"] != "TranslationUnitDecl":
        print("Malformed AST dump")
        exit(1)

    nodes = decoded["inner"]
    for n in nodes:
        if n["kind"] == "FunctionDecl" and n["name"] not in seen_functions:
            exit_code = 1
            str = "Encountered declaration of function '{}'{}."
            print(str.format(n["name"], get_file_from_fn(n)))
            seen_functions.add(n["name"])

exit(exit_code)
