#include <mlibc/all-sysdeps.hpp>

#include <piggy/syscall.h>

namespace mlibc {
    int Sysdeps<ClockGet>::operator()(int clock, time_t* secs, long* nanos) {
        struct timespec ts;

        long ret = syscall2(SYS_GETCLOCK, clock, (long) &ts);
        if (ret < 0) {
            return -ret;
        }

        *secs = ts.tv_sec;
        *nanos = ts.tv_nsec;
        return 0;
    }

    int Sysdeps<ClockGetres>::operator()(int clock, time_t* secs, long* nanos) {
        struct timespec ts;

        long ret = syscall2(SYS_GETCLOCKRES, clock, (long) &ts);
        if (ret < 0) {
            return -ret;
        }

        *secs = ts.tv_sec;
        *nanos = ts.tv_nsec;
        return 0;
    }

    int Sysdeps<ClockSet>::operator()(int clock, time_t secs, long nanos) {
        struct timespec ts = { .tv_sec = secs, .tv_nsec = nanos };

        long ret = syscall2(SYS_SETCLOCK, clock, (long) &ts);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Sleep>::operator()(time_t* secs, long* nanos) {
        struct timespec ts = {
            .tv_sec = *secs,
            .tv_nsec = *nanos,
        };

        long ret = syscall1(SYS_SLEEP, (long) &ts);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }
} // namespace mlibc
