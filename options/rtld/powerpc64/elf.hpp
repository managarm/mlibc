#pragma once

#include <elf.h>

#define ELF_CLASS ELFCLASS64
#define ELF_MACHINE EM_PPC64

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

#define R_NONE R_PPC64_NONE
#define R_JUMP_SLOT R_PPC64_JMP_SLOT
#define R_ABSOLUTE R_PPC64_ADDR64
#define R_GLOB_DAT R_PPC64_GLOB_DAT
#define R_RELATIVE R_PPC64_RELATIVE
#define R_IRELATIVE R_PPC64_IRELATIVE
// #define R_OFFSET
#define R_COPY R_PPC64_COPY
#define R_TLS_DTPMOD R_PPC64_DTPMOD64
#define R_TLS_DTPREL R_PPC64_DTPREL64
#define R_TLS_TPREL R_PPC64_TPREL64
#define R_TLSDESC R_PPC64_TLS
