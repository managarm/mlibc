
#include <ctype.h>

#include <mlibc/ensure.h>

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
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int tolower(int c) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int toupper(int c) {
	// TODO: this really needs to be redesigned and support other charsets.
	if(c >= 'a' && c <= 'z')
		return c - 'a' + 'A';
	return c;
}

