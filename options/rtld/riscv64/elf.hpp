#pragma once

#include <elf.h>

#define ELF_CLASS ELFCLASS64
#define ELF_MACHINE EM_RISCV

using elf_ehdr = Elf64_Ehdr;
using elf_phdr = Elf64_Phdr;
using elf_dyn = Elf64_Dyn;
using elf_rel = Elf64_Rel;
using elf_rela = Elf64_Rela;
using elf_relr = Elf64_Relr;
using elf_sym = Elf64_Sym;
using elf_addr = Elf64_Addr;

using elf_info = Elf64_Xword;
using elf_addend = Elf64_Sxword;

using elf_version = Elf64_Half;
using elf_verdef = Elf64_Verdef;
using elf_verdaux = Elf64_Verdaux;
using elf_verneed = Elf64_Verneed;
using elf_vernaux = Elf64_Vernaux;

#define ELF_R_SYM ELF64_R_SYM
#define ELF_R_TYPE ELF64_R_TYPE
#define ELF_ST_BIND ELF64_ST_BIND

#define R_NONE R_RISCV_NONE
#define R_JUMP_SLOT R_RISCV_JUMP_SLOT
#define R_ABSOLUTE R_RISCV_64
#define R_GLOB_DAT R_RISCV_64
#define R_RELATIVE R_RISCV_RELATIVE
#define R_IRELATIVE R_RISCV_IRELATIVE
// #define R_OFFSET
#define R_COPY R_RISCV_COPY
#define R_TLS_DTPMOD R_RISCV_TLS_DTPMOD64
#define R_TLS_DTPREL R_RISCV_TLS_DTPREL64
#define R_TLS_TPREL R_RISCV_TLS_TPREL64
#define R_TLSDESC R_RISCV_TLSDESC

#define TP_TCB_OFFSET 0
