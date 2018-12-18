
#include <errno.h>
#include <wchar.h>
#include <bits/ensure.h>

#include <mlibc/charcode.hpp>
#include <mlibc/debug.hpp>

wint_t btowc(int c)
	MLIBC_STUB_BODY
int wctob(wint_t)
	MLIBC_STUB_BODY

int mbsinit(const mbstate_t *)
	MLIBC_STUB_BODY

size_t mbrlen(const char *__restrict s, size_t n, mbstate_t *__restrict) {
	size_t res;
	auto cc = mlibc::current_charcode();
	if(auto e = cc->validate_length(s, n, &res); e != mlibc::charcode_error::null) {
		// In contrast to mblen(), mbrlen() detects multibyte sequences that are too short.
		if(e == mlibc::charcode_error::not_enough_units) {
			return -2;
		}else{
			errno = EILSEQ;
			return -1;
		}
	}
	return res;
}

size_t mbrtowc(wchar_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict)
	MLIBC_STUB_BODY
size_t wcrtomb(char *__restrict, wchar_t, mbstate_t *__restrict)
	MLIBC_STUB_BODY
size_t mbsrtowcs(wchar_t *__restrict, const char **__restrict, size_t, mbstate_t *__restrict)
	MLIBC_STUB_BODY
size_t wcsrtombs(char *__restrict, const wchar_t **__restrict, size_t, mbstate_t *__restrict)
	MLIBC_STUB_BODY

int wcwidth(wchar_t) {
	static bool warned = false;
	if(!warned)
		mlibc::infoLogger() << "\e[35mmlibc: wcwidth() always returns 1\e[39m" << frg::endlog;
	warned = true;
	return 1;
}

