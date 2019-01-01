
#include <ctype.h>
#include <wctype.h>

#include <bits/ensure.h>
#include <mlibc/charset.hpp>

// --------------------------------------------------------------------------------------
// char ctype functions.
// --------------------------------------------------------------------------------------

int isalpha(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alpha(cp);
}

int isdigit(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_digit(cp);
}

int isxdigit(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_xdigit(cp);
}

int isalnum(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alnum(cp);
}

int ispunct(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_punct(cp);
}

int isgraph(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_graph(cp);
}

int isblank(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_blank(cp);
}

int isspace(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_space(cp);
}

int isprint(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_print(cp);
}

int islower(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_lower(cp);
}

int isupper(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_upper(cp);
}

int iscntrl(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::generic_is_control(cp);
}

// --------------------------------------------------------------------------------------
// wchar_t ctype functions.
// --------------------------------------------------------------------------------------

int iswalpha(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alpha(cp);
}

int iswdigit(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_digit(cp);
}

int iswxdigit(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_xdigit(cp);
}

int iswalnum(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_alnum(cp);
}

int iswpunct(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_punct(cp);
}

int iswgraph(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_graph(cp);
}

int iswblank(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_blank(cp);
}

int iswspace(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_space(cp);
}

int iswprint(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_print(cp);
}

int iswlower(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_lower(cp);
}

int iswupper(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::current_charset()->is_upper(cp);
}

int iswcntrl(wint_t nc) {
	auto cc = mlibc::platform_wide_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return 0;
	return mlibc::generic_is_control(cp);
}

// --------------------------------------------------------------------------------------
// char conversion functions.
// --------------------------------------------------------------------------------------

int tolower(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return nc;
	return mlibc::current_charset()->to_lower(cp);
}

int toupper(int nc) {
	auto cc = mlibc::current_charcode();
	mlibc::codepoint cp;
	if(auto e = cc->promote(nc, cp); e != mlibc::charcode_error::null)
		return nc;
	return mlibc::current_charset()->to_upper(cp);
}

