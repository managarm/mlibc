
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

