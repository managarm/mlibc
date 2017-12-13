
#include <stdlib.h>
#include <string.h>

#include <mlibc/ensure.h>

char *strdup(const char *string) {
	int num_bytes = strlen(string) + 1;
	char *new_string = (char *)malloc(num_bytes);
	if(!new_string) // TODO: set errno
		return nullptr;
	memcpy(new_string, string, num_bytes);
	return new_string;
}

char *strndup(const char *string, size_t) {
	__ensure(!"strndup() not implemented");
	__builtin_unreachable();
}

size_t strnlen(const char *, size_t) {
	__ensure(!"strnlen() not implemented");
	__builtin_unreachable();
}

char *strtok_r(char *__restrict, const char *__restrict, char **__restrict) {
	__ensure(!"strtok_r() not implemented");
	__builtin_unreachable();
}

