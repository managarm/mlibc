#!/bin/env python3

# HOW THIS WORKS
#
# This script takes mlibc header files and generates bindings to be used with rust's "libc" crate.
# A configuration file is needed for its proper function; an example is provided alongside this
# script. Please do note that it is used for managarm, which lives under `unix/linux_like` in the
# "libc" crate. If your OS does not live under this directory, but e.g. under just `unix` instead,
# you will need to adapt the configuration to fit your use.
#
# HOW TO USE
#
# > python rust-libc <path/to/your/installed/mlibc/headers> [<single-header.h>]
#
# By default, the script parses all header files in the directory supplied, except for when a
# single header is provided, where it will only parse that.

import argparse
import io
import os
import pathlib
import string
import sys

import clang.cindex
import colorama
import yaml
from clang.cindex import Cursor, CursorKind, TokenKind, TypeKind
from dataclasses import dataclass

dry_run = True
errors_emitted = 0


def log_err(prefix, msg):
    global errors_emitted

    print(
        f"{colorama.Fore.RED}{prefix}{colorama.Style.RESET_ALL}: {msg}", file=sys.stderr
    )
    errors_emitted += 1


def emit(msg):
    if not dry_run:
        print(msg)


def no_system_includes(cursor, level):
    """filter out verbose stuff from system include files"""
    return (level != 1) or (
        cursor.location.file is not None
        and not cursor.location.file.name.startswith("/usr/include")
    )


class Type:
    def __init__(
        self,
        c: clang.cindex.Cursor,
        t: clang.cindex.Type = None,
        convert_arrays_to_ptrs=False,
    ):
        self.cursor = c
        self.type = t if t else c.type
        self.convert_arrays_to_ptrs = convert_arrays_to_ptrs

    @property
    def kind(self):
        return self.type.kind

    def convert_ptr_type(self, c, ty, is_pointee=False):
        pointee = ty if is_pointee else ty.get_pointee()

        if pointee.kind == TypeKind.FUNCTIONPROTO:
            arg_list = []
            for f in pointee.argument_types():
                arg_list.append(f"{Type(c, f)}")
            args = ", ".join(arg_list)
            ret_type = Type(c, pointee.get_result())
            if c.semantic_parent.spelling in config["force_raw_function_pointer"]:
                return f'extern "C" fn({args})' + (
                    f" -> {ret_type}" if str(ret_type) != "c_void" else ""
                )
            else:
                return f'Option<unsafe extern "C" fn({args})' + (
                    f" -> {ret_type}>" if str(ret_type) != "c_void" else ">"
                )

        is_mut = not pointee.spelling.startswith("const")
        prefix = "*" + ("mut" if is_mut else "const") + " "

        type_iter = pointee

        while type_iter.kind == TypeKind.POINTER:
            prefix += "*mut "
            type_iter = type_iter.get_pointee()

        t = type_iter.spelling.removeprefix("const ")
        tokens = t.split(" ")

        match tokens:
            case ["char", *_]:
                return prefix + "c_char"
            case ["struct", x, *_] if x in config["force_local_type"]:
                return prefix + x
            case ["struct", x, *_]:
                return f"{prefix} crate::{x}"
            case ["int", *_]:
                return prefix + "c_int"
            case ["unsigned", "char", *_]:
                return prefix + "c_uchar"
            case ["unsigned", "short", *_]:
                return prefix + "c_ushort"
            case ["unsigned", "int", *_]:
                return prefix + "c_uint"
            case ["unsigned", "long", *_]:
                return prefix + "c_ulong"
            case ["unsigned", *_]:
                log_err("unhandled unsigned type", f"'{t}'")
            case ["void", *_]:
                return prefix + "c_void"
            case ["double", *_]:
                return prefix + "c_double"
            case [*_]:
                return prefix + "crate::" + t

    def __str__(self):
        typename = str(self.kind)
        match self.kind:
            case TypeKind.VOID:
                typename = "c_void"
            case TypeKind.LONG:
                typename = "c_long"
            case TypeKind.LONGLONG:
                typename = "c_longlong"
            case TypeKind.UINT:
                typename = "c_uint"
            case TypeKind.INT:
                typename = "c_int"
            case TypeKind.ULONG:
                typename = "c_ulong"
            case TypeKind.ULONGLONG:
                typename = "c_ulonglong"
            case TypeKind.USHORT:
                typename = "c_ushort"
            case TypeKind.SHORT:
                typename = "c_short"
            case TypeKind.CHAR_S:
                typename = "c_char"
            case TypeKind.UCHAR:
                typename = "c_uchar"
            case TypeKind.DOUBLE:
                typename = "c_double"
            case TypeKind.LONGDOUBLE:
                typename = "c_longdouble"
            case TypeKind.FLOAT:
                typename = "c_float"
            case TypeKind.CONSTANTARRAY:
                if self.convert_arrays_to_ptrs:
                    typename = self.convert_ptr_type(
                        self.cursor, self.type.get_array_element_type(), is_pointee=True
                    )
                else:
                    typename = f"[{str(Type(self.cursor, self.type.get_array_element_type()))}; {self.type.element_count}]"
            case TypeKind.INCOMPLETEARRAY:
                typename = "*mut " + str(
                    Type(self.cursor, self.type.get_array_element_type())
                )
            case TypeKind.ELABORATED:
                if self.is_va_list():
                    typename = "*mut c_char"
                elif self.cursor.is_anonymous():
                    typename = "crate::" + Type.cursor_name(self.cursor)
                elif self.type.get_declaration().displayname in (
                    "uint8_t",
                    "__mlibc_uint8",
                ):
                    typename = "u8"
                elif self.type.get_declaration().displayname in (
                    "int8_t",
                    "__mlibc_int8",
                ):
                    typename = "i8"
                elif self.type.get_declaration().displayname in (
                    "uint16_t",
                    "__mlibc_uint16",
                ):
                    typename = "u16"
                elif self.type.get_declaration().displayname in (
                    "int16_t",
                    "__mlibc_int16",
                ):
                    typename = "i16"
                elif self.type.get_declaration().displayname in (
                    "uint32_t",
                    "__mlibc_uint32",
                ):
                    typename = "u32"
                elif self.type.get_declaration().displayname in (
                    "int32_t",
                    "__mlibc_int32",
                ):
                    typename = "i32"
                elif self.type.get_declaration().displayname in (
                    "uint64_t",
                    "__mlibc_uint64",
                ):
                    typename = "u64"
                elif self.type.get_declaration().displayname in (
                    "int64_t",
                    "__mlibc_int64",
                ):
                    typename = "i64"
                elif self.type.get_declaration().displayname in (
                    "intptr_t",
                    "__mlibc_intptr",
                ):
                    typename = "isize"
                elif self.type.get_declaration().displayname in ("__mlibc_size"):
                    typename = "usize"
                else:
                    typename = "crate::" + str(self.type.get_declaration().displayname)
            case TypeKind.POINTER:
                typename = self.convert_ptr_type(self.cursor, self.type)
            case TypeKind.TYPEDEF:
                return str(self.type.spelling)
            case TypeKind.RECORD:
                return ""
        return typename

    @property
    def canonical(self):
        return str(Type(self.type.get_canonical()))

    def is_valid(self):
        return self.kind != TypeKind.INVALID

    def is_va_list(self):
        return (
            self.kind == TypeKind.ELABORATED
            and self.type.get_declaration().displayname == "__builtin_va_list"
        )

    def escape_name(name: str):
        if name in ("type", "in"):
            return f"r#{name}"
        return name

    def cursor_name(c: Cursor):
        d = c.type.get_declaration()
        if d and d.is_anonymous():
            return (
                f"anon_{pathlib.Path(str(d.location.file)).stem}_line{d.location.line}"
            )
        return Type.escape_name(c.displayname)


@dataclass
class State:
    functions = []
    macros = []
    types = []
    structs = []
    variables = []


@dataclass
class RustBindingGenerator:
    config: dict
    in_function_block = False
    in_struct_block = False
    in_union_block = False

    def handle_macro(self, cursor, gen, state):
        def is_num(s):
            if s.removeprefix("0o").isnumeric():
                return True
            if set(s.removeprefix("0x")).issubset(string.hexdigits):
                return True
            return False

        done = False
        is_negative = False

        assert len(gen) >= 1
        assert gen[0].kind == TokenKind.IDENTIFIER
        gen.pop(0)

        if len(gen) >= 1:
            tokens = []
            c_type = "c_int"
            is_unsigned = False
            i = 0
            while not done and gen and i < len(gen):
                c_type = "int"
                if gen[i].kind == TokenKind.PUNCTUATION and gen[i].spelling in (
                    "(",
                    ")",
                ):
                    if not (i == 0 or i == (len(gen) - 1)):
                        tokens.append(gen[i].spelling)
                    i += 1
                elif gen[i].kind == TokenKind.PUNCTUATION and gen[i].spelling == "-":
                    is_unsigned = False
                    i += 1
                elif gen[i].kind in (
                    TokenKind.LITERAL,
                    TokenKind.IDENTIFIER,
                    TokenKind.PUNCTUATION,
                ):
                    spelling = gen[i].spelling
                    if spelling.endswith("ULL") and is_num(spelling[:-3]):
                        spelling = spelling.removesuffix("ULL")
                        c_type = "longlong"
                        is_unsigned = True
                    if spelling.endswith("LL") and is_num(spelling[:-2]):
                        spelling = spelling.removesuffix("LL")
                        c_type = "longlong"
                        is_unsigned = False
                    if spelling.endswith("UL") and is_num(spelling[:-2]):
                        spelling = spelling.removesuffix("UL")
                        c_type = "long"
                        is_unsigned = True
                    elif spelling.endswith("L") and is_num(spelling[:-1]):
                        spelling = spelling.removesuffix("L")
                        c_type = "long"
                        is_unsigned = False
                    elif spelling.endswith("U") and is_num(spelling[:-1]):
                        spelling = spelling.removesuffix("U")
                        is_unsigned = True

                    if (
                        is_num(spelling)
                        and spelling.startswith("0")
                        and not spelling.startswith("0x")
                        and spelling != "0"
                    ):
                        spelling = f"0o{spelling[1:]}"

                    tokens.append(spelling)
                    i += 1
                else:
                    log_err(
                        f"unexpected token in macro '{cursor.displayname}'",
                        f"{gen[i].kind} {gen[i].spelling} at {gen[0].location}, skipping macro",
                    )
                    done = True
                c_type = "c_" + ("u" if is_unsigned else "") + c_type
            if not self.is_ignored("macros", state.macros, cursor.displayname):
                for name in config["force_macro_type"]:
                    if cursor.displayname in config["force_macro_type"][name]:
                        c_type = name
                        break
                emit(
                    "pub const {}: {} = {}{};".format(
                        cursor.displayname,
                        c_type,
                        "-" if is_negative else "",
                        "".join(tokens),
                    )
                )
                state.macros.append(cursor.displayname)

    def indent(self, level=0):
        if self.in_function_block or self.in_struct_block or self.in_union_block:
            return "\t" * (level + 1)
        return ""

    def handle_field_decl(self, cursor, c, inline_defs):
        tc = Type(c)
        assert tc.is_valid()
        name = str(tc)
        if c.is_anonymous():
            name = Type.cursor_name(c)
        if Type.cursor_name(cursor) in config["force_struct_member_type"]:
            info = config["force_struct_member_type"][Type.cursor_name(cursor)]
            if list(filter(lambda x: x["name"] == c.displayname, info)):
                detail = next(filter(lambda x: x["name"] == c.displayname, info))
                if "type" in detail:
                    assert "replace" not in detail
                    name = (
                        detail["rename-to"] if "rename-to" in detail else c.displayname
                    )
                    emit(
                        self.indent(1)
                        + f"pub {Type.escape_name(name)}: {detail['type']},"
                    )
                    if inline_defs[-1].get_usr() == c.type.get_declaration().get_usr():
                        inline_defs.pop()
                    return
                elif "replace" in detail:
                    for member in detail["replace"]:
                        emit(
                            self.indent(1)
                            + "pub {}: {},".format(member["name"], member["type"])
                        )
                    return
                else:
                    log_err(
                        "invalid configuration",
                        f"missing info for override for struct '{c.displayname}'",
                    )
        emit(self.indent(1) + f"pub {Type.escape_name(c.displayname)}: {name},")

    def handle_data_structs(self, cursor, state, level=0):
        inline_defs = []

        children = [i for i in cursor.get_children()]

        if (
            not children
            and Type.cursor_name(cursor) not in config["forced_empty_structs"]
        ):
            return

        if self.in_struct_block and cursor.kind != CursorKind.STRUCT_DECL:
            emit("}")
            self.in_struct_block = False
        if self.in_union_block and cursor.kind != CursorKind.UNION_DECL:
            emit("}")
            self.in_union_block = False

        match cursor.kind:
            case CursorKind.STRUCT_DECL:
                if not self.in_struct_block:
                    emit("s! {")
                    self.in_struct_block = True
                packed = False
                for m in cursor.get_children():
                    if CursorKind.PACKED_ATTR == m.kind:
                        packed = True
                        break
                if packed:
                    emit(self.indent() + "#[repr(packed)]")
                emit(self.indent() + f"pub struct {Type.cursor_name(cursor)} {{")
                state.structs.append(Type.cursor_name(cursor))
            case CursorKind.UNION_DECL:
                if not self.in_union_block:
                    emit("s_no_extra_traits! {")
                    self.in_union_block = True
                emit("#[repr(C)]")
                emit(self.indent() + f"pub union {Type.cursor_name(cursor)} {{")
                state.structs.append(Type.cursor_name(cursor))
            case CursorKind.ENUM_DECL:
                if cursor.type.get_declaration().is_anonymous() and level == 1:
                    # ignore anonymous enums in the global scope
                    return
                emit(self.indent() + f"pub enum {Type.cursor_name(cursor)} {{")
                state.structs.append(Type.cursor_name(cursor))
            case _:
                log_err("unhandled data struct kind", f"{cursor.kind}")

        if Type.cursor_name(cursor) in config["force_struct_zero_fill"]:
            struct_size = cursor.type.get_size()
            emit("\t\t#[doc(hidden)]")
            emit(f"\t\tsize: [u8; {struct_size}],")
        else:
            for c in children:
                match c.kind:
                    case CursorKind.FIELD_DECL:
                        self.handle_field_decl(cursor, c, inline_defs)
                    case CursorKind.STRUCT_DECL | CursorKind.UNION_DECL:
                        inline_defs.append(c)
                    case CursorKind.ENUM_CONSTANT_DECL:
                        emit(f"{c.displayname} = {c.enum_value},")
                    case CursorKind.PACKED_ATTR:
                        pass
                    case _:
                        log_err(f"unhandled {cursor.kind} member", f"kind {c.kind}")
        emit(self.indent() + "}")

        if cursor.kind == CursorKind.ENUM_DECL:
            emit(f"impl Copy for {Type.cursor_name(cursor)} " + "{}")
            emit(f"impl Clone for {Type.cursor_name(cursor)} " + "{")
            emit(f"\tfn clone(&self) -> {Type.cursor_name(cursor)} {{")
            emit("\t\t*self")
            emit("\t}")
            emit("}")

        for s in inline_defs:
            self.handle_data_structs(s, state, level + 1)

    def is_ignored(self, typename, ignorelist, name):
        if typename == "macros" and name.startswith("_") and name.endswith("_H"):
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

    def is_ignored_file(base_dir: pathlib.Path, file: pathlib.Path, config):
        if not str(file).startswith(str(base_dir)):
            return True

        for p in config["ignored_files"]:
            stripped_file = str(file).removeprefix(str(base_dir)).removeprefix("/")
            if stripped_file == p:
                return True
        for p in config["includes"]:
            if str(base_dir).startswith(p):
                return True
        return False

    def from_cursor(self, base_dir, header, cursor, filter_pred, level=0):
        global state

        if cursor.location.file:
            f = pathlib.Path(str(cursor.location.file))

            if RustBindingGenerator.is_ignored_file(base_dir, f, config):
                return

        if filter_pred(cursor, level):
            t = Type(cursor)

            if self.in_struct_block and cursor.kind != CursorKind.STRUCT_DECL:
                emit("}")
                self.in_struct_block = False
            if self.in_union_block and cursor.kind != CursorKind.STRUCT_DECL:
                emit("}")
                self.in_union_block = False
            if self.in_function_block and cursor.kind != CursorKind.FUNCTION_DECL:
                emit("}")
                self.in_function_block = False

            match cursor.kind:
                case CursorKind.MACRO_DEFINITION:
                    if not self.is_ignored("macros", [], cursor.displayname):
                        gen = [token for token in cursor.get_tokens()]
                        self.handle_macro(cursor, gen, state)
                case CursorKind.STRUCT_DECL:
                    if self.is_ignored("structs", state.structs, cursor.displayname):
                        return

                    self.handle_data_structs(cursor, state, level)
                case CursorKind.UNION_DECL:
                    if self.is_ignored("unions", state.structs, cursor.displayname):
                        return

                    self.handle_data_structs(cursor, state, level)
                case CursorKind.ENUM_DECL:
                    if self.is_ignored("enums", state.structs, cursor.displayname):
                        return

                    self.handle_data_structs(cursor, state, level)
                case CursorKind.TYPEDEF_DECL:
                    if not self.is_ignored("types", state.types, cursor.displayname):
                        underlying = Type(cursor, cursor.underlying_typedef_type)
                        self.from_cursor(
                            base_dir,
                            header,
                            cursor.underlying_typedef_type.get_declaration(),
                            filter_pred,
                            level,
                        )
                        if cursor.displayname not in state.structs:
                            emit(f"pub type {cursor.displayname} = {underlying};")
                        state.types.append(cursor.displayname)
                case CursorKind.FUNCTION_DECL:
                    if self.is_ignored("functions", state.functions, cursor.spelling):
                        return

                    args = []
                    for c in cursor.get_arguments():
                        arg_name = c.displayname if c.displayname else f"arg{len(args)}"
                        tc = Type(c, convert_arrays_to_ptrs=True)
                        if tc.is_va_list():
                            arg_name = c.displayname if c.displayname else "arg_list"
                        assert tc.is_valid()
                        if str(tc):
                            args.append(f"{Type.escape_name(arg_name)}: {str(tc)}")
                    if cursor.type.is_function_variadic():
                        args.append("...")
                    arg_str = ", ".join(args)
                    ret_type = str(Type(cursor, cursor.type.get_result()))
                    if not self.in_function_block:
                        emit('extern "C" {')
                        self.in_function_block = True
                    emit(
                        f"\tpub fn {cursor.spelling}({arg_str})"
                        + (f" -> {ret_type};" if ret_type != "c_void" else ";")
                    )
                    state.functions.append(cursor.spelling)
                case CursorKind.TRANSLATION_UNIT:
                    for c in cursor.get_children():
                        self.from_cursor(base_dir, header, c, filter_pred, level + 1)
                case CursorKind.INCLUSION_DIRECTIVE:
                    pass
                case CursorKind.VAR_DECL:
                    if self.is_ignored(
                        "var_declarations", state.variables, cursor.spelling
                    ):
                        return
                    else:
                        log_err(
                            "munhandled cursor type",
                            f"VAR_DECL of '{cursor.spelling}'",
                        )
                case CursorKind.STATIC_ASSERT | CursorKind.UNEXPOSED_DECL:
                    pass
                case CursorKind.MACRO_INSTANTIATION:
                    # TODO: cross-reference this with constant arrays?
                    pass
                case _:
                    log_err(
                        "unhandled cursor type",
                        f"{cursor.kind} {cursor.spelling} {cursor.displayname} {cursor.location}",
                    )

                    if t.is_valid():
                        emit(f"type '{t}' canonical '{t.canonical}'")

        if level == 0 and self.in_struct_block:
            emit("}")
            self.in_struct_block = False

        if level == 0 and self.in_union_block:
            emit("}")
            self.in_union_block = False

        if level == 0 and self.in_function_block:
            emit("}")
            self.in_function_block = False


def parse(file: pathlib.Path, base_dir: pathlib.Path):
    index = clang.cindex.Index.create()
    tu = None

    try:
        tu = index.parse(
            base_dir / file,
            args=[f"-I{p}" for p in config["includes"]] + ["-I" + str(base_dir), "-D_GNU_SOURCE"],
            options=clang.cindex.TranslationUnit.PARSE_DETAILED_PROCESSING_RECORD
            | clang.cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES,
        )
    except:
        log_err("parsing error", file)
        return

    assert tu

    if not RustBindingGenerator.is_ignored_file(base_dir, base_dir / file, config):
        if tu.diagnostics:
            [log_err("compile error", d) for d in tu.diagnostics]
            print(f"\n{errors_emitted + 1} errors emitted")
            exit(errors_emitted + 1)

        parser = RustBindingGenerator(config)
        emit("")
        print(f"// {tu.spelling.removeprefix(str(base_dir)).removeprefix('/')}")
        parser.from_cursor(base_dir, file, tu.cursor, no_system_includes)


if __name__ == "__main__":
    argparser = argparse.ArgumentParser()
    argparser.add_argument("-n", dest="dry_run", action="store_true")
    argparser.add_argument("path")
    argparser.add_argument("file", nargs="?")

    args = argparser.parse_args()

    dry_run = args.dry_run

    colorama.just_fix_windows_console()

    with io.open(os.path.join(os.path.dirname(__file__), "rust-libc-config.yml"), "r") as f:
        config = yaml.load(f, yaml.CSafeLoader)

    path = pathlib.Path(args.path)

    with io.open(os.path.join(os.path.dirname(__file__), "rust-libc-header.rs"), "r") as f:
        emit(f.read())

    state = State()

    if not args.file:
        for header in sorted(path.rglob("*.h")):
            parse(str(header).removeprefix(str(path)).removeprefix("/"), path)
    else:
        parse(pathlib.Path(args.file), path)

    if errors_emitted > 0:
        print(f"\n{errors_emitted} errors emitted")

    exit(errors_emitted)
