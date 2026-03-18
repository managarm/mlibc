
#include <errno.h>
#include <sys/resource.h>

#include <bits/ensure.h>
#include <mlibc-config.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

int getpriority(int which, id_t who) {
	int value = 0;
	if(int e = mlibc::sysdep_or_enosys<GetPriority>(which, who, &value); e) {
		errno = e;
	}
	return value;
}

int setpriority(int which, id_t who, int prio) {
	if(int e = mlibc::sysdep_or_enosys<SetPriority>(which, who, prio); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getrusage(int scope, struct rusage *usage) {
	if(int e = mlibc::sysdep_or_enosys<GetRusage>(scope, usage); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int getrlimit(int resource, struct rlimit *limit) {
	if(int e = mlibc::sysdep_or_enosys<GetRlimit>(resource, limit); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("getrlimit")]] int getrlimit64(int resource, struct rlimit *limit);
#endif /* !__MLIBC_LINUX_OPTION */

int setrlimit(int resource, const struct rlimit *limit) {
	if(int e = mlibc::sysdep_or_enosys<SetRlimit>(resource, limit); e) {
		errno = e;
		return -1;
	}
	return 0;
}

#if __MLIBC_LINUX_OPTION
[[gnu::alias("setrlimit")]] int setrlimit64(int resource, const struct rlimit *limit);
#endif /* !__MLIBC_LINUX_OPTION */

int prlimit(pid_t, int, const struct rlimit *, struct rlimit *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
