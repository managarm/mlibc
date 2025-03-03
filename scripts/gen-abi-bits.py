#!/usr/bin/env python3

"""
Generates the necessary ABI bits files by copying the ABI bits from the Linux ABI files
"""

import os
import shutil

PORT_NAME = "pinceros"


os.makedirs(f"abis/{PORT_NAME}", exist_ok=True)
os.makedirs(f"sysdeps/{PORT_NAME}/include/abi-bits", exist_ok=True)

for file in os.listdir("abis/linux"):
    shutil.copyfile(f"abis/linux/{file}", f"abis/{PORT_NAME}/{file}")
    try:
        os.remove(f"sysdeps/{PORT_NAME}/include/abi-bits/{file}")
    except FileNotFoundError:
        pass
    os.symlink(
        f"../../../../abis/{PORT_NAME}/{file}",
        f"sysdeps/{PORT_NAME}/include/abi-bits/{file}",
    )
