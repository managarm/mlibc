#pragma once

#include <concepts>

#include <abi-bits/mode_t.h>
#include <abi-bits/pid_t.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>
#include <bits/ensure.h>
#include <bits/off_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/sigset_t.h>

#include <mlibc/ansi-sysdeps.hpp>

namespace mlibc {

template <typename T>
concept RequiredSysdeps = requires(
    T t,
    int v_int,
    size_t v_size,
    ssize_t v_ssize,
    off_t v_off,
    mode_t v_mode,
    void *p_void,
    const char *p_char,
    int *p_int,
    long *p_long,
    ssize_t *p_ssize,
	off_t *p_off,
	time_t *p_time,
	struct timespec *p_timespec
) {
	// [[noreturn]] void sys_exit(int status);
	{ t.exit(v_int) } -> std::same_as<void>;
	// [[noreturn]] void sys_thread_exit();
	{ t.thread_exit() } -> std::same_as<void>;

	// int sys_futex_wait(int *pointer, int expected, const struct timespec *time);
	{ t.futex_wait(p_int, v_int, p_timespec) } -> std::same_as<int>;
	// int sys_futex_wake(int *pointer);
	{ t.futex_wake(p_int) } -> std::same_as<int>;

	// int sys_open(const char *pathname, int flags, mode_t mode, int *fd);
    { t.open(p_char, v_int, v_mode, p_int) } -> std::same_as<int>;
	// int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
    { t.write(v_int, p_void, v_size, p_ssize) } -> std::same_as<int>;

	// int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
    { t.read(v_int, p_void, v_size, p_ssize) } -> std::same_as<int>;

	// int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
	{ t.seek(v_int, v_off, v_int, p_off) } -> std::same_as<int>;

	// int sys_close(int fd);
	{ t.close(v_int) } -> std::same_as<int>;

	// int sys_clock_get(int clock, time_t *secs, long *nanos);
	{ t.clock_get(v_int, p_time, p_long) } -> std::same_as<int>;

	// pid_t sys_getpid();
	{ t.getpid() } -> std::same_as<pid_t>;
};

struct AnsiCompatSysdeps : public AnsiSysdeps {
	int prepare_stack(void **stack, void *entry, void *user_arg, void* tcb, size_t *stack_size, size_t *guard_size, void **stack_base) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_prepare_stack, -1);
		return sysdep(stack, entry, user_arg, tcb, stack_size, guard_size, stack_base);
	}

	int clone(void *tcb, pid_t *pid_out, void *stack) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_clone, -1);
		return sysdep(tcb, pid_out, stack);
	}

	int flock(int fd, int options) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_flock, -1);
		return sysdep(fd, options);
	}

	int open_dir(const char *path, int *handle) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_open_dir, -1);
		return sysdep(path, handle);
	}

	int read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_read_entries, -1);
		return sysdep(handle, buffer, max_size, bytes_read);
	}

	int pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_pread, -1);
		return sysdep(fd, buf, n, off, bytes_read);
	}

	int clock_set(int clock, time_t secs, long nanos) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_clock_set, -1);
		return sysdep(clock, secs, nanos);
	}

	int clock_getres(int clock, time_t *secs, long *nanos) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_clock_getres, -1);
		return sysdep(clock, secs, nanos);
	}

	int sleep(time_t *secs, long *nanos) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sleep, -1);
		return sysdep(secs, nanos);
	}

	int isatty(int fd) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_isatty, -1);
		return sysdep(fd);
	}

	int rmdir(const char *path) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_rmdir, -1);
		return sysdep(path);
	}

	int unlinkat(int dirfd, const char *path, int flags) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_unlinkat, -1);
		return sysdep(dirfd, path, flags);
	}

	int rename(const char *path, const char *new_path) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_rename, -1);
		return sysdep(path, new_path);
	}

	int renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_renameat, -1);
		return sysdep(olddirfd, old_path, newdirfd, new_path);
	}

	int sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigprocmask, -1);
		return sysdep(how, set, retrieve);
	}

	int sigaction(int sig, const struct sigaction *__restrict sa, struct sigaction *__restrict sa_old) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_sigaction, -1);
		return sysdep(sig, sa, sa_old);
	}

	int fork(pid_t *child) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_fork, -1);
		return sysdep(child);
	}

	int waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_waitpid, -1);
		return sysdep(pid, status, flags, ru, ret_pid);
	}

	int execve(const char *path, char *const argv[], char *const envp[]) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_execve, -1);
		return sysdep(path, argv, envp);
	}

	int kill(int pid, int num) {
		auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_kill, -1);
		return sysdep(pid, num);
	}
};

} // namespace mlibc
