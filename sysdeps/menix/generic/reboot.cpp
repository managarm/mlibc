#include <errno.h>
#include <menix/syscall.hpp>
#include <sys/reboot.h>

#define MENIX_REBOOT_MAGIC 0xDEADBEEF

int reboot(int what) {
	auto r = menix_syscall(SYSCALL_REBOOT, MENIX_REBOOT_MAGIC, what);
	if (r.error) {
		errno = r.error;
		return -1;
	}
	return r.value;
}
