#include <mlibc/all-sysdeps.hpp>

#include <piggy/syscall.h>

namespace mlibc {
#ifndef MLIBC_BUILDING_RTLD
    extern "C" void __mlibc_restorer();

    int Sysdeps<Sigaction>::operator()(int signal, const struct sigaction* __restrict act, struct sigaction* __restrict oldact) {
        if (act != nullptr) {
            struct sigaction modified = *act;
            modified.sa_restorer = __mlibc_restorer;

            long ret = syscall3(SYS_SIGACTION, signal, (long) &modified, (long) oldact);
            if (ret < 0) {
                return -ret;
            }
            return 0;
        }

        long ret = syscall3(SYS_SIGACTION, signal, 0, (long) oldact);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }
#endif

    int Sysdeps<Sigaltstack>::operator()(const stack_t* __restrict ss, stack_t* __restrict oldss) {
        long ret = syscall2(SYS_SIGALTSTACK, (long) ss, (long) oldss);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Sigpending>::operator()(sigset_t* set) {
        long ret = syscall1(SYS_SIGPENDING, (long) set);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t* __restrict set, sigset_t* __restrict retrieve) {
        long ret = syscall3(SYS_SIGPROCMASK, how, (long) set, (long) retrieve);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Sigsuspend>::operator()(const sigset_t* mask) {
        long ret = syscall1(SYS_SIGSUSPEND, (long) mask);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Pause>::operator()(void) {
        sigset_t mask;

        int ret = sysdep<Sigprocmask>(0, NULL, &mask);
        if (ret != 0) {
            return ret;
        }

        return sysdep<Sigsuspend>(&mask);
    }
} // namespace mlibc
