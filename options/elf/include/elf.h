#ifndef _ELF_H
#define _ELF_H

#include <stdint.h>
#include <bits/inline-definition.h>
#include <abi-bits/auxv.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Convert the enums to #defines so that they work with #ifdef. */

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ELFOSABI_SYSV 0
#define EM_X86_64 62

#define SHF_WRITE 1
#define SHF_ALLOC 2
#define SHF_EXECINSTR 4
#define SHF_STRINGS 32
#define SHF_INFO_LINK 64
#define SHF_TLS 1024

#define NT_AUXV 6

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;
typedef uint16_t Elf64_Section;
typedef Elf64_Half Elf64_Versym;

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_Sword;
typedef uint64_t Elf32_Xword;
typedef int64_t Elf32_Sxword;
typedef uint16_t Elf32_Section;
typedef Elf32_Half Elf32_Versym;

#define EI_NIDENT (16)

typedef struct {
	unsigned char e_ident[EI_NIDENT]; /* ELF identification */
	Elf32_Half e_type; /* Object file type */
	Elf32_Half e_machine; /* Machine type */
	Elf32_Word e_version; /* Object file version */
	Elf32_Addr e_entry; /* Entry point address */
	Elf32_Off e_phoff; /* Program header offset */
	Elf32_Off e_shoff; /* Section header offset */
	Elf32_Word e_flags; /* Processor-specific flags */
	Elf32_Half e_ehsize; /* ELF header size */
	Elf32_Half e_phentsize; /* Size of program header entry */
	Elf32_Half e_phnum; /* Number of program header entries */
	Elf32_Half e_shentsize; /* Size of section header entry */
	Elf32_Half e_shnum; /* Number of section header entries */
	Elf32_Half e_shstrndx; /* Section name string table index */
} Elf32_Ehdr;

typedef struct {
	unsigned char e_ident[EI_NIDENT]; /* ELF identification */
	Elf64_Half e_type; /* Object file type */
	Elf64_Half e_machine; /* Machine type */
	Elf64_Word e_version; /* Object file version */
	Elf64_Addr e_entry; /* Entry point address */
	Elf64_Off e_phoff; /* Program header offset */
	Elf64_Off e_shoff; /* Section header offset */
	Elf64_Word e_flags; /* Processor-specific flags */
	Elf64_Half e_ehsize; /* ELF header size */
	Elf64_Half e_phentsize; /* Size of program header entry */
	Elf64_Half e_phnum; /* Number of program header entries */
	Elf64_Half e_shentsize; /* Size of section header entry */
	Elf64_Half e_shnum; /* Number of section header entries */
	Elf64_Half e_shstrndx; /* Section name string table index */
} Elf64_Ehdr;

typedef struct {
	Elf32_Half vd_version; /* Version revision */
	Elf32_Half vd_flags; /* Version information */
	Elf32_Half vd_ndx; /* Version Index */
	Elf32_Half vd_cnt; /* Number of associated aux entries */
	Elf32_Word vd_hash; /* Version name hash value */
	Elf32_Word vd_aux; /* Offset in bytes to verdaux array */
	Elf32_Word vd_next; /* Offset in bytes to next verdef entry */
} Elf32_Verdef;

typedef struct {
	Elf64_Half vd_version; /* Version revision */
	Elf64_Half vd_flags; /* Version information */
	Elf64_Half vd_ndx; /* Version Index */
	Elf64_Half vd_cnt; /* Number of associated aux entries */
	Elf64_Word vd_hash; /* Version name hash value */
	Elf64_Word vd_aux; /* Offset in bytes to verdaux array */
	Elf64_Word vd_next; /* Offset in bytes to next verdef entry */
} Elf64_Verdef;

typedef struct {
	Elf32_Word vda_name; /* Version or dependency names */
	Elf32_Word vda_next; /* Offset in bytes to next verdaux entry */
} Elf32_Verdaux;

typedef struct {
	Elf64_Word vda_name; /* Version or dependency names */
	Elf64_Word vda_next; /* Offset in bytes to next verdaux entry */
} Elf64_Verdaux;

typedef struct {
	Elf32_Half vn_version;
	Elf32_Half vn_cnt;
	Elf32_Word vn_file;
	Elf32_Word vn_aux;
	Elf32_Word vn_next;
} Elf32_Verneed;

typedef struct {
	Elf64_Half vn_version;
	Elf64_Half vn_cnt;
	Elf64_Word vn_file;
	Elf64_Word vn_aux;
	Elf64_Word vn_next;
} Elf64_Verneed;

typedef struct {
	Elf32_Word vna_hash;
	Elf32_Half vna_flags;
	Elf32_Half vna_other;
	Elf32_Word vna_name;
	Elf32_Word vna_next;
} Elf32_Vernaux;

typedef struct {
	Elf64_Word vna_hash;
	Elf64_Half vna_flags;
	Elf64_Half vna_other;
	Elf64_Word vna_name;
	Elf64_Word vna_next;
} Elf64_Vernaux;

typedef struct {
	Elf64_Xword m_value;
	Elf64_Xword m_info;
	Elf64_Xword m_poffset;
	Elf64_Half m_repeat;
	Elf64_Half m_stride;
} Elf64_Move;

typedef struct {
	Elf64_Word l_name;
	Elf64_Word l_time_stamp;
	Elf64_Word l_checksum;
	Elf64_Word l_version;
	Elf64_Word l_flags;
} Elf64_Lib;

enum {
	ET_NONE = 0,
	ET_REL = 1,
	ET_EXEC = 2,
	ET_DYN = 3,
	ET_CORE = 4
};

enum {
	SHN_UNDEF = 0,
	SHN_ABS = 0xFFF1
};

enum {
	STN_UNDEF = 0
};

typedef struct {
	Elf32_Word st_name;
	Elf32_Addr st_value;
	Elf32_Word st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Section st_shndx;
} Elf32_Sym;

typedef struct {
	Elf64_Word st_name;
	unsigned char st_info;
	unsigned char st_other;
	Elf64_Half st_shndx;
	Elf64_Addr st_value;
	Elf64_Xword st_size;
} Elf64_Sym;

__MLIBC_INLINE_DEFINITION unsigned char ELF64_ST_BIND(unsigned char info) {
	return info >> 4;
}
__MLIBC_INLINE_DEFINITION unsigned char ELF64_ST_TYPE(unsigned char info) {
	return info & 0x0F;
}
__MLIBC_INLINE_DEFINITION unsigned char ELF64_ST_INFO(unsigned char bind, unsigned char type) {
	return (bind << 4) | type;
}

typedef struct {
	Elf64_Half si_boundto;
	Elf64_Half si_flags;
} Elf64_Syminfo;

__MLIBC_INLINE_DEFINITION unsigned char ELF32_ST_BIND(unsigned char info) {
	return info >> 4;
}
__MLIBC_INLINE_DEFINITION unsigned char ELF32_ST_TYPE(unsigned char info) {
	return info & 0xF;
}
__MLIBC_INLINE_DEFINITION unsigned char ELF32_ST_INFO(unsigned char bind, unsigned char type) {
	return (bind << 4) | (type & 0xF);
}

enum {
	STB_GLOBAL = 1,
	STB_WEAK = 2,
	STB_GNU_UNIQUE = 10,
	STB_LOPROC = 13,
	STB_HIPROC = 15
};

enum {
	STT_OBJECT = 1,
	STT_FUNC = 2,
	STT_TLS = 6,
	STT_LOPROC = 13,
	STT_HIPROC = 15
};

enum {
	R_X86_64_NONE = 0,
	R_X86_64_64 = 1,
	R_X86_64_PC32 = 2,
	R_X86_64_PLT32 = 4,
	R_X86_64_COPY = 5,
	R_X86_64_GLOB_DAT = 6,
	R_X86_64_JUMP_SLOT = 7,
	R_X86_64_RELATIVE = 8,
	R_X86_64_GOTPCREL = 9,
	R_X86_64_32 = 10,
	R_X86_64_32S = 11,
	R_X86_64_PC16 = 13,
	R_X86_64_PC8 = 15,
	R_X86_64_DTPMOD64 = 16,
	R_X86_64_DTPOFF64 = 17,
	R_X86_64_TPOFF64 = 18,
	R_X86_64_PC64 = 24,
	R_X86_64_GOTPC32 = 26,
	R_X86_64_TLSDESC = 36,
	R_X86_64_IRELATIVE = 37
};

enum {
	R_386_NONE = 0,
	R_386_32 = 1,
	R_386_PC32 = 2,
	R_386_COPY = 5,
	R_386_GLOB_DAT = 6,
	R_386_JMP_SLOT = 7,
	R_386_RELATIVE = 8,
	R_386_TLS_TPOFF = 14,
	R_386_TLS_DTPMOD32 = 35,
	R_386_TLS_DTPOFF32 = 36,
	R_386_TLS_DESC = 41,
	R_386_IRELATIVE = 42
};

enum {
	R_AARCH64_NONE = 0,
	R_AARCH64_ABS64 = 257,
	R_AARCH64_COPY = 1024,
	R_AARCH64_GLOB_DAT = 1025,
	R_AARCH64_JUMP_SLOT = 1026,
	R_AARCH64_RELATIVE = 1027,
	R_AARCH64_TLS_DTPMOD64 = 1028,
	R_AARCH64_TLS_DTPREL64 = 1029,
	R_AARCH64_TLS_TPREL64 = 1030,
	R_AARCH64_TLSDESC = 1031,
	R_AARCH64_IRELATIVE = 1032
};

#define R_AARCH64_TLS_DTPREL R_AARCH64_TLS_DTPREL64
#define R_AARCH64_TLS_DTPMOD R_AARCH64_TLS_DTPMOD64
#define R_AARCH64_TLS_TPREL R_AARCH64_TLS_TPREL64

enum {
	R_RISCV_NONE = 0,
	R_RISCV_32 = 1,
	R_RISCV_64 = 2,
	R_RISCV_RELATIVE = 3,
	R_RISCV_COPY = 4,
	R_RISCV_JUMP_SLOT = 5,
	R_RISCV_TLS_DTPMOD32 = 6,
	R_RISCV_TLS_DTPMOD64 = 7,
	R_RISCV_TLS_DTPREL32 = 8,
	R_RISCV_TLS_DTPREL64 = 9,
	R_RISCV_TLS_TPREL32 = 10,
	R_RISCV_TLS_TPREL64 = 11,
	R_RISCV_TLSDESC = 12, /* currently a draft but looking good */
	R_RISCV_IRELATIVE = 58
};

enum {
	R_68K_NONE = 0,
	R_68K_32 = 1,
	R_68K_PC32 = 4,
	R_68K_COPY = 19,
	R_68K_GLOB_DAT = 20,
	R_68K_JMP_SLOT = 21,
	R_68K_RELATIVE = 22,

	R_68K_TLS_DTPMOD32 = 40,
	R_68K_TLS_DTPREL32= 41,
	R_68K_TLS_TPREL32= 42
};

enum {
	R_LARCH_NONE = 0,
	R_LARCH_32 = 1,
	R_LARCH_64 = 2,
	R_LARCH_RELATIVE = 3,
	R_LARCH_COPY = 4,
	R_LARCH_JUMP_SLOT = 5,
	R_LARCH_TLS_DTPMOD32 = 6,
	R_LARCH_TLS_DTPMOD64 = 7,
	R_LARCH_TLS_DTPREL32 = 8,
	R_LARCH_TLS_DTPREL64 = 9,
	R_LARCH_TLS_TPREL32 = 10,
	R_LARCH_TLS_TPREL64 = 11,
	R_LARCH_IRELATIVE = 12
};

typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
	Elf64_Addr r_offset;
	uint64_t   r_info;
} Elf64_Rel;

typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
	Elf32_Sword r_addend;
} Elf32_Rela;

typedef struct {
	Elf64_Addr r_offset;
	Elf64_Xword r_info;
	Elf64_Sxword r_addend;
} Elf64_Rela;

typedef Elf32_Word Elf32_Relr;
typedef Elf64_Xword Elf64_Relr;

__MLIBC_INLINE_DEFINITION Elf64_Xword ELF64_R_SYM(Elf64_Xword info) {
	return info >> 32;
}
__MLIBC_INLINE_DEFINITION Elf64_Xword ELF64_R_TYPE(Elf64_Xword info) {
	return info & 0xFFFFFFFF;
}
__MLIBC_INLINE_DEFINITION Elf64_Xword ELF64_R_INFO(Elf64_Xword sym, Elf64_Xword type) {
	return ((((Elf64_Xword)(sym)) << 32) + (type));
}

__MLIBC_INLINE_DEFINITION Elf32_Word ELF32_R_SYM(Elf32_Word info) {
	return info >> 8;
}
__MLIBC_INLINE_DEFINITION Elf32_Word ELF32_R_TYPE(Elf32_Word info) {
	return info & 0xFF;
}

enum {
	PT_NULL = 0,
	PT_LOAD = 1,
	PT_DYNAMIC = 2,
	PT_INTERP = 3,
	PT_NOTE = 4,
	PT_SHLIB = 5,
	PT_PHDR = 6,
	PT_TLS = 7,
	PT_NUM = 8,
	PT_LOOS = 0x60000000,
	PT_GNU_EH_FRAME = 0x6474E550,
	PT_GNU_STACK = 0x6474E551,
	PT_GNU_RELRO = 0x6474E552,
	PT_GNU_PROPERTY = 0x6474E553,
	PT_SUNWBSS = 0x6ffffffa,
	PT_SUNWSTACK = 0x6ffffffb,
	PT_HISUNW = 0x6fffffff,
	PT_HIOS = 0x6fffffff,
	PT_LOPROC = 0x70000000,
	PT_ARM_EXIDX = 0x70000001,
	PT_RISCV_ATTRIBUTES = 0x70000003,
	PT_HIPROC = 0x7fffffff
};

enum {
	PF_X = 1,
	PF_W = 2,
	PF_R = 4
};

typedef struct {
	Elf32_Word p_type; /* Type of segment */
	Elf32_Off p_offset; /* Offset in file */
	Elf32_Addr p_vaddr; /* Virtual address in memory */
	Elf32_Addr p_paddr; /* Reserved */
	Elf32_Word p_filesz; /* Size of segment in file */
	Elf32_Word p_memsz; /* Size of segment in memory */
	Elf32_Word p_flags; /* Segment attributes */
	Elf32_Word p_align; /* Alignment of segment */
} Elf32_Phdr;

typedef struct {
	Elf64_Word p_type; /* Type of segment */
	Elf64_Word p_flags; /* Segment attributes */
	Elf64_Off p_offset; /* Offset in file */
	Elf64_Addr p_vaddr; /* Virtual address in memory */
	Elf64_Addr p_paddr; /* Reserved */
	Elf64_Xword p_filesz; /* Size of segment in file */
	Elf64_Xword p_memsz; /* Size of segment in memory */
	Elf64_Xword p_align; /* Alignment of segment */
} Elf64_Phdr;

enum {
	DT_NULL = 0,
	DT_NEEDED = 1,
	DT_PLTRELSZ = 2,
	DT_PLTGOT = 3,
	DT_HASH = 4,
	DT_STRTAB = 5,
	DT_SYMTAB = 6,
	DT_RELA = 7,
	DT_RELASZ = 8,
	DT_RELAENT = 9,
	DT_STRSZ = 10,
	DT_SYMENT = 11,
	DT_INIT = 12,
	DT_FINI = 13,
	DT_SONAME = 14,
	DT_RPATH = 15,
	DT_SYMBOLIC = 16,
	DT_REL = 17,
	DT_RELSZ = 18,
	DT_RELENT = 19,
	DT_TEXTREL = 22,
	DT_BIND_NOW = 24,
	DT_INIT_ARRAY = 25,
	DT_FINI_ARRAY = 26,
	DT_INIT_ARRAYSZ = 27,
	DT_FINI_ARRAYSZ = 28,
	DT_RUNPATH = 29,
	DT_PLTREL = 20,
	DT_DEBUG = 21,
	DT_JMPREL = 23,
	DT_FLAGS = 30,
	DT_PREINIT_ARRAY = 32,
	DT_PREINIT_ARRAYSZ = 33,
	DT_RELRSZ = 35,
	DT_RELR = 36,
	DT_RELRENT = 37,
	DT_LOOS = 0x6000000d,
	DT_HIOS = 0x6ffff000,
	DT_GNU_HASH = 0x6ffffef5,
	DT_TLSDESC_PLT = 0x6ffffef6,
	DT_TLSDESC_GOT = 0x6ffffef7,
	DT_VERSYM = 0x6ffffff0,
	DT_RELACOUNT = 0x6ffffff9,
	DT_RELCOUNT = 0x6ffffffa,
	DT_FLAGS_1 = 0x6ffffffb,
	DT_VERDEF = 0x6ffffffc,
	DT_VERDEFNUM = 0x6ffffffd,
	DT_VERNEED = 0x6ffffffe,
	DT_VERNEEDNUM = 0x6fffffff
};

enum {
	/* For DT_FLAGS. */
	DF_SYMBOLIC = 0x02,
	DF_TEXTREL = 0x04,
	DF_BIND_NOW = 0x08,
	DF_STATIC_TLS = 0x10,

	/* For DT_FLAGS_1. */
	DF_1_NOW = 0x00000001,
	DF_1_NODELETE = 0x00000008,
	DF_1_PIE = 0x08000000
};

/* Valid values for note segment descriptor files for core files */
#define NT_PRSTATUS	1
#define NT_FPREGSET	2
#define NT_PRPSINFO 3

/* Build ID bits as generated by ld --build-id */
#define NT_GNU_BUILD_ID 3

typedef struct {
	Elf32_Sword d_tag;
	union {
		Elf32_Word d_val;
		Elf32_Addr d_ptr;
	} d_un;
} Elf32_Dyn;

typedef struct {
	Elf64_Sxword d_tag;
	union {
		Elf64_Xword d_val;
		Elf64_Addr d_ptr;
	} d_un;
} Elf64_Dyn;

typedef struct {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
	Elf64_Word sh_name;
	Elf64_Word sh_type;
	Elf64_Xword sh_flags;
	Elf64_Addr sh_addr;
	Elf64_Off sh_offset;
	Elf64_Xword sh_size;
	Elf64_Word sh_link;
	Elf64_Word sh_info;
	Elf64_Xword sh_addralign;
	Elf64_Xword sh_entsize;
} Elf64_Shdr;

typedef struct {
  uint64_t a_type;
  union {
    uint64_t a_val;
  } a_un;
} Elf64_auxv_t;

typedef struct {
	uint32_t a_type;
	union {
		uint32_t a_val;
	} a_un;
} Elf32_auxv_t;

typedef struct {
  Elf32_Word n_namesz;
  Elf32_Word n_descsz;
  Elf32_Word n_type;
} Elf32_Nhdr;

typedef struct {
  Elf64_Word n_namesz;
  Elf64_Word n_descsz;
  Elf64_Word n_type;
} Elf64_Nhdr;

/* ST_TYPE (subfield of st_info) values (symbol type) */
#define STT_NOTYPE	0
#define STT_OBJECT	1
#define STT_FUNC	2
#define STT_SECTION	3
#define STT_FILE	4

/* ST_BIND (subfield of st_info) values (symbol binding) */
#define STB_LOCAL	0
#define STB_GLOBAL	1
#define STB_WEAK	2

/* sh_type (section type) values */
#define SHT_NULL		0
#define SHT_PROGBITS		1
#define SHT_SYMTAB		2
#define SHT_STRTAB		3
#define SHT_RELA		4
#define SHT_HASH    5
#define SHT_DYNAMIC 6
#define SHT_NOTE	  7
#define SHT_NOBITS		8
#define SHT_REL			9
#define SHT_DYNSYM  11
#define SHT_INIT_ARRAY		14
#define SHT_FINI_ARRAY		15
#define SHT_SYMTAB_SHNDX	18

/* special section indices */
#define SHN_UNDEF	0
#define SHN_LORESERVE	0xff00
#define SHN_COMMON	0xfff2
#define SHN_XINDEX	0xffff
#define SHN_HIRESERVE	0xff00

/* values for e_machine */
#define EM_NONE			0
#define EM_M32			1
#define EM_SPARC		2
#define EM_386			3
#define EM_68K			4
#define EM_MIPS			8
#define EM_PARISC		15
#define EM_PPC			20
#define EM_PPC64		21
#define EM_S390			22
#define EM_ARM			40
#define EM_SH			42
#define EM_SPARCV9		43
#define EM_IA_64		50
#define EM_X86_64		62
#define EM_BLACKFIN		106
#define EM_AARCH64		183
#define EM_RISCV		243
#define EM_LOONGARCH	258

/* Linux notes this value as being interim; however applications are using this (Qt6), so we define it here. */
#define EM_ALPHA		0x9026

/* values for e_version */
#define EV_NONE		0
#define EV_CURRENT	1
#define EV_NUM		2

/* e_indent constants */
#define EI_MAG0		0
#define ELFMAG0		0x7f

#define EI_MAG1		1
#define ELFMAG1		'E'

#define EI_MAG2		2
#define ELFMAG2		'L'

#define EI_MAG3		3
#define ELFMAG3		'F'

#define ELFMAG    "\177ELF"
#define SELFMAG   4

#define EI_CLASS	4
#define ELFCLASSNONE	0
#define ELFCLASS32	1
#define ELFCLASS64	2
#define ELFCLASSNUM	3

#define EI_DATA		5
#define ELFDATANONE	0
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2
#define ELFDATANUM	3

#define EI_VERSION	6

#define EI_OSABI	7
#define ELFOSABI_HPUX		1
#define ELFOSABI_NETBSD		2
#define ELFOSABI_GNU		3
#define ELFOSABI_LINUX ELFOSABI_GNU
#define ELFOSABI_SOLARIS	6
#define ELFOSABI_AIX		7
#define ELFOSABI_IRIX		8
#define ELFOSABI_FREEBSD	9
#define ELFOSABI_OPENBSD	12

#define EI_ABIVERSION	8

#define ELF_NOTE_GNU "GNU"

/* Values for a_type
 * these are standard values and shared across at least glibc, musl and freebsd
 */

#define AT_NULL 0
#define AT_IGNORE 1
#define AT_EXECFD 2
#define AT_PHDR 3
#define AT_PHENT 4
#define AT_PHNUM 5
#define AT_PAGESZ 6
#define AT_BASE 7
#define AT_FLAGS 8
#define AT_ENTRY 9
#define AT_NOTELF 10
#define AT_UID 11
#define AT_EUID 12
#define AT_GID 13
#define AT_EGID 14

/* Values for Elfxx_Verdef::vd_flags and Elfxx_Vernaux::vna_flags */
#define VER_FLG_BASE 1 /* Version definition of the file itself */
#define VER_FLG_WEAK 2 /* Weak version identifier */

/* rtld requires presence of some a_type (AT_*) values that are not standardized in the ELF spec */
#if !defined(AT_EXECFN) || !defined(AT_RANDOM) || !defined(AT_SECURE)
#error "sysdeps' auxv.h is missing some defines that are required for rtld operation"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ELF_H */
