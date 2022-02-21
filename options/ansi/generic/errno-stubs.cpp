#include <errno.h>

int __thread __mlibc_errno;

char *program_invocation_name = nullptr;
char *program_invocation_short_name = nullptr;
extern char *__progname __attribute__ ((weak, alias ("program_invocation_short_name")));
extern char *__progname_full __attribute__ ((weak, alias ("program_invocation_name")));

int *__errno_location() {
	return &__mlibc_errno;
}
