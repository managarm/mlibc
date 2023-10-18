#include <unistd.h>

#include <hel.h>
#include <hel-syscalls.h>
#include <mlibc/debug.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/linux-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>

int sys_sysinfo(struct sysinfo *info) {
	mlibc::infoLogger() << "mlibc: sys_sysinfo top" << frg::endlog;
	// TODO: fill in missing fields
	uint64_t uptimeNanos;
	HEL_CHECK(helGetSystemInformation(&uptimeNanos));

	info->uptime = uptimeNanos / 1'000'000'000;
	return 0;
}

int sys_sysconf(int num, long *ret) {
	mlibc::infoLogger() << "mlibc: sys_sysconf top" << frg::endlog;
	switch(num) {
		case _SC_NPROCESSORS_ONLN:
		case _SC_NPROCESSORS_CONF:
			*ret = 0;
			mlibc::infoLogger() << "mlibc: ret = " << ret << ", *ret = " << *ret << frg::endlog;
			HEL_CHECK(helGetCpuInformation((uint32_t*)ret));
			mlibc::infoLogger() << "mlibc: ret = " << ret << ", *ret = " << *ret << frg::endlog;
			return 0;
		default:
			return EINVAL;
	}
}
