#include <stdint.h>
#include <stdlib.h>
#include <bits/ensure.h>
#include <mlibc/elf/startup.h>

// defined by the POSIX library
void __mlibc_initLocale();

extern "C" uintptr_t *__dlapi_entrystack();
extern "C" void __dlapi_enter(uintptr_t *);

extern char **environ;
static mlibc::exec_stack_data __mlibc_stack_data;

struct LibraryGuard {
	LibraryGuard();
};

static LibraryGuard guard;

LibraryGuard::LibraryGuard() {
	__mlibc_initLocale();

	// Parse the exec() stack.
	mlibc::parse_exec_stack(__dlapi_entrystack(), &__mlibc_stack_data);
	mlibc::set_startup_data(__mlibc_stack_data.argc, __mlibc_stack_data.argv,
			__mlibc_stack_data.envp);
}

extern "C" void __mlibc_entry(uintptr_t *entry_stack, int (*main_fn)(int argc, char *argv[], char *env[])) {
	__dlapi_enter(entry_stack);
	auto result = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ);
	exit(result);
}
