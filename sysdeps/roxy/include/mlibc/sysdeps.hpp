#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct RoxySysdepTags :
	LibcPanic,
	LibcLog,
	FutexWait,
	FutexWake,
	Open,
	OpenDir,
	ReadEntries,
	GetCwd,
	Chdir,
	Read,
	Close,
	ClockGet,
	Sleep,
	Isatty,
	Ioctl,
	Tcgetattr,
	Tcsetattr,
	Tcgetwinsize,
	Tcsetwinsize,
	Write,
	TcbSet,
	AnonAllocate,
	AnonFree,
	VmMap,
	VmUnmap,
	VmProtect,
	Stat,
	Seek,
	GetGid,
	GetEgid,
	GetUid,
	GetEuid,
	GetPid,
	GetPpid,
	Kill,
	Fork,
	Waitpid,
	Sigprocmask,
	Sigaction,
	Poll,
	Ppoll,
	Pselect,
	Uname,
	Mkdir,
	Mkdirat,
	Rmdir,
	Unlinkat,
	Readlink,
	Readlinkat,
	Link,
	Linkat,
	Symlink,
	Symlinkat,
	Rename,
	Renameat,
	Sync,
	Fsync,
	Execve,
	Exit
{};

template<typename Tag>
using Sysdeps = SysdepOf<RoxySysdepTags, Tag>;

struct SysdepTraits {
	static constexpr bool usesRtNetlink = false;
};

} // namespace mlibc
