
#include <strings.h>
#include <string.h>

#include <ctype.h>
#include <bits/ensure.h>

char *index (const char *s, int c) {
	return strchr(s, c);
}

char *rindex(const char *s, int c) {
	return strrchr(s, c);
}

int ffs(int word) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int strcasecmp(const char *a, const char *b) {
	size_t i = 0;
	while(true) {
		unsigned char a_byte = tolower(a[i]);
		unsigned char b_byte = tolower(b[i]);
		if(!a_byte && !b_byte)
			return 0;
		// If only one char is null, one of the following cases applies.
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
		i++;
	}
}

int strncasecmp(const char *a, const char *b, size_t size) {
	for(size_t i = 0; i < size; i++) {
		unsigned char a_byte = tolower(a[i]);
		unsigned char b_byte = tolower(b[i]);
		if(!a_byte && !b_byte)
			return 0;
		// If only one char is null, one of the following cases applies.
		if(a_byte < b_byte)
			return -1;
		if(a_byte > b_byte)
			return 1;
	}
	return 0;
}

