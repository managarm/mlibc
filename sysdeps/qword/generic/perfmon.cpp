
#include <errno.h>
#include <qword/perfmon.h>

int perfmon_create() {
	int ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(21)
			: "rcx", "r11");

	if(ret == -1) {
		errno = sys_errno;
		return -1;
	}
	return ret;
}

int perfmon_attach(int fd) {
	int ret;
	int sys_errno;

	asm volatile ("syscall"
			: "=a"(ret), "=d"(sys_errno)
			: "a"(22), "D"(fd)
			: "rcx", "r11");

	if(ret) {
		errno = sys_errno;
		return -1;
	}
	return 0;
}

