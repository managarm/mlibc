
#include <stdlib.h>
#include <string.h>

#include <bits/ensure.h>

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

	// Skip initial delimiters.
	char *w = s ? s : *m;
	while(*w && strchr(del, *w))
		w++;

	*m = w;

	// Replace the following delimiter by a null-terminator.
	while(*w && !strchr(del, *w))
		w++;
	*w = 0;

	return *m;
}

