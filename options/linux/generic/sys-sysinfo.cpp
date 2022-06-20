#include <errno.h>
#include <sys/sysinfo.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/linux-sysdeps.hpp>

int sysinfo(struct sysinfo *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
