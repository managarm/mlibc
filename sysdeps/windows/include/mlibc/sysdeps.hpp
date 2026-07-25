#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct WindowsSysdepTags :
	LibcPanic,
	LibcLog,
	FutexTid,
	Isatty,
	Write,
	TcbSet,
	TcbGet,
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
	ClockGet
{};

template<typename Tag>
using Sysdeps = SysdepOf<WindowsSysdepTags, Tag>;

struct SysdepTraits {
	static constexpr bool usesRtNetlink = false;
};

} // namespace mlibc
