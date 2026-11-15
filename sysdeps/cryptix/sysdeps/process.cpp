#include <cryptix/syscall.h>
#include <errno.h>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <string.h>

#ifndef MLIBC_BUILDING_RTLD
extern "C" int
__mlibc_spawn_thread(int flags, void *stack, void *pid_out, void *child_tid, void *tcb);
#endif

namespace mlibc {
pid_t Sysdeps<GetPid>::operator()() { return Syscall(SYS_GETPID); }
void Sysdeps<Exit>::operator()(int code) {
	Syscall(SYS_EXIT, code);

	__builtin_unreachable();
}
int
Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	auto ret = Syscall(SYS_WAIT4, pid, status, flags, ru);
	if (auto e = syscall_error(ret); e)
		return e;

	*ret_pid = static_cast<pid_t>(ret);
	return 0;
}
int Sysdeps<Kill>::operator()(int pid, int sig) {
	auto ret = Syscall(SYS_KILL, pid, sig);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

uid_t Sysdeps<GetUid>::operator()() { return Syscall(SYS_GETUID); }
gid_t Sysdeps<GetGid>::operator()() { return Syscall(SYS_GETGID); }
int Sysdeps<SetUid>::operator()(uid_t uid) {
	auto ret = Syscall(SYS_SETUID, uid);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<SetGid>::operator()(gid_t gid) {
	auto ret = Syscall(SYS_SETGID, gid);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
uid_t Sysdeps<GetEuid>::operator()() { return Syscall(SYS_GETEUID); }
gid_t Sysdeps<GetEgid>::operator()() { return Syscall(SYS_GETEGID); }
int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	auto ret = Syscall(SYS_SETPGID, pid, pgid);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
pid_t Sysdeps<GetPpid>::operator()() { return Syscall(SYS_GETPPID); }
pid_t Sysdeps<SetSid>::operator()(pid_t *out) {
	auto ret = Syscall(SYS_SETSID);
	if (auto e = syscall_error(ret); e)
		return e;

	*out = static_cast<pid_t>(ret);
	return 0;
}
int Sysdeps<SetReuid>::operator()(uid_t ruid, uid_t euid) {
	auto ret = Syscall(SYS_SETREUID, ruid, euid);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<SetRegid>::operator()(gid_t rgid, gid_t egid) {
	auto ret = Syscall(SYS_SETREGID, rgid, egid);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<SetResuid>::operator()(uid_t ruid, uid_t euid, uid_t suid) {
	auto ret = Syscall(SYS_SETRESUID, ruid, euid, suid);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid) {
	auto ret = Syscall(SYS_GETRESUID, ruid, euid, suid);
	if (auto e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<SetResgid>::operator()(gid_t rgid, gid_t egid, gid_t sgid) {
	auto ret = Syscall(SYS_SETRESGID, rgid, egid, sgid);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	auto ret = Syscall(SYS_GETRESGID, rgid, egid, sgid);
	if (auto e = syscall_error(ret); e)
		return e;
	return 0;
}
pid_t Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *out) {
	auto ret = Syscall(SYS_GETPGID, pid);
	if (auto e = syscall_error(ret); e)
		return e;

	*out = ret;
	return 0;
}
pid_t Sysdeps<GetSid>::operator()(pid_t pid, pid_t *out) {
	auto ret = Syscall(SYS_GETSID, pid);
	if (auto e = syscall_error(ret); e)
		return e;

	*out = ret;
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
int Sysdeps<Clone>::operator()(void *tcb, pid_t *pid_out, void *stack) {
	unsigned long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD
	                      | CLONE_SYSVSEM | CLONE_SETTLS | CLONE_PARENT_SETTID;

#if defined(__aarch64__)
	// TP should point to the address 16 bytes before the end of the TCB.
	tcb = reinterpret_cast<char *>(tcb) + sizeof(Tcb) - 0x10;
#endif

	auto ret = __mlibc_spawn_thread(flags, stack, pid_out, nullptr, tcb);
	if (ret < 0)
		return -ret;

	return 0;
}
#endif
int Sysdeps<Fork>::operator()(pid_t *child) {
	auto ret = Syscall(SYS_FORK);
	if (auto e = syscall_error(ret); e)
		return e;

	*child = static_cast<pid_t>(ret);
	return 0;
}
int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	auto ret = Syscall(SYS_EXECVE, path, argv, envp);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<Prctl>::operator()(int op, va_list va, int *out) {
	unsigned long x[4];
	for (int i = 0; i < 4; i++)
		x[i] = va_arg(va, unsigned long);

	auto ret = Syscall(SYS_PRCTL, op, x[0], x[1], x[2], x[3]);
	if (int e = syscall_error(ret); e)
		return e;
	*out = ret;
	return 0;
}
int Sysdeps<TcbSet>::operator()(void *pointer) {
#if defined(__x86_64__)
	constexpr int ARCH_SET_FS = 0x1002;
	auto ret = Syscall(SYS_ARCH_PRCTL, ARCH_SET_FS, pointer);
	if (auto e = syscall_error(ret); e)
		return e;
#elif defined(__aarch64__)
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb) - 0x10;
	asm volatile("msr tpidr_el0, %0" ::"r"(thread_data));
#else
#error "Missing architecture specific code."
#endif

	return 0;
}
int Sysdeps<GetTid>::operator()() { return Syscall(SYS_GETTID); }
int Sysdeps<FutexTid>::operator()() { return sysdep<GetTid>(); }
int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
	auto ret = Syscall(SYS_FUTEX_WAIT, pointer, expected, time);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
	// futex_wake(uaddr, mask, nr_wake, flags)
	auto ret = Syscall(SYS_FUTEX_WAKE, pointer, ~0ul, all ? INT_MAX : 1, 0);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

int Sysdeps<GetHostname>::operator()(char *buffer, size_t bufsize) {
	utsname data{};
	if (auto e = sysdep<Uname>(&data); e)
		return e;

	size_t hostname_size = strlen(data.nodename);
	if (hostname_size >= bufsize)
		return ENAMETOOLONG;

	memcpy(buffer, data.nodename, hostname_size);
	buffer[hostname_size] = '\0';
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
void Sysdeps<ThreadExit>::operator()() {
	sysdep<Exit>(0);
	__builtin_trap();
}
#endif
} // namespace mlibc
