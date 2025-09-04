#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <cryptix/syscall.h>

namespace mlibc
{
    int sys_sigaction(int signum, const struct sigaction* __restrict act,
                      struct sigaction* __restrict oldact)
    {
        auto ret = Syscall(SYS_SIGACTION, signum, act, oldact);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_sigprocmask(int how, const sigset_t* set, sigset_t* retrieve)
    {
        auto ret = Syscall(SYS_SIGPROCMASK, how, set, retrieve);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
}; // namespace mlibc
