#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>

extern "C" void __dlapi_enter(uintptr_t *);
extern "C" int main(int argc, char **argv, char **envp);

extern char **environ;

extern "C" uint64_t __robu_heap_base;
uint64_t __robu_heap_base;

#define ROBU_ENTRY_MAX_ARGS 512

extern "C" [[noreturn]] void __mlibc_robu_entry(uint64_t argc, char **argv, char **envp,
                                                uint64_t heap_base, uint64_t spawn_info) {
	__robu_heap_base = heap_base;
	(void)spawn_info;

	if (argc > ROBU_ENTRY_MAX_ARGS) {
		argc = ROBU_ENTRY_MAX_ARGS;
	}
	uint64_t envc = 0;
	while (envp && envc < ROBU_ENTRY_MAX_ARGS && envp[envc]) {
		envc++;
	}

	static uintptr_t stack_blob[1 + ROBU_ENTRY_MAX_ARGS + 1 + ROBU_ENTRY_MAX_ARGS + 1 + 1];
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
	*sp++ = 0;

	__dlapi_enter(stack_blob);

	auto result = main(mlibc::entry_stack.argc, mlibc::entry_stack.argv, environ);
	exit(result);
}
