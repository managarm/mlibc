#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <type_traits>
#include <string.h>

/* As with other string functions in C, this takes a const char * and returns a
 * char * so that it autocasts to your desired constness. This function never
 * actually modifies the string.
 */
char *basename(const char *path) {
	char *basename_component = strrchr(path, '/');
	if (!basename_component)
		return const_cast<char *>(path);
	return basename_component + 1;
}
