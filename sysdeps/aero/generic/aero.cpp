#include <abi-bits/pid_t.h>
#include <aero/syscall.h>
#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/thread-entry.hpp>
#include <stddef.h>

#include <frg/vector.hpp>
#include <mlibc/allocator.hpp>

#define ARCH_SET_GS 0x1001
#define ARCH_SET_FS 0x1002
#define ARCH_GET_FS 0x1003
#define ARCH_GET_GS 0x1004

struct Slice {
    void *ptr;
    uint64_t len;
};

/// Helper function to construct a slice vector from the provided argument
/// array. A slice basically consists of a pointer to the data and the length of
/// it.
///
/// ## Examples
/// ```cc
/// auto slice = create_slice({ "hello", "world" });
/// ```
///
/// The `slice` will look like the following:
///
/// ```cc
/// vector<Slice>(
///     Slice { .ptr: hello_ptr, .size: hello_size },
///     Slice { .ptr: world_ptr, .size: world_size }
/// )
/// ```
static frg::vector<Slice, MemoryAllocator> create_slice(char *const arg[]) {
    if (arg == nullptr) {
        return frg::vector<Slice, MemoryAllocator>{getAllocator()};
    }

    // Find out the length of arg:
    size_t len = 0;

    while (arg[len] != nullptr) {
        len += 1;
    }

    frg::vector<Slice, MemoryAllocator> params{getAllocator()};
    params.resize(len);

    // Construct the slice vector:
    for (size_t i = 0; i < len; ++i) {
        params[i].ptr = (void *)arg[i];
        params[i].len = strlen(arg[i]);
    }

    return params;
}

namespace mlibc {
int sys_uname(struct utsname *buf) {
    auto result = syscall(SYS_UNAME, buf);

    if (result < 0) {
        return -result;
    }

    return result;
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
    // auto result = syscall(SYS_FUTEX_WAIT, pointer, expected, time);
    //
    // if (result < 0) {
    //     return -result;
    // }
    //
    return 0;
}

int sys_futex_wake(int *pointer) {
    // auto result = syscall(SYS_FUTEX_WAKE, pointer);
    //
    // if (result < 0) {
    //     return -result;
    // }
    //
    return 0;
}

int sys_tcb_set(void *pointer) {
    auto result = syscall(SYS_ARCH_PRCTL, ARCH_SET_FS, (uint64_t)pointer);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd,
               off_t offset, void **window) {
    auto result = syscall(SYS_MMAP, hint, size, prot, flags, fd, offset);

    if (result < 0) {
        return -result;
    }

    *window = (void *)result;
    return 0;
}

int sys_vm_unmap(void *address, size_t size) {
    return syscall(SYS_MUNMAP, address, size);
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
    auto res = syscall(SYS_MPROTECT, pointer, size, prot);
    if (res < 0)
        return -res;

    return 0;
}

int sys_anon_allocate(size_t size, void **pointer) {
    return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, pointer);
}

int sys_anon_free(void *pointer, size_t size) {
    return sys_vm_unmap(pointer, size);
}

void sys_libc_panic() {
    mlibc::infoLogger() << "libc_panic: panicked at 'unknown'" << frg::endlog;
    __ensure(!syscall(SYS_BACKTRACE));

    sys_exit(1);
}

void sys_libc_log(const char *msg) { syscall(SYS_LOG, msg, strlen(msg)); }

void sys_exit(int status) {
    syscall(SYS_EXIT, status);

    __builtin_unreachable();
}

#ifndef MLIBC_BUILDING_RTLD

pid_t sys_getpid() {
    auto result = syscall(SYS_GETPID);
    __ensure(result >= 0);

    return result;
}

pid_t sys_getppid() {
    auto result = syscall(SYS_GETPPID);
    __ensure(result != 0);

    return result;
}

int sys_kill(int pid, int sig) {
    auto result = syscall(SYS_KILL, pid, sig);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
    struct timespec ts;
    auto result = syscall(SYS_GETTIME, clock, &ts);

    if (result < 0) {
        return -result;
    }

    *secs = ts.tv_sec;
    *nanos = ts.tv_nsec;

    return 0;
}

int sys_getcwd(char *buffer, size_t size) {
    auto result = syscall(SYS_GETCWD, buffer, size);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_chdir(const char *path) {
    auto result = syscall(SYS_CHDIR, path, strlen(path));

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_gethostname(char *buffer, size_t bufsize) {
    auto result = syscall(SYS_GETHOSTNAME, buffer, bufsize);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_sleep(time_t *sec, long *nanosec) {
    struct timespec ts = {.tv_sec = *sec, .tv_nsec = *nanosec};

    auto result = syscall(SYS_SLEEP, &ts);

    if (result < 0) {
        return -result;
    }

    return 0;
}

pid_t sys_getpgid(pid_t pid, pid_t *pgid) {
    auto ret = syscall(SYS_GETPGID, pid);
    if(int e = sc_error(ret); e)
        return e;
    *pgid = ret;
    return 0;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
    auto ret = syscall(SYS_SETPGID, pid, pgid);
    if(int e = sc_error(ret); e)
		return e;
	return 0;
}

uid_t sys_getuid() {
    mlibc::infoLogger() << "mlibc: sys_setuid is a stub" << frg::endlog;
    return 0;
}

uid_t sys_geteuid() {
    mlibc::infoLogger() << "mlibc: sys_seteuid is a stub" << frg::endlog;
    return 0;
}

int sys_setsid(pid_t *sid) {
    auto ret = syscall(SYS_SETSID);
    if(int e = sc_error(ret); e)
        return e;
    *sid = ret;
    return 0;
}

int sys_seteuid(uid_t euid) UNIMPLEMENTED("sys_seteuid")

    gid_t sys_getgid() {
    mlibc::infoLogger() << "mlibc: sys_setgid is a stub" << frg::endlog;
    return 0;
}

gid_t sys_getegid() {
    mlibc::infoLogger() << "mlibc: sys_getegid is a stub" << frg::endlog;
    return 0;
}

int sys_setgid(gid_t gid) {
    mlibc::infoLogger() << "mlibc: sys_setgid is a stub" << frg::endlog;
    return 0;
}

int sys_setegid(gid_t egid) {
    mlibc::infoLogger() << "mlibc: sys_setegid is a stub" << frg::endlog;
    return 0;
}

void sys_yield() {
    mlibc::infoLogger() << "mlibc: sys_yield is a stub" << frg::endlog;
    __ensure(!syscall(SYS_BACKTRACE));
}

int sys_clone(void *tcb, pid_t *tid_out, void *stack) {
    auto result = syscall(SYS_CLONE, (uintptr_t)__mlibc_start_thread, stack);

    if (result < 0) {
        return -result;
    }

    *tid_out = (pid_t)result;
    return 0;
}

int sys_thread_setname(void *tcb, const char *name) {
    mlibc::infoLogger() << "The name of this thread is " << name << frg::endlog;
    return 0;
}

void sys_thread_exit() {
    syscall(SYS_EXIT);
    __builtin_trap();
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru,
                pid_t *ret_pid) {
    if (ru) {
        mlibc::infoLogger()
            << "mlibc: struct rusage in sys_waitpid is unsupported"
            << frg::endlog;
        return ENOSYS;
    }

    auto result = syscall(SYS_WAITPID, pid, status, flags);

    if (result < 0) {
        return -result;
    }

    *ret_pid = result;
    return 0;
}

int sys_fork(pid_t *child) {
    auto result = syscall(SYS_FORK);

    if (result < 0) {
        return -result;
    }

    *child = result;
    return 0;
}

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
    auto envv_slice = create_slice(envp);
    auto argv_slice = create_slice(argv);

    auto path_ptr = (uintptr_t)path;
    auto path_len = strlen(path);

    auto result =
        syscall(SYS_EXEC, path_ptr, path_len, argv_slice.data(),
                argv_slice.size(), envv_slice.data(), envv_slice.size());

    if (result < 0) {
        return -result;
    }

    __builtin_unreachable();
}

// int sys_getentropy(void *buffer, size_t length)
// UNIMPLEMENTED("sys_getentropy")

#endif
} // namespace mlibc
