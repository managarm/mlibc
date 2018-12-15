
#include <ctype.h>
#include <wctype.h>

#include <bits/ensure.h>

int isalnum(int c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}
int isalpha(int c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
int isblank(int c) {
	return c == ' ' || c == '\t';
}
int iscntrl(int c) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int isdigit(int c) {
	// TODO: this really needs to be redesigned and support other charsets.
	return c >= '0' && c <= '9';
}
int isgraph(int c) {
	return c >= 32 && c < 127;
}
int islower(int c) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int isprint(int c) {
	return c >= 0x20 && c <= 0x7E;
}
int ispunct(int c) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int isspace(int c) {
	// TODO: this really needs to be redesigned and support other charsets.
	return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}
int isupper(int c) {
	// TODO: this really needs to be redesigned and support other charsets.
	return (c >= 'A' && c <= 'Z');
}
int isxdigit(int c) {
	// TODO: this really needs to be redesigned and support other charsets.
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int iswalnum(wint_t) MLIBC_STUB_BODY
int iswalpha(wint_t) MLIBC_STUB_BODY
int iswblank(wint_t) MLIBC_STUB_BODY
int iswcntrl(wint_t) MLIBC_STUB_BODY
int iswdigit(wint_t) MLIBC_STUB_BODY
int iswgraph(wint_t) MLIBC_STUB_BODY
int iswlower(wint_t) MLIBC_STUB_BODY
int iswprint(wint_t) MLIBC_STUB_BODY
int iswpunct(wint_t) MLIBC_STUB_BODY
int iswspace(wint_t) MLIBC_STUB_BODY
int iswupper(wint_t) MLIBC_STUB_BODY
int iswxdigit(wint_t) MLIBC_STUB_BODY


int tolower(int c) {
	// TODO: this really needs to be redesigned and support other charsets.
	if(c >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	return c;
}

int toupper(int c) {
	// TODO: this really needs to be redesigned and support other charsets.
	if(c >= 'a' && c <= 'z')
		return c - 'a' + 'A';
	return c;
}

