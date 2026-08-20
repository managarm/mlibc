#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct RobuSysdepTags :
	LibcPanic,
	LibcLog,
	Isatty,
	Write,
	TcbSet,
	AnonAllocate,
	AnonFree,
	Seek,
	Exit,
	Close,
	FutexWake,
	FutexWait,
	Read,
	Open,
	VmMap,
	VmUnmap,
	ClockGet,
	Stat,
	OpenDir,
	ReadEntries,
	GetCwd,
	Chdir,
	Waitpid,
	Utimensat,
	Fork,
	FutexTid,
	GetPid,
	Pipe,
	Dup,
	Dup2,
	Execve,
	Sigaction,
	Kill,
	Tgkill,
	Sigprocmask,
	Sigpending,
	Sigsuspend,
	Tcgetattr,
	Tcsetattr,
	Tcgetwinsize,
	Tcsetwinsize,
	Ioctl,
	SetPgid,
	GetPgid,
	SetSid,
	Pselect,
	GetUid,
	GetEuid,
	GetGid,
	GetEgid,
	SetUid,
	SetGid,
	Mkdir,
	Rmdir,
	Unlinkat,
	Link,
	Mknodat,
	Sync,
	Sleep,
	GetPpid,
	Ttyname,
	GetResuid,
	GetResgid,
	GetHostname,
	GetGroups,
	Fcntl
{};

template<typename Tag>
using Sysdeps = SysdepOf<RobuSysdepTags, Tag>;

struct SysdepTraits {
	static constexpr bool usesRtNetlink = false;
};

}
