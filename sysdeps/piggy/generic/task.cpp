#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>

#include <piggy/syscall.h>

#include <sys/mman.h>

#include <errno.h>

#define DEFAULT_STACK_SIZE 0x80000

namespace mlibc {
    int Sysdeps<Fork>::operator()(pid_t* pid) {
        long ret = syscall0(SYS_FORK);
        if (ret < 0) {
            return -ret;
        }

        *pid = ret;
        return 0;
    }

    int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
        long ret = syscall3(SYS_EXEC, (long) path, (long) argv, (long) envp);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Waitpid>::operator()(pid_t pid, int* status, int flags, struct rusage* ru, pid_t* ret_pid) {
        (void) ru;

        long ret = syscall3(SYS_WAIT, pid, (long) status, flags);
        if (ret < 0) {
            return -ret;
        }

        *ret_pid = ret;
        return 0;
    }

    int Sysdeps<Kill>::operator()(pid_t pid, int signal) {
        long ret = syscall2(SYS_KILL, pid, signal);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    pid_t Sysdeps<GetPid>::operator()(void) {
        return syscall0(SYS_GETPID);
    }

    pid_t Sysdeps<GetPpid>::operator()(void) {
        return syscall0(SYS_GETPPID);
    }

    pid_t Sysdeps<GetPgid>::operator()(pid_t pid, pid_t* pgid) {
        long ret = syscall1(SYS_GETPGID, pid);
        if (ret < 0) {
            return -ret;
        }

        *pgid = ret;
        return 0;
    }

    int Sysdeps<SetPgid>::operator()(pid_t pgid, pid_t pid) {
        long ret = syscall2(SYS_SETPGID, pid, pgid);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

#ifndef MLIBC_BUILDING_RTLD
    extern "C" void __mlibc_thread_entry();

    int Sysdeps<Clone>::operator()(void* tcb, pid_t* pid_out, void* stack) {
        (void) tcb;

        long ret = syscall2(SYS_THREADNEW, (long) __mlibc_thread_entry, (long) stack);
        if (ret < 0) {
            return -ret;
        }

        *pid_out = ret;
        return 0;
    }

    [[noreturn]] void Sysdeps<ThreadExit>::operator()(void) {
        syscall0(SYS_THREADEXIT);
        __builtin_unreachable();
    }

    pid_t Sysdeps<GetTid>::operator()(void) {
        return syscall0(SYS_GETTID);
    }

    void Sysdeps<Yield>::operator()(void) {
        syscall0(SYS_YIELD);
    }
#endif

    extern "C" void __mlibc_thread_trampoline(void* (*fn)(void*), Tcb* tcb, void* arg) {
        if (mlibc::sysdep<TcbSet>(tcb)) {
            __ensure(!"failed to set tcb for new thread");
        }

        while (__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED) == 0) {
            mlibc::sysdep<FutexWait>(&tcb->tid, 0, nullptr);
        }

        tcb->invokeThreadFunc(reinterpret_cast<void*>(fn), arg);

        __atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
        mlibc::sysdep<FutexWake>(&tcb->didExit, true);

        mlibc::sysdep<ThreadExit>();
    }

    int Sysdeps<PrepareStack>::operator()(void** stack, void* entry, void* arg, void* tcb, size_t* stack_size, size_t* guard_size, void** stack_base) {
        *guard_size = 0;

        *stack_size = *stack_size ? *stack_size : DEFAULT_STACK_SIZE;

        if(!*stack) {
            *stack_base = mmap(NULL, *stack_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            if(*stack_base == MAP_FAILED) {
                return errno;
            }
        } else {
            *stack_base = *stack;
        }

        *stack = (void *) ((char*) *stack_base + *stack_size);

        void** stack_it = (void**) *stack;
        *--stack_it = arg;
        *--stack_it = tcb;
        *--stack_it = entry;

        *stack = (void *) stack_it;

        return 0;
    }
} // namespace mlibc
