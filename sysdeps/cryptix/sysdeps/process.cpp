#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <algorithm>
#include <cryptix/syscall.h>

namespace mlibc
{
    pid_t sys_getpid() { return Syscall(SYS_GETPID); }
    void  sys_exit(int code)
    {
        Syscall(SYS_EXIT, code);

        __builtin_unreachable();
    }
    int sys_waitpid(pid_t pid, int* status, int flags, struct rusage* ru,
                    pid_t* ret_pid)
    {
        auto ret = Syscall(SYS_WAIT4, pid, status, flags, ru);
        if (auto e = syscall_error(ret); e) return e;

        *ret_pid = static_cast<pid_t>(ret);
        return 0;
    }
    int sys_kill(int pid, int sig)
    {
        auto ret = Syscall(SYS_KILL, pid, sig);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }

    uid_t sys_getuid()
    {
        auto ret = Syscall(SYS_GETUID);
        if (auto e = syscall_error(ret); e) return e;

        return static_cast<pid_t>(ret);
    }
    gid_t sys_getgid()
    {
        auto ret = Syscall(SYS_GETGID);
        if (auto e = syscall_error(ret); e) return e;

        return static_cast<pid_t>(ret);
    }
    int sys_setuid(uid_t uid)
    {
        auto ret = Syscall(SYS_SETUID, uid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_setgid(gid_t gid)
    {
        auto ret = Syscall(SYS_SETGID, gid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    uid_t sys_geteuid()
    {
        auto ret = Syscall(SYS_GETEUID);
        if (auto e = syscall_error(ret); e) return e;

        return ret;
    }
    gid_t sys_getegid()
    {
        auto ret = Syscall(SYS_GETEGID);
        if (auto e = syscall_error(ret); e) return e;

        return ret;
    }
    int sys_setpgid(pid_t pid, pid_t pgid)
    {
        auto ret = Syscall(SYS_SETPGID, pid, pgid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    pid_t sys_getppid()
    {
        auto ret = Syscall(SYS_GETPPID);
        if (auto e = syscall_error(ret); e) return e;

        return static_cast<pid_t>(ret);
    }
    pid_t sys_setsid(pid_t* out)
    {
        auto ret = Syscall(SYS_SETSID);
        if (auto e = syscall_error(ret); e) return e;

        *out = static_cast<pid_t>(ret);
        return 0;
    }
    int sys_setreuid(uid_t ruid, uid_t euid)
    {
        auto ret = Syscall(SYS_SETREUID, ruid, euid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_setregid(gid_t rgid, gid_t egid)
    {
        auto ret = Syscall(SYS_SETREGID, rgid, egid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid)
    {
        auto ret = Syscall(SYS_SETRESUID, ruid, euid, suid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid)
    {
        auto ret = Syscall(SYS_SETRESGID, rgid, egid, sgid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    pid_t sys_getpgid(pid_t pid, pid_t* out)
    {
        auto ret = Syscall(SYS_GETPGID, pid);
        if (auto e = syscall_error(ret); e) return e;

        *out = ret;
        return 0;
    }
    pid_t sys_getsid(pid_t pid, pid_t* out)
    {
        auto ret = Syscall(SYS_GETSID, pid);
        if (auto e = syscall_error(ret); e) return e;

        *out = ret;
        return 0;
    }

    int sys_clone(void* tcb, pid_t* pid_out, void* stack)
    {
        unsigned long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND
                            | CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS
                            | CLONE_PARENT_SETTID;

        auto ret = Syscall(SYS_CLONE, flags, stack, pid_out, nullptr, tcb);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_fork(pid_t* child)
    {
        auto ret = Syscall(SYS_FORK);
        if (auto e = syscall_error(ret); e) return e;

        *child = static_cast<pid_t>(ret);
        return 0;
    }
    int sys_execve(const char* path, char* const argv[], char* const envp[])
    {
        return Syscall(SYS_EXECVE, path, argv, envp);
    }

    int sys_tcb_set(void* pointer)
    {
        auto ret = Syscall(SYS_ARCH_PRCTL, 0x1002, pointer);
        if (auto e = syscall_error(ret); e) return e;

        return ret;
    }
    int sys_gettid()
    {
        auto ret = Syscall(SYS_GETTID);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_futex_tid() { return sys_gettid(); }
    int sys_futex_wait(int* pointer, int expected, const struct timespec* time)
    {
        auto ret = Syscall(SYS_FUTEX_WAIT, pointer, expected, time);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_futex_wake(int* pointer)
    {
        auto ret = Syscall(SYS_FUTEX_WAKE, pointer);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }

    int sys_gethostname(char* buffer, size_t bufsize)
    {
        utsname data{};
        auto    e = sys_uname(&data);
        if (e) return e;

        size_t hostname_size = strlen(data.nodename);
        memcpy(buffer, data.nodename, std::min(bufsize, hostname_size));
        return 0;
    }
}; // namespace mlibc
