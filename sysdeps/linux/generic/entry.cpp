#include <mlibc/init-priority.hpp>
#include <stddef.h>
#include <sys/auxv.h>

size_t __hwcap;

[[gnu::constructor(MLIBC_INIT_PRIORITY_ENTRY)]]
static void init_hwcap() {
	__hwcap = getauxval(AT_HWCAP);
}
