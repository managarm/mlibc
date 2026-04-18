
#include <errno.h>
#include <sys/procdesc.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tid.hpp>
#include <mlibc/thread.hpp>

pid_t pdfork(int *fdp, int flags) {
	auto self = mlibc::get_current_tcb();
	pid_t child;

	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Pdfork>, -1);

	auto hand = self->atforkEnd;
	while (hand) {
		if (hand->prepare)
			hand->prepare();

		hand = hand->prev;
	}

	if(int e = mlibc::sysdep_or_panic<Pdfork>(fdp, flags, &child); e) {
		errno = e;
		return -1;
	}

	// update the cached TID in the TCB
	if (!child)
		__atomic_store_n(&self->tid, mlibc::refetch_tid(), __ATOMIC_RELAXED);

	hand = self->atforkBegin;
	while (hand) {
		if (!child) {
			if (hand->child)
				hand->child();
		} else {
			if (hand->parent)
				hand->parent();
		}
		hand = hand->next;
	}

	return child;
}

int pdkill(int fd, int sig) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Pdkill>, -1);

	if (int e = mlibc::sysdep_or_enosys<Pdkill>(fd, sig); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int pdgetpid(int fd, pid_t *pidp) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Pdgetpid>, -1);

	if (int e = mlibc::sysdep_or_enosys<Pdgetpid>(fd, pidp); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int pdwait(int fd, int *status, int options, struct __wrusage *rusage, siginfo_t *info) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::IsImplemented<Pdwait>, -1);

	if (int e = mlibc::sysdep_or_enosys<Pdwait>(fd, status, options, rusage, info); e) {
		errno = e;
		return -1;
	}
	return 0;
}
