
#include <bits/ensure.h>
#include <wchar.h>

#include <frigg/debug.hpp>

int wcwidth(wchar_t) {
	static bool warned = false;
	if(!warned)
		frigg::infoLogger() << "\e[35mmlibc: wcwidth() always returns 1\e[39m" << frigg::endLog;
	warned = true;
	return 1;
}

