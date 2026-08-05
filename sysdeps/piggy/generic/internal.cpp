#include <mlibc/all-sysdeps.hpp>

#include <piggy/archctl.h>
#include <piggy/syscall.h>

#include <string.h>

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

namespace mlibc {
    void Sysdeps<LibcLog>::operator()(const char *message) {
        long ret;
        sysdep<Write>(2, message, strlen(message), &ret);
        sysdep<Write>(2, "\n", 1, &ret);
    }

    [[noreturn]] void Sysdeps<LibcPanic>::operator()() {
        sysdep<LibcLog>("mlibc: panic");
        sysdep<Exit>(1);
    }

    int Sysdeps<TcbSet>::operator()(void* pointer) {
        long ret = syscall2(SYS_ARCHCTL, ARCHCTL_SET_FSBASE, (long) pointer);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<FutexTid>::operator()(void) {
        return syscall0(SYS_GETTID);
    }

    int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec* time) {
        (void) time;

        long ret = syscall3(SYS_FUTEX, (long) pointer, FUTEX_WAIT, expected);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<FutexWake>::operator()(int* pointer, bool all) {
        long ret = syscall3(SYS_FUTEX, (long) pointer, FUTEX_WAKE, all ? UINT32_MAX : 1);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    [[noreturn]] void Sysdeps<Exit>::operator()(int status) {
        syscall1(SYS_EXIT, status);
        __builtin_unreachable();
    }

    int Sysdeps<Openat>::operator()(int dirfd, const char* path, int flags, mode_t mode, int* fd) {
        (void) mode;

        long ret = syscall3(SYS_OPEN, dirfd, (long) path, flags);
        if (ret < 0) {
            return -ret;
        }

        *fd = ret;
        return 0;
    }

    int Sysdeps<Open>::operator()(const char* pathname, int flags, mode_t mode, int* fd) {
        return sysdep<Openat>(AT_FDCWD, pathname, flags, mode, fd);
    }

    int Sysdeps<Close>::operator()(int fd) {
        long ret = syscall1(SYS_CLOSE, fd);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Read>::operator()(int fd, void* buf, size_t count, ssize_t* bytes_read) {
        long ret = syscall3(SYS_READ, fd, (long) buf, count);
        if (ret < 0) {
            return -ret;
        }

        *bytes_read = ret;
        return 0;
    }

    int Sysdeps<Write>::operator()(int fd, const void* buf, size_t count, ssize_t* bytes_written) {
        long ret = syscall3(SYS_WRITE, fd, (long) buf, count);
        if (ret < 0) {
            return -ret;
        }

        *bytes_written = ret;
        return 0;
    }

    int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t* new_offset) {
        long ret = syscall3(SYS_SEEK, fd, offset, whence);
        if (ret < 0) {
            return -ret;
        }

        *new_offset = ret;
        return 0;
    }

    int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char* path, int flags, struct stat* statbuf) {
        switch (fsfdt) {
            case fsfd_target::fd:
                flags |= AT_EMPTY_PATH;
                path = "";
                break;
            case fsfd_target::fd_path:
                break;
            case fsfd_target::path:
                fd = AT_FDCWD;
                break;
            default:
                __builtin_unreachable();
        }

        long ret = syscall4(SYS_STAT, fd, (long) path, (long) statbuf, flags);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<VmMap>::operator()(void* hint, size_t size, int prot, int flags, int fd, off_t offset, void** window) {
        long ret = syscall6(SYS_MMAP, (long) hint, size, prot, flags, fd, offset);
        if (ret < 0 && ret >= -4095) {
            return -ret;
        }

        *window = (void*) ret;
        return 0;
    }

    int Sysdeps<VmUnmap>::operator()(void* pointer, size_t size) {
        long ret = syscall2(SYS_MUNMAP, (long) pointer, size);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<VmProtect>::operator()(void* pointer, size_t size, int prot) {
        long ret = syscall3(SYS_MPROTECT, (long) pointer, size, prot);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<AnonAllocate>::operator()(size_t size, void** pointer) {
        size += 4096 - (size % 4096);
        return sysdep<VmMap>(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0, pointer);
    }

    int Sysdeps<AnonFree>::operator()(void* pointer, size_t size) {
        size += 4096 - (size % 4096);
        return sysdep<VmUnmap>(pointer, size);
    }
} // namespace mlibc
