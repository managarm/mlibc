
#include <bits/ensure.h>
#include <wchar.h>

#include <frigg/debug.hpp>

int wcwidth(wchar_t) {
	frigg::infoLogger() << "\e[35mmlibc: wcwidth() always returns 1\e[39m" << frigg::endLog;
	return 1;
}

