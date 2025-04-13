#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <aarch64/syscall.h>

extern "C"
{
    int dup3(int oldfd, int newfd, int flags)
    {
        if (oldfd == newfd)
        {
            errno = EINVAL;
            return -1;
        }
        MLIBC_CHECK_OR_ENOSYS(mlibc::sys_dup2, -1);
        if (int e = mlibc::sys_dup2(oldfd, flags, newfd); e)
        {
            errno = e;
            return -1;
        }
        return newfd;
    }

    [[noreturn]] void pinceros_shutdown()
    {
        __do_syscall0(SHUTDOWN);
        __builtin_unreachable();
    }
}