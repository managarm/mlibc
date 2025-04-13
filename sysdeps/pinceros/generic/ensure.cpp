
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>

#include <aarch64/syscall.h>

namespace mlibc
{
    void sys_libc_log(const char *message)
    {
        ssize_t n = 0;
        while (message[n])
            n++;

        ssize_t total_written = 0;
        while (total_written < n)
        {
            ssize_t bytes_written = 0;
            if (sys_pwrite(2, message + total_written, n - total_written, 0, &bytes_written))
            {
                sys_libc_panic();
            }
            total_written += bytes_written;
        }
        char lf = '\n';
        ssize_t bytes_written = 0;
        if (sys_pwrite(2, &lf, 1, 0, &bytes_written))
        {
            sys_libc_panic();
        }
    }

    [[noreturn]] void sys_libc_panic()
    {
        __builtin_trap();
    }
}