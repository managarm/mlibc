
#include <bits/ensure.h>
#include <wchar.h>

#include <mlibc/debug.hpp>

wint_t btowc(int c) MLIBC_STUB_BODY
int wctob(wint_t) MLIBC_STUB_BODY

int mbsinit(const mbstate_t *) MLIBC_STUB_BODY
size_t mbrlen(const char *__restrict, size_t, mbstate_t *__restrict) MLIBC_STUB_BODY
size_t mbrtowc(wchar_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict) MLIBC_STUB_BODY
size_t wcrtomb(char *__restrict, wchar_t, mbstate_t *__restrict) MLIBC_STUB_BODY
size_t mbsrtowcs(wchar_t *__restrict, const char **__restrict, size_t, mbstate_t *__restrict) MLIBC_STUB_BODY
size_t wcsrtombs(char *__restrict, const wchar_t **__restrict, size_t, mbstate_t *__restrict) MLIBC_STUB_BODY

int wcwidth(wchar_t) {
	static bool warned = false;
	if(!warned)
		mlibc::infoLogger() << "\e[35mmlibc: wcwidth() always returns 1\e[39m" << frg::endlog;
	warned = true;
	return 1;
}

