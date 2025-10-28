/*#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.hpp>
#include <stdint.h>

extern "C" int main(int argc, char **argv, char **envp);

struct dl_find_object {};

extern "C" int __dlapi_find_object(void *__address, dl_find_object *__result) {
    (void)__address;
    (void)__result;
    return -1;
}

// These are provided by the OS or loader (if not, you can hardcode 0s).
extern char **environ;

namespace mlibc {
void initRuntime() {}
void initConstructors() {}
void finiConstructors() {}
}

extern "C" void __mlibc_entry(int argc, char **argv, char **envp) {
    mlibc::sys_libc_log("mlibc: entering user program\n");

    // Initialize internal mlibc state (malloc, TLS, etc.)
    mlibc::initRuntime();

    // Run global constructors (C++ static initializers)
    mlibc::initConstructors();

    int ret = main(argc, argv, envp);

    // Run global destructors before exit
    mlibc::finiConstructors();

    // Call system exit
    mlibc::sys_exit(ret);

    // Should never return
    while (1)
        ;
}
*/
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
