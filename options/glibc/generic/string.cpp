#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <type_traits>

#include <ctype.h>
#include <string.h>

/* This is a bit of a weird detail of the GNU implementation and C's lack of
 * overloading and strictness: GNU takes const char * and returns a char * so
 * that it autocasts to your desired constness, this function never actually
 * modifies the string.
 */
char *__mlibc_gnu_basename_c(const char *path) {
	char *basename_component = strrchr(path, '/');
	if (!basename_component) {
		return const_cast<char *>(path);
	}
	return basename_component + 1;
}

/* GNU exposes these overloads, and as a result, we should probably have them
 * checked, to make sure we actually match expectations.
 */
static_assert(
	std::is_same_v<decltype(basename((const char *)nullptr)), const char*>,
	"C++ overloads broken"
);

static_assert(
	std::is_same_v<decltype(basename((char *)nullptr)), char*>,
	"C++ overloads broken"
);

// Taken from musl.
int strverscmp(const char *l0, const char *r0) {
	const unsigned char *l = (const unsigned char *)l0;
	const unsigned char *r = (const unsigned char *)r0;
	size_t i, dp, j;
	int z = 1;

	/* Find maximal matching prefix and track its maximal digit
	 * suffix and whether those digits are all zeros. */
	for(dp = i = 0; l[i] == r[i]; i++) {
		int c = l[i];
		if(!c)
			return 0;
		if(!isdigit(c))
			dp = i + 1, z = 1;
		else if(c != '0')
			z = 0;
	}

	if(l[dp] != '0' && r[dp] != '0') {
		/* If we're not looking at a digit sequence that began
		 * with a zero, longest digit string is greater. */
		for(j = i; isdigit(l[j]); j++) {
			if(!isdigit(r[j]))
				return 1;
		}
		if(isdigit(r[j]))
			return -1;
	} else if(z && dp < i && (isdigit(l[i]) || isdigit(r[i]))) {
		/* Otherwise, if common prefix of digit sequence is
		 * all zeros, digits order less than non-digits. */
		return (unsigned char)(l[i] - '0') - (unsigned char)(r[i] - '0');
	}

	return l[i] - r[i];
}

void *mempcpy(void *dest, const void *src, size_t len) {
	return (char *)memcpy(dest, src, len) + len;
}

void explicit_bzero(void *s, size_t len) {
  memset (s, 0, len);
  // Compiler barrier to prevent optimizing away the memset
  asm volatile ("" ::: "memory");
}
