#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <bits/ensure.h>

#include <mlibc/charcode.hpp>
#include <mlibc/debug.hpp>

namespace {
	// All conversion functions mbrlen(), mbrtowc(), wcrtomb(),
	// mbsrtowcs(), mbsnrtowcs() and wcsrtombs() have an internal state.
	__mlibc_mbstate mbrlen_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate mbrtowc_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate mbsrtowcs_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate mbsnrtowcs_state = __MLIBC_MBSTATE_INITIALIZER;
	__mlibc_mbstate wcsrtombs_state = __MLIBC_MBSTATE_INITIALIZER;
} // namespace

wint_t btowc(int c) {
	if(c == EOF)
		return WEOF;

	char nc = c;
	auto cc = mlibc::current_charcode();
	wchar_t wc;

	auto e = cc->promote_wtranscode(nc, wc);
	if(e != mlibc::transcode_status::null_terminator && e != mlibc::transcode_status::input_exhausted)
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

size_t mbrlen(const char *__restrict mbs, size_t mb_limit, mbstate_t *__restrict stp) {
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
	auto e = cc->decode_wtranscode(nseq, wseq, *stp);
	if (e == mlibc::transcode_status::input_exhausted || e == mlibc::transcode_status::output_exhausted) {
		return nseq.it - mbs;
	} else if (e == mlibc::transcode_status::null_terminator) {
		return 0;
	} else if (e == mlibc::transcode_status::input_underflow) {
		return static_cast<size_t>(-2);
	} else {
		__ensure(e == mlibc::transcode_status::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}

size_t mbrtowc(wchar_t *__restrict wcp, const char *__restrict mbs, size_t mb_limit, mbstate_t *__restrict stp) {
	auto cc = mlibc::current_charcode();

	if(!stp)
		stp = &mbrtowc_state;
	if(!mbs) {
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		return 0;
	}

	wchar_t temp = 0;
	if(!wcp)
		wcp = &temp;

	mlibc::code_seq<const char> nseq{mbs, mbs + mb_limit};
	mlibc::code_seq<wchar_t> wseq{wcp, wcp + 1};
	auto e = cc->decode_wtranscode(nseq, wseq, *stp);
	if (e == mlibc::transcode_status::input_exhausted || e == mlibc::transcode_status::output_exhausted) {
		return nseq.it - mbs;
	} else if (e == mlibc::transcode_status::null_terminator) {
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		*wcp = 0;
		return 0;
	} else if(e == mlibc::transcode_status::input_underflow) {
		return static_cast<size_t>(-2);
	} else {
		__ensure(e == mlibc::transcode_status::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}

size_t wcrtomb(char *__restrict mbs, wchar_t wc, mbstate_t *__restrict stp) {
	auto cc = mlibc::current_charcode();

	// wcrtomb() always takes a mbstate_t.
	__ensure(stp);

	// TODO: Implement the following case:
	__ensure(mbs);

	mlibc::code_seq<const wchar_t> wseq{&wc, &wc + 1};
	mlibc::code_seq<char> nseq{mbs, mbs + MB_LEN_MAX};

	auto e = cc->encode_wtranscode(nseq, wseq, *stp);
	if (e == mlibc::transcode_status::input_exhausted) {
		size_t n = nseq.it - mbs;
		// If we reached a null terminator, we need to reset mbstate_t
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		return n;
	} else if (e == mlibc::transcode_status::null_terminator) {
		mbs[0] = '\0';
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		return 1;
	} else {
		__ensure(e == mlibc::transcode_status::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}

size_t mbsrtowcs(wchar_t *__restrict wcs, const char **__restrict mbsp, size_t wc_limit, mbstate_t *__restrict stp) {
	__ensure(mbsp);

	auto cc = mlibc::current_charcode();
	mlibc::code_seq<const char> nseq{*mbsp, nullptr};
	mlibc::code_seq<wchar_t> wseq{wcs, wcs ? wcs + wc_limit : nullptr};

	if(!stp)
		stp = &mbsrtowcs_state;

	if(!wcs) {
		size_t size;
		if(auto e = cc->decode_wtranscode_length(nseq, &size, *stp); e != mlibc::transcode_status::null_terminator) {
			__ensure(e == mlibc::transcode_status::illegal_input || e == mlibc::transcode_status::input_underflow);
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		return size;
	}

	auto e = cc->decode_wtranscode(nseq, wseq, *stp);
	if (e == mlibc::transcode_status::null_terminator) {
		size_t n = wseq.it - wcs;
		*mbsp = nullptr;
		// If we reached a null terminator, we need to reset mbstate_t
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		if(n < wc_limit) // Null-terminate resulting wide string.
			wcs[n] = 0;
		return n;
	} else if (e == mlibc::transcode_status::input_exhausted || e == mlibc::transcode_status::output_exhausted) {
		size_t n = wseq.it - wcs;
		*mbsp = nseq.it;
		return n;
	} else {
		__ensure(e == mlibc::transcode_status::input_underflow || e == mlibc::transcode_status::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}

size_t mbsnrtowcs(wchar_t *__restrict wcs, const char **__restrict mbsp, size_t mb_limit, size_t wc_limit, mbstate_t *__restrict stp) {
	__ensure(mbsp);

	auto cc = mlibc::current_charcode();
	mlibc::code_seq<const char> nseq{*mbsp, (*mbsp) + mb_limit};
	mlibc::code_seq<wchar_t> wseq{wcs, wcs ? wcs + wc_limit : nullptr};

	if(!stp)
		stp = &mbsnrtowcs_state;

	if(!wcs) {
		size_t size;
		if(auto e = cc->decode_wtranscode_length(nseq, &size, *stp); e != mlibc::transcode_status::null_terminator && e != mlibc::transcode_status::input_exhausted && e != mlibc::transcode_status::input_underflow) {
			__ensure(e == mlibc::transcode_status::illegal_input);
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		return size;
	}

	auto e = cc->decode_wtranscode(nseq, wseq, *stp);
	if (e == mlibc::transcode_status::null_terminator) {
		size_t n = wseq.it - wcs;
		*mbsp = nullptr;
		// If we reached a null terminator, we need to reset mbstate_t
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		if(n < wc_limit) // Null-terminate resulting wide string.
			wcs[n] = 0;
		return n;
	} else if (e == mlibc::transcode_status::input_underflow || e == mlibc::transcode_status::input_exhausted || e == mlibc::transcode_status::output_exhausted) {
		size_t n = wseq.it - wcs;
		*mbsp = nseq.it;
		return n;
	} else {
		__ensure(e == mlibc::transcode_status::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}

size_t wcsrtombs(char *__restrict mbs, const wchar_t **__restrict wcsp, size_t mb_limit, mbstate_t *__restrict stp) {
	__ensure(wcsp && "wcsrtombs() with null input");
	auto cc = mlibc::current_charcode();
	mlibc::code_seq<const wchar_t> wseq{*wcsp, nullptr};

	if(!stp)
		stp = &wcsrtombs_state;

	if(!mbs) {
		size_t size;
		if(auto e = cc->encode_wtranscode_length(wseq, &size, *stp); e != mlibc::transcode_status::null_terminator) {
			__ensure(e == mlibc::transcode_status::illegal_input || e == mlibc::transcode_status::input_underflow);
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		return size;
	}

	mlibc::code_seq<char> nseq{mbs, mbs + mb_limit};

	auto e = cc->encode_wtranscode(nseq, wseq, *stp);
	if (e == mlibc::transcode_status::null_terminator) {
		size_t n = nseq.it - mbs;
		*wcsp = nullptr;
		// If we reached a null terminator, we need to reset mbstate_t
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		if(n < mb_limit) // Null-terminate resulting wide string.
			mbs[n] = 0;
		return n;
	} else if (e == mlibc::transcode_status::input_underflow || e == mlibc::transcode_status::output_exhausted) {
		size_t n = nseq.it - mbs;
		*wcsp = wseq.it;
		return n;
	} else {
		__ensure(e == mlibc::transcode_status::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}

size_t wcsnrtombs(char *__restrict mbs, const wchar_t **__restrict wcsp, size_t wc_limit, size_t mb_limit, mbstate_t *__restrict stp) {
	__ensure(wcsp && "wcsrtombs() with null input");
	auto cc = mlibc::current_charcode();
	mlibc::code_seq<char> nseq{mbs, mbs ? mbs + mb_limit : nullptr};
	mlibc::code_seq<const wchar_t> wseq{*wcsp, (*wcsp) + wc_limit};

	if(!stp)
		stp = &wcsrtombs_state;

	if(!mbs) {
		size_t size;
		if(auto e = cc->encode_wtranscode_length(wseq, &size, *stp); e != mlibc::transcode_status::null_terminator && e != mlibc::transcode_status::input_underflow && e != mlibc::transcode_status::input_exhausted) {
			__ensure(e == mlibc::transcode_status::illegal_input);
			errno = EILSEQ;
			return static_cast<size_t>(-1);
		}
		return size;
	}

	auto e = cc->encode_wtranscode(nseq, wseq, *stp);
	if (e == mlibc::transcode_status::null_terminator) {
		size_t n = nseq.it - mbs;
		*wcsp = nullptr;
		// If we reached a null terminator, we need to reset mbstate_t
		*stp = __MLIBC_MBSTATE_INITIALIZER;
		if(n < mb_limit) // Null-terminate resulting wide string.
			mbs[n] = 0;
		return n;
	} else if (e == mlibc::transcode_status::input_underflow || e == mlibc::transcode_status::input_exhausted || e == mlibc::transcode_status::output_exhausted) {
		size_t n = nseq.it - mbs;
		*wcsp = wseq.it;
		return n;
	} else {
		__ensure(e == mlibc::transcode_status::illegal_input);
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}
