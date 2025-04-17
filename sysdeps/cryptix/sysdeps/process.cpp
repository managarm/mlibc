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

        *ret_pid = ret;
        return 0;
    }

    uid_t sys_getuid() { return Syscall(SYS_GETUID); }
    gid_t sys_getgid() { return Syscall(SYS_GETGID); }
    uid_t sys_geteuid() { return Syscall(SYS_GETEUID); }
    gid_t sys_getegid() { return Syscall(SYS_GETEGID); }
    int   sys_setpgid(pid_t pid, pid_t pgid)
    {
        auto ret = Syscall(SYS_SETPGID, pid, pgid);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    pid_t sys_getppid() { return Syscall(SYS_GETPPID); }
    pid_t sys_setsid(pid_t* out)
    {
        auto ret = Syscall(SYS_SETSID);
        if (auto e = syscall_error(ret); e) return e;

        *out = ret;
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

    int sys_fork(pid_t* child)
    {
        auto ret = Syscall(SYS_FORK);
        if (auto e = syscall_error(ret); e) return e;

        *child = ret;
        return 0;
    }
    int sys_execve(const char* path, char* const argv[], char* const envp[])
    {
        return Syscall(SYS_EXECVE, path, argv, envp);
    }

    int sys_futex_tid()
    {
        // TODO(v1tr10l7): implement sys_futex_tid
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
