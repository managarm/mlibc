#include <stdint.h>
#include <errno.h>

#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/internal-sysdeps.hpp>

#include <obos/syscall.h>
#include <obos/error.h>
#include <obos/vma.h>

#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>

namespace [[gnu::visibility("hidden")]] mlibc {

void sys_libc_log(char const* str)
{
    syscall1(Sys_LibCLog, str);
}

[[noreturn]] void sys_libc_panic()
{
    sys_libc_log("mlibc panicked! exiting program...");
    sys_exit(-1);
    __builtin_unreachable();
}

[[noreturn]] void sys_exit(int status)
{
    (void)(status);
    syscall0(Sys_ExitCurrentThread);
    __builtin_unreachable();
}

[[noreturn]] void sys_thread_exit()
{
    sys_exit(0);
}

int sys_anon_allocate(size_t sz, void **pointer)
{
    struct
    {
        uint32_t prot;
        uint32_t flags;
        handle file;
        uintptr_t offset;
    } extra_args = {0,0,HANDLE_INVALID};
    *pointer = (void*)syscall5(Sys_VirtualMemoryAlloc, HANDLE_CURRENT, NULL, sz, &extra_args, NULL);
    return 0;
}

int sys_anon_free(void *blk, size_t sz)
{
    return syscall3(Sys_VirtualMemoryFree, HANDLE_CURRENT, blk, sz) == 0 ? 0 : ENOSYS;
}

int sys_futex_wait(int *pointer, int expected, const struct timespec *time)
{
    obos_status status = (obos_status)syscall3(Sys_FutexWait, pointer, expected, time ? time->tv_sec*1000 : UINT64_MAX);
    switch (status)
    {
        case OBOS_STATUS_SUCCESS:
            return 0;
        case OBOS_STATUS_INVALID_ARGUMENT:
            return EINVAL;
        case OBOS_STATUS_RETRY:
        case OBOS_STATUS_TIMED_OUT:
            return EAGAIN;
        case OBOS_STATUS_ABORTED:
            return EINTR;
         default: sys_libc_panic(); // TODO: Generic errno value?
   }
}

int sys_futex_wake(int *pointer)
{
    obos_status status = (obos_status)syscall2(Sys_FutexWake, pointer, 1);
    switch (status)
    {
        case OBOS_STATUS_SUCCESS:
            return 0;
        case OBOS_STATUS_INVALID_ARGUMENT:
            return EINVAL;
        default: return ENOSYS;
    }
}

int sys_clock_get(int clock, time_t *secs, long *nanos)
{
    return ENOSYS;
}

static int parse_file_status(obos_status status)
{
    switch (status)
    {
        case OBOS_STATUS_SUCCESS: return 0;
        case OBOS_STATUS_NOT_FOUND: return ENOENT;
        case OBOS_STATUS_INVALID_ARGUMENT: return EINVAL;
        case OBOS_STATUS_PAGE_FAULT: return EFAULT;
        case OBOS_STATUS_NOT_A_FILE: return EISDIR;
        case OBOS_STATUS_UNINITIALIZED: return EBADF;
        case OBOS_STATUS_EOF: return EIO;
        case OBOS_STATUS_ACCESS_DENIED: return EACCES;
        default: sys_libc_panic();
    }
}

int sys_open(const char *pathname, int flags, mode_t mode, int *fd)
{
    handle hnd = (handle)syscall0(Sys_FdAlloc);
    // TODO: mode?
    (void)(mode);
    uint32_t real_flags = 0;

    if (~flags & O_WRONLY)
        real_flags |= 1 /*FD_OFLAGS_READ*/;
    if (flags & O_RDWR)
        real_flags |= 1|2 /*FD_OFLAGS_READ|FD_OFLAGS_WRITE*/;
    if (flags & O_CLOEXEC)
        real_flags |= 8 /* FD_OFLAGS_NOEXEC */;
    if (flags & O_DIRECT)
        real_flags |= 4 /* FD_OFLAGS_UNCACHED */;

    obos_status st = (obos_status)syscall3(Sys_FdOpen, hnd, pathname, real_flags);
    if (int ec = parse_file_status(st); st != 0)
        return ec;
    *fd = hnd;

    return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read)
{
    return parse_file_status((obos_status)syscall4(Sys_FdRead, fd, buf, count, bytes_read));
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written)
{
    return parse_file_status((obos_status)syscall4(Sys_FdWrite, fd, buf, count, bytes_written));
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset)
{
    if (offset == 0)
    {
        *new_offset = syscall1(Sys_FdTellOff, fd);
        if (*new_offset == -1)
            return EBADF;
    }
    if (whence > SEEK_DATA)
        return ENOSYS;
    obos_status st = (obos_status)syscall3(Sys_FdSeek, fd, offset, whence);
    if (int ec = parse_file_status(st); ec)
        return ec;
    *new_offset = syscall1(Sys_FdTellOff, fd);
    if (*new_offset == -1)
        return EBADF;
    return 0;
}

int sys_close(int fd)
{
    return parse_file_status((obos_status)syscall1(Sys_HandleClose, fd));
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window)
{
    unsigned real_flags = 0;
    unsigned prot_flags = 0;
    if (prot == PROT_NONE)
        real_flags |= VMA_FLAGS_RESERVE;
    if ((prot & PROT_READ) && (~prot & PROT_WRITE))
        prot_flags |= OBOS_PROTECTION_READ_ONLY;
    if (prot & PROT_EXEC)
        prot_flags |= OBOS_PROTECTION_EXECUTABLE;

    if (flags & MAP_PRIVATE)
        real_flags |= VMA_FLAGS_PRIVATE;
    //if (flags & MAP_ANON)
    //    real_flags = 0;
    if (flags & MAP_STACK)
        real_flags |= VMA_FLAGS_GUARD_PAGE;
    if (flags & MAP_HUGETLB)
        real_flags |= VMA_FLAGS_HUGE_PAGE;
    if ((~flags & MAP_FIXED) && (~flags & MAP_FIXED_NOREPLACE))
        real_flags |= VMA_FLAGS_HINT;
    if (flags & MAP_POPULATE)
        real_flags |= VMA_FLAGS_PREFAULT;

    struct
    {
        uint32_t prot;
        uint32_t flags;
        handle file;
        uintptr_t offset;
    } extra_args = { prot_flags, real_flags, fd, offset };

    obos_status status = OBOS_STATUS_SUCCESS;

    *window = (void*)syscall5(Sys_VirtualMemoryAlloc, HANDLE_CURRENT, hint, size, &extra_args, &status);
    switch (status)
    {
        case OBOS_STATUS_SUCCESS: return 0;
        case OBOS_STATUS_INVALID_ARGUMENT: return EINVAL;
        case OBOS_STATUS_IN_USE: return EEXIST;
        case OBOS_STATUS_UNINITIALIZED: return EACCES;
        case OBOS_STATUS_NOT_ENOUGH_MEMORY: return ENOMEM;
        default: return ENOSYS;
    }
}

int sys_vm_unmap(void *pointer, size_t size)
{
    return sys_anon_free(pointer, size);
}

} // namespace mlibc
