
#include <signal.h>

#include <mlibc/ensure.h>

#pragma GCC visibility push(hidden)

#include <frigg/debug.hpp>

#pragma GCC visibility pop

int sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	frigg::infoLogger.log() << "mlibc: Broken sigprocmask() called!" << frigg::EndLog();
	*retrieve = 0;
	return 0;
}

int sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict) {
	frigg::infoLogger.log() << "mlibc: Broken sigaction() called!" << frigg::EndLog();
	return 0;
}

