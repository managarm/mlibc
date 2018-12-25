
#include <errno.h>
#include <wchar.h>
#include <bits/ensure.h>

#include <mlibc/charcode.hpp>
#include <mlibc/debug.hpp>

namespace {
	// All conversion functions mbrlen(), mbrtowc(), wcrtomb(),
	// mbsrtowcs() and wcsrtombs() have an internal state.
	__mlibc_mbstate mbrlen_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate mbrtowc_state = __MLIBC_MBSTATE_INITIALIZER;
}

wint_t btowc(int c)
	MLIBC_STUB_BODY
int wctob(wint_t)
	MLIBC_STUB_BODY

int mbsinit(const mbstate_t *stp) {
	if(!stp)
		return -1;
	return !stp->__progress && !stp->__shift;
}

size_t mbrlen(const char *mbs, size_t mb_limit, mbstate_t *stp) {
	auto cc = mlibc::current_charcode();
	wchar_t wc;

	if(!stp)
		stp = &mbrlen_state;
	if(!mbs) {
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		return 0;
	}

	mlibc::code_seq<const char> nseq{mbs, mbs + mb_limit};
	mlibc::code_seq<wchar_t> wseq{&wc, &wc + 1};
	if(auto e = cc->decode_wtranscode(nseq, wseq, *stp); e != mlibc::charcode_error::null)
		__ensure(!"decode_wtranscode() errors are not handled");
	return nseq.it - mbs;
}

size_t mbrtowc(wchar_t *wcp, const char *mbs, size_t mb_limit, mbstate_t *stp) {
	auto cc = mlibc::current_charcode();

	if(!stp)
		stp = &mbrtowc_state;
	if(!mbs) {
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		return 0;
	}

	// TODO: Decode to a local wchar_t.
	__ensure(wcp);

	mlibc::code_seq<const char> nseq{mbs, mbs + mb_limit};
	mlibc::code_seq<wchar_t> wseq{wcp, wcp + 1};
	if(auto e = cc->decode_wtranscode(nseq, wseq, *stp); e != mlibc::charcode_error::null) {
		__ensure(!"decode_wtranscode() errors are not handled");
	}else{
		size_t n = wseq.it - wcp;
		if(!n) // Null-terminate resulting wide string.
			*wcp = 0;
		return n;
	}
}

size_t wcrtomb(char *__restrict, wchar_t, mbstate_t *__restrict)
	MLIBC_STUB_BODY

size_t mbsrtowcs(wchar_t *__restrict wcs, const char **__restrict mbs,
		size_t max_wcs, mbstate_t *__restrict)
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

