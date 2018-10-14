
#include <bits/ensure.h>
#include <wchar.h>

#include <mlibc/debug.hpp>

int wcwidth(wchar_t) {
	static bool warned = false;
	if(!warned)
		mlibc::infoLogger() << "\e[35mmlibc: wcwidth() always returns 1\e[39m" << frg::endlog;
	warned = true;
	return 1;
}

