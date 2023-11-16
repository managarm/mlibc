#include <sys/syscall.h>
#include <sys/mac.h>
#include <errno.h>
#include <string.h>

extern "C" {

unsigned long get_mac_capabilities(void) {
	int ret, errn;
	SYSCALL0(SYSCALL_GET_MAC_CAPABILITIES);
	errno = errn;
	return ret;
}

int set_mac_capabilities(unsigned long request) {
	int ret, errn;
	SYSCALL1(SYSCALL_SET_MAC_CAPABILITIES, request);
	errno = errn;
	return ret;
}

int add_mac_permissions(const char *path, int flags) {
	int ret, errn;
	SYSCALL3(SYSCALL_ADD_MAC_PERMISSIONS, path, strlen(path), flags);
	errno = errn;
	return ret;
}

int set_mac_enforcement(unsigned long enforcement) {
	int ret, errn;
	SYSCALL1(SYSCALL_SET_MAC_ENFORCEMENT, enforcement);
	errno = errn;
	return ret;
}

}
