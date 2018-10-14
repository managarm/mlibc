
#include <bits/ensure.h>
#include <sys/prctl.h>

#include <mlibc/debug.hpp>

int prctl(int, ...) {
	mlibc::infoLogger() << "\e[31mmlibc: prctl() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}

