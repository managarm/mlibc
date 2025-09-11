#include <cryptix/syscall.h>

#include <execinfo.h>
#include <mlibc/posix-sysdeps.hpp>

#include <sys/mman.h>

namespace mlibc
{
    int sys_sleep(time_t* secs, long* nanos)
    {
        timespec duration = {.tv_sec = *secs, .tv_nsec = nanos ? *nanos : 0};
        timespec remaining;
        auto     ret = Syscall(SYS_NANOSLEEP, &duration, &remaining);

        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_clock_get(int clockid, time_t* secs, long* nanos)
    {
        timespec ts;
        auto     ret = Syscall(SYS_CLOCK_GETTIME, clockid, &ts);
        if (auto e = syscall_error(ret); e) return e;

        *secs  = ts.tv_sec;
        *nanos = ts.tv_nsec;
        return 0;
    }
}; // namespace mlibc
