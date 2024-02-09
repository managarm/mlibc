#include <sys/syscall.h>
#include <sys/mac.h>
#include <errno.h>
#include <string.h>

extern "C" {

unsigned long get_mac_capabilities(void) {
	int ret;
	SYSCALL0(SYSCALL_GET_MAC_CAPABILITIES);
	return ret;
}

int set_mac_capabilities(unsigned long request) {
	int ret;
	SYSCALL1(SYSCALL_SET_MAC_CAPABILITIES, request);
	return ret;
}

int add_mac_permissions(const char *path, int flags) {
	int ret;
	SYSCALL3(SYSCALL_ADD_MAC_PERMISSIONS, path, strlen(path), flags);
	return ret;
}

int set_mac_enforcement(unsigned long enforcement) {
	int ret;
	SYSCALL1(SYSCALL_SET_MAC_ENFORCEMENT, enforcement);
	return ret;
}

}
