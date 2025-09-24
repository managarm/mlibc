#!/usr/bin/env python3

import argparse
import os
import pathlib
import re
import subprocess
import sys
import tempfile
import typing
from dataclasses import dataclass, field

import clang.cindex
import colorama
import yaml
from clang.cindex import CursorKind, TokenKind, TypeKind

dry_run = True
errors_emitted = 0


def on_ci() -> bool:
    return "CI" in os.environ


def log_err(prefix, msg):
    global errors_emitted

    if on_ci():
        print(f"{prefix}: {msg}", file=sys.stderr)
    else:
        print(
            f"{colorama.Fore.RED}{prefix}{colorama.Style.RESET_ALL}: {msg}",
            file=sys.stderr,
        )
    errors_emitted += 1


def no_system_includes(cursor, level):
    """filter out verbose stuff from system include files"""
    return (level != 1) or (
        cursor.location.file is not None
        and not cursor.location.file.name.startswith("/usr/include")
    )


class Type:
    def __init__(self, t: clang.cindex.Type):
        self.t = t
        self.kind = t.kind
        self.typename = (
            str(self.t.spelling).removesuffix("restrict").removeprefix("const ")
        )
        self.compat_typename = None

        match t.kind:
            case TypeKind.ELABORATED:
                replacement = next(
                    filter(
                        lambda x: self.typename in x.keys(),
                        config["map_record_to_struct"],
                    ),
                    None,
                )
                if replacement is not None:
                    self.compat_typename = (
                        replacement[self.typename]
                        .removesuffix("restrict")
                        .removeprefix("const ")
                    )
            case TypeKind.RECORD:
                replacement = next(
                    filter(
                        lambda x: self.typename in x.keys(),
                        config["map_record_to_struct"],
                    ),
                    None,
                )
                if replacement is not None:
                    self.kind = TypeKind.POINTER
                    self.compat_typename = (
                        replacement[self.typename]
                        .removesuffix("restrict")
                        .removeprefix("const ")
                    )
            case TypeKind.POINTER:
                ptr_type = (
                    self.t.get_pointee()
                    .spelling.removesuffix("restrict")
                    .removeprefix("const ")
                )
                replacement = next(
                    filter(
                        lambda x: ptr_type in x.keys(), config["equivalent_structs"]
                    ),
                    None,
                )
                if replacement is not None:
                    self.compat_typename = (
                        replacement[ptr_type]
                        .removesuffix("restrict")
                        .removeprefix("const ")
                        + " *"
                    )

    @property
    def canonical(self):
        return Type(self.t.get_canonical())

    @property
    def pointee_type(self):
        if self.kind == TypeKind.POINTER:
            return Type(self.t.get_pointee())
        if (
            self.kind == TypeKind.INCOMPLETEARRAY
            or self.kind == TypeKind.CONSTANTARRAY
            or self.kind == TypeKind.VARIABLEARRAY
        ):
            return Type(self.t.get_array_element_type())
        if self.kind == TypeKind.ELABORATED:
            return None
        log_err(
            "unhandled pointee resolution", str(self.kind).removeprefix("TypeKind.")
        )
        return None

    def __str__(self):
        return self.typename

    def __eq__(self, other):
        if self.kind != other.kind:
            if (
                self.kind == TypeKind.INCOMPLETEARRAY
                or other.kind == TypeKind.INCOMPLETEARRAY
            ):
                if self.pointee_type is None or other.pointee_type is None:
                    return False
                return self.pointee_type == other.pointee_type
            elif (
                self.kind == TypeKind.CONSTANTARRAY
                or other.kind == TypeKind.CONSTANTARRAY
            ):
                if self.pointee_type is None or other.pointee_type is None:
                    return False
                return self.pointee_type == other.pointee_type
            elif (
                self.kind == TypeKind.VARIABLEARRAY
                or other.kind == TypeKind.VARIABLEARRAY
            ):
                if self.pointee_type is None or other.pointee_type is None:
                    return False
                return self.pointee_type == other.pointee_type
            elif self.kind == TypeKind.ELABORATED or other.kind == TypeKind.ELABORATED:
                return (
                    self.t.get_size() == other.t.get_size()
                    and self.t.get_align() == other.t.get_align()
                )
            else:
                return False

        if str(self) == str(other):
            return True
        return (
            self.compat_typename == str(other)
            or self.compat_typename == other.compat_typename
        )

    def is_valid(self):
        return self.t.kind != TypeKind.INVALID


@dataclass
class Function:
    name: str
    linkage: clang.cindex.LinkageKind
    ret_type: clang.cindex.Type
    location: clang.cindex.SourceLocation
    arguments: typing.List[Type]

    def __init__(self, c: clang.cindex.Cursor):
        self.c = c
        self.name = c.mangled_name
        self.linkage = c.linkage
        self.ret_type = c.result_type
        self.location = c.location
        self.arguments = list()

        for arg in c.get_arguments():
            self.arguments.append(Type(arg.type))


@dataclass
class MacroDefinition:
    name: str
    location: clang.cindex.SourceLocation

    def __init__(self, c: clang.cindex.Cursor):
        self.c = c
        self.name = c.spelling
        self.location = c.location
        self.tokens = list(self.c.get_tokens())

    @property
    def first_token(self):
        return self.tokens[1] if len(self.tokens) > 1 else None


@dataclass
class EnumDecl:
    name: str
    location: clang.cindex.SourceLocation

    def __init__(self, c: clang.cindex.Cursor):
        self.c = c
        self.name = c.spelling
        self.location = c.location


@dataclass
class StructDecl:
    name: str
    location: clang.cindex.SourceLocation

    def __init__(self, c: clang.cindex.Cursor):
        self.c = c
        self.name = c.spelling
        self.location = c.location
        self.alignment = c.type.get_align()
        self.size = c.type.get_size()


@dataclass
class Typedef:
    name: str
    location: clang.cindex.SourceLocation

    def __init__(self, c: clang.cindex.Cursor):
        self.c = c
        self.name = c.spelling
        self.location = c.location
        self.alignment = c.type.get_align()
        self.size = c.type.get_size()

@dataclass
class State:
    """
    Represents the parsed state of a set of headers.
    """

    path: pathlib.Path
    functions: typing.Dict[str, Function] = field(default_factory=dict)
    macros: typing.Dict[str, MacroDefinition] = field(default_factory=dict)
    enums: typing.Dict[str, EnumDecl] = field(default_factory=dict)
    structs: typing.Dict[str, StructDecl] = field(default_factory=dict)
    typedefs: typing.Dict[str, StructDecl] = field(default_factory=dict)

    def __init__(self, path: pathlib.Path):
        self.path = path
        self.functions = dict()
        self.macros = dict()
        self.enums = dict()
        self.structs = dict()
        self.typedefs = dict()


@dataclass
class Comparison:
    config: dict

    def is_ignored(self, typename, ignorelist, name):
        if (
            typename == "macros"
            and (name.startswith("_") or name.startswith("MLIBC_"))
            and name.endswith("_H")
        ):
            return True
        if name in ignorelist:
            return True
        if "forced_" + typename in config and name in config["forced_" + typename]:
            return False
        if name.startswith("__"):
            return True
        if "ignored_" + typename in config and name in config["ignored_" + typename]:
            return True
        return False

    @staticmethod
    def is_skipped_file(base_dir: pathlib.Path, file: pathlib.Path, config):
        if Comparison.is_ignored_file(base_dir, file, config):
            return True

        if base_dir == args.reference:
            for p in config["base_skipped_directories"]:
                if str(file).startswith(os.path.join(base_dir, p)):
                    return True
            for p in config["base_skipped_files"]:
                stripped_file = str(file).removeprefix(str(base_dir)).removeprefix("/")
                if stripped_file == p:
                    return True
        return False

    @staticmethod
    def is_ignored_file(base_dir: pathlib.Path, file: pathlib.Path, config):
        if not str(file).startswith(str(base_dir)):
            return True

        if base_dir == args.reference:
            for p in config["base_ignored_directories"]:
                if str(file).startswith(os.path.join(base_dir, p)):
                    return True
            for p in config["base_ignored_files"]:
                stripped_file = str(file).removeprefix(str(base_dir)).removeprefix("/")
                if stripped_file == p:
                    return True
        for p in config["ignored_files"]:
            stripped_file = str(file).removeprefix(str(base_dir)).removeprefix("/")
            if stripped_file == p:
                return True
        for p in config["includes"]:
            if str(base_dir).startswith(p):
                return True
        return False

    def from_cursor(self, base_dir, header, cursor, filter_pred, state: State, level=0):
        if cursor.location.file:
            f = pathlib.Path(str(cursor.location.file))

            if Comparison.is_ignored_file(base_dir, f, config):
                return

        if filter_pred(cursor, level):
            if args.dump_tree:
                print(f"{"-" * level} {cursor.kind} {cursor.spelling}")
                for c in cursor.get_children():
                    self.from_cursor(base_dir, header, c, filter_pred, state, level + 1)

            match cursor.kind:
                case CursorKind.TRANSLATION_UNIT:
                    for c in cursor.get_children():
                        self.from_cursor(
                            base_dir, header, c, filter_pred, state, level + 1
                        )
                case CursorKind.INCLUSION_DIRECTIVE:
                    pass
                case CursorKind.FUNCTION_DECL:
                    if not cursor.mangled_name.startswith("__"):
                        f = Function(cursor)
                        state.functions.update({f.name: f})
                case CursorKind.STATIC_ASSERT | CursorKind.UNEXPOSED_DECL:
                    pass
                case CursorKind.ENUM_DECL:
                    if not self.is_ignored("enums", [], cursor.spelling):
                        for x in cursor.get_children():
                            state.enums.update({x.spelling: EnumDecl(x)})
                case CursorKind.MACRO_DEFINITION:
                    if not self.is_ignored("macros", [], cursor.spelling):
                        state.macros.update({cursor.spelling: MacroDefinition(cursor)})
                case CursorKind.STRUCT_DECL:
                    if not self.is_ignored("structs", [], cursor.spelling):
                        if cursor.is_definition():
                            state.structs.update({cursor.spelling: StructDecl(cursor)})
                case CursorKind.UNION_DECL:
                    if not self.is_ignored("unions", [], cursor.spelling):
                        if cursor.is_definition():
                            state.structs.update({cursor.spelling: StructDecl(cursor)})
                case CursorKind.TYPEDEF_DECL:
                    if not self.is_ignored("typedefs", [], cursor.spelling):
                        children = list(cursor.get_children())
                        if not children:
                            return

                        state.typedefs.update({cursor.spelling: Typedef(cursor)})

                        if children[0].kind == CursorKind.TYPE_REF:
                            child_struct_name = children[0].spelling.removeprefix(
                                "struct "
                            )

                            if child_struct_name in state.structs:
                                state.structs.update(
                                    {cursor.spelling: state.structs[child_struct_name]}
                                )
                case CursorKind.MACRO_INSTANTIATION | CursorKind.VAR_DECL:
                    # don't care (for now)
                    pass
                case _:
                    log_err(
                        "unhandled cursor type",
                        f"{cursor.kind} {cursor.spelling} {cursor.displayname} {cursor.location}",
                    )


def cc_name():
    if args.clang_version:
        return [f"clang-{args.clang_version}", f"--target={f"{args.arch}-linux-gnu"}"]
    return ["clang", f"--target={f"{args.arch}-linux-gnu"}"]


def cxx_name():
    # m68k on clang defaults to a small codemodel that doesn't work
    # and I have not found a way to change it outside of `llc` other
    if args.arch == "m68k":
        return ["m68k-linux-gnu-g++"]
    if args.clang_version:
        return [f"clang++-{args.clang_version}", f"--target={f"{args.arch}-linux-gnu"}"]
    return ["clang++", f"--target={f"{args.arch}-linux-gnu"}"]


def parse(
    file: pathlib.Path, resource_dir: pathlib.Path, base_dir: pathlib.Path, state: State
):
    index = clang.cindex.Index.create()
    tu = None

    clang_args = [f"-I{resource_dir}"]
    clang_args += [f"-I{p}" for p in config["includes"]]
    clang_args += [f"-I{base_dir}"]
    clang_args += [f"-I{base_dir / f"{args.arch}-linux-gnu"}"]
    clang_args += [f"--target={f"{args.arch}-linux-gnu"}"]
    clang_args += ["-D_GNU_SOURCE", "-D_FILE_OFFSET_BITS=64", "-Wno-macro-redefined"]

    try:
        tu = index.parse(
            base_dir / file,
            args=clang_args,
            options=clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
            | clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES,
        )
    except Exception as e:
        log_err("parsing error", f"{file}: {e}")
        return

    assert tu

    if not Comparison.is_skipped_file(base_dir, base_dir / file, config):
        if tu.diagnostics:
            [log_err("compile error", d) for d in tu.diagnostics]
            print(f"\n{errors_emitted} errors emitted")
            exit(errors_emitted)

        parser = Comparison(config)
        if args.verbose:
            print(f"// {tu.spelling.strip()}")
        parser.from_cursor(base_dir, file, tu.cursor, no_system_includes, state)


def compare_states(a, b):
    global errors_emitted

    a_symbols = sorted(a.functions.keys())
    b_symbols = sorted(b.functions.keys())
    symbols = a_symbols
    symbols.extend(x for x in b_symbols if x not in symbols)
    c = Comparison(config)

    if args.function_signatures:
        lines = []

        for s in symbols:
            if s not in a.functions or s not in b.functions:
                continue

            a_func = a.functions[s]
            b_func = b.functions[s]

            a_ret_type = Type(a_func.ret_type.get_canonical())
            b_ret_type = Type(b_func.ret_type.get_canonical())

            if (
                a_ret_type != b_ret_type
                and a_func.ret_type.spelling != b_func.ret_type.spelling
            ):
                lines.append(
                    f"\t{s}: mismatched return type ({a_ret_type} vs. {b_ret_type})"
                )
                errors_emitted += 1

            a_args = a_func.arguments
            b_args = b_func.arguments

            if len(a_args) != len(b_args):
                lines.append(
                    f"\t{s}: argument count mismatch ({len(a_args)} vs. {len(b_args)})"
                )
                errors_emitted += 1

            for i, at in enumerate(a_args):
                bt = b_args[i]
                if at != bt and at.canonical != bt.canonical:
                    lines.append(
                        f"\t{s}: mismatched type for argument at position {(i + 1)} ({at} ({str(at.kind).removeprefix("TypeKind.")}) vs. {bt} ({str(bt.kind).removeprefix("TypeKind.")}))"
                    )
                    errors_emitted += 1

        if lines:
            print()
            print(f"checking {len(symbols)} functions for signature mismatches:")
            for line in lines:
                print(line)

    if args.missing_functions:
        a_unique_symbols = list(filter(lambda e: e not in b_symbols, a_symbols))
        b_unique_symbols = list(filter(lambda e: e not in a_symbols, b_symbols))

        if args.verbose and len(a_unique_symbols) > 0:
            print()
            print(f"{len(a_unique_symbols)} symbols only defined in {a.path}:")
            for s in sorted(a_unique_symbols):
                print(f"{s} defined in {a.functions[s].location}")

        if len(b_unique_symbols) > 0:
            print()
            print(f"{len(b_unique_symbols)} symbols only defined in {b.path}:")
            for s in sorted(b_unique_symbols):
                print(f"{s} defined in {b.functions[s].location}")

    def loc(s):
        return f"{s.location.file}:{s.location.line}"

    if args.structs:

        for mapping in config["equivalent_structs"]:
            (a_name, b_name), = mapping.items()
            a_name = a_name.removeprefix("struct ")
            b_name = b_name.removeprefix("struct ")
            if a_name not in a.structs or b_name not in b.structs:
                continue
            if c.is_ignored("structs", [], a_name) or c.is_ignored("structs", [], b_name):
                continue
            if (a_name in a.typedefs and c.is_ignored("typedefs", [], a_name)) or (c.is_ignored("typedefs", [], b_name)):
                continue
            b.structs[a_name] = b.structs[b_name]

        common_structs = sorted(set(a.structs) & set(b.structs))
        lines = []

        for name in common_structs:
            if c.is_ignored("typedefs", [], name):
                continue;

            sa = a.structs[name]
            sb = b.structs[name]

            if sa.alignment != sb.alignment:
                lines.append(
                    f"\t{name}: alignment {sa.alignment} vs. {sb.alignment} ({loc(sa)}, {loc(sb)})"
                )
                errors_emitted += 1

            if sa.size != sb.size:
                lines.append(
                    f"\t{name}: size {sa.size} vs. {sb.size} ({loc(sa)}, {loc(sb)})"
                )
                errors_emitted += 1

        if lines:
            print()
            print(
                f"checking {len(common_structs)} structs for size/alignment mismatches:"
            )
            for line in lines:
                print(line)

    if args.typedefs:
        common_typedefs = sorted(set(a.typedefs) & set(b.typedefs))
        lines = []

        for name in common_typedefs:
            if (name in a.structs or name in b.structs) and c.is_ignored("structs", [], name):
                continue;

            ta = a.typedefs[name]
            tb = b.typedefs[name]

            if ta.alignment != tb.alignment and ta.alignment > 0 and tb.alignment > 0:
                lines.append(
                    f"\t{name}: alignment {ta.alignment} vs. {tb.alignment} ({loc(ta)}, {loc(tb)})"
                )
                errors_emitted += 1

            if ta.size != tb.size and ta.size > 0 and tb.size > 0:
                lines.append(
                    f"\t{name}: size {ta.size} vs. {tb.size} ({loc(ta)}, {loc(tb)})"
                )
                errors_emitted += 1

        if lines:
            print()
            print(
                f"checking {len(common_typedefs)} typedefs for size/alignment mismatches:"
            )
            for line in lines:
                print(line)

    if args.macro_definitions:
        tempdir = tempfile.TemporaryDirectory(prefix="abichecker")
        td = pathlib.Path(tempdir.name)
        script_path = pathlib.Path(__file__).resolve().parent

        atp = open(td / "test-a-primary.hpp", "w")
        btp = open(td / "test-b-primary.hpp", "w")

        print(f'#include "{script_path}/linux-headers.h"', file=atp)
        print(f'#include "{script_path}/linux-headers.h"', file=btp)

        def filter_preprocessed_file(input, output):
            include_next_line = False

            with open(output, "w") as o:
                with open(input, "r") as i:
                    for line in i:
                        if line.startswith("const auto __v_"):
                            o.write(line)
                            include_next_line = not line.strip().endswith(";")
                        elif include_next_line:
                            if not line.strip().startswith("#"):
                                o.write(line)
                                include_next_line = not line.strip().endswith(";")

        a_included_files = list()
        b_included_files = list()
        tested_macros = list()

        def is_macro_literal(obj):
            if type(obj) is not MacroDefinition:
                return False
            return obj.first_token and obj.first_token.kind == TokenKind.LITERAL

        def is_enum(obj):
            return type(obj) is EnumDecl

        for name, bm in (b.macros | b.enums).items():
            if name in (a.macros | a.enums):
                am = (a.macros | a.enums)[name]
                header = (
                    str(am.location.file)
                    .removeprefix(str(args.reference))
                    .removeprefix("/")
                )
                if header not in a_included_files and not c.is_skipped_file(
                    args.reference, args.reference / header, config
                ):
                    print(f"#include <{header}>", file=atp)
                    a_included_files.append(header)
            header = (
                str(bm.location.file).removeprefix(str(args.mlibc)).removeprefix("/")
            )
            if header not in b_included_files:
                print(f"#include <{header}>", file=btp)
                b_included_files.append(header)
            if name in (a.macros | a.enums) and (
                is_macro_literal(bm)
                or (is_enum(bm) and not c.is_ignored("enum_constants", [], name))
            ):
                print(f"const auto __v_{name} = {name};", file=atp)
                print(f"const auto __v_{name} = {name};", file=btp)
                tested_macros.append(name)

        atp.close()
        btp.close()

        a_preprocess = subprocess.run(
            cxx_name()
            + [
                "-E",
                "-std=c++23",
                "-nostdlib",
                f"-I{args.reference}",
                "-o",
                f"{tempdir.name}/test-a-preprocessed.hpp",
                f"{tempdir.name}/test-a-primary.hpp",
                "-D_GNU_SOURCE",
                "-D_FILE_OFFSET_BITS=64",
                "-D_REGEX_LARGE_OFFSETS"
                "-Wno-macro-redefined",
            ],
            capture_output=True,
        )
        if a_preprocess.returncode != 0:
            print(f"Preprocessing the macro list of {args.reference} failed:")
            print(f"\tCommand: '{' '.join(a_preprocess.args)}'")
            print(a_preprocess.stderr.decode("utf-8"))
        b_preprocess = subprocess.run(
            cxx_name()
            + [
                "-E",
                "-std=c++23",
                "-nostdlib",
                f"-I{args.mlibc}",
                "-o",
                f"{tempdir.name}/test-b-preprocessed.hpp",
                f"{tempdir.name}/test-b-primary.hpp",
                "-D_GNU_SOURCE",
                "-D_FILE_OFFSET_BITS=64",
                "-D_REGEX_LARGE_OFFSETS"
                "-Wno-macro-redefined",
            ],
            capture_output=True,
        )
        if b_preprocess.returncode != 0:
            print(f"Preprocessing the macro list of {args.mlibc} failed:")
            print(b_preprocess.stderr.decode("utf-8"))

        filter_preprocessed_file(
            td / "test-a-preprocessed.hpp", td / "test-a-filtered.hpp"
        )
        filter_preprocessed_file(
            td / "test-b-preprocessed.hpp", td / "test-b-filtered.hpp"
        )

        at = open(td / "test-a.cpp", "w")
        bt = open(td / "test-b.cpp", "w")

        print(f'#include "{script_path}/linux-headers.h"', file=at)
        print(f'#include "{script_path}/linux-headers.h"', file=bt)

        for inc in a_included_files:
            print(f"#include <{inc}>", file=at)
        for inc in b_included_files:
            print(f"#include <{inc}>", file=bt)
        print("", file=at)
        print("", file=bt)

        print(f'#include "{tempdir.name}/test-a-filtered.hpp"', file=at)
        print(f'#include "{tempdir.name}/test-b-filtered.hpp"', file=bt)
        print(f'#include "{script_path}/to_integral.hpp"', file=at)
        print(f'#include "{script_path}/to_integral.hpp"', file=bt)

        print("int main() {", file=at)
        print("int main() {", file=bt)

        for name in tested_macros:
            print(f'\tmacro_print("{name}", __v_{name});', file=at)
            print(f'\tmacro_print("{name}", __v_{name});', file=bt)

        print("\treturn 0;", file=at)
        print("\treturn 0;", file=bt)
        print("}", file=at)
        print("}", file=bt)

        at.close()
        bt.close()

        a_compile = subprocess.run(
            cxx_name()
            + [
                "-std=c++23",
                "-I",
                f"{args.reference}",
                "-o",
                f"{tempdir.name}/test-a",
                f"{tempdir.name}/test-a.cpp",
                "-D_GNU_SOURCE",
                "-D_FILE_OFFSET_BITS=64",
                "-D_REGEX_LARGE_OFFSETS"
                "-Wno-macro-redefined",
            ],
            capture_output=True,
        )
        if a_compile.returncode != 0:
            log_err("Compiling macro test failed", f"test.cpp for {args.reference}")
            print(a_compile.stderr.decode("utf-8"))
            sys.exit(1)

        b_compile = subprocess.run(
            cxx_name()
            + [
                "-std=c++23",
                "-I",
                f"{args.mlibc}",
                "-o",
                f"{tempdir.name}/test-b",
                f"{tempdir.name}/test-b.cpp",
                "-D_GNU_SOURCE",
                "-D_FILE_OFFSET_BITS=64",
                "-D_REGEX_LARGE_OFFSETS"
                "-Wno-macro-redefined",
            ],
            capture_output=True,
        )
        if b_compile.returncode != 0:
            log_err("Compiling macro test failed", f"test.cpp for {args.mlibc}")
            print(b_compile.stderr.decode("utf-8"))
            sys.exit(1)

        test_a_file = tempfile.NamedTemporaryFile(dir=tempdir.name)
        test_b_file = tempfile.NamedTemporaryFile(dir=tempdir.name)
        qemu_cmd = []

        if args.arch != "x86_64":
            qemu_cmd = [f"qemu-{args.arch}"]
            if args.ld_lib:
                qemu_cmd += ["-L", args.ld_lib]

        test_a = subprocess.run(
            qemu_cmd + [f"{tempdir.name}/test-a"], stdout=test_a_file
        )
        if test_a.returncode != 0:
            log_err("Running macro test failed", f"test for {args.reference}")
        test_b = subprocess.run(
            qemu_cmd + [f"{tempdir.name}/test-b"], stdout=test_b_file
        )
        if test_b.returncode != 0:
            log_err("Running macro test failed", f"test for {args.mlibc}")

        color_output = ["--color=always"] if not on_ci() else []

        diff = subprocess.run(
            ["diff", test_a_file.name, test_b_file.name] + color_output,
            capture_output=True,
            text=True,
        )
        diff_str = diff.stdout.strip()
        if diff_str:
            print()
            print("diff of macro definitions:")
            print(diff_str)
            ansi_escape = re.compile(r"\x1B\[[0-?]*[ -/]*[@-~]")
            errors_emitted += sum(
                1
                for line in diff_str.splitlines()
                if ansi_escape.sub("", line).startswith("< ")
            )


if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument(
        "-m",
        dest="missing_functions",
        action="store_true",
        help="search for missing functions",
    )
    argparser.add_argument(
        "-M",
        dest="macro_definitions",
        action="store_true",
        help="compare macro definitions",
    )
    argparser.add_argument(
        "-f",
        dest="function_signatures",
        action="store_true",
        help="check function signatures",
    )
    argparser.add_argument(
        "-s", dest="structs", action="store_true", help="check structs"
    )
    argparser.add_argument(
        "-t", dest="typedefs", action="store_true", help="check structs"
    )
    argparser.add_argument(
        "-v", "--verbose", dest="verbose", action="store_true", help="verbose output"
    )
    argparser.add_argument(
        "-T",
        dest="dump_tree",
        action="store_true",
        help="dump tree (for debug, extremely verbose)",
    )
    argparser.add_argument(
        "--config",
        help="path to the configuration file",
        dest="config",
        type=argparse.FileType("r"),
        required=True,
    )
    argparser.add_argument(
        "--arch", help="target architecture", dest="arch", type=str, default="x86_64"
    )
    argparser.add_argument(
        "--ld-library-path",
        help="additional LD_LIBRARY_PATH to supply to qemu-user",
        dest="ld_lib",
        type=str,
    )
    argparser.add_argument(
        "--clang-version",
        help="specify which versioned clang to use",
        dest="clang_version",
        type=int,
    )
    argparser.add_argument(
        "--exit-with-zero-for-abi-mismatches",
        help="exit with zero even if ABI mismatches are detected",
        dest="exit_zero",
        action="store_true",
    )
    argparser.add_argument(
        "reference", help="path to the references libc's sysroot", type=pathlib.Path
    )
    argparser.add_argument(
        "mlibc", help="mlibc headers to be checked", type=pathlib.Path
    )
    argparser.add_argument("file", nargs="?", help="limit scope to this file")

    colorama.just_fix_windows_console()

    args = argparser.parse_args()

    config = yaml.load(args.config, yaml.CSafeLoader)
    reference_state = State(args.reference)
    mlibc_state = State(args.mlibc)

    # determine the path to clang's resource dir (like /usr/lib/clang/20/include)
    resource_dir_result = subprocess.run(
        cc_name() + ["-print-resource-dir"], capture_output=True
    )
    resource_dir = pathlib.Path(resource_dir_result.stdout.decode().strip()) / "include"

    for pair in ((args.reference, reference_state), (args.mlibc, mlibc_state)):
        (path, state) = pair
        if not args.file:
            for header in sorted(path.rglob("*.h")):
                parse(
                    pathlib.Path(str(header).removeprefix(str(path)).removeprefix("/")),
                    resource_dir,
                    path,
                    state,
                )
        else:
            parse(pathlib.Path(args.file), resource_dir, path, state)

    compare_states(reference_state, mlibc_state)

    if errors_emitted > 0:
        print(f"\n{errors_emitted} errors emitted.")
    else:
        print("No ABI differences found.")

    if args.exit_zero:
        exit(0)

    exit(min(errors_emitted, 0xFF))
