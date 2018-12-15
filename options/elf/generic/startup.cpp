
#include <stdint.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>

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

} // namespace mlibc

