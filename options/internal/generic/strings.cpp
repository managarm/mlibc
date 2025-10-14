#include <ctype.h>

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

} // namespace mlibc
