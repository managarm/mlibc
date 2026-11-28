
#include <errno.h>
#include <sys/procdesc.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/atfork.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/dlapi.hpp>
#include <mlibc/tid.hpp>
#include <mlibc/thread.hpp>

pid_t pdfork(int *fdp, int flags) {
	auto self = mlibc::get_current_tcb();
	auto parent_tid = self->tid;
	pid_t child;

	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Pdfork>, -1);

	mlibc::atfork_prepare();
	__dlapi_prefork();

	if(int e = mlibc::sysdep_or_panic<Pdfork>(fdp, flags, &child); e) {
		__dlapi_postfork_finish();
		// The parent handlers run even when the fork failed.
		mlibc::atfork_parent();
		errno = e;
		return -1;
	}

	if (!child) {
		// update the cached TID in the TCB
		__atomic_store_n(&self->tid, mlibc::refetch_tid(), __ATOMIC_RELAXED);
		__dlapi_postfork_rebind(parent_tid);
	}

	__dlapi_postfork_finish();
	if (!child)
		mlibc::atfork_child();
	else
		mlibc::atfork_parent();

	return child;
}

int pdkill(int fd, int sig) {
	if (int e = mlibc::sysdep_or_enosys<Pdkill>(fd, sig); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int pdgetpid(int fd, pid_t *pidp) {
	if (int e = mlibc::sysdep_or_enosys<Pdgetpid>(fd, pidp); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int pdwait(int fd, int *status, int options, struct __wrusage *rusage, siginfo_t *info) {
	if (int e = mlibc::sysdep_or_enosys<Pdwait>(fd, status, options, rusage, info); e) {
		errno = e;
		return -1;
	}
	return 0;
}
