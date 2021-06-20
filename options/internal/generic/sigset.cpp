#include <bits/sigset_t.h>
#include <bits/ensure.h>

int sigemptyset(sigset_t *sigset) {
	*sigset = 0;
	return 0;
}

int sigfillset(sigset_t *sigset) {
	*sigset = ~sigset_t(0);
	return 0;
}

// TODO: Return EINVAL instead of __ensure()ing.

int sigaddset(sigset_t *sigset, int sig) {
	int signo = sig - 1;
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)signo < sizeof(sigset_t) * 8);
	*sigset |= sigset_t(1) << signo;
	return 0;
}

int sigdelset(sigset_t *sigset, int sig) {
	int signo = sig - 1;
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)signo < sizeof(sigset_t) * 8);
	*sigset &= ~(sigset_t(1) << signo);
	return 0;
}

int sigismember(const sigset_t *set, int sig) {
	int signo = sig - 1;
	// TODO: do not hard code CHAR_BITS
	__ensure((unsigned int)signo < sizeof(sigset_t) * 8);
	return (*set) & (sigset_t(1) << signo);
}
