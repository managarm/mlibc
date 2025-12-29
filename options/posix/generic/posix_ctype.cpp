#include <ctype.h>
#include <wctype.h>

#include <bits/ensure.h>
#include <mlibc/charset.hpp>
#include <mlibc/locale.hpp>
#include <mlibc/wide.hpp>

int isalnum_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alnum(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isalpha_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alpha(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isblank_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_blank(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iscntrl_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_cntrl(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isdigit_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_digit(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isgraph_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_graph(cp, static_cast<mlibc::localeinfo *>(loc));
}

int islower_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_lower(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isprint_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_print(cp, static_cast<mlibc::localeinfo *>(loc));
}

int ispunct_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_punct(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isspace_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_space(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isupper_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_upper(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isxdigit_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_xdigit(cp, static_cast<mlibc::localeinfo *>(loc));
}

int isascii_l(int c, locale_t) {
	return isascii(c);
}

int tolower_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->to_lower(cp, static_cast<mlibc::localeinfo *>(loc));
}

int toupper_l(int nc, locale_t loc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->to_upper(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswalnum_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alnum(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswblank_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_blank(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswcntrl_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_cntrl(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswdigit_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_digit(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswgraph_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_graph(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswlower_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_lower(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswprint_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_print(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswpunct_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_punct(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswspace_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_space(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswupper_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_upper(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswxdigit_l(wint_t nc, locale_t loc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_xdigit(cp, static_cast<mlibc::localeinfo *>(loc));
}

int iswalpha_l(wint_t c, locale_t l) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(c, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alpha(cp, static_cast<mlibc::localeinfo *>(l));
}

wctype_t wctype_l(const char* p, locale_t loc) {
	auto l = static_cast<mlibc::localeinfo *>(loc);
	return mlibc::current_charset()->wctype({p}, l);
}

int iswctype_l(wint_t wc, wctype_t t, locale_t loc) {
	auto l = static_cast<mlibc::localeinfo *>(loc);
	return mlibc::current_charset()->iswctype(wc, t, l);
}

wint_t towlower_l(wint_t c, locale_t l) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(c, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->to_lower(cp, static_cast<mlibc::localeinfo *>(l));
}

wint_t towupper_l(wint_t c, locale_t l) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(c, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->to_upper(cp, static_cast<mlibc::localeinfo *>(l));
}

wctrans_t wctrans_l(const char* c, locale_t l) {
	return find_wctrans(c, static_cast<mlibc::localeinfo *>(l));
}

wint_t towctrans_l(wint_t c, wctrans_t desc, locale_t l) {
	return mlibc::current_charset()->towctrans(c, desc, static_cast<mlibc::localeinfo *>(l));
}
