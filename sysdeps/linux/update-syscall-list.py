#!/usr/bin/env python3
# SPDX-License-Identifier: MIT

import argparse
import hashlib
import io
import json
import logging
import os
import os.path as path
import pathlib
import subprocess
import tarfile
import tempfile
import urllib.request as urr
from dataclasses import dataclass

KERNEL_RELEASES = "https://www.kernel.org/releases.json"
NR_PREFIX = "__NR_"
logging.basicConfig(level=logging.DEBUG)
LOGGER = logging.getLogger(__name__)

# ADD NEW ARCHITECTURES HERE: these should match directory names in Linuxes arch/
# directory or the second element of one of the harnesses below
WANTED_ARCHES = ["riscv64", "x86_64", "arm64", "i386", "m68k", "loongarch64"]
# How to translate from Linux arch directory names to gnutools architecture fields
LINUX_GNU_TRANSLATION = {
    "arm64": "aarch64",
    "i386": "x86",
    "loongarch": "loongarch64",
}

argp = argparse.ArgumentParser()
argp.add_argument("kver", help="Kernel version to download", default="latest")


@dataclass
class Kernel:
    version: str
    srchash: str
    srcdir: pathlib.Path


@dataclass
class Context:
    dirpath: pathlib.Path


def run(args: list[str], **kwargs):
    LOGGER.debug("running %s", args)
    subprocess.check_call(args, **kwargs)


def collect(args: list[str], **kwargs):
    LOGGER.debug("running %s (kw: %s)", args, kwargs)
    kwargs.setdefault("text", True)
    return subprocess.check_output(args, **kwargs)


def fetch_kernel(ctx: Context, kver: str) -> Kernel:
    LOGGER.debug("fetching %s", KERNEL_RELEASES)
    with urr.urlopen(KERNEL_RELEASES) as f:
        if f.status != 200:
            LOGGER.error("meta fetch failed: %s", f)
            raise RuntimeError("failed to get kernel meta")
        metadata = json.load(f)

    if kver == "latest":
        kver = metadata["latest_stable"]["version"]

    LOGGER.debug("figured out kernel version: %s", kver)

    for rel in metadata["releases"]:
        if rel["version"] == kver:
            kernel_release = rel
            break
    else:
        raise RuntimeError("failed to find kver " + kver)

    kurl = kernel_release["source"]
    LOGGER.debug("kernel src url: %s", kurl)
    source_path = ctx.dirpath / ("source.tar" + path.splitext(kurl)[1])

    with urr.urlopen(kurl) as f, open(source_path, "wb") as g:
        if f.status != 200:
            LOGGER.error("ksrc fetch failed: %s", f)
            raise RuntimeError("failed to get kernel source")

        hasher = hashlib.blake2b()
        while buf := f.read(16 * 1024):
            hasher.update(buf)
            g.write(buf)

    srchash = hasher.hexdigest()
    srcpath = ctx.dirpath / "src"

    with tarfile.open(source_path) as srctar:
        memb: tarfile.TarInfo
        for memb in srctar:
            if not memb.name.startswith("linux-"):
                raise RuntimeError(
                    "malformed tar member in %s (%s): %s has bad prefix".format(
                        kurl, srchash, memb.name
                    )
                )
            memb.name = "./" + memb.name[memb.name.find("/") + 1 :]
            srctar.extract(memb, path=srcpath)

    return Kernel(
        version=kver,
        srchash=srchash,
        srcdir=srcpath,
    )


def run_harness(ctx: Context, kernel: Kernel, harness: (str, str)) -> dict[str, int]:
    LOGGER.debug("running harness %s", harness)
    (archdir, arch, defines) = harness
    flags = [f"-D{x}" for x in defines]
    archout = ctx.dirpath / "headers" / archdir
    if not archout.exists():
        run(
            [
                "make",
                "-sC",
                kernel.srcdir,
                f"ARCH={archdir}",
                f"O={archout}",
                "headers_install",
            ]
        )

    hdrdir = archout / "usr/include"
    callset = set()
    for x in collect(
        ["gcc", "-E", "-dM", "-I", hdrdir, *flags, "-"], input="#include <asm/unistd.h>"
    ).splitlines():
        x = x.split(maxsplit=2)
        if len(x) < 2 or x[0] != "#define":
            # skip invalid lines
            continue

        defname = x[1]
        if not defname.startswith(NR_PREFIX):
            # not a syscall
            continue

        defname = defname[len(NR_PREFIX) :]

        if (
            "SYSCALL" in defname
            or defname.startswith("available")
            or defname.startswith("reserved")
            or defname.startswith("unused")
        ):
            continue

        # dead syscalls
        if defname in [
            "Linux",
            "afs_syscall",
            "break",
            "ftime",
            "gtty",
            "lock",
            "mpx",
            "oldwait4",
            "prof",
            "profil",
            "putpmsg",
            "security",
            "stty",
            "tuxcall",
            "ulimit",
            "vserver",
            "arm_sync_file_range",
            "utimesat",
            "ni_syscall",
            "xtensa",
        ]:
            continue
        callset.add(defname)

    # alright, we have the set of all syscalls, time to produce their numbers
    syscall_dumper = """
/* my sincerest apologies */
#include <stdio.h>
"""

    for x in defines:
        syscall_dumper += "#define {}\n".format(x.replace("=", " "))

    syscall_dumper += """
#include <asm/unistd.h>

int main() {
    puts("{");
    """

    comma = ""
    for x in callset:
        syscall_dumper += 'printf("{comma}\\"{sc}\\": %d\\n", {pfx}{sc});\n    '.format(
            sc=x, comma=comma, pfx=NR_PREFIX
        )
        comma = ","

    syscall_dumper += """
    puts("}");
}
"""

    dumper = archout / "dumper"
    with tempfile.NamedTemporaryFile(suffix=".c") as src:
        src.write(syscall_dumper.encode())
        run(["gcc", "-o", dumper, "-I", hdrdir, src.name])
    return json.loads(collect([dumper]))


def main(ctx: Context):
    args = argp.parse_args()
    kernel = fetch_kernel(ctx, args.kver)

    LOGGER.info("got kernel version %s (%s)", kernel.version, kernel.srchash)

    archlist = os.listdir(kernel.srcdir / "arch")
    archlist.remove("Kconfig")
    archlist.remove("um")

    # harnesses converted from
    # https://github.com/hrw/syscalls-table/blob/c638834d9b5d71bb40a555755ea07735cace58f2/do_all_tables.sh
    # (arch_dirname, archname, list[defines])
    harnesses: (str, str, list[str])
    harnesses = [
        (arch,) + x
        for arch in archlist
        if not arch.startswith(".")
        for x in {
            # arch specific overrides
            # format: (archname, list[defines]), gets prefixed with archdir outside
            "arm": [
                ("armoabi", []),
                ("arm", ["__ARM_EABI_"]),
            ],
            "loongarch": [
                ("loongarch64", ["_LOONGARCH_SZLONG=64"]),
            ],
            "mips": [
                ("mipso32", ["_MIPS_SIM=_MIPS_SIM_ABI32"]),
                ("mips64n32", ["_MIPS_SIM=_MIPS_SIM_NABI32"]),
                ("mips64", ["_MIPS_SIM=_MIPS_SIM_ABI64"]),
            ],
            "powerpc": [
                ("powerpc", []),
                ("powerpc64", []),
            ],
            "riscv": [
                ("riscv32", ["__SIZEOF_POINTER__=4"]),
                ("riscv64", ["__LP64__"]),
            ],
            "s390": [
                ("s390", []),
                ("s390x", []),
            ],
            "sparc": [
                ("sparc", ["__32bit_syscall_numbers__"]),
                ("sparc64", ["__arch64__"]),
            ],
            "tile": [
                ("tile", []),
                ("tile64", ["__LP64__", "__tilegx__"]),
            ],
            "x86": [
                ("i386", ["__i386__"]),
                ("x32", ["__ILP32__"]),
                ("x86_64", ["__LP64__"]),
            ],
        }.get(arch, [(arch, [])])
    ]

    syscall_set = set()

    for x in harnesses:
        syscalls = run_harness(ctx, kernel, x)
        syscall_set |= syscalls.keys()

        wanted_arch = x[1]
        if wanted_arch not in WANTED_ARCHES:
            continue

        wanted_arch = LINUX_GNU_TRANSLATION.get(wanted_arch, wanted_arch)

        pathlib.Path(wanted_arch).mkdir(exist_ok=True)

        with open(wanted_arch + "/syscallnos.h", "w") as f:
            print("#ifndef __MLIBC_SYSCALLNOS_h", file=f)
            print("#define __MLIBC_SYSCALLNOS_h", file=f)
            print("/* This file is autogenerated. Don't bother. */", file=f)
            print(
                "/* Generator script: sysdeps/linux/update-syscall-list.py. */", file=f
            )

            for name, num in sorted(syscalls.items(), key=lambda x: x[1]):
                print("#define {p}{sc} {n}".format(p=NR_PREFIX, sc=name, n=num), file=f)

            print("#endif /* __MLIBC_SYSCALLNOS_h */", file=f)

    with open("include/bits/syscall_aliases.h", "w") as f:
        print("#ifndef __MLIBC_SYSCALL_ALIAS_BIT", file=f)
        print("#define __MLIBC_SYSCALL_ALIAS_BIT", file=f)
        print("/* This file is autogenerated. Don't bother. */", file=f)
        print("/* Generator script: sysdeps/linux/update-syscall-list.py. */", file=f)

        for x in sorted(list(syscall_set)):
            print(
                f"""\
#ifdef\t{NR_PREFIX}{x}
#\tdefine SYS_{x} {NR_PREFIX}{x}
#endif
""",
                end="",
                file=f,
            )

        print("#endif /* __MLIBC_SYSCALL_ALIAS_BIT */", file=f)


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as td:
        main(
            Context(
                dirpath=pathlib.Path(td),
            )
        )
