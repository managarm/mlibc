#include <errno.h>
#include <sys/reboot.h>
#include <sys/syscall.h>

int reboot(int what) {
	int ret, errno;
	SYSCALL2(SYSCALL_REBOOT, what, 0);
	return ret;
}
