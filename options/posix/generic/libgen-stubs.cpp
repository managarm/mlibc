
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

char *dirname(char *s) {
	if (!s || !(*s))
		return const_cast<char *>(".");

	auto i = strlen(s) - 1;

	// Skip trailing slashes.
	for (; s[i] == '/'; i--)
		if(!i) // Path only consists of slashes.
			return const_cast<char *>("/");

	// Skip the last non-slash path component.
	for (; s[i] != '/'; i--)
		if(!i) // Path only contains a single component.
			return const_cast<char *>(".");

	// Skip slashes.
	for (; s[i] == '/'; i--)
		if(!i) // Path is entry in root directory.
			return const_cast<char *>("/");

	s[i+1] = 0;

	return s;
}

