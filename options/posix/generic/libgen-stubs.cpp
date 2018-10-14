
#include <bits/ensure.h>
#include <libgen.h>
#include <string.h>

#include <mlibc/debug.hpp>

// Adopted from musl's code.
char *basename(char *s) {
	// This empty string behavior is specified by POSIX.
	if (!s || !*s)
		return const_cast<char *>(".");
	
	// Delete trailing slashes.
	// Note that we do not delete the slash at index zero.
	auto i = strlen(s) - 1;
	for(; i && s[i] == '/'; i--)
		s[i] = 0;

	// Find the last non-trailing slash.
	for(; i && s[i - 1] != '/'; i--)
		;
	return s + i;
}

char *dirname(char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

