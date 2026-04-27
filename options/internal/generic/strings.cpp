#include <ctype.h>
#include <string.h>

#include <mlibc/strings.hpp>

namespace mlibc {

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

size_t strnlen(const char *s, size_t n) {
	size_t len = 0;
	while(len < n && s[len])
		++len;
	return len;
}

size_t strlcpy(char *d, const char *s, size_t n) {
	const char *src = s;
	char *dst = d;
	size_t left = n;

	if (left) {
		while (--left > 0)
			if ((*dst++ = *src++) == '\0')
				break;
	}

	if (left == 0) {
		if (n)
			*dst = '\0';
		while (*src++);
	}

	return (src - s - 1);
}

char *stpncpy(char *__restrict dest, const char *__restrict src, size_t n) {
	size_t nulls, copied, srcLen = strlen(src);
	if (n >= srcLen) {
		nulls = n - srcLen;
		copied = srcLen;
	} else {
		nulls = 0;
		copied = n;
	}

	memcpy(dest, src, copied);
	memset(dest + srcLen, 0, nulls);
	return dest + n - nulls;
}

} // namespace mlibc
