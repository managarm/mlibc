
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>
#include <mlibc/environment.hpp>

extern "C" size_t __init_array_start[];
extern "C" size_t __init_array_end[];
extern "C" size_t __preinit_array_start[];
extern "C" size_t __preinit_array_end[];

extern "C" uintptr_t *__dlapi_entrystack();

namespace mlibc {

exec_stack_data entry_stack;

[[gnu::constructor]]
void init_libc() {
	mlibc::parse_exec_stack(__dlapi_entrystack(), &entry_stack);
	mlibc::set_startup_data(entry_stack.argc, entry_stack.argv, entry_stack.envp);
}

void parse_exec_stack(void *opaque_sp, exec_stack_data *data) {
	auto sp = reinterpret_cast<uintptr_t *>(opaque_sp);
	data->argc = *sp++;
	data->argv = reinterpret_cast<char **>(sp);
	sp += data->argc; // Skip all arguments.
	__ensure(!*sp); // Skip the terminating null element.
	sp++;
	data->envp = reinterpret_cast<char **>(sp);
}

// TODO: This does not have to be here; we could also move it to options/internal.
void set_startup_data(int argc, char **argv, char **envp) {
	if(argc) {
		program_invocation_name = argv[0];

		if(auto slash = strrchr(argv[0], '/'); slash) {
			program_invocation_short_name = slash + 1;
		}else{
			program_invocation_short_name = argv[0];
		}
	}

	// Initialize environ.
	// TODO: Copy the arguments instead of pointing to them?
	auto ev = envp;
	while(*ev) {
		auto fail = mlibc::putenv(*ev);
		__ensure(!fail);
		ev++;
	}
}

} // namespace mlibc

