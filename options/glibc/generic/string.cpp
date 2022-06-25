#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <type_traits>
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
