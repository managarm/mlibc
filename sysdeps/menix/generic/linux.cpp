#include <abi-bits/pid_t.h>
#include <bits/ensure.h>
#include <bits/file.h>
#include <menix/power.hpp>
#include <menix/syscall.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <stdio.h>

namespace mlibc {

int sys_mount(
    const char *source,
    const char *target,
    const char *fstype,
    unsigned long flags,
    const void *data
) {
	return menix_syscall(
	           SYSCALL_MOUNT, (size_t)source, (size_t)target, (size_t)fstype, flags, (size_t)data
	)
	    .error;
}

int sys_umount2(const char *target, int flags) {
	return menix_syscall(SYSCALL_UMOUNT, (size_t)target, flags).error;
}

int sys_ptrace(long req, pid_t pid, void *addr, void *data, long *out) {
	auto r = menix_syscall(SYSCALL_PTRACE, req, pid, (size_t)addr, (size_t)data);
	if (r.error)
		return r.error;
	*out = (long)r.value;
	return 0;
}

int sys_klogctl(int type, char *bufp, int len, int *out) {
	auto r = menix_syscall(SYSCALL_SYSLOG, type, (size_t)bufp, len);
	if (r.error)
		return r.error;
	*out = (int)r.value;
	return 0;
}

int sys_reboot(int cmd) {
	return menix_syscall(SYSCALL_REBOOT, MENIX_REBOOT_MAGIC1, MENIX_REBOOT_MAGIC2, cmd).error;
}

int sys_getcpu(int *cpu) { return menix_syscall(SYSCALL_GETCPU, (size_t)cpu, 0, 0).error; }

int sys_sysinfo(struct sysinfo *info) { return menix_syscall(SYSCALL_SYSINFO, (size_t)info).error; }

int sys_swapon(const char *path, int flags) {
	return menix_syscall(SYSCALL_SWAPON, (size_t)path, flags).error;
}

int sys_swapoff(const char *path) { return menix_syscall(SYSCALL_SWAPOFF, (size_t)path).error; }

} // namespace mlibc
