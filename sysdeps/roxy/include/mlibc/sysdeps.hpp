#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct RoxySysdepTags :
	LibcPanic,
	LibcLog,
	FutexWait,
	FutexWake,
	Open,
	Read,
	Close,
	ClockGet,
	Isatty,
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
	Fork,
	Waitpid,
	Sigprocmask,
	Sigaction,
	Execve,
	Exit
{};

template<typename Tag>
using Sysdeps = SysdepOf<RoxySysdepTags, Tag>;

struct SysdepTraits {
	static constexpr bool usesRtNetlink = false;
};

} // namespace mlibc
