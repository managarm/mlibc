
#include <strings.h>
#include <string.h>

#include <ctype.h>
#include <bits/ensure.h>
#include <mlibc/strings.hpp>

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
		for(size_t b = 0; b < sizeof(T) * 8;)
			if(i & (static_cast<T>(0x1) << b++))
				return b;

		return 0;
	}

}

// On RISC-V, __builtin_ffs just calls into ffs, so we can't use it here.
#if defined(__has_builtin) && !defined(__riscv)
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
	return mlibc::strncasecmp(a, b, size);
}

// Marked as obsolete in posix 2008 but used by at least tracker
int bcmp(const void *s1, const void *s2, size_t n) {
	return memcmp(s1, s2, n);
}

void bcopy(const void *s1, void *s2, size_t n) {
	memmove(s2, s1, n);
}

void bzero(void *s, size_t n) {
	memset(s, 0, n);
}

