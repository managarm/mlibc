#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <aarch64/syscall.h>

namespace mlibc
{
    extern "C" [[noreturn]] void pincer_shutdown()
    {
        __do_syscall0(SHUTDOWN);
        __builtin_unreachable();
    }

    int sys_futex_tid()
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    int sys_futex_wait(int * /* pointer */, int /* expected */, const struct timespec * /* time */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    int sys_futex_wake(int * /* pointer */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    [[noreturn]] void sys_exit(int status)
    {
        __do_syscall1(EXIT, status);
        __builtin_unreachable();
    }

    int sys_tcb_set(void * /* pointer */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
} // namespace mlibc
