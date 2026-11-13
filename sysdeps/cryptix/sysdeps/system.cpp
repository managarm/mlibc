#include <algorithm>
#include <cryptix/syscall.h>
#include <errno.h>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/arch-defs.hpp>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace mlibc {
int Sysdeps<GetRlimit>::operator()(int resource, struct rlimit *rlim) {
	auto ret = Syscall(SYS_GETRLIMIT, resource, rlim);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<GetRusage>::operator()(int who, struct rusage *usage) {
	auto ret = Syscall(SYS_GETRUSAGE, who, usage);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<Sysinfo>::operator()(struct sysinfo *info) {
	auto ret = Syscall(SYS_SYSINFO, info);
	if (auto e = syscall_error(ret); e)
		return e;
	return 0;
}
int Sysdeps<Reboot>::operator()(int what) {
	auto ret = Syscall(SYS_REBOOT, what);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}
int Sysdeps<InitModule>::operator()(void *module, unsigned long length, const char *args) {
	auto ret = Syscall(SYS_INIT_MODULE, module, length, args);
	if (auto e = syscall_error(ret); e)
		return e;

	return 0;
}

#ifndef MLIBC_BUILDING_RTLD
int Sysdeps<Sysconf>::operator()(int num, long *ret) {
	switch (num) {
		case _SC_OPEN_MAX: {
			struct rlimit ru;
			if (int e = sysdep<GetRlimit>(RLIMIT_NOFILE, &ru); e)
				return e;
			*ret = (ru.rlim_cur == RLIM_INFINITY) ? -1 : ru.rlim_cur;
			break;
		}
		case _SC_PHYS_PAGES: {
			struct sysinfo info;
			if (int e = sysdep<Sysinfo>(&info); e)
				return e;
			unsigned unit = (info.mem_unit) ? info.mem_unit : 1;
			*ret = std::min(long((info.totalram * unit) / mlibc::page_size), LONG_MAX);
			break;
		}
		case _SC_CHILD_MAX: {
			struct rlimit ru;
			if (int e = sysdep<GetRlimit>(RLIMIT_NPROC, &ru); e)
				return e;
			*ret = (ru.rlim_cur == RLIM_INFINITY) ? -1 : ru.rlim_cur;
			break;
		}
		case _SC_LINE_MAX:
			*ret = -1;
			break;
		default:
			return EINVAL;
	}

	return 0;
}
#endif
} // namespace mlibc
