
#include <ctype.h>
#include <wctype.h>

#include <bits/ensure.h>
#include <mlibc/charset.hpp>
#include <mlibc/locale.hpp>

// --------------------------------------------------------------------------------------
// char ctype functions.
// --------------------------------------------------------------------------------------

int isalpha(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alpha(cp, mlibc::getActiveLocale());
}

int isdigit(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_digit(cp, mlibc::getActiveLocale());
}

int isxdigit(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_xdigit(cp, mlibc::getActiveLocale());
}

int isalnum(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alnum(cp, mlibc::getActiveLocale());
}

int ispunct(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_punct(cp, mlibc::getActiveLocale());
}

int isgraph(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_graph(cp, mlibc::getActiveLocale());
}

int isblank(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_blank(cp, mlibc::getActiveLocale());
}

int isspace(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_space(cp, mlibc::getActiveLocale());
}

int isprint(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_print(cp, mlibc::getActiveLocale());
}

int islower(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_lower(cp, mlibc::getActiveLocale());
}

int isupper(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_upper(cp, mlibc::getActiveLocale());
}

int iscntrl(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_cntrl(cp, mlibc::getActiveLocale());
}

int isascii(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return cp <= 0x7F;
}

// --------------------------------------------------------------------------------------
// wchar_t ctype functions.
// --------------------------------------------------------------------------------------

int iswalpha(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alpha(cp, mlibc::getActiveLocale());
}

int iswdigit(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_digit(cp, mlibc::getActiveLocale());
}

int iswxdigit(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_xdigit(cp, mlibc::getActiveLocale());
}

int iswalnum(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alnum(cp, mlibc::getActiveLocale());
}

int iswpunct(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_punct(cp, mlibc::getActiveLocale());
}

int iswgraph(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_graph(cp, mlibc::getActiveLocale());
}

int iswblank(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_blank(cp, mlibc::getActiveLocale());
}

int iswspace(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_space(cp, mlibc::getActiveLocale());
}

int iswprint(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_print(cp, mlibc::getActiveLocale());
}

int iswlower(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_lower(cp, mlibc::getActiveLocale());
}

int iswupper(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_upper(cp, mlibc::getActiveLocale());
}

int iswcntrl(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_cntrl(cp, mlibc::getActiveLocale());
}

// --------------------------------------------------------------------------------------
// iswctype functions.
// --------------------------------------------------------------------------------------

namespace {
	enum {
		ct_null,
		ct_alnum,
		ct_alpha,
		ct_blank,
		ct_cntrl,
		ct_digit,
		ct_graph,
		ct_lower,
		ct_print,
		ct_punct,
		ct_space,
		ct_upper,
		ct_xdigit,
		ct_count
	};
} // namespace

wctype_t wctype(const char *cs) {
	frg::string_view s{cs};
	if(s == "alnum") return ct_alnum;
	if(s == "alpha") return ct_alpha;
	if(s == "blank") return ct_blank;
	if(s == "cntrl") return ct_cntrl;
	if(s == "digit") return ct_digit;
	if(s == "graph") return ct_graph;
	if(s == "lower") return ct_lower;
	if(s == "print") return ct_print;
	if(s == "punct") return ct_punct;
	if(s == "space") return ct_space;
	if(s == "upper") return ct_upper;
	if(s == "xdigit") return ct_xdigit;
	mlibc::infoLogger() << "mlibc: wctype(\"" << cs << "\") is not supported" << frg::endlog;
	return ct_null;
}

int iswctype(wint_t wc, wctype_t type) {
	switch (type) {
		case ct_alnum:
			return iswalnum(wc);
		case ct_alpha:
			return iswalpha(wc);
		case ct_blank:
			return iswblank(wc);
		case ct_cntrl:
			return iswcntrl(wc);
		case ct_digit:
			return iswdigit(wc);
		case ct_graph:
			return iswgraph(wc);
		case ct_lower:
			return iswlower(wc);
		case ct_print:
			return iswprint(wc);
		case ct_punct:
			return iswpunct(wc);
		case ct_space:
			return iswspace(wc);
		case ct_upper:
			return iswupper(wc);
		case ct_xdigit:
			return iswxdigit(wc);
	}
	return 0;
}

// --------------------------------------------------------------------------------------
// char conversion functions.
// --------------------------------------------------------------------------------------

int tolower(int c) {
	auto l = mlibc::getActiveLocale();
	return c >= -128 && c < 256 ? l->ctype.map_tolower()[c + 128] : c;
}

int toupper(int c) {
	auto l = mlibc::getActiveLocale();
	return c >= -128 && c < 256 ? l->ctype.map_toupper()[c + 128] : c;
}

// --------------------------------------------------------------------------------------
// wchar_t conversion functions.
// --------------------------------------------------------------------------------------

wint_t towlower(wint_t wc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(wc, cp); e != mlibc::charcode_error::null)
		return wc;
	return mlibc::current_charset()->to_lower(cp, mlibc::getActiveLocale());
}

wint_t towupper(wint_t wc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(wc, cp); e != mlibc::charcode_error::null)
		return wc;
	return mlibc::current_charset()->to_upper(cp, mlibc::getActiveLocale());
}
