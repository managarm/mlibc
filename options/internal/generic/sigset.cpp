#include <bits/sigset_t.h>
#include <bits/ensure.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

int sigemptyset(sigset_t *sigset) {
	memset(sigset, 0, sizeof(*sigset));
	return 0;
}

int sigfillset(sigset_t *sigset) {
	memset(sigset, ~0, sizeof(*sigset));
	return 0;
}

int sigaddset(sigset_t *sigset, int sig) {
	int signo = sig - 1;
	if(signo < 0 || static_cast<unsigned int>(signo) >= (sizeof(sigset_t) * CHAR_BIT)) {
		errno = EINVAL;
		return -1;
	}
	auto ptr = reinterpret_cast<char *>(sigset);
	ptr[signo / CHAR_BIT] |= (1 << (signo % CHAR_BIT));
	return 0;
}

int sigdelset(sigset_t *sigset, int sig) {
	int signo = sig - 1;
	if(signo < 0 || static_cast<unsigned int>(signo) >= (sizeof(sigset_t) * CHAR_BIT)) {
		errno = EINVAL;
		return -1;
	}
	auto ptr = reinterpret_cast<char *>(sigset);
	ptr[signo / CHAR_BIT] &= ~(1 << (signo % CHAR_BIT));
	return 0;
}

int sigismember(const sigset_t *sigset, int sig) {
	int signo = sig - 1;
	if(signo < 0 || static_cast<unsigned int>(signo) >= (sizeof(sigset_t) * CHAR_BIT)) {
		errno = EINVAL;
		return -1;
	}
	auto ptr = reinterpret_cast<const char *>(sigset);
	return (ptr[signo / CHAR_BIT] & (1 << (signo % CHAR_BIT))) != 0;
}
