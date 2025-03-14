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
#include <abi-bits/vm-flags.h>

namespace mlibc {

static int parse_file_status(obos_status status);

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

int sys_kill(pid_t pid, int sigval)
{
    if (pid == -1)
        return ENOSYS;
    if (pid == 0)
        return ENOSYS;
    if (pid < -1)
        pid = -pid;
    handle hnd = (handle)syscall1(Sys_ProcessOpen, pid);
    if (hnd == HANDLE_INVALID)
        return ESRCH;
    int err = interpret_signal_status((obos_status)syscall2(Sys_KillProcess, hnd, sigval));
    syscall1(Sys_HandleClose, hnd);
    return err;
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid)
{
    // TODO(oberrow): struct rusage and pid values that are < -1 or zero
    if (ru)
    {
        mlibc::infoLogger() << "mlibc: " << __func__ << "struct rusage in sys_waitpid is unsupported" << frg::endlog;
        return ENOSYS;
    }
    if (pid < -1 || pid == 0)
    {
        mlibc::infoLogger() << "mlibc:" << __func__ << " pid value " << pid << " is unsupported" << frg::endlog;
        return ENOSYS;
    }
    int ec = 0;
    handle hnd = pid == -1 ? HANDLE_ANY : (handle)syscall1(Sys_ProcessOpen, pid);
    if (hnd == HANDLE_INVALID)
        return ESRCH;
    obos_status st = (obos_status)syscall4(Sys_WaitProcess, hnd, status, flags, ret_pid);
    switch (st)
    {
        case OBOS_STATUS_INVALID_ARGUMENT: ec = EINVAL; goto exit;
        case OBOS_STATUS_ABORTED: ec = EINTR; goto exit;
        case OBOS_STATUS_NOT_FOUND: ec = ESRCH; goto exit;
        default: break;
    }
    exit:
    if (hnd != HANDLE_ANY)
        syscall1(Sys_HandleClose, hnd);
    return ec;
}

//define_stub(int sys_isatty(int fd), 1)

// Architecture-specific.
//int sys_fork(pid_t* child)

int sys_execve(const char *path, char *const argv[], char *const envp[])
{
    obos_status st = (obos_status)syscall3(Sys_ExecVE, path, argv, envp);
    switch (st)
    {
        case OBOS_STATUS_UNIMPLEMENTED: return ENOSYS;
        case OBOS_STATUS_INVALID_ARGUMENT: return EINVAL;
        case OBOS_STATUS_PAGE_FAULT: return EFAULT;
        case OBOS_STATUS_INVALID_FILE: return ENOEXEC;
        case OBOS_STATUS_NOT_FOUND: return ENOENT;
        default: return parse_file_status(st);
    }
}

void sys_libc_log(char const* str)
{
    syscall1(Sys_LibCLog, str);
}

int sys_isatty(int fd)
{
    if (fd <= 2) return 0;
    return ENOTTY;
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
    return (pid_t)syscall1(Sys_ProcessGetPID, HANDLE_CURRENT);
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
    syscall1(Sys_ExitCurrentProcess, status);
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
#if defined(__x86_64__)
	enum { SysS_ClockGet = 0x80000002 };
	// NOTE(oberrow): This clock is not precise.
	obos_status st = (obos_status)syscall3(SysS_ClockGet, clock, secs, nanos);
	switch (st)
	{
		case OBOS_STATUS_INVALID_ARGUMENT: return EINVAL;
		case OBOS_STATUS_PAGE_FAULT: return EFAULT;
		case OBOS_STATUS_SUCCESS: return 0;
		default: return ENOSYS;
	}
#else
	return ENOSYS;
#endif
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
        case OBOS_STATUS_NO_SYSCALL: return ENOSYS;
        case OBOS_STATUS_NOT_ENOUGH_MEMORY: return ENOSPC;
        case OBOS_STATUS_ALREADY_INITIALIZED: return EEXIST;
        case OBOS_STATUS_PIPE_CLOSED: return EPIPE;
        case OBOS_STATUS_ALREADY_MOUNTED: return EBUSY;
        default: sys_libc_panic();
    }
}

int sys_open_dir(const char *path, int *hnd)
{
    obos_status st = OBOS_STATUS_SUCCESS;
    handle dir = (handle)syscall2(Sys_OpenDir, path, &st);
    if (obos_is_error(st))
        return parse_file_status(st);
    *hnd = (int)dir;
    return 0;
}

int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read)
{
    return parse_file_status((obos_status)syscall4(Sys_ReadEntries, handle, buffer, max_size, bytes_read));
}

int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events)
{
    *num_events = 1;
    return 0;
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd)
{
    (void)mode;
    int real_flags = 0;

    if (~flags & O_WRONLY)
        real_flags |= 1 /*FD_OFLAGS_READ*/;
    if (flags & O_RDWR)
        real_flags |= 1|2 /*FD_OFLAGS_READ|FD_OFLAGS_WRITE*/;
    if (flags & O_CLOEXEC)
        real_flags |= 8 /* FD_OFLAGS_NOEXEC */;
    if (flags & O_DIRECT)
        real_flags |= 4 /* FD_OFLAGS_UNCACHED */;
    if (flags & O_CREAT)
        real_flags |= 16 /* FD_OFLAGS_CREATE */;

    handle hnd = syscall0(Sys_FdAlloc);
    obos_status st = (obos_status)syscall5(Sys_FdOpenAtEx, hnd, dirfd, path, real_flags, mode);

    if (int ec = parse_file_status(st); ec != 0)
        return ec;
    *fd = hnd;

    return 0;
}

int sys_open(const char *pathname, int flags, mode_t mode, int *fd)
{
    handle hnd = (handle)syscall0(Sys_FdAlloc);
    // TODO: mode?
    (void)(mode);
    uint32_t real_flags = 0;

    if ((flags & 3) == O_RDONLY)
        real_flags |= 1 /*FD_OFLAGS_READ*/;
    if ((flags & 3) == O_WRONLY)
        real_flags |= 2 /*FD_OFLAGS_WRITE*/;
    if ((flags & 3) == O_RDWR)
        real_flags |= 1|2 /*FD_OFLAGS_READ|FD_OFLAGS_WRITE*/;
    if (flags & O_CLOEXEC)
        real_flags |= 8 /* FD_OFLAGS_NOEXEC */;
    if (flags & O_DIRECT)
        real_flags |= 4 /* FD_OFLAGS_UNCACHED */;
    if (flags & O_CREAT)
        real_flags |= 16 /* FD_OFLAGS_CREATE */;

    obos_status st = OBOS_STATUS_SUCCESS;

    st = (obos_status)syscall4(Sys_FdOpenEx, hnd, pathname, real_flags, mode);
    if (int ec = parse_file_status(st); ec != 0)
        return ec;
    *fd = hnd;

    return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf)
{
    return parse_file_status((obos_status)syscall5(Sys_Stat, fsfdt, fd, path, flags, statbuf));
}

int sys_mkdir(const char *path, mode_t mode)
{    
    return parse_file_status((obos_status)syscall2(Sys_Mkdir, path, mode));
}
int sys_mkdirat(int dirfd, const char *path, mode_t mode)
{
    return parse_file_status((obos_status)syscall3(Sys_MkdirAt, (handle)dirfd, path, mode));
}

typedef struct drv_fs_info {
    size_t fsBlockSize;
    size_t freeBlocks; // in units of 'fsBlockSize'

    size_t partBlockSize;
    size_t szFs; // in units of 'partBlockSize'

    size_t fileCount;
    size_t availableFiles; // the count of files that can be made until the partition cannot hold anymore

    size_t nameMax;

    uint32_t flags;
} drv_fs_info;

enum {
    FS_FLAGS_NOEXEC = 1<<0,
    FS_FLAGS_RDONLY = 1<<1,
};

int sys_statvfs(const char *path, struct statvfs *out)
{
    obos_status status = OBOS_STATUS_SUCCESS;
    handle hnd = (handle)syscall2(Sys_OpenDir, path, &status);
    if (status != OBOS_STATUS_SUCCESS)
        return parse_file_status(status);
    int ec = sys_fstatvfs(hnd, out);
    syscall1(Sys_HandleClose, hnd);
    return ec;
}

int sys_fstatvfs(int fd, struct statvfs *out)
{
    handle hnd = (handle)fd;
    if (HANDLE_TYPE(hnd) != 2 /* HANDLE_TYPE_DIRENT */)
        return EINVAL;
    if (!out)
        return EINVAL;

    drv_fs_info info = {};
    obos_status status = (obos_status)syscall2(Sys_StatFSInfo, hnd, &info);
    if (status != OBOS_STATUS_SUCCESS)
        return parse_file_status(status);

    out->f_bsize = info.fsBlockSize;
    out->f_bfree = info.freeBlocks;
    out->f_bavail = out->f_bfree;

    out->f_frsize = info.partBlockSize;
    out->f_blocks = info.szFs;

    out->f_files = info.fileCount;
    out->f_ffree = info.availableFiles;
    out->f_favail = out->f_ffree;

    out->f_fsid = 0;
    out->f_namemax = info.nameMax;
    out->f_flag = ST_NOSUID;

    if (info.flags & FS_FLAGS_RDONLY)
        out->f_flag |= ST_RDONLY;
#ifdef ST_NOEXEC
    if (info.flags & FS_FLAGS_NOEXEC)
        out->f_flag |= ST_NOEXEC;
#endif

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
    /*if (offset == 0 && whence == SEEK_CUR)
    {
        *new_offset = syscall1(Sys_FdTellOff, fd);
        if (*new_offset == -1)
            return EBADF;
    }*/
    if (whence > SEEK_DATA)
        return ENOSYS;
    obos_status st = (obos_status)syscall3(Sys_FdSeek, fd, offset, whence);
    if (int ec = parse_file_status(st); ec)
        return ec;
    if (new_offset)
    {
        *new_offset = syscall1(Sys_FdTellOff, fd);
        if (*new_offset == -1)
            return EBADF;
    }
    return 0;
}

int sys_close(int fd)
{
    return parse_file_status((obos_status)syscall1(Sys_HandleClose, fd));
}

void sys_sync()
{
    syscall0(Sys_Sync);
}

int sys_fsync(int fd)
{
    return parse_file_status((obos_status)syscall1(Sys_FdFlush, fd));
}

int sys_fdatasync(int fd)
{
    return sys_fsync(fd);
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
#if __MLIBC_LINUX_OPTION
    if (flags & MAP_STACK)
        real_flags |= VMA_FLAGS_GUARD_PAGE;
    if ((~flags & MAP_FIXED) && (~flags & MAP_FIXED_NOREPLACE))
        real_flags |= VMA_FLAGS_HINT;
    if (flags & MAP_POPULATE)
        real_flags |= VMA_FLAGS_PREFAULT;
    if (flags & MAP_HUGETLB)
        real_flags |= VMA_FLAGS_HUGE_PAGE;
#else
    if (~flags & MAP_FIXED)
        real_flags |= VMA_FLAGS_HINT;
#endif

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

int sys_sysconf(int num, long* ret)
{
    return syscall2(Sys_SysConf, num, ret) == OBOS_STATUS_SUCCESS ? 0 : ENOSYS;
}

void sys_yield()
{ syscall0(Sys_Yield); }

/*int sys_tcgetattr(int fd, struct termios *attr)
{
    infoLogger() << __func__ << " is unimplemented" << frg::endlog;
    return 0;
}
int sys_tcsetattr(int, int, const struct termios *attr)
{
    infoLogger() << __func__ << " is unimplemented" << frg::endlog;
    return 0;
}*/

} // namespace mlibc
