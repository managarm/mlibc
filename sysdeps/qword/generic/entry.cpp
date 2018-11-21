
#include <stdlib.h>

// TODO: Fill this as we do in managarm.
static int __mlibc_argc;
static char **__mlibc_argv;

extern char **environ;

extern "C" void __mlibc_entry(int (*main_function)(int argc, char *argv[], char *env[])) {
	auto result = main_function(__mlibc_argc, __mlibc_argv, environ);
	exit(result);
}

