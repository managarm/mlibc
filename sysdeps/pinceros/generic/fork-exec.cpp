#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc
{

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

    [[noreturn]] void sys_exit(int /* status */)
    {
        MLIBC_UNIMPLEMENTED();
        __builtin_unreachable();
    }

    int sys_tcb_set(void * /* pointer */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
} // namespace mlibc
