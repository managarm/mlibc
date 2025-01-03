#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <frg/logging.hpp>
#include <mlibc/debug.hpp>

#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/internal-sysdeps.hpp>

#include <obos/syscall.h>
#include <obos/error.h>
#include <obos/vma.h>

#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>

namespace mlibc {

#define define_stub(signature, ret) \
signature {\
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" \
                        << frg::endlog; \
    return (ret); \
}

static int interpret_signal_status(obos_status st)
{
    switch (st)
    {
        case OBOS_STATUS_SUCCESS: return 0;
        case OBOS_STATUS_INVALID_ARGUMENT: return EINVAL;
        default: return ENOSYS;
    }
}

int sys_sigprocmask(int how, const sigset_t *__restrict set,
		sigset_t *__restrict retrieve)
{
    if (how > 3)
        return EINVAL;
    // 'how' in oboskrnl has the same values as in Linux (the abi we "borrow" from).
    return interpret_signal_status((obos_status)syscall3(Sys_SigProcMask, how, set, retrieve));
}

typedef struct user_sigaction {
    union {
        void(*handler)(int signum);
        void(*fn_sigaction)(int signum, siginfo_t* info, void* unknown);
    } un;
    // NOTE(oberrow): Set to __mlibc_restorer in the mlibc sysdeps.
    uintptr_t trampoline_base; // required
    uint32_t  flags;
} user_sigaction;

int sys_sigaction(int sigval, const struct sigaction *__restrict newact_mlibc,
		struct sigaction *__restrict oldact_mlibc)
{
    user_sigaction newact = {}, oldact = {};
    if (newact_mlibc)
    {
        newact.un.handler = newact_mlibc->sa_handler;
        newact.flags = newact_mlibc->sa_flags;
        newact.trampoline_base = (uintptr_t)newact_mlibc->sa_restorer;
    }
    int err = interpret_signal_status((obos_status)syscall3(Sys_SigAction, sigval, newact_mlibc ? &newact : nullptr, oldact_mlibc ? &oldact : nullptr));
    if (oldact_mlibc && !err)
    {
        oldact_mlibc->sa_handler = oldact.un.handler;
        oldact_mlibc->sa_flags = oldact.flags;
        oldact_mlibc->sa_restorer = (void(*)())oldact.trampoline_base;
    }
    return err;
}

int sys_sigaltstack(const stack_t *ss, stack_t *oss)
{
    // stack_t is the same on obos and on Linux (the abi we "borrow" from)
    return interpret_signal_status((obos_status)syscall2(Sys_SigAltStack, ss, oss));
}

int sys_kill(int tid, int sigval)
{
    if (tid == -1)
        return ENOSYS;
    if (tid == 0)
        return ENOSYS;
    if (tid < -1)
        tid = -tid;
    // TODO: Support opening the threads of other processes?
    handle hnd = (handle)syscall2(Sys_ThreadOpen, HANDLE_CURRENT, tid);
    if (hnd == HANDLE_INVALID)
        return ESRCH;
    int err = interpret_signal_status((obos_status)syscall2(Sys_Kill, hnd, sigval));
    syscall1(Sys_HandleClose, hnd);
    return err;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid)
{
    (void)(pid);
    (void)(status);
    (void)(flags);
    (void)(ru);
    (void)(ret_pid);
    // TODO(oberrow): sys_waitpid
    return ENOSYS;
}

//define_stub(int sys_isatty(int fd), 1)

// Architecture-specific.
//int sys_fork(pid_t* child)

void sys_libc_log(char const* str)
{
    syscall1(Sys_LibCLog, str);
}

int sys_isatty(int fd)
{
    if (fd <= 2) return 0;
    return 1;
}

uid_t sys_getuid()
{ return 0; }
uid_t sys_geteuid()
{ return 0; }

gid_t sys_getgid()
{ return 0; }
gid_t sys_getegid() { return 0; }

pid_t sys_gettid()
{
    return (pid_t)syscall1(Sys_ThreadGetTid, HANDLE_CURRENT);
}
pid_t sys_getpid()
{
    return (pid_t)syscall1(Sys_ProcessGetTid, HANDLE_CURRENT);
}
pid_t sys_getppid()
{
    return (pid_t)syscall1(Sys_ProcessGetPPID, HANDLE_CURRENT);
}

// TODO: Progress group IDs
int sys_getpgid(pid_t pid, pid_t* pgid)
{
    *pgid = pid;
    return 0;
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
    for (volatile bool b = true; b; )
        asm volatile ("" :"=r"(b) : "r"(b) : "memory");
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

#ifndef MLIBC_BUILDING_RTLD
static char* cwd;
static size_t sz_cwd;
static handle cwd_hnd = HANDLE_INVALID;

int sys_getcwd(char *buffer, size_t size)
{
    if (cwd == NULL)
    {
        if (size < 2)
            return ERANGE;
        memcpy(buffer, "/\0", 2);
        return 0;
    }
    if (size < sz_cwd)
        return ERANGE;
    memcpy(buffer, cwd, sz_cwd);
    return 0;
}

int sys_chdir(const char *path)
{
    if (cwd_hnd != HANDLE_INVALID)
        syscall1(Sys_HandleClose, cwd_hnd);

    obos_status status = OBOS_STATUS_SUCCESS;
    cwd_hnd = syscall2(Sys_OpenDir, path, &status);
    if (int ec = parse_file_status(status); ec)
        return ec;
    // Keep the directory open to prevent anyone from deleting it while our CWD
    // is set to it.

    auto old_errno = errno;
    free(cwd);
    sz_cwd = strlen(path)+1;
    cwd = (char*)malloc(sz_cwd);
    memcpy(cwd, path, sz_cwd);
    errno = old_errno;
    return 0;
}
#else
static handle cwd_hnd = HANDLE_INVALID;
#endif

int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events)
{
    *num_events = 1;
    return 0;
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

    obos_status st = OBOS_STATUS_SUCCESS;

    if (cwd_hnd == HANDLE_INVALID || pathname[0] == '/')
        st = (obos_status)syscall3(Sys_FdOpen, hnd, pathname, real_flags);
    else
        st = (obos_status)syscall4(Sys_FdOpenAt, hnd, cwd_hnd, pathname, real_flags);
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
    } extra_args = { prot_flags, real_flags, (handle)fd, offset };

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
