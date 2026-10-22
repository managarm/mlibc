#pragma once

#include <elf.h>

#define ELF_CLASS ELFCLASS64
#define ELF_MACHINE EM_MIPS

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
#define ELF_ST_TYPE ELF64_ST_TYPE

#define R_NONE R_MIPS_NONE
#define R_JUMP_SLOT R_MIPS_JUMP_SLOT
#define R_ABSOLUTE R_MIPS_64
#define R_GLOB_DAT R_MIPS_GLOB_DAT
#define R_COPY R_MIPS_COPY
#define R_RELATIVE R_MIPS_RELATIVE
#define R_TLS_DTPMOD R_MIPS_TLS_DTPMOD64
#define R_TLS_DTPREL R_MIPS_TLS_DTPREL64
#define R_TLS_TPREL R_MIPS_TLS_TPREL64
#define R_SYM_OR_REL ((R_MIPS_64 << 8) | R_MIPS_REL32)

#define TP_TCB_OFFSET 0x7000
