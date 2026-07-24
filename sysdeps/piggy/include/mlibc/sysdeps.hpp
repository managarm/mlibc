#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct PiggySysdepTags :
    // file.cpp
    OpenDir,
    Mkdirat,
    Mkdir,
    Renameat,
    Rename,
    Linkat,
    Link,
    Symlinkat,
    Symlink,
    Readlinkat,
    Readlink,
    Unlinkat,
    Rmdir,
    Pread,
    Pwrite,
    Ioctl,
    Ftruncate,
    Ppoll,
    Poll,
    Pselect,
    Fsync,
    Sync,
    ReadEntries,
    Stat,
    Utimensat,
    Fchdir,
    Chdir,
    Fcntl,
    Dup,
    Dup2,
    Chroot,
    Pipe,
    Isatty,
    Ttyname,
    Tcgetattr,
    Tcsetattr,
    Tcgetwinsize,
    Tcsetwinsize,

    // internal.cpp
    LibcLog,
    LibcPanic,
    TcbSet,
    FutexTid,
    FutexWait,
    FutexWake,
    Exit,
    Openat,
    Open,
    Close,
    Read,
    Write,
    Seek,
    VmMap,
    VmUnmap,
    VmProtect,
    AnonAllocate,
    AnonFree,

    // misc.cpp
    GetHostname,
    SetHostname,
    Uname,
    Access,
    GetUid,
    GetEuid,
    GetGid,
    GetEgid,
    GetResuid,
    GetResgid,

    // signal.cpp
    Sigaction,
    Sigaltstack,
    Sigpending,
    Sigprocmask,
    Sigsuspend,
    Pause,

    // socket.cpp
    Socket,
    Bind,
    Connect,
    Recvfrom,
    Sendto,
    Sockname,
    Peername,
    Shutdown,

    // task.cpp
    Fork,
    Execve,
    Waitpid,
    Kill,
    GetPid,
    GetPpid,
    GetPgid,
    SetPgid,
    Clone,
    ThreadExit,
    GetTid,
    Yield,
    PrepareStack,

    // time.cpp
    ClockGet,
    ClockGetres,
    ClockSet,
    Sleep
{};

template<typename Tag>
using Sysdeps = SysdepOf<PiggySysdepTags, Tag>;

struct SysdepTraits {
    static constexpr bool usesRtNetlink = false;
};

} // namespace mlibc
