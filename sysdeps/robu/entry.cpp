#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>

extern "C" void __dlapi_enter(uintptr_t *);
extern "C" int main(int argc, char **argv, char **envp);

extern char **environ;

extern "C" uint64_t __robu_heap_base;
uint64_t __robu_heap_base;

extern "C" void __robu_fd_inherit(uint64_t spawn_info);

// Auto-defined by the linker to the start of the ELF header, since every
// apps/link/*.ld script for mlibc-based apps uses "FILEHDR PHDRS" on the
// first PT_LOAD so the header/phdrs are actually part of the mapped image
// (interpreterMain()'s auxv walk needs a real AT_PHDR to point at).
extern "C" char __ehdr_start[];

struct RobuElf64Ehdr {
	unsigned char e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

#define ROBU_AT_NULL   0
#define ROBU_AT_PHDR   3
#define ROBU_AT_PHENT  4
#define ROBU_AT_PHNUM  5
#define ROBU_AT_PAGESZ 6
#define ROBU_AT_BASE   7
#define ROBU_AT_ENTRY  9

#define ROBU_ENTRY_MAX_ARGS 512
#define ROBU_AUXV_PAIRS 6 /* PHDR, PHENT, PHNUM, PAGESZ, BASE, ENTRY -- plus one AT_NULL pair */

extern "C" [[noreturn]] void __mlibc_robu_entry(uint64_t argc, char **argv, char **envp,
                                                uint64_t heap_base, uint64_t spawn_info) {
	__robu_heap_base = heap_base;
	__robu_fd_inherit(spawn_info);

	if (argc > ROBU_ENTRY_MAX_ARGS) {
		argc = ROBU_ENTRY_MAX_ARGS;
	}
	uint64_t envc = 0;
	while (envp && envc < ROBU_ENTRY_MAX_ARGS && envp[envc]) {
		envc++;
	}

	static uintptr_t stack_blob[1 + ROBU_ENTRY_MAX_ARGS + 1 + ROBU_ENTRY_MAX_ARGS + 1
	                            + 2 * (ROBU_AUXV_PAIRS + 1)];
	uintptr_t *sp = stack_blob;
	*sp++ = argc;
	for (uint64_t i = 0; i < argc; i++) {
		*sp++ = (uintptr_t)argv[i];
	}
	*sp++ = 0;
	for (uint64_t i = 0; i < envc; i++) {
		*sp++ = (uintptr_t)envp[i];
	}
	*sp++ = 0;

	const RobuElf64Ehdr *ehdr = (const RobuElf64Ehdr *)__ehdr_start;
	auto put_aux = [&](uintptr_t type, uintptr_t val) {
		*sp++ = type;
		*sp++ = val;
	};
	put_aux(ROBU_AT_PHDR, (uintptr_t)__ehdr_start + ehdr->e_phoff);
	put_aux(ROBU_AT_PHENT, ehdr->e_phentsize);
	put_aux(ROBU_AT_PHNUM, ehdr->e_phnum);
	put_aux(ROBU_AT_PAGESZ, 4096);
	put_aux(ROBU_AT_BASE, 0);
	put_aux(ROBU_AT_ENTRY, ehdr->e_entry);
	put_aux(ROBU_AT_NULL, 0);

	// __dlapi_enter() runs interpreterMain(), which -- via Loader::initObjects()
	// / doInitialize() -- already walks this binary's __init_array_start..end
	// and calls every global constructor exactly once. A second walk here used
	// to double-run them (e.g. file-io.cpp's init_stdio() a second time), which
	// tripped frg::manual_box's reentrancy assert on stdin_box/stdout_box.
	__dlapi_enter(stack_blob);

	auto result = main(mlibc::entry_stack.argc, mlibc::entry_stack.argv, environ);
	exit(result);
}
