#include <sys/module.h>
#include <zinnia/syscall.hpp>

int insertmod(const char *path, const char *cmdline) {
	return zinnia_syscall(SYSCALL_MODULE_INSERT, (size_t)path, (size_t)cmdline).error;
}

int removemod(const char *name) {
	return zinnia_syscall(SYSCALL_MODULE_REMOVE, (size_t)name).error;
}
