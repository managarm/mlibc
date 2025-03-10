#include <pthread.h>
#include <stdlib.h>
#include <sys/auxv.h>

#include <frg/eternal.hpp>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/elf/startup.h>

extern "C" void __dlapi_enter(uintptr_t *);


extern char **environ;
static mlibc::exec_stack_data __mlibc_stack_data;

extern "C" void __mlibc_entry(uintptr_t *entry_stack, int (*main_fn)(int argc, char *argv[], char *env[])) {
	__dlapi_enter(entry_stack);
	auto result = main_fn(__mlibc_stack_data.argc, __mlibc_stack_data.argv, environ);
	exit(result);
}
