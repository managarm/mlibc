
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

namespace {

	template<typename T>
	int ffs_generic(T i) {
		//Non-portably assume a byte has 8 bits; fine in all plausible cases.
		for(int b = 0; b < sizeof(T) * 8;)
			if(i & (static_cast<T>(0x1) << b++))
				return b;

		return 0;
	}

}

#ifdef __has_builtin
#	if __has_builtin(__builtin_ffs)
#		define __mlibc_ffs __builtin_ffs
#	endif
#	if __has_builtin(__builtin_ffsl)
#		define __mlibc_ffsl __builtin_ffsl
#	endif
#	if __has_builtin(__builtin_ffsll)
#		define __mlibc_ffsll __builtin_ffsll
#	endif
#endif

int ffs(int i) {
#ifdef __mlibc_ffs
	return __mlibc_ffs(i);
#else
	return ffs_generic<int>(i);
#endif
}

/*
	Both ffsl() and ffsll() are glibc extensions
	defined in string.h. They are however implemented
	here because of similarity in logic and
	shared code.
*/

int ffsl(long i) {
#ifdef __mlibc_ffsl
	return __mlibc_ffsl(i);
#else
	return ffs_generic<long>(i);
#endif
}

int ffsll(long long i) {
#ifdef __mlibc_ffsll
	return __mlibc_ffsll(i);
#else
	return ffs_generic<long long>(i);
#endif
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

