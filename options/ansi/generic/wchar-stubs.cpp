
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <bits/ensure.h>

#include <mlibc/charcode.hpp>
#include <mlibc/debug.hpp>

namespace {
	// All conversion functions mbrlen(), mbrtowc(), wcrtomb(),
	// mbsrtowcs() and wcsrtombs() have an internal state.
	__mlibc_mbstate mbrlen_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate mbrtowc_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate mbsrtowcs_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate wcsrtombs_state = __MLIBC_MBSTATE_INITIALIZER;
}

wint_t btowc(int c) {
	if(c == EOF)
		return WEOF;

	char nc = c;
	auto cc = mlibc::current_charcode();
	wchar_t wc;
	if(auto e = cc->promote_wtranscode(nc, wc); e != mlibc::charcode_error::null)
		return WEOF;
	return wc;
}

int wctob(wint_t wc) {
	// TODO: Revisit this once we have character encoding functions.
	return wc;
}

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
		if(e == mlibc::charcode_error::input_underflow)
			return static_cast<size_t>(-2);
		__ensure(e == mlibc::charcode_error::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}else{
		size_t n = nseq.it - mbs;
		if(!n) // Null-terminate resulting wide string.
			*wcp = 0;
		return n;
	}
}

size_t wcrtomb(char *mbs, wchar_t wc, mbstate_t *stp) {
	auto cc = mlibc::current_charcode();

	// wcrtomb() always takes a mbstate_t.
	__ensure(stp);

	// TODO: Implement the following case:
	__ensure(mbs);

	mlibc::code_seq<const wchar_t> wseq{&wc, &wc + 1};
	mlibc::code_seq<char> nseq{mbs, mbs + 4}; // TODO: Replace 4 by some named constant.
	if(auto e = cc->encode_wtranscode(nseq, wseq, *stp); e != mlibc::charcode_error::null) {
		__ensure(!"encode_wtranscode() errors are not handled");
		__builtin_unreachable();
	}else{
		size_t n = nseq.it - mbs;
		if(!n) // Null-terminate resulting wide string.
			*mbs = 0;
		return n;
	}
}

size_t mbsrtowcs(wchar_t *wcs, const char **mbsp, size_t wc_limit, mbstate_t *stp) {
	__ensure(mbsp);

	auto cc = mlibc::current_charcode();
	__mlibc_mbstate st = __MLIBC_MBSTATE_INITIALIZER;
	mlibc::code_seq<const char> nseq{*mbsp, nullptr};
	mlibc::code_seq<wchar_t> wseq{wcs, wcs + wc_limit};

	if(!stp)
		stp = &mbsrtowcs_state;

	if(!wcs) {
		size_t size;
		if(auto e = cc->decode_wtranscode_length(nseq, &size, st); e != mlibc::charcode_error::null)
			__ensure(!"decode_wtranscode() errors are not handled");
		return size;
	}

	if(auto e = cc->decode_wtranscode(nseq, wseq, st); e != mlibc::charcode_error::null) {
		__ensure(!"decode_wtranscode() errors are not handled");
		__builtin_unreachable();
	}else{
		size_t n = wseq.it - wcs;
		if(n < wc_limit) // Null-terminate resulting wide string.
			wcs[n] = 0;
		*mbsp = nullptr;
		return n;
	}
}

size_t mbsnrtowcs(wchar_t *wcs, const char **mbsp, size_t mb_limit, size_t wc_limit, mbstate_t *stp) {
	__ensure(mbsp);

	auto cc = mlibc::current_charcode();
	__mlibc_mbstate st = __MLIBC_MBSTATE_INITIALIZER;
	mlibc::code_seq<const char> nseq{*mbsp, (*mbsp) + mb_limit};
	mlibc::code_seq<wchar_t> wseq{wcs, wcs + wc_limit};

	if(!stp)
		stp = &mbsrtowcs_state;

	if(!wcs) {
		size_t size;
		if(auto e = cc->decode_wtranscode_length(nseq, &size, st); e != mlibc::charcode_error::null)
			__ensure(!"decode_wtranscode() errors are not handled");
		return size;
	}

	if(auto e = cc->decode_wtranscode(nseq, wseq, st); e != mlibc::charcode_error::null) {
		__ensure(!"decode_wtranscode() errors are not handled");
		__builtin_unreachable();
	}else{
		size_t n = wseq.it - wcs;
		if(n < wc_limit) // Null-terminate resulting wide string.
			wcs[n] = 0;
		*mbsp = nullptr;
		return n;
	}
}

size_t wcsrtombs(char *mbs, const wchar_t **wcsp, size_t mb_limit, mbstate_t *stp) {
	__ensure(wcsp && "wcsrtombs() with null input");
	auto cc = mlibc::current_charcode();
	mlibc::code_seq<char> nseq{mbs, mbs + mb_limit};
	mlibc::code_seq<const wchar_t> wseq{*wcsp, nullptr};

	if(!stp)
		stp = &wcsrtombs_state;

	if(!mbs) {
		size_t size;
		if(auto e = cc->encode_wtranscode_length(wseq, &size, *stp); e != mlibc::charcode_error::null)
			__ensure(!"decode_wtranscode() errors are not handled");
		return size;
	}

	if(auto e = cc->encode_wtranscode(nseq, wseq, *stp); e != mlibc::charcode_error::null) {
		__ensure(!"encode_wtranscode() errors are not handled");
		__builtin_unreachable();
	}else{
		*wcsp = wseq.it;
		size_t n = nseq.it - mbs;
		if(n < mb_limit) // Null-terminate resulting narrow string.
			mbs[n] = 0;
		return n;
	}
}

size_t wcsnrtombs(char *mbs, const wchar_t **wcsp, size_t wc_limit, size_t mb_limit, mbstate_t *stp) {
	__ensure(wcsp && "wcsrtombs() with null input");
	auto cc = mlibc::current_charcode();
	mlibc::code_seq<char> nseq{mbs, mbs + mb_limit};
	mlibc::code_seq<const wchar_t> wseq{*wcsp, (*wcsp) + wc_limit};

	if(!stp)
		stp = &wcsrtombs_state;

	if(!mbs) {
		size_t size;
		if(auto e = cc->encode_wtranscode_length(wseq, &size, *stp); e != mlibc::charcode_error::null)
			__ensure(!"decode_wtranscode() errors are not handled");
		return size;
	}

	if(auto e = cc->encode_wtranscode(nseq, wseq, *stp); e != mlibc::charcode_error::null) {
		__ensure(!"encode_wtranscode() errors are not handled");
		__builtin_unreachable();
	}else{
		*wcsp = wseq.it;
		size_t n = nseq.it - mbs;
		if(n < mb_limit) // Null-terminate resulting narrow string.
			mbs[n] = 0;
		return n;
	}
}

int wcwidth(wchar_t) {
	static bool warned = false;
	if(!warned)
		mlibc::infoLogger() << "\e[35mmlibc: wcwidth() always returns 1\e[39m" << frg::endlog;
	warned = true;
	return 1;
}

int wcswidth(const wchar_t *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

wchar_t *wcsdup(const wchar_t *s) {
	size_t len = wcslen(s);
	wchar_t *ret = (wchar_t *) malloc(sizeof(wchar_t) * (len + 1));
	if(!ret)
		return NULL;
	wmemcpy(ret, s, len + 1);
	return ret;
}

int wcsncasecmp(const wchar_t* s1, const wchar_t* s2, size_t n) {
	for(size_t i = 0; i < n; i++) {
		wint_t c1 = towlower(s1[i]);
		wint_t c2 = towlower(s2[i]);
		if(c1 == L'\0' && c2 == L'\0')
			return 0;
		if(c1 < c2)
			return -1;
		if(c1 > c2)
			return 1;
	}
	return 0;
}
