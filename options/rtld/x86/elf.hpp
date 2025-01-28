#pragma once

#include <elf.h>

#define ELF_CLASS ELFCLASS32
#define ELF_MACHINE EM_386

using elf_ehdr = Elf32_Ehdr;
using elf_phdr = Elf32_Phdr;
using elf_dyn = Elf32_Dyn;
using elf_rel = Elf32_Rel;
using elf_rela = Elf32_Rela;
using elf_relr = Elf32_Relr;
using elf_sym = Elf32_Sym;
using elf_addr = Elf32_Addr;

using elf_info = Elf32_Word;
using elf_addend = Elf32_Sword;

using elf_version = Elf32_Half;
using elf_verdef = Elf32_Verdef;
using elf_verdaux = Elf32_Verdaux;
using elf_verneed = Elf32_Verneed;
using elf_vernaux = Elf32_Vernaux;

#define ELF_R_SYM ELF32_R_SYM
#define ELF_R_TYPE ELF32_R_TYPE
#define ELF_ST_BIND ELF32_ST_BIND

#define R_NONE R_386_NONE
#define R_JUMP_SLOT R_386_JMP_SLOT
#define R_ABSOLUTE R_386_32
#define R_GLOB_DAT R_386_GLOB_DAT
#define R_RELATIVE R_386_RELATIVE
#define R_IRELATIVE R_386_IRELATIVE
#define R_OFFSET R_386_PC32
#define R_COPY R_386_COPY
#define R_TLS_DTPMOD R_386_TLS_DTPMOD32
#define R_TLS_DTPREL R_386_TLS_DTPOFF32
#define R_TLS_TPREL R_386_TLS_TPOFF
#define R_TLSDESC R_386_TLS_DESC

#define TP_TCB_OFFSET 0
