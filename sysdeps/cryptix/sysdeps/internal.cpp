#include <cstddef>
#include <stdio.h>
#include <stdlib.h>

#include <cryptix/syscall.h>

#include <execinfo.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <sys/mman.h>

namespace mlibc
{
    void sys_libc_log(const char* message)
    {
        static constexpr char MLIBC_SIG[] = "[mlibc]: ";

        size_t                len         = strlen(message) + 1;
        char*                 messageEOL
            = reinterpret_cast<char*>(alloca(len + sizeof(MLIBC_SIG)));

        memcpy(messageEOL, MLIBC_SIG, sizeof(MLIBC_SIG) - 1);
        memcpy(messageEOL + sizeof(MLIBC_SIG) - 1, message, len);
        len += sizeof(MLIBC_SIG) - 1;

        messageEOL[len - 1] = '\n';
        Syscall(SYS_WRITE, 1, messageEOL, len);
    }
    [[noreturn]] void sys_libc_panic()
    {
        Syscall(SYS_PANIC, "mlibc crashed");

        __builtin_unreachable();
    }
} // namespace mlibc
