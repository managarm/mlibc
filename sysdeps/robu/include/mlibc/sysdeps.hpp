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
	Utimensat
{};

template<typename Tag>
using Sysdeps = SysdepOf<RobuSysdepTags, Tag>;

struct SysdepTraits {
	static constexpr bool usesRtNetlink = false;
};

}
