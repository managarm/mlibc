
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>

extern "C" size_t __init_array_start[];
extern "C" size_t __init_array_end[];

static int constructors_ran_already = 0;

struct global_constructor_guard {
    global_constructor_guard() {
        constructors_ran_already = 1;
    }
};

static global_constructor_guard g;

void __mlibc_run_constructors() {
    if (!constructors_ran_already) {
        size_t constructor_count = (size_t)__init_array_end - (size_t)__init_array_start;
        constructor_count /= sizeof(void*);
        for (size_t i = 0; i < constructor_count; i++) {
            void (*ptr)(void) = (void(*)(void))(__init_array_start[i]);
            ptr();
        }
    }
}

namespace mlibc {

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
		auto fail = putenv(*ev);
		__ensure(!fail);
		ev++;
	}
}

} // namespace mlibc

