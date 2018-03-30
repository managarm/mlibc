
#include <bits/ensure.h>
#include <stdlib.h>
#include <string.h>

#include <frigg/debug.hpp>

char *strdup(const char *string) {
	auto num_bytes = strlen(string);

	char *new_string = (char *)malloc(num_bytes + 1);
	if(!new_string) // TODO: set errno
		return nullptr;

	memcpy(new_string, string, num_bytes);
	new_string[num_bytes] = 0;
	return new_string;
}

char *strndup(const char *string, size_t max_size) {
	auto num_bytes = strlen(string);
	if(num_bytes > max_size)
		num_bytes = max_size;

	char *new_string = (char *)malloc(num_bytes + 1);
	if(!new_string) // TODO: set errno
		return nullptr;

	memcpy(new_string, string, num_bytes);
	new_string[num_bytes] = 0;
	return new_string;
}

size_t strnlen(const char *s, size_t n) {
	__ensure(n >= 0);
	size_t len = 0;
	for(size_t i = 0; s[i]; i++) {
		if(len == n)
			break;
		len++;
	}
	return len;
}

char *strtok_r(char *__restrict s, const char *__restrict del, char **__restrict m) {
	__ensure(m);
	
	// We use *m = null to memorize that the entire string was consumed.
	char *tok;
	if(s) {
		tok = s;
	}else if(*m) {
		tok = *m;
	}else {
		return nullptr;
	}

	// Skip initial delimiters.
	// After this loop: *tok is non-null iff we return a token.
	while(*tok && strchr(del, *tok))
		tok++;
	
	// Replace the following delimiter by a null-terminator.
	// After this loop: *p is null iff we reached the end of the string.
	auto p = tok;
	while(*p && !strchr(del, *p))
		p++;
	
	if(*p) {
		*p = 0;
		*m = p + 1;
	}else{
		*m = nullptr;
	}
	return tok;
}

char *strsep(char **m, const char *del) {
	__ensure(m);

	auto tok = *m;
	if(!tok)
		return nullptr;

	// Replace the following delimiter by a null-terminator.
	// After this loop: *p is null iff we reached the end of the string.
	auto p = tok;
	while(*p && !strchr(del, *p))
		p++;
	
	if(*p) {
		*p = 0;
		*m = p + 1;
	}else{
		*m = nullptr;
	}
	return tok;
}

char *strsignal(int sig) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

