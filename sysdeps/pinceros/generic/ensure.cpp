
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc
{
    void sys_libc_log(const char * /* message */)
    {
        MLIBC_UNIMPLEMENTED();
    }

    [[noreturn]] void sys_libc_panic()
    {
        MLIBC_UNIMPLEMENTED();
        __builtin_unreachable();
    }
}