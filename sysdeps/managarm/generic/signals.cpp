
#include <signal.h>

#include <bits/ensure.h>

#include <frigg/debug.hpp>

namespace mlibc {

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	frigg::infoLogger() << "mlibc: Broken sigprocmask() called!" << frigg::endLog;
	if(retrieve)
		*retrieve = 0;
	return 0;
}

int sys_sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict) {
	frigg::infoLogger() << "mlibc: Broken sigaction() called!" << frigg::endLog;
	return 0;
}

} //namespace mlibc

